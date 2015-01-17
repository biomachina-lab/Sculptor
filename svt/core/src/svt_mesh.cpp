/***************************************************************************
                          svt_mesh
			  --------
    begin                : 20.01.2003
    author               : Maik Boltes, Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt_includes
#include <svt_stlStack.h>
#include <svt_quadric.h>
#include <svt_mesh.h>
#include <svt_matrix4.h>
#include <svt_force_tool.h>
#include <svt_iomanip.h>
#include <svt_cmath.h>
#include <svt_time.h>
#include <svt_threads.h>
#include <svt_semaphore.h>
#include <svt_opengl.h>

#include <float.h>
#include <time.h>


// global mesh creation/deletion/updateLOD semaphore
svt_semaphore g_oMeshSemaphore;

extern "C" void APIENTRY glDrawRangeElements (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);

/**
 * Simplification Thread
 */
void* svt_simplificationThread(void* pArg)
{
    simplifyMeshThread* pThread = (simplifyMeshThread*)pArg;

    // inside this function the pThread variable is accessed and
    // could also be manipulated somewhere else, what would lead to severe problems/segfaults.
    // Therefore the semaphore is used in the destructor when pThread is set to false
    while(pThread->bValid)
    {
        // creation/deletion of mesh semaphore
        g_oMeshSemaphore.P();
        
        int iTime = 1000;
        
        if (pThread->bValid)
        {
            if (pThread->pMesh->updateSimplify())
                pThread->pMesh->rebuildDL();
            iTime = pThread->pMesh->getAdaptionTime();
        }
        
        // ok, now release semaphore
        g_oMeshSemaphore.V();
        
        // and go to sleep
        svt_sleep(iTime);
    }
    
    printf("svt_mesh> deleting simplification thread....\n");
    
    // we have to delete this thing, the mesh cannot do it, it is long gone itself...
    delete pThread;

    return NULL;
}

/**
 * callback function called for reading vertices in ply files
 */
int svt_vertex_cb(p_ply_argument argument) {
    svt_mesh* mesh;
    static Point3f pkt;
    static svt_color col;
    long idx;

    ply_get_argument_user_data(argument, (void **) &mesh, &idx);

    pkt[idx] = ply_get_argument_value(argument);
    if (idx == 3) col.setR(ply_get_argument_value(argument)/255.);
    if (idx == 4) col.setG(ply_get_argument_value(argument)/255.);
    if (idx == 5) col.setB(ply_get_argument_value(argument)/255.);

    if ((idx == 2)  && !mesh->getVertexColored())
        mesh->addVertex(pkt);
    if ((idx == 5)  && mesh->getVertexColored())
        mesh->addVertex(pkt, col);

    return 1;
}
/**
 * callback function called for reading faces in ply files
 */
int svt_face_cb(p_ply_argument argument) {
    svt_mesh* mesh;
    long length, value_index;
    static UInt32 triVer[3];
    UInt32 swap;
    long idx;

    ply_get_argument_user_data(argument, (void **) &mesh, &idx);
    ply_get_argument_property(argument, NULL, &length, &value_index);

    triVer[value_index] = (UInt32)(ply_get_argument_value(argument));

    if (value_index == 2)
    {
        // swap because vertex numbers are wisa versa
        if (idx == -1)
        {
            swap = triVer[2]; 
            triVer[2] = triVer[1]; 
            triVer[1] = swap;
        }
        if (!(mesh->addTriangle(triVer)))
            printf("(Warning: Triangle (%d, %d, %d) not added, may be because of not existing vertex number or reading triangles before vertices!) ", triVer[0], triVer[1], triVer[2]);
    }
    return 1;
}
/**
 * callback function called for reading faces in ply files
 */
int svt_tristrip_cb(p_ply_argument argument) {
    svt_mesh* mesh;
    static long length=0, value_index=0;
    static UInt32 triVer[3];
    static bool toTurn=false; // flag for toggling between normal directions
    UInt32 triVer2[3];
    int val;
    long number;

    ply_get_argument_user_data(argument, (void **) &mesh, NULL);
    ply_get_argument_property(argument, NULL, NULL, &number);
    val = (int)(ply_get_argument_value(argument));

    if (number == -1) // first call, just ignore value and set toTurn to false
    {
        toTurn=false;
        return 1;
    }

    if (val == -1)
    {
        value_index = 0;
        length = 0;
        toTurn = false;
    }
    else
    {
        triVer[value_index] = (UInt32)(val);
        value_index = (value_index+1)%3;
        length++;
        if (length>2)
        {
            if (toTurn)
            {
                triVer2[0] = triVer[1];
                triVer2[1] = triVer[0];
                triVer2[2] = triVer[2];
                if (!(mesh->addTriangle(triVer2)))
                    printf("(Warning: Triangle (%d, %d, %d) not added, may be because of not existing vertex number or reading triangles before vertices!) ", triVer2[0], triVer2[1], triVer2[2]);       
                toTurn = false;
            }
            else
            {
                if (!(mesh->addTriangle(triVer)))
                    printf("(Warning: Triangle (%d, %d, %d) not added, may be because of not existing vertex number or reading triangles before vertices!) ", triVer[0], triVer[1], triVer[2]);       
                toTurn = true;
            }
        }
    }

    return 1;
}

// !!!! Waehrend der Berechnung der Vereinfachung darf keine Kopie vom Netz erstellt werden, 
//      da sonst Zeiger undefiniert sind (Vertex, Edge) !!!!

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_mesh::svt_mesh(svt_pos* pPos, svt_properties* pProp) : svt_node(pPos,pProp)
{
    setName( "svt_mesh" );

    m_fAdaptionStep = 0.3;
    m_fAdaptionTolerance = 0.25;
    m_iAdaptionTime = 1500;
    m_pThread = NULL;
    m_borderPenalizeFactor = 1000.;
    m_switchingIdentityPenalizeFactor = 1000.;
    m_switchingIdentityPenalizeAdd = 0.001;
    m_penalizeSwitchArc = -0.5;
    m_triNumber = 0;
    m_compNumber = 0;
    m_edgeNumber1 = -1;
    m_edgeNumber2 = -1;
    m_edgeNumber3 = -1;
    //setAutoDL(false); //jedes neuzeichnen durchlaeuft drawgl routine

    m_isSimplifiedFlag = true; // wird bei simplifizierung aufbau kurzzeitig auf false gesetzt
    m_simpOptionsChanged = true;
    m_SimplifyRule = 0; // FPS ist default
    m_RuleMinimalFPS = 20;
    m_RuleMinimalFUS = 300;
    m_RuleLocalError = 0.1; // sollte spaeter dynamisch gesetz werden
    m_RuleVertexNumbers = 1; // wird nach Vereinfachungsberechnung gesetzt

    m_penalizeBordersFlag = true;
    m_penalizeSwitchFlag = true;
    m_penalizeIdentityFlag = true;

    m_edge_threshold = 0.; 

    m_globalErrorMax = 0.;
    m_globalErrorAvg = 0.;
    m_globalErrorSum = 0.;

    m_lightTwoSide = false;
    m_culling = false;
    m_switchNormals = false;

    m_bSwitchNormalDirectionForReadingPly = false;
    m_bDelUnusedVertices = false;

    m_bVertexColored = false;

    m_pMcTriMat = NULL;
}

/**
 * Destructor
 */
svt_mesh::~svt_mesh()
{
    // first stop the thread
    if (m_pThread != NULL)
    {
        printf("DEBUG: ~svt_mesh stopping thread!\n");

        g_oMeshSemaphore.P();
        m_pThread->bValid = false;
        g_oMeshSemaphore.V();
    }
}

/**
 * compute the size of the surface of the mesh
 * \return size of the surface of the mesh
 */ 
double svt_mesh::surfaceSize()
{
    double size = 0.;

    for (unsigned i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            size += crossProduct(m_oVertices[m_oTriangles[i].ver[1]].pkt - m_oVertices[m_oTriangles[i].ver[0]].pkt, m_oVertices[m_oTriangles[i].ver[2]].pkt - m_oVertices[m_oTriangles[i].ver[0]].pkt).length()/2.;
    return size;
}

/**
 * switch the circular direction of vertices for switch the normal direction for reading PLY file
 * \return bool if switch is active
 */
bool svt_mesh::switchNormalDirectionForReadingPly()
{
    return m_bSwitchNormalDirectionForReadingPly = !m_bSwitchNormalDirectionForReadingPly;
}

/**
 * set status for deleting unused vertices while reading smf and ply files
 * \return bool true, if unused vertices will be deleted after reading smf and ply files
 */
bool svt_mesh::setDelUnusedVertices(bool b)
{
    return m_bDelUnusedVertices = b;
}

/**
 * set the flag that the vertices have their own color
 * \param b true, if vertices have their own color
 */
void svt_mesh::setVertexColored(bool b)
{
    m_bVertexColored = b;
}
/**
 * get the flag that shows, if the vertices have their own color
 */
bool svt_mesh::getVertexColored()
{
    return m_bVertexColored;
}

/**
 * set the flag that shows, if surface should be lightend from both sides
 */
void svt_mesh::setLightTwoSide(bool b)
{
   m_lightTwoSide = b;
}
/**
 * get the flag that shows, if surface should be lightend from both sides
 */
bool svt_mesh::getLightTwoSide()
{
    return m_lightTwoSide;
}
/**
 * set the flag that shows, if surface should be culled
 */
void svt_mesh::setCulling(bool b)
{
   m_culling = b;
}
/**
 * get the flag that shows, if surface should be culled
 */
bool svt_mesh::getCulling()
{
    return m_culling;
}
/**
 * set the flag that shows, if normals should be switched for visu
 */
void svt_mesh::setSwitchNormals(bool b)
{
   m_switchNormals = b;
}
/**
 * get the flag that shows, if normals should be switched for visu
 */
bool svt_mesh::getSwitchNormals()
{
    return m_switchNormals;
}

/**
 * read PLY file
 * \param readFile pointer to filename
 * \return bool if reading was successful
 */
bool svt_mesh::readPly(const char *readFile)
{
    long nvertices, ntriangles;
    p_ply_element ply_elem;

    p_ply ply = ply_open(readFile, NULL);
    if (!ply) 
        return false;

    printf("svt_mesh> Reading %s... ", readFile); fflush(stdout);

    if (!ply_read_header(ply)) 
        return false;

    ply_elem = ply_find_element(ply, "vertex");
    if (ply_find_property(ply_elem, "red") && ply_find_property(ply_elem, "green") && ply_find_property(ply_elem, "blue"))
        setVertexColored(true);
    if (ply_find_property(ply_elem, "diffuse_red") && ply_find_property(ply_elem, "diffuse_green") && ply_find_property(ply_elem, "diffuse_blue"))
        setVertexColored(true);

    nvertices = ply_set_read_cb(ply, "vertex", "x", svt_vertex_cb, this, 0);
                ply_set_read_cb(ply, "vertex", "y", svt_vertex_cb, this, 1);
                ply_set_read_cb(ply, "vertex", "z", svt_vertex_cb, this, 2);
                ply_set_read_cb(ply, "vertex", "red", svt_vertex_cb, this, 3);
                ply_set_read_cb(ply, "vertex", "green", svt_vertex_cb, this, 4);
                ply_set_read_cb(ply, "vertex", "blue", svt_vertex_cb, this, 5);
                ply_set_read_cb(ply, "vertex", "diffuse_red", svt_vertex_cb, this, 3);
                ply_set_read_cb(ply, "vertex", "diffuse_green", svt_vertex_cb, this, 4);
                ply_set_read_cb(ply, "vertex", "diffuse_blue", svt_vertex_cb, this, 5);
    reserveVertex(nvertices);

    if (ply_find_element(ply, "face") != NULL)
    {
        if (m_bSwitchNormalDirectionForReadingPly)
            ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", svt_face_cb, this, -1);
        else
            ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", svt_face_cb, this, 0);
        reserveTriangle(ntriangles);
    }
    else // tristrips
    {
        ntriangles = ply_set_read_cb(ply, "tristrips", "vertex_indices", svt_tristrip_cb, this, 0);
        reserveTriangle(2*nvertices); // just estimate
    }

    if (!ply_read(ply)) 
        return false;
    ply_close(ply);
    if (getVertexColored())
    {
        printf("%ld vertices (colored), %d triangles finished!\n", nvertices, getTriNumber()); fflush(stdout);
    }
    else
    {
        printf("%ld vertices, %d triangles finished!\n", nvertices, getTriNumber()); fflush(stdout);
    }

    if (m_bDelUnusedVertices)
        delUnusedVertices();

    calcMinMaxAll(); // minmax aller knoten (auch nach vereinfachung)
    setMinX(getMinXAll());
    setMinY(getMinYAll()); 
    setMinZ(getMinZAll());
    setMaxX(getMaxXAll()); 
    setMaxY(getMaxYAll()); 
    setMaxZ(getMaxZAll());
    
    // just nice, to know where the data is in the space: (-1..1)
    //      normalizeData();
    
    calcAllNormals();
    return true;
}

/**
 * calculates field with new vertex numbers for vertices in current mesh
 * \param field with new vertex numbers for vertices in current mesh
 * \return int vertex number realy within triangles
 */
int svt_mesh::calcVerTab(int* verTab)
{
    int realNr = 0;
    unsigned i, j;

    for (i=0; i<getVerOrigNumber(); i++)
        verTab[i] = -1;
    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (verTab[m_oTriangles[i].ver[j]] == -1)
                    verTab[m_oTriangles[i].ver[j]] = realNr++;
    return realNr;
}
/**
 * save SMF file from mesh with current simplification level
 * \param saveFile pointer to filename
 * \return bool if writing was successful
 */
bool svt_mesh::saveSimplSmf(const char *saveFile)
{
    FILE *fp;
    unsigned i, j, k;
//     int memRuleVerNum, memRule;
    int* verTab;
    int realNr; // vertex number realy within triangles, 
                // because some vertices sometimes (cow.smf) are in no triangle (<= getVerNumber())
    int nr = -1;

    verTab = new int[getVerOrigNumber()];

    if((fp = fopen(saveFile, "w")) == NULL)
    {
        fprintf(stderr, "svt_mesh> Error: File %s could not be opened.\n", saveFile);
        return false;
    }

    g_oMeshSemaphore.P();
//     // fix state of mesh LOD (copy mesh and than reset is critical because of update thread)
//     memRuleVerNum = getRuleVertexNumbers();
//     memRule = getSimplifyRule();
//     setRuleVertexNumbers(getVerNumber());
//     setSimplifyRule(2);
//     updateSimplify();

    printf("svt_mesh> Writing mesh with current simplification level to %s... ", saveFile); fflush(stdout);
    
    // field with new vertex numbers for vertices in current mesh
    realNr = calcVerTab(verTab);
    fprintf(fp, "#$SMF 1.0\n#$vertices %d\n#$faces %d\n# written by svt_mesh\n", realNr, getTriNumber());
        
    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (verTab[m_oTriangles[i].ver[j]] > nr)
                {
                    fprintf(fp, "v");
                    for (k=0; k<3; k++)
                        fprintf(fp, " %f", m_oVertices[m_oTriangles[i].ver[j]].pkt[k]);
                    fprintf(fp, "\n");
                    nr++;
                }
    for (i=0; i<m_oTriangles.size(); i++)
    {
        if (m_oTriangles[i].in)
        {
            fprintf(fp, "f");
            for (j=0; j<3; j++)
                fprintf(fp, " %d", verTab[m_oTriangles[i].ver[j]]+1); // in smf knotennummer beginnt bei 1
            fprintf(fp, "\n");
        }
    }
    
    fclose(fp);

    printf("%d vertices, %d triangles finished", realNr, getTriNumber());
    if (realNr != (int)(getVerNumber()))
    {
        printf("(%d vertices outside triangles not written)!\n", getTriNumber()-realNr); fflush(stdout);
    }
    else
    {
        printf("!\n"); fflush(stdout);
    }

    // release semaphore
    g_oMeshSemaphore.V();
//     // mesh to state before writing
//     setRuleVertexNumbers(memRuleVerNum);
//     setSimplifyRule(memRule);

    delete [] verTab;
    return true;
}
/**
 * save SMF file from mesh
 * \param readFile pointer to filename
 * \return bool if writing was successful
 */
bool svt_mesh::saveSmf(const char *saveFile)
{
    FILE *fp;
    unsigned i, j;
    int memRuleVerNum, memRule, memVerNum;
    
    if((fp = fopen(saveFile, "w")) == NULL)
    {
        fprintf(stderr, "svt_mesh> Error: File %s could not be opened.\n", saveFile);
        return false;
    }
    printf("svt_mesh> Writing full mesh to %s... ", saveFile); fflush(stdout);
    fprintf(fp, "#$SMF 1.0\n#$vertices %d\n#$faces %d\n# written by svt_mesh\n", getVerOrigNumber(), getTriOrigNumber());
    
    // reset original mesh (copy mesh and than reset is critical because of update thread)
    memVerNum = getVerNumber();
    memRuleVerNum = getRuleVertexNumbers();
    memRule = getSimplifyRule();
    setRuleVertexNumbers(getVerOrigNumber());
    setSimplifyRule(2);
    g_oMeshSemaphore.P();
    updateSimplify();
    g_oMeshSemaphore.V();
    
    for (i=0; i<m_oVertices.size(); i++)
    {
        fprintf(fp, "v");
        for (j=0; j<3; j++)
            fprintf(fp, " %f", m_oVertices[i].pkt[j]);
        fprintf(fp, "\n");
    }
    for (i=0; i<m_oTriangles.size(); i++)
    {
        fprintf(fp, "f");
        for (j=0; j<3; j++)
            fprintf(fp, " %d", (m_oTriangles[i].ver[j])+1); // in smf knotennummer beginnt bei 1
        fprintf(fp, "\n");
    }
    
    fclose(fp);

    // mesh to state before writing
    setRuleVertexNumbers(memVerNum);
    g_oMeshSemaphore.P();
    updateSimplify();
    g_oMeshSemaphore.V();
    setRuleVertexNumbers(memRuleVerNum);
    setSimplifyRule(memRule);

    printf("%d vertices, %d triangles finished!\n", getVerOrigNumber(), getTriOrigNumber()); fflush(stdout);
    return true;
}
/**
 * read SMF file
 * \param readFile pointer to filename
 * \return bool if reading was successful
 */
bool svt_mesh::readSmf(const char *readFile)
{
  FILE *fp;
  char line[181];
  int c;
  char dummy;
  //int isSMF = 0;
  int vnr=0, tnr=0;
  int vi=0, ti=0;
  Point3f pkt;
  UInt32 triVer[3];
  // only neccesary if comparison-flag is true and vertices will match
  vector<int> match;

  if((fp = fopen(readFile, "r")) == NULL)
  {
      fprintf(stderr, "svt_mesh> Error: File %s could not be opened.\n", readFile);
      return false;
  }
  line[0] = '\0';

  printf("svt_mesh> Reading %s... ", readFile); fflush(stdout);

  while (fscanf(fp, "%180[^\n]", line) != EOF)
  {
      while (((c = fgetc(fp)) != '\n') && (c != EOF))
    	  ;

      //if (isSMF)
      {
          if (!strncmp("#$vertices", line, 10))
    	  {
              sscanf(line,"#$vertices %d", &vnr);
    	      reserveVertex(vnr);

              match.resize(vnr);
              for (int i=0; i<vnr; i++)
                  match[i]= i;
          }
    	  else if (!strncmp("#$faces", line, 7))
    	  {
              sscanf(line,"#$faces %d", &tnr);
    	      reserveTriangle(tnr);
    	  }
    	  else if (('v' == line[0]))
    	  {
              sscanf(line,"v %f %f %f", &pkt[0], &pkt[1], &pkt[2]);
              if (vi >= (int) match.size())
                  match.push_back(vi); // ok?
              match[vi] = addVertex(pkt);
              vi++;
    	  }
    	  else if ((('f' == line[0]) || ('t' == line[0])))
    	  {
              sscanf(line,"%c %d %d %d", &dummy, &triVer[0], &triVer[1], &triVer[2]);
    	      // data begins by 1 in the  SMF format!!!
    	      triVer[0]--; triVer[1]--; triVer[2]--;
              // attention: if tri read before vert and vertices are more than vnr than it will crash! (that's unusual)
              triVer[0] = match[triVer[0]]; triVer[1] = match[triVer[1]]; triVer[2] = match[triVer[2]]; 
              if (!addTriangle(triVer))
                  printf("\nsvt_mesh> Warning: Triangle %d not added, may be because of not existing vertex number (%d, %d, %d) or reading triangles before vertices!\n", ti, triVer[0], triVer[1], triVer[2]);
              ti++;
    	  }
      }

      //if (!isSMF && !strncmp("#$SMF 1.0", line, 9))
      //  isSMF = 1;

      line[0] = '\0';
  }

  fclose(fp);

  printf("%d vertices, %d triangles finished!\n", vnr, tnr); fflush(stdout);
  if (getCompareFlag())
      printf("svt_mesh> %d vertices have been mapped because of comparison.\n", getRemappedVertices());

  if (vnr > vi)
  {
      printf("\nsvt_mesh> Warning: Specified Number %d of vertices is greater than the really number %d!\n", vnr, vi);
      vnr = vi;
  } else if (vnr < vi)
  {
      printf("\nsvt_mesh> Warning: No vertex number specified or specified Number %d of vertices is smaller than the really number %d!\n", vnr, vi);
  }

  if (tnr > ti)
  {
      printf("\nsvt_mesh> Warning: Specified Number %d of triangles is greater than the really number %d!\n", tnr, ti);
      tnr = ti;
  } else if (tnr < ti)
  {
      printf("\nsvt_mesh> Warning: No triangle number specified or specified Number %d of triangles is smaller than the really number %d!\n", tnr, ti);
  }

  if (m_bDelUnusedVertices)
      delUnusedVertices();

  calcMinMaxAll(); // minmax aller knoten (auch nach vereinfachung)
  setMinX(getMinXAll());
  setMinY(getMinYAll()); 
  setMinZ(getMinZAll());
  setMaxX(getMaxXAll()); 
  setMaxY(getMaxYAll()); 
  setMaxZ(getMaxZAll());
 
  // just nice, to know where the data is in the space: (-1..1)
  //  normalizeData();

  calcAllNormals();

  return true;
}

/**
 * draw the mesh in povray
 */
void svt_mesh::drawPOV()
{
    unsigned int i, j;
    int* verTab = new int[getVerOrigNumber()];
    int realNr;
    int nr;

    g_oMeshSemaphore.P();

    realNr = calcVerTab(verTab);

    cout << "  mesh2 {" << endl;

    // output vertices
    cout << "    vertex_vectors {" << endl;
    cout << "      " << realNr << "," << endl;
    nr = -1;
    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (verTab[m_oTriangles[i].ver[j]] > nr)
                {
                    if (nr < realNr-2)
                        cout << "      <" << m_oVertices[m_oTriangles[i].ver[j]].pkt.x() << ", " << m_oVertices[m_oTriangles[i].ver[j]].pkt.y() << ", " << m_oVertices[m_oTriangles[i].ver[j]].pkt.z() << "> , " << endl;
                    else
                        cout << "      <" << m_oVertices[m_oTriangles[i].ver[j]].pkt.x() << ", " << m_oVertices[m_oTriangles[i].ver[j]].pkt.y() << ", " << m_oVertices[m_oTriangles[i].ver[j]].pkt.z() << ">" << endl;
                    nr++;
                }
    cout << "    }" << endl;

    // output normal vectors
    cout << "    normal_vectors {" << endl;
    cout << "      " << realNr << "," << endl;
    nr = -1;
    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (verTab[m_oTriangles[i].ver[j]] > nr)
                {
                    if (nr < realNr-2)
                        cout << "      <" << m_oVertices[m_oTriangles[i].ver[j]].norm.x() << ", " << m_oVertices[m_oTriangles[i].ver[j]].norm.y() << ", " << m_oVertices[m_oTriangles[i].ver[j]].norm.z() << ">, " << endl;
                    else
                        cout << "      <" << m_oVertices[m_oTriangles[i].ver[j]].norm.x() << ", " << m_oVertices[m_oTriangles[i].ver[j]].norm.y() << ", " << m_oVertices[m_oTriangles[i].ver[j]].norm.z() << ">" << endl;
                    nr++;
                }
    cout << "    }" << endl;

    if (getVertexColored())
    {
        // output color at vertices
        cout << "    texture_list {" << endl;
        cout << "      " << realNr << "," << endl;
        nr = -1;
        for (i=0; i<m_oTriangles.size(); i++)
            if (m_oTriangles[i].in)
                for (j=0; j<3; j++)
                    if (verTab[m_oTriangles[i].ver[j]] > nr)
                    {
                        cout << "      texture {pigment {rgb <" << m_oVertices[m_oTriangles[i].ver[j]].col.getR() << ", " << m_oVertices[m_oTriangles[i].ver[j]].col.getG() << ", " << m_oVertices[m_oTriangles[i].ver[j]].col.getB() << ">} finish {ambient rgb <0.2, 0.2, 0.2 > brilliance 1.7 diffuse 0.8 roughness 0.06 ambient 0.05 specular 0.5}} " << endl;
                        nr++;
                    }
        cout << "    }" << endl;
    }

    // output face indices
    cout << "    face_indices {" << endl;
    cout << "      " << getTriNumber() << "," << endl;
    nr = 0;
    for (i=0; i<m_oTriangles.size(); i++)
    {
        if (m_oTriangles[i].in)
        {
            if (getVertexColored())
            {
                if ((unsigned) nr < getTriNumber()-1)
                    cout << "      <" << verTab[m_oTriangles[i].ver[0]] << ", " << verTab[m_oTriangles[i].ver[1]] << ", " << verTab[m_oTriangles[i].ver[2]] << ">, " << verTab[m_oTriangles[i].ver[0]] << ", " << verTab[m_oTriangles[i].ver[1]] << ", " << verTab[m_oTriangles[i].ver[2]] << ", " << endl;
                else
                    cout << "      <" << verTab[m_oTriangles[i].ver[0]] << ", " << verTab[m_oTriangles[i].ver[1]] << ", " << verTab[m_oTriangles[i].ver[2]] << ">, " << verTab[m_oTriangles[i].ver[0]] << ", " << verTab[m_oTriangles[i].ver[1]] << ", " << verTab[m_oTriangles[i].ver[2]] << endl;
            }
            else
            {
                if ((unsigned) nr < getTriNumber()-1)
                    cout << "      <" << verTab[m_oTriangles[i].ver[0]] << ", " << verTab[m_oTriangles[i].ver[1]] << ", " << verTab[m_oTriangles[i].ver[2]] << ">, " << endl;
                else
                    cout << "      <" << verTab[m_oTriangles[i].ver[0]] << ", " << verTab[m_oTriangles[i].ver[1]] << ", " << verTab[m_oTriangles[i].ver[2]] << ">" << endl;
            }
            nr++;
        }
    }
    cout << "    }" << endl;

    cout << "  }" << endl;

    g_oMeshSemaphore.V();
    delete [] verTab;
}

/**
 * draw the mesh in openinventor
 * Attention: only correct results with not simplified mesh
 */
void svt_mesh::drawIV()
{
    unsigned int i;

    // first write vertices/points
    cout << "  Coordinate3 {" << endl;
    cout << "    point [" << endl;

    for (i=0; i<m_oVertices.size(); i++)
    {
	cout << "      "
	    << m_oVertices[i].pkt.x() << " "
	    << m_oVertices[i].pkt.y() << " "
	    << m_oVertices[i].pkt.z();

	if (i < m_oVertices.size()-1)
	    cout << ",";

	cout << endl;
    }
    cout << "    ]" << endl;
    cout << "  }" << endl;

    // now write nomal vectors
    cout << "  Normal {" << endl;
    cout << "    vector [" << endl;

    for (i=0; i<m_oVertices.size(); i++)
    {
	cout << "      "
	    << m_oVertices[i].norm.x() << " "
	    << m_oVertices[i].norm.y() << " "
	    << m_oVertices[i].norm.z();

	if (i < m_oVertices.size()-1)
	    cout << ",";

	cout << endl;
    }
    cout << "    ]" << endl;
    cout << "  }" << endl;

    // now write face index maps
    cout << "  IndexedFaceSet {" << endl;
    cout << "    coordIndex [" << endl;

    for (i=0; i<m_oTriangles.size(); i++)
    {
	cout << "      "
	    << m_oTriangles[i].ver[0] << ","
	    << m_oTriangles[i].ver[1] << ","
	    << m_oTriangles[i].ver[2] << ", -1";

	if (i != m_oTriangles.size()-1)
	    cout << ",";

	cout << endl;
    }
    cout << "    ]" << endl;

    cout << "    normalIndex [" << endl;

    for (i=0; i<m_oTriangles.size(); i++)
    {
	cout << "      "
	    << m_oTriangles[i].ver[0] << ","
	    << m_oTriangles[i].ver[1] << ","
	    << m_oTriangles[i].ver[2] << ", -1";

	if (i != m_oTriangles.size()-1)
	    cout << ",";

	cout << endl;
    }
    cout << "    ]" << endl;
    cout << "  }" << endl;
}

/**
 * draw the mesh as yafray xml script
 * Attention: only correct results with not simplified mesh
 */
void svt_mesh::drawYafray()
{
    unsigned int i;

    cout << "    <mesh autosmooth = \"100.0\" >" << endl;

    // first write vertices/points
    cout << "      <points>" << endl;
    for (i=0; i<m_oVertices.size(); i++)
	cout << "        <p x=\"" << m_oVertices[i].pkt.x() << "\" y=\"" << m_oVertices[i].pkt.y() << "\" z=\"" << m_oVertices[i].pkt.z() << "\" />" << endl;
    cout << "      </points>" << endl;
    cout << endl;

    // now write face index maps
    cout << "      <faces>" << endl;
    for (i=0; i<m_oTriangles.size(); i++)
	cout << "        <f a=\"" << m_oTriangles[i].ver[0] << "\" b=\"" << m_oTriangles[i].ver[1] << "\" c=\"" << m_oTriangles[i].ver[2] << "\" />" << endl;
    cout << "      </faces>" << endl;
    cout << endl;

    cout << "    </mesh>" << endl;
}

/**
 * draw mesh in alias/wavefront obj format
 */
void svt_mesh::drawOBJ()
{
    vector< svt_vector4<Real32> > aVecs;
    vector< svt_vector4<Real32> > aNorms;
    vector< svt_face > aFaces;

    unsigned int i, j;
    int* verTab = new int[getVerOrigNumber()];
    int realNr;
    int nr;

    g_oMeshSemaphore.P();

    realNr = calcVerTab(verTab);

    // output vertices
    svt_vector4<Real32> oVec;
    //char oStr[256];
    nr = -1;
    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (verTab[m_oTriangles[i].ver[j]] > nr)
		{
                    oVec.set( m_oVertices[m_oTriangles[i].ver[j]].pkt.x(), m_oVertices[m_oTriangles[i].ver[j]].pkt.y(), m_oVertices[m_oTriangles[i].ver[j]].pkt.z() );
		    aVecs.push_back( oVec );
                    nr++;
                }

    // output normal vectors
    nr = -1;
    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (verTab[m_oTriangles[i].ver[j]] > nr)
                {
		    oVec.set( m_oVertices[m_oTriangles[i].ver[j]].norm.x(), m_oVertices[m_oTriangles[i].ver[j]].norm.y(), m_oVertices[m_oTriangles[i].ver[j]].norm.z() );
		    aNorms.push_back( oVec );
                    nr++;
                }

    // output face indices
    nr = 0;
    svt_face oFace;
    for (i=0; i<m_oTriangles.size(); i++)
    {
        if (m_oTriangles[i].in)
	{
            oFace.setVI_A( verTab[m_oTriangles[i].ver[0]]+1 );
            oFace.setVI_B( verTab[m_oTriangles[i].ver[1]]+1 );
	    oFace.setVI_C( verTab[m_oTriangles[i].ver[2]]+1 );
            oFace.setNI_A( verTab[m_oTriangles[i].ver[0]]+1 );
            oFace.setNI_B( verTab[m_oTriangles[i].ver[1]]+1 );
	    oFace.setNI_C( verTab[m_oTriangles[i].ver[2]]+1 );
            aFaces.push_back( oFace );
            nr++;
        }
    }

    g_oMeshSemaphore.V();

    svt_getScene()->getAliasOBJ().addMesh( aVecs, aNorms, aFaces );

    delete [] verTab;
};

/**
 * set color of mesh object
 * \param r red value between 0 and 1
 * \param g green value between 0 and 1
 * \param b blue value between 0 and 1
 */
void svt_mesh::setColor(float r, float g, float b)
{
    svt_color* pColor = getProperties()->getColor();
    pColor->setR(r);
    pColor->setG(g);
    pColor->setB(b);
}
/**
 * get red color fraction of mesh object
 * \return red color fraction of mesh object between 0 and 1
 */
float svt_mesh::getColorR()
{
    return getProperties()->getColor()->getR();
}
/**
 * get green color fraction of mesh object
 * \return green color fraction of mesh object between 0 and 1
 */
float svt_mesh::getColorG()
{
    return getProperties()->getColor()->getG();
}
/**
 * get blue color fraction of mesh object
 * \return blue color fraction of mesh object between 0 and 1
 */
float svt_mesh::getColorB()
{
    return getProperties()->getColor()->getB();
}

/**
 * set transparency level of mesh object
 * \param f transpyrency level between 0 and 1
 */
void svt_mesh::setTransparency(float f)
{
    getProperties()->getColor()->setTransparency(f);
}
/**
 * set how transparency of mesh object should done
 * \param b set stripple on
 */
void svt_mesh::setStipple(bool b)
{
    getProperties()->getColor()->setStipple(b);
}

/**
 * init storage and data for special marching cube mesh generation
 * \param l, x, y dimension of volume
 * \param fastMesh shows if the mesh is generated fast and not that exact
 */
void svt_mesh::initMarchingCube(int l, int x, int y, bool fastMesh)
{
    int i;

    setCompareFlag(false); // no need of comparision because of special insertion

    //4*(dims[0]-1-step)/step*(dims[1]-1-step)/step*(dims[2]-1-step)/step // maximum triangle number
    //2*(dims[0]-1-step)/step*(dims[1]-1-step)/step*(dims[2]-1-step)/step // maximum vertex number
    // just an estimation for the triangle number - for faster processing
    reserveTriangle(4*(l+x+y)*(l+x+y));
    // just an estimation for the triangle number - for faster processing
    if (fastMesh)
        reserveVertex(12*(l+x+y)*(l+x+y));
    else
    {
        reserveVertex(2*(l+x+y)*(l+x+y));

        //two slices of volume for four triangles
        //x, y dimension of one slice
        m_pMcTriMat = new int[x*y*4*4]; // *4 because of maximal 4 triangles in one voxel (12 vertices)
        for (i=0; i < (x*y*4*4); i++)
        {
            m_pMcTriMat[i] = -1;
        }
        m_iMcXdim = x;
        m_iMcYdim = y;
    }
}

/**
 * free storage for special marching cube mesh generation
 */
void svt_mesh::freeMarchingCube()
{
    if (m_pMcTriMat)
        delete [] m_pMcTriMat;

    m_pMcTriMat = NULL;
}

/**
 * checks, if oTriVer is already in voxel voxel
 * \param voxel voxel index from which up the next 4 triangles will be analysed
 * \param oTriVer vertex which is searched in voxel
 */
int svt_mesh::checkVoxelForVertex(int voxel, const Vector3f &oTriVer)
{
    int j, k;
    int triTemp, verTemp;

    for (k=0; ((triTemp = m_pMcTriMat[voxel+k])!= -1) && (k<4); k++) // for every triangle in voxel (k<nr for same voxel)
        if (triTemp != -2) // triangle was not eliminated because of same vertices
        {
            for (j=0; (j<3); j++) // for every vertex of triangle in voxel
            {
                verTemp = m_oTriangles[triTemp].ver[j];
                if (m_oVertices[verTemp].pkt.nearEqual(oTriVer))
                    return verTemp;
                // no distance cheque and looking for really nearest vertex, because eps is very small
            }
        }
    return -1;
}

/**
 * add a triangle to the mesh
 * special to triangles from marching cube algorithm
 * assuming, that the voxels will be traversed in correct sequential order
 * \param oTriangle new triangle object
 * \param oNormals normal vectors for the vertices of new triangle
 * \param x, y, z voxel index (0..)
 * \param nr number of triangle in voxel (0..3)
 */
void svt_mesh::addTriangle(const Triangle &oTriangle, const Triangle &oNormals, int x, int y, int z, int nr)
{
    static int layer = 0;
    static int z_old = -1;
    int i, j, k;
    svt_triangle_type triangle;
    int voxel, voxelB, voxelL, voxelT, voxelU, voxelP;
    int sameVer;

    // to make the insertion yet faster, the position of the triangle in the voxel
    // can save some searching, because in some voxels have not to be searched

    if (m_pMcTriMat == NULL)
    {
        cout << "svt_mesh.cpp> Error: for using this addTriangle function you have to call initMarchingCube first!" << endl;
        return;
    }

    // not counting the remapped vertices m_remapped_vertices in svt_vertex_array!!!

    if (z != z_old) // next layer in volume dataset
    {
        layer = (layer+1)%2; // layer toggling between 0 and 1
        // init switched slice
        voxel = 4*m_iMcXdim*m_iMcYdim;
        for (i=0; i<voxel; i++)
            m_pMcTriMat[i+layer*voxel] = -1;
    }
    z_old = z;

    voxel = 4*(layer*m_iMcXdim*m_iMcYdim + m_iMcXdim*y + x);
    // check for already existing vertex in same triangle
    // (very small triangles will be eliminated,
    //   no hole results, because adjacent triangles will also have the same vertex)
    for (i=1; i<3; i++)
        for (j=0; j<i; j++)
            if ((fabs(oTriangle.m_fP[j][0]-oTriangle.m_fP[i][0]) < EQ_EPS) && 
                (fabs(oTriangle.m_fP[j][1]-oTriangle.m_fP[i][1]) < EQ_EPS) && 
                (fabs(oTriangle.m_fP[j][2]-oTriangle.m_fP[i][2]) < EQ_EPS)) // maximum norm
            {
                m_pMcTriMat[voxel+nr] = -2;
                return;
            }

    voxelB = voxel - 4; //4*(layer*m_iMcXdim*m_iMcYdim + m_iMcXdim*y + x - 1);
    voxelL = voxel - 4*m_iMcXdim; //4*(layer*m_iMcXdim*m_iMcYdim + m_iMcXdim*(y-1) + x);
    voxelT = 4*(((layer+1)%2)*m_iMcXdim*m_iMcYdim + m_iMcXdim*y + x);
    voxelU = voxelT - 4*m_iMcXdim; //4*(((layer+1)%2)*m_iMcXdim*m_iMcYdim + m_iMcXdim*(y-1) + x);
    voxelP = voxelT + 4*m_iMcXdim; //4*(((layer+1)%2)*m_iMcXdim*m_iMcYdim + m_iMcXdim*(y+1) + x);
    for (i=0; i<3; i++) // for every vertex of new triangle
    {
        sameVer = -1;

        // check for already existing vertex in same voxel 
        if (sameVer<0)
            sameVer = checkVoxelForVertex(voxel, oTriangle.m_fP[i]);
        // check for already existing vertex in adjacent voxels
        if ((sameVer<0) && (x>0))
            sameVer = checkVoxelForVertex(voxelB, oTriangle.m_fP[i]); // voxel just before 
        if ((sameVer<0) && (y>0))
            sameVer = checkVoxelForVertex(voxelL, oTriangle.m_fP[i]); // voxel one row next to it in same layer 
        if (sameVer<0)
            sameVer = checkVoxelForVertex(voxelT, oTriangle.m_fP[i]); // voxel on top in layer above 
        // following tests have seldom a positive result
        if ((sameVer<0) && (x>0) && (y>0))
            sameVer = checkVoxelForVertex(voxelL-4, oTriangle.m_fP[i]);
        if ((sameVer<0) && (y>0) && (x<(m_iMcXdim-1)))
            sameVer = checkVoxelForVertex(voxelL+4, oTriangle.m_fP[i]);
        if ((sameVer<0) && (x>0))
            sameVer = checkVoxelForVertex(voxelT-4, oTriangle.m_fP[i]);
        if ((sameVer<0) && (x<(m_iMcXdim-1)))
            sameVer = checkVoxelForVertex(voxelT+4, oTriangle.m_fP[i]);
        if ((sameVer<0) && (y>0))
            sameVer = checkVoxelForVertex(voxelU, oTriangle.m_fP[i]);
// 11, 12, 15 not needed (checked by evaluation)
//         if ((sameVer<0) && (x>0) && (y<(m_iMcYdim-1)))
//         {
//             sameVer = checkVoxelForVertex(voxelU+4, oTriangle.m_fP[i]);
//             if (sameVer != -1) count11++;
//         }
//         if ((sameVer<0) && (x>0) && (y>0))
//         {
//             sameVer = checkVoxelForVertex(voxelU-4, oTriangle.m_fP[i]);
//             if (sameVer != -1) count12++;
//         }
        if ((sameVer<0) && (y<(m_iMcYdim-1)))
            sameVer = checkVoxelForVertex(voxelP, oTriangle.m_fP[i]);
        if ((sameVer<0) && (x<(m_iMcXdim-1)) && (y<(m_iMcYdim-1)))
            sameVer = checkVoxelForVertex(voxelP+4, oTriangle.m_fP[i]);
//         if ((sameVer<0) && (x<(m_iMcXdim-1)) && (y>0))
//         {
//             sameVer = checkVoxelForVertex(voxelP-4, oTriangle.m_fP[i]);
//             if (sameVer != -1) count15++;
//         }
        if (sameVer != -1)
        {
            triangle.ver[i] = sameVer;
// not needed, because all data from vertex will be the same: 
//         int triSize = m_oVertices[sameVer](*this)[indexMin].tri.size();
//         m_remapped_vertices++;
//         // averaging normal, texture and color
//         for (i=0; i<3; i++)
//             m_oVertices[sameVer].norm[i] = (triSize * m_oVertices[sameVer].norm[i] + oNormals.m_fP[i]) / (triSize + 1);
//         for (i=0; i<2; i++)
//             m_oVertices[sameVer].tex[i] = (triSize * m_oVertices[sameVer].tex[i] + oTexTriangle.m_fP[i]) / (triSize + 1);
//         m_oVertices[sameVer].col.set( //default: col = (1.,1.,1.)
//                (triSize * m_oVertices[sameVer].col.getR() + col.getR()) / (triSize + 1),
//                (triSize * m_oVertices[sameVer].col.getG() + col.getG()) / (triSize + 1),
//                (triSize * m_oVertices[sameVer].col.getB() + col.getB()) / (triSize + 1));
        }
        else
        {
            TexTriangle oTexTriangle; // just here because of performance

            for (j=0; j<3; j++)
                for (k=0; k<2; k++)
                    oTexTriangle.m_fP[j][k] = 0.;

            triangle.ver[i] = addVertex(oTriangle.m_fP[i], oNormals.m_fP[i], oTexTriangle.m_fP[i]);
        }

        // hierzu muss vorher der knotenvektor eingelesen worden sein, was durch die zeile darueber gewaehrleistet ist
        m_oVertices[triangle.ver[i]].tri.push_front(m_triNumber); 
    }

    m_oTriangles.push_back(triangle);

    //m_oTriangles[m_triNumber].norm = oNormals.m_fP[0]; // just the first normal of vertex (faster)
    calcTriNormal(m_triNumber); // normal for triangle, only the vertex normals are included in oNormals
    m_pMcTriMat[voxel+nr] = m_triNumber;

    m_triNumber++;
}

/**
 * add a triangle to the mesh
 * \param oTriangle new triangle object
 * \param oNormals normal vectors for the new triangle
 */
void svt_mesh::addTriangle(const Triangle &oTriangle, const Triangle &oNormals)
{
    int i, j;
    TexTriangle oTexTriangle;
    for (i=0; i<3; i++)
        for (j=0; j<2; j++)
            oTexTriangle.m_fP[i][j] = 0.;
    addTriangle(oTriangle, oNormals, oTexTriangle);
}

/**
 * add a triangle with its vertices to the mesh
 * \param oTriangle new Triangle object
 */
void svt_mesh::addTriangle(const Triangle &oTriangle)
{
    addTriangle(oTriangle, calcTriNormal(oTriangle));
}

/**
 * add a triangle with its vertices to the mesh
 * \param oTriangle new Triangle object
 * \param oTexTriangle texture coordinates for the triangle
 */
void svt_mesh::addTriangle(const Triangle &oTriangle, const TexTriangle &oTexTriangle)
{
    addTriangle(oTriangle, calcTriNormal(oTriangle), oTexTriangle);
}

/**
 * add a triangle with its vertices to the mesh
 * \param oTriangle new Triangle object
 * \param oNormals normal vectors for the new triangle
 * \param oTexTriangle texture coordinates for the triangle
 */
void svt_mesh::addTriangle(const Triangle &oTriangle, const Triangle &oNormals, const TexTriangle &oTexTriangle)
{
    svt_triangle_type triangle;
    int i;

    for (i=0; i<3; i++)
    {
        triangle.ver[i] = addVertex(oTriangle.m_fP[i], oNormals.m_fP[i], oTexTriangle.m_fP[i]);
        m_oVertices[triangle.ver[i]].tri.push_front(m_triNumber);
        // hierzu muss vorher der knotenvektor eingelesen worden sein, was durch die zeile darueber gewaehrleistet ist
    }

    m_oTriangles.push_back(triangle);

    //m_oTriangles[m_triNumber].norm = oNormals.m_fP[0]; // just the first normal of vertex (faster)
    calcTriNormal(m_triNumber); // normal for triangle, only the vertex normals are included in oNormals

    m_triNumber++;
}

/**
 * add a triangle to the mesh
 * \param ver pointer to three vertex numbers
 * \return true if triangle could added
 */
bool svt_mesh::addTriangle(UInt32 *ver)
{
    svt_triangle_type triangle;
    int i;

    for (i=0; i<3; i++)
    {
        if (ver[i] >= getVerNumber())
            return false;
        triangle.ver[i] = ver[i]; 
        m_oVertices[triangle.ver[i]].tri.push_front(m_oTriangles.size()); // hierzu muss vorher der knotenvektor eingelesen worden sein, was durch obiges if gewaehrleistet ist!
    }

    m_triNumber++;

    m_oTriangles.push_back(triangle);
    return true;
}

/**
 * activate a triangle
 * \param i activate triangle number i for visualization
 */
void svt_mesh::addTriangle(int i)
{
    m_oTriangles[i].in = true;
}

/**
 * deactivate a triangle
 * \param i deactivate triangle number i for visualization
 */
void svt_mesh::delTriangle(int i)
{
    m_oTriangles[i].in = false;
    m_triNumber--;
}

/**
 * reserve vertices in vertex vector for performace enhancement
 * \param n number of vertices in vertex vector
 */  
void svt_mesh::reserveVertex(int n) { m_oVertices.reserve(n); }
/**
 * reserve triangles in triangle vector for performace enhancement
 * \param n number of triangles in triangle vector
 */  
void svt_mesh::reserveTriangle(int n) { m_oTriangles.reserve(n); }

/**
 * resize vertex vector 
 * \param n new number of vertices in vertex vector
 */  
void svt_mesh::resizeVertex(int n) { m_oVertices.resize(n); }
/**
 * resize triangle vector 
 * \param n new number of triangles in triangle vector
 */  
void svt_mesh::resizeTriangle(int n) { m_oTriangles.resize(n); }

/**
 * add a vertex to the mesh
 * \param pkt vertex vector to be inserted
 * \return number of vertex in the mesh
 */ 
int svt_mesh::addVertex(const svt_vector3<double> &pkt) { return m_oVertices.addVertex(pkt); }
/**
 * add a vertex to the mesh
 * \param pkt vertex vector to be inserted
 * \param col color for vertex
 * \return number of vertex in the mesh
 */ 
int svt_mesh::addVertex(const svt_vector3<double> &pkt, const svt_color &col) { return m_oVertices.addVertex(pkt, col); }
/**
 * add a vertex to the mesh
 * \param pkt vertex vector to be inserted
 * \param norm normal vector for vertex
 * \param tex texture coordinate for vertex
 * \return number of vertex in the mesh
 */ 
int svt_mesh::addVertex(const svt_vector3<double> &pkt, const svt_vector3<double> &norm, const Point2f &tex) { return m_oVertices.addVertex(pkt, norm, tex); }
/**
 * add a vertex to the mesh
 * \param pkt vertex point to be inserted
 * \return number of vertex in the mesh
 */ 
int svt_mesh::addVertex(const Point3f &pkt) { return m_oVertices.addVertex((const svt_vector3<double>) svt_vector3<double>(pkt)); }
/**
 * add a vertex to the mesh
 * \param pkt vertex point to be inserted
 * \param norm normal vector for vertex
 * \param tex texture coordinate for vertex
 * \return number of vertex in the mesh
 */ 
int svt_mesh::addVertex(const Point3f &pkt, const Point3f &norm, const Point2f &tex) { return m_oVertices.addVertex((const svt_vector3<double>) svt_vector3<double>(pkt), (const svt_vector3<double>) svt_vector3<double>(norm), tex); }

/**
 * get flag for searching for identical vertex in mesh for inserting a vertex
 * \return if the comparison should done
 */ 
bool svt_mesh::getCompareFlag() { return m_oVertices.getCompareFlag(); }
/**
 * set flag for searching for identical vertex in mesh for inserting a vertex
 * \param b set if the comparison should done
 */ 
void svt_mesh::setCompareFlag(bool b) { m_oVertices.setCompareFlag(b); }

/**
 * get number of remapped vertices
 * \return number of remapped vertices
 */ 
int svt_mesh::getRemappedVertices() { return m_oVertices.getRemappedVertices(); }

/**
 * calculate the minimum and maximum expansion of the mesh and set internal variables
 */
void svt_mesh::calcMinMax()
{ 
    vector<bool> visit(m_oVertices.size(), false);
    int nr, j;
    unsigned int i;
    // maximale X Y Z der Knoten
    m_fMinX = FLT_MAX; m_fMinY = FLT_MAX; m_fMinZ = FLT_MAX;
    // minimale X Y Z der Knoten
    m_fMaxX = -FLT_MAX; m_fMaxY = -FLT_MAX; m_fMaxZ = -FLT_MAX;

    for (i=0; i<m_oTriangles.size(); i++)
        if (m_oTriangles[i].in)
            for (j=0; j<3; j++)
                if (!visit[m_oTriangles[i].ver[j]])
                {
                    nr = m_oTriangles[i].ver[j];

                    if (m_oVertices[nr].pkt[0] < m_fMinX)
                        m_fMinX = m_oVertices[nr].pkt[0];
                    else if (m_oVertices[nr].pkt[0] > m_fMaxX)
                        m_fMaxX = m_oVertices[nr].pkt[0];
                    if (m_oVertices[nr].pkt[1] < m_fMinY)
                        m_fMinY = m_oVertices[nr].pkt[1];
                    else if (m_oVertices[nr].pkt[1] > m_fMaxY)
                        m_fMaxY = m_oVertices[nr].pkt[1];
                    if (m_oVertices[nr].pkt[2] < m_fMinZ)
                        m_fMinZ = m_oVertices[nr].pkt[2];
                    else if (m_oVertices[nr].pkt[2] > m_fMaxZ)
                        m_fMaxZ = m_oVertices[nr].pkt[2];
                }
}
 
/**
 * calculate the minimum and maximum expansion of all vertices in the mesh and set internal variables (also vertices not in the detail level)
 */ 
void svt_mesh::calcMinMaxAll() { m_oVertices.calcMinMax(); }

/**
 * get size of the mesh in x direction
 * \return mesh size in x direction
 */ 
float svt_mesh::getSizeX() { return getMaxX()-getMinX(); }
/**
 * get size of the mesh in y direction
 * \return mesh size in y direction
 */ 
float svt_mesh::getSizeY() { return getMaxY()-getMinY(); }
/**
 * get size of the mesh in z direction
 * \return mesh size in z direction
 */ 
float svt_mesh::getSizeZ() { return getMaxZ()-getMinZ(); }

/**
 * get minimum of the mesh in x direction
 * \return mesh minimum in x direction
 */ 
float svt_mesh::getMinX() { return m_fMinX; }
/**
 * get minimum of the mesh in y direction
 * \return mesh minimum in y direction
 */ 
float svt_mesh::getMinY() { return m_fMinY; }
/**
 * get minimum of the mesh in z direction
 * \return mesh minimum in z direction
 */ 
float svt_mesh::getMinZ() { return m_fMinZ; }
/**
 * get maximum of the mesh in x direction
 * \return mesh maximum in x direction
 */ 
float svt_mesh::getMaxX() { return m_fMaxX; }
/**
 * get maximum of the mesh in y direction
 * \return mesh maximum in y direction
 */ 
float svt_mesh::getMaxY() { return m_fMaxY; }
/**
 * get maximum of the mesh in z direction
 * \return mesh maximum in z direction
 */ 
float svt_mesh::getMaxZ() { return m_fMaxZ; }

/**
 * set minimum of the mesh in x direction
 * \param t minimum of the mesh in x direction
 */ 
void svt_mesh::setMinX(float t) { m_fMinX = t; }
/**
 * set minimum of the mesh in y direction
 * \param t minimum of the mesh in y direction
 */ 
void svt_mesh::setMinY(float t) { m_fMinY = t; }
/**
 * set minimum of the mesh in z direction
 * \param t minimum of the mesh in z direction
 */ 
void svt_mesh::setMinZ(float t) { m_fMinZ = t; }
/**
 * set maximum of the mesh in x direction
 * \param t maximum of the mesh in x direction
 */ 
void svt_mesh::setMaxX(float t) { m_fMaxX = t; }
/**
 * set maximum of the mesh in y direction
 * \param t maximum of the mesh in y direction
 */ 
void svt_mesh::setMaxY(float t) { m_fMaxY = t; }
/**
 * set maximum of the mesh in y direction
 * \param t maximum of the mesh in y direction
 */ 
void svt_mesh::setMaxZ(float t) { m_fMaxZ = t; }

/**
 * get the expansion of all vertices in x direction (also vertices not in the detail level)
 * \return expansion of all vertices in x direction
 */ 
float svt_mesh::getSizeXAll() { return m_oVertices.getMaxX()-m_oVertices.getMinX(); }
/**
 * get the expansion of all vertices in y direction (also vertices not in the detail level)
 * \return expansion of all vertices in y direction
 */ 
float svt_mesh::getSizeYAll() { return m_oVertices.getMaxY()-m_oVertices.getMinY(); }
/**
 * get the expansion of all vertices in z direction (also vertices not in the detail level)
 * \return expansion of all vertices in z direction
 */ 
float svt_mesh::getSizeZAll() { return m_oVertices.getMaxZ()-m_oVertices.getMinZ(); }

/**
 * get minimum of all vertices in x direction (also vertices not in the detail level)
 * \return minimum of all vertices in x direction
 */ 
float svt_mesh::getMinXAll() { return m_oVertices.getMinX(); }
/**
 * get minimum of all vertices in y direction (also vertices not in the detail level)
 * \return minimum of all vertices in y direction
 */ 
float svt_mesh::getMinYAll() { return m_oVertices.getMinY(); }
/**
 * get minimum of all vertices in z direction (also vertices not in the detail level)
 * \return minimum of all vertices in z direction
 */ 
float svt_mesh::getMinZAll() { return m_oVertices.getMinZ(); }
/**
 * get maximum of all vertices in x direction (also vertices not in the detail level)
 * \return maximum of all vertices in x direction
 */ 
float svt_mesh::getMaxXAll() { return m_oVertices.getMaxX(); }
/**
 * get maximum of all vertices in y direction (also vertices not in the detail level)
 * \return maximum of all vertices in y direction
 */ 
float svt_mesh::getMaxYAll() { return m_oVertices.getMaxY(); }
/**
 * get maximum of all vertices in z direction (also vertices not in the detail level)
 * \return maximum of all vertices in z direction
 */ 
float svt_mesh::getMaxZAll() { return m_oVertices.getMaxZ(); }

/**
 * set miminum of all vertices in x direction (also vertices not in the detail level)
 * \param miminum of all vertices in x direction
 */ 
void svt_mesh::setMinXAll(float t) { m_oVertices.setMinX(t); }
/**
 * set miminum of all vertices in y direction (also vertices not in the detail level)
 * \param miminum of all vertices in y direction
 */ 
void svt_mesh::setMinYAll(float t) { m_oVertices.setMinY(t); }
/**
 * set miminum of all vertices in z direction (also vertices not in the detail level)
 * \param miminum of all vertices in z direction
 */ 
void svt_mesh::setMinZAll(float t) { m_oVertices.setMinZ(t); }
/**
 * set maximum of all vertices in x direction (also vertices not in the detail level)
 * \param maximum of all vertices in x direction
 */ 
void svt_mesh::setMaxXAll(float t) { m_oVertices.setMaxX(t); }
/**
 * set maximum of all vertices in y direction (also vertices not in the detail level)
 * \param maximum of all vertices in y direction
 */ 
void svt_mesh::setMaxYAll(float t) { m_oVertices.setMaxY(t); }
/**
 * set maximum of all vertices in z direction (also vertices not in the detail level)
 * \param maximum of all vertices in z direction
 */ 
void svt_mesh::setMaxZAll(float t) { m_oVertices.setMaxZ(t); }

/**
 * get the number of triangles in the mesh; must not be equivalent to .size()
 * \return number of triangles in the mesh
 */
unsigned int svt_mesh::getNumber() { return getTriNumber(); }

/**
 * get the number of triangles in the mesh; must not be equivalent to .size()
 * \return number of triangles in the mesh
 */
unsigned int svt_mesh::getTriNumber() { return m_triNumber; }

/**
 * get the number of triangles in the original mesh
 * \return number of triangles in the original mesh
 */
unsigned int svt_mesh::getTriOrigNumber() { return m_oTriangles.size(); }

/**
 * set the number of triangles in the mesh
 * \param i number of triangles in the mesh
 */
void svt_mesh::setTriNumber(unsigned int i) { m_triNumber = i; }

/**
 * get the number of vertices in the mesh; must not be equivalent to .size()
 * \return number of vertices in the mesh
 */
unsigned int svt_mesh::getVerNumber() { return m_oVertices.getNumber(); }

/**
 * set the number of vertices in the mesh
 * \param i number of vertices in the mesh
 */
void svt_mesh::setVerNumber(unsigned int i) { m_oVertices.setNumber(i); }

/**
 * get the number of vertices in the original mesh
 * \return number of vertices in the original mesh
 */
unsigned int svt_mesh::getVerOrigNumber() { return m_oVertices.size(); }

/**
 * get the pointer to the vertex array
 * \return pointer to the vertex array
 */
svt_vertex_array * svt_mesh::getVertexArray() { return &m_oVertices; }

/**
 * get the pointer to the triangle array
 * \return pointer to the triangle array
 */
svt_triangle_array * svt_mesh::getTriangleArray() { return &m_oTriangles; }

/**
 * delete all triangles in the mesh, and therefor all vertices and the progressive mesh
 */
void svt_mesh::deleteTriangles()
{
    m_simpOptionsChanged = true;
    setTriNumber(0);
    m_oTriangles.clear();
    // auch abhaengige knoten loeschen
    m_oVertices.deleteAll();
    // auch datenstruktur zur vereinfachung freigeben; wird direkt nach vereinfachung gemacht
    m_pm.clearAll();

    m_bVAR_Generated = false;
    //rebuildDL();
}

/**
 * delete all triangles in the mesh, and therefor all vertices and the progressive mesh
 */ 
void svt_mesh::deleteMesh()
{
    deleteTriangles();
}

/**
 * draw the mesh
 */
void svt_mesh::drawGL()
{
    unsigned int i;
    int j;

    // sollte die hinterseite die gleiche fabe haben?, dann schwerer zu unterscheiden
    if (m_lightTwoSide)
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    else
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    if (m_culling)
        glEnable( GL_CULL_FACE );

    // the color of the isosurface may be transparent, so copy four color values (RGBA)
    svt_vector4< Real32 > oV;
    memcpy(oV.c_data(), (getProperties()->getColor())->getDiffuse(), 16);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, oV.c_data());

    // increase ambient to highlight ambient occlusion effect
    oV = oV*0.75f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, oV.c_data());


    /*

    //
    // Old direct output routine
    //
    if (m_switchNormals)
    {
	// Dreiecke zeichnen
	glBegin(GL_TRIANGLES);

	for (i=0; i<m_oTriangles.size(); i++)
        {
            if (m_oTriangles[i].in)
            {
                for (j=0; j<3; (j==0)?(j=2):((j==2)?(j=1):((j==1)?(j=3):(j)))) // switch 1 and 2
                {
                    if (getTexture() != NULL)
                        glTexCoord2f(m_oVertices[m_oTriangles[i].ver[j]].tex[0], m_oVertices[m_oTriangles[i].ver[j]].tex[1]);
                    if (!getProperties()->getSmooth())
                        glNormal3dv((-m_oTriangles[i].norm).cast()); 
                    else
                        glNormal3dv((-m_oVertices[m_oTriangles[i].ver[j]].norm).cast()); 
                    if (getVertexColored())
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, m_oVertices[m_oTriangles[i].ver[j]].col.getDiffuse());
                    
                    glVertex3dv(m_oVertices[m_oTriangles[i].ver[j]].pkt.cast());
                }
            }
	}

        glEnd();
    }
    else
    {
	// Dreiecke zeichnen
	glBegin(GL_TRIANGLES);

        for (i=0; i<m_oTriangles.size(); i++)
        {
            if (m_oTriangles[i].in)
	    {
                for (j=0; j<3; j++)
                {
                    if (getTexture() != NULL)
                        glTexCoord2f(m_oVertices[m_oTriangles[i].ver[j]].tex[0], m_oVertices[m_oTriangles[i].ver[j]].tex[1]);
                    if (!getProperties()->getSmooth())
			glNormal3dv(m_oTriangles[i].norm.cast());
                    else
                        glNormal3dv(m_oVertices[m_oTriangles[i].ver[j]].norm.cast());
                    if (getVertexColored())
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, m_oVertices[m_oTriangles[i].ver[j]].col.getDiffuse());

		    glVertex3dv(m_oVertices[m_oTriangles[i].ver[j]].pkt.cast());
                }
            }
	}

	glEnd();
    }
*/
    //
    // new vertex object output routine
    //
    if (!m_bVAR_Generated)
    {
        m_aVAR_Verts.clear();
        m_aVAR_Faces.clear();

	svt_n3f_v3f oVec;

	int* verTab = new int[getVerOrigNumber()];
	int realNr;
	int nr;

	realNr = calcVerTab(verTab);

	// calc vertex array
	nr = -1;
	for (i=0; i<m_oTriangles.size(); i++)
	    if (m_oTriangles[i].in)
		for (j=0; j<3; j++)
		    if (verTab[m_oTriangles[i].ver[j]] > nr)
		    {
			oVec.m_oN[0] = m_oVertices[m_oTriangles[i].ver[j]].norm.x();
			oVec.m_oN[1] = m_oVertices[m_oTriangles[i].ver[j]].norm.y();
			oVec.m_oN[2] = m_oVertices[m_oTriangles[i].ver[j]].norm.z();

			oVec.m_oV[0] = m_oVertices[m_oTriangles[i].ver[j]].pkt.x();
			oVec.m_oV[1] = m_oVertices[m_oTriangles[i].ver[j]].pkt.y();
			oVec.m_oV[2] = m_oVertices[m_oTriangles[i].ver[j]].pkt.z();

			m_aVAR_Verts.push_back( oVec );

			nr++;
		    }

	// output face indices
	for (i=0; i<m_oTriangles.size(); i++)
	{
            if (m_oTriangles[i].in)
            {
                m_aVAR_Faces.push_back( verTab[m_oTriangles[i].ver[0]] );
                m_aVAR_Faces.push_back( verTab[m_oTriangles[i].ver[1]] );
                m_aVAR_Faces.push_back( verTab[m_oTriangles[i].ver[2]] );
            }
	}

	delete [] verTab;

        m_bVAR_Generated = true;
    }

    // now draw the var
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glVertexPointer( 3, GL_FLOAT, sizeof( svt_n3f_v3f ), (char*)(&m_aVAR_Verts[0]) + (sizeof(Real32)*3));
    glNormalPointer( GL_FLOAT, sizeof( svt_n3f_v3f ), (char*)(&m_aVAR_Verts[0]));

    glDrawElements(GL_TRIANGLES, m_aVAR_Faces.size(), GL_UNSIGNED_INT, &m_aVAR_Faces[0] );
    //glDrawRangeElements(GL_TRIANGLES, 0, m_aVAR_Faces.size(), m_aVAR_Faces.size(), GL_UNSIGNED_INT, (char*)(&m_aVAR_Faces[0]) );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );

    if (m_culling)
        glDisable( GL_CULL_FACE );
}

/**
 * normalize mesh to expansion of -1..1 in maximal direction
 * \return scaling factor
 */
double svt_mesh::normalizeData()
{
	double scale, xshift, yshift, zshift;
	unsigned int i;

	scale = svt_max(svt_max((getMaxX()-getMinX()),(getMaxY()-getMinY())), (getMaxZ()-getMinZ()))/2.;
	xshift = (getMaxX()+getMinX())/2.;
	yshift = (getMaxY()+getMinY())/2.;
	zshift = (getMaxZ()+getMinZ())/2.;
  
	for (i=0; i < m_oVertices.size(); i++)
	{
		m_oVertices[i].pkt[0] = (m_oVertices[i].pkt[0]-xshift)/scale;
		m_oVertices[i].pkt[1] = (m_oVertices[i].pkt[1]-yshift)/scale;
		m_oVertices[i].pkt[2] = (m_oVertices[i].pkt[2]-zshift)/scale;
	}

	setMinX((getMinX()-xshift)/scale);
	setMinY((getMinY()-yshift)/scale);
	setMinZ((getMinZ()-zshift)/scale);
	setMaxX((getMaxX()-xshift)/scale);
	setMaxY((getMaxY()-yshift)/scale);
	setMaxZ((getMaxZ()-zshift)/scale);

	setMinXAll((getMinXAll()-xshift)/scale);
	setMinYAll((getMinYAll()-yshift)/scale);
	setMinZAll((getMinZAll()-zshift)/scale);
	setMaxXAll((getMaxXAll()-xshift)/scale);
	setMaxYAll((getMaxYAll()-yshift)/scale);
	setMaxZAll((getMaxZAll()-zshift)/scale);

    return scale;
}

/**
 * calculate all triangles for each vertex which contain the triangle
 */
void svt_mesh::calcAllVerInTri()
{
    unsigned int i;
    int j;

    for (i = 0; i < m_oVertices.size(); i++)
        m_oVertices[i].tri.clear();
    for (i = 0; i < m_oTriangles.size(); i++)
        for (j = 0; j < 3; j++)
            m_oVertices[m_oTriangles[i].ver[j]].tri.push_front(i);
}

/**
 * visit all triangles which can be reached from a triangle
 * \param t triangle number from which all neighbour triangles will be visited
 * \param visit vector who shows which triangles already have been visited
 * \return number of triangles in component
 */
int svt_mesh::compTest(int t, vector<bool> &visit)
{
	int i;
	stack<int> triToVisit;
    int compTriNumber = 0;
	int tt;

	triToVisit.push(t);
    visit[t] = true;

	while (!triToVisit.empty())
    {
        tt = triToVisit.top();
        triToVisit.pop();
        compTriNumber++;
		for (i = 0; i < 3; i++)
			for (list<int>::iterator it = m_oVertices[m_oTriangles[tt].ver[i]].tri.begin(); 
				 it != m_oVertices[m_oTriangles[tt].ver[i]].tri.end(); 
				 ++it)
				if (visit[*it] == false)
                {
					triToVisit.push(*it);
                    visit[*it] = true;
                }
    }
    return compTriNumber;
}

/**
 * calculate the number of mesh components
 * \param verb verbose mode: print out number of triangles in every component
 * \return number of components
 */
int svt_mesh::calcCompNumber(bool verb) 
{
    bool def = false;
    vector<bool> visit(m_oTriangles.size(), def);
    unsigned int i;
    int triNum;

    m_compNumber=0;
    for (i=0; i < m_oTriangles.size(); i++)
        if (m_oTriangles[i].in && !visit[i]) 
        {
    	    m_compNumber++;
            triNum = compTest(i, visit);
            if (verb)
                cout << "  Number of triangles in component " << m_compNumber << ": " << triNum << endl;
        }

    return m_compNumber;
}

/**
 * disable mesh components with triangles less than triAnz
 * \param triAnz number of triangles up to which the components will be deleted
 * \return number of disabled components
 */
int svt_mesh::disableComp(int triAnz) 
{
    bool def = false;
    vector<bool> visit(m_oTriangles.size(), def);
    vector<bool> disable(m_oTriangles.size(), def);
    unsigned int i, j;
    int disableNumber=0;

    for (i=0; i < m_oTriangles.size(); i++)
        if (m_oTriangles[i].in && !visit[i]) 
        {
    	    if  (compTest(i, visit) <= triAnz)
            {
                disableNumber++;
                for (j=0; j < m_oTriangles.size(); j++)
                    if (visit[j] != disable[j])
                    {
                        delTriangle(j);
                        disable[j] = true;
                    }
            }
            else
                for (j=0; j < m_oTriangles.size(); j++)
                    if (visit[j] != disable[j])
                        disable[j] = true;
        }

    return disableNumber;
}

/**
 * get the number of mesh components
 * \return number of components
 */ 
int svt_mesh::getCompNumber() { return m_compNumber; }

/**
 * searches for triangle in mesh which lies in outer envelope and near a plane in mesh
 * \param triangle
 * \return triangle index
 */ 
int svt_mesh::searchSnuggleTriangle(svt_vector3<double> oTriangle[3])
{
    int bestTri = -1;
    unsigned outerNum, innerNum; 
    int diffMax = 0;
    unsigned i, j, k;
    double epsStep = ((getMaxX() - getMinX())+(getMaxY() - getMinY())+(getMaxZ() - getMinZ()))/100.;
    unsigned int p = 0, pp;
    int jStep = 1;

    printf("svt_mesh> Searching:   0%%"); fflush(stdout);

    if (m_oVertices.size()>10000)
        jStep = m_oVertices.size()/1000;

    for (i=0; i < m_oTriangles.size(); i++)
    {
        if (m_oTriangles[i].in)
        {
            outerNum = innerNum = 0;
            for (j=0; j<m_oVertices.size(); j+=jStep)
            {
                if (((m_oVertices[m_oTriangles[i].ver[0]].pkt - m_oTriangles[i].norm*epsStep)*m_oTriangles[i].norm -
                     m_oVertices[j].pkt*m_oTriangles[i].norm) < 0)
                    outerNum++;
                if (((m_oVertices[m_oTriangles[i].ver[0]].pkt + m_oTriangles[i].norm*epsStep)*m_oTriangles[i].norm -
                     m_oVertices[j].pkt*m_oTriangles[i].norm) < 0)
                    innerNum++;
            }
            if (innerNum < m_oVertices.size()/100) // == 0
            {
                if (diffMax < (int) (outerNum-innerNum))
                {
                    diffMax = outerNum-innerNum;
                    bestTri = i;
                }
            }
        }
        // ein if, da sonst die ausgabe zuviel zeit kostet!
        if ( (pp = ((int) (100.*i/(double)m_oTriangles.size()))) > p)
        {
            p = pp;
            printf("\b\b\b\b%3d%%", p); fflush(stdout);
        }
    }
    for (k=0; k<3; k++)
        oTriangle[k] = m_oVertices[m_oTriangles[bestTri].ver[k]].pkt;

    printf("\b\b\b\b100%%\n");
    return bestTri;
}

/**
 * color codes mesh separated through triangle plane
 * \param oTriangle triangle at which the mesh will be separated by color
 * \param checkNorm if true, then only triangles will be separeted, which have e similar normal than the separating triangle
 * \return percentage of separated mesh
 */ 
double svt_mesh::splitAtTriangle(svt_vector3<double> oTriangle[3], bool checkNorm) // checkNorm = false ist default
{
    svt_color red(1.0,0.0,0.0);
    svt_color green(0.0,1.0,0.0);
    unsigned i, j;
    svt_vector3<double> norm = crossProduct(oTriangle[0] - oTriangle[2], oTriangle[1] - oTriangle[2]).normalize();
    double d = oTriangle[0]*norm;
    int rc = 0, gc = 0;
    double size;
    double rSize = 0., gSize = 0.;

    if (getVertexColored())
        svtout << "WARNING: Mesh is already color coded!" << endl;
    setVertexColored(true);

    for (i=0; i < m_oTriangles.size(); i++)
    {
        if (m_oTriangles[i].in)
        {
            // only add 1/3 of triangle size because every vertex is evaluated
            size = crossProduct(m_oVertices[m_oTriangles[i].ver[1]].pkt - m_oVertices[m_oTriangles[i].ver[0]].pkt, m_oVertices[m_oTriangles[i].ver[2]].pkt - m_oVertices[m_oTriangles[i].ver[0]].pkt).length()/6.;
            for (j=0; j < 3; j++)
            {
                if ((d - m_oVertices[m_oTriangles[i].ver[j]].pkt*norm) < 0)
                {
                    if (checkNorm)
                    {
                        if ((norm*m_oVertices[m_oTriangles[i].ver[j]].norm > 0.9))
                        {
                            m_oVertices[m_oTriangles[i].ver[j]].col = red;
                            rc++;
                            rSize += size;
                        }
                        else 
                        {
                            m_oVertices[m_oTriangles[i].ver[j]].col = green;
                            gc++;
                            gSize += size;
                        }
                    }
                    else
                    {
                            m_oVertices[m_oTriangles[i].ver[j]].col = red;
                            rc++;
                            rSize += size;
                    }
                }
                else
                {
                    m_oVertices[m_oTriangles[i].ver[j]].col = green;
                    gc++;
                    gSize += size;
                }
            }
        }
    }
    cout << "Red   surface size " << rSize << endl;
    cout << "Green surface size " << gSize << endl;
    cout << "Splitting triangle coordinates (" << oTriangle[0][0] << ", " << oTriangle[0][1] << ", " << oTriangle[0][2] << "), ";
    cout <<                                "(" << oTriangle[1][0] << ", " << oTriangle[1][1] << ", " << oTriangle[1][2] << "), ";
    cout <<                                "(" << oTriangle[2][0] << ", " << oTriangle[2][1] << ", " << oTriangle[2][2] << ")" << endl;

    return rc/(double)(rc+gc);
}

/**
 * calculate the number of edges with one, two or more adjacent triangles and save in internal variables
 * \return number of all edges
 */
int svt_mesh::calcEdgeNumber() 
{
    // visit ist 0==false, wenn knoten noch nicht besucht wurde,
    // 1, wenn er besucht wurde
    // temporaer ist jedes vielfache von 2 die anzahl von dreiecken an einer kante
    vector<int> visit(m_oVertices.size(), 0);
    unsigned int i, j, k, tAnz;
    list<int>::iterator it;

    m_edgeNumber1 = 0;
    m_edgeNumber2 = 0;
    m_edgeNumber3 = 0;

    for (i=0; i < m_oVertices.size(); i++)
        if (!visit[i])
        {
            visit[i] = 1;
            // einsammeln der dreiecksanzahl an den kanten
            for (it = m_oVertices[i].tri.begin(); it != m_oVertices[i].tri.end(); ++it)
                if (m_oTriangles[*it].in)
                    for (j = 0; j < 3; j++)
                        if (m_oTriangles[*it].ver[j] == i)
                            for (k = 1; k < 3; k++)
                                if (!(visit[m_oTriangles[*it].ver[(j+k)%3]] % 2))
                                    visit[m_oTriangles[*it].ver[(j+k)%3]] += 2; // fuer jede kante zu diesem punkt 2 addieren
            // zaehlen der dreieckshaeufigkeiten
            for (it = m_oVertices[i].tri.begin(); it != m_oVertices[i].tri.end(); ++it)
                if (m_oTriangles[*it].in)
                    for (j = 0; j < 3; j++)
                        if (m_oTriangles[*it].ver[j] == i)
                            for (k = 1; k < 3; k++)
                                if ((!((tAnz = visit[m_oTriangles[*it].ver[(j+k)%3]]) % 2)) && (tAnz != 0))
                                {
                                    tAnz /= 2;
                                    if (tAnz == 1)
                                        m_edgeNumber1++; // 1 dreieck an kante
                                    else if (tAnz == 2)
                                        m_edgeNumber2++; // 2 dreiecke an kante
                                    else if (tAnz > 2)
                                        m_edgeNumber3++; // mehr als 2 dreiecke an kante
                                    visit[m_oTriangles[*it].ver[(j+k)%3]] = 0;
                                }
        }

    return m_edgeNumber1 + m_edgeNumber2 + m_edgeNumber3;
}

/**
 * get the number of edges with one adjacent triangle
 * \return number of edges with one adjacent triangle
 */ 
int svt_mesh::getEdgeNumber1() { return m_edgeNumber1; }
/**
 * get the number of edges with two adjacent triangles
 * \return number of edges with two adjacent triangles
 */ 
int svt_mesh::getEdgeNumber2() { return m_edgeNumber2; }
/**
 * get the number of edges with more than two adjacent triangles
 * \return number of edges with more than two adjacent triangles
 */ 
int svt_mesh::getEdgeNumber3() { return m_edgeNumber3; }

/**
 * calculate all normals at triangles and vertices
 */
void svt_mesh::calcAllNormals()
{
    unsigned int i;
    int j;
    svt_vector3<double> vc;
     
    for (i = 0; i < m_oTriangles.size(); i++) // Dreiecke
    {
        vc = crossProduct(m_oVertices[m_oTriangles[i].ver[0]].pkt - m_oVertices[m_oTriangles[i].ver[2]].pkt, 
                          m_oVertices[m_oTriangles[i].ver[1]].pkt - m_oVertices[m_oTriangles[i].ver[2]].pkt);
        for (j = 0; j < 3; j++) // Knoten in Dreieck (mgl langsam, da size haeufig in liste aufgerufen wird)
            m_oVertices[m_oTriangles[i].ver[j]].norm += vc;
        m_oTriangles[i].norm = vc;
        m_oTriangles[i].norm.normalize();
    }

    // normalize normal at vertices
    for (i = 0; i < m_oVertices.size(); i++)
    	m_oVertices[i].norm.normalize();

}

/**
 * delete all vertices, which are not used from the triangles
 */
void svt_mesh::delUnusedVertices()
{
    unsigned int i, nr;
    int j;
    svt_vertex_array::iterator it=m_oVertices.begin();
    int delanz = 0;

    while (it!=m_oVertices.end())
        if (it->tri.empty())
        {   
            delanz++;
            nr = it-m_oVertices.begin();
            cout << "svt_mesh> Delete unused vertex " << nr << " (real: " << nr+delanz << ")!" << endl;
            it = m_oVertices.erase(it);
            m_oVertices.setNumber(m_oVertices.getNumber() - 1);
            // knotennummer bei Dreiecken anpassen
            for (i = 0; i < m_oTriangles.size(); i++)
                for (j = 0; j < 3; j++)
                    if (m_oTriangles[i].ver[j] > nr)
                        m_oTriangles[i].ver[j]--;
        } 
        else 
            ++it;
    if (delanz > 0)
	cout << "svt_mesh> " << delanz << " vertices deleted!" << endl;
}

/**
 * simplify the mesh and save the result to a progressive mesh
 * reset the mesh to anz vertices after simplification 
 * or with anz=-1 leaves the mesh completely simplified
 * \param anz number of vertices the mesh should have or with anz=-1 leave the mesh completely simplified
 */
void svt_mesh::simplify(int anz)
{
    svt_edge edge;
    svt_edgeHeapSet::iterator itFront;
    unsigned int p = 0, pp;
    double origVerNr = ((double) getVerOrigNumber())/100.;
    int start, finish;
    double duration = 0;
    unsigned int undo = 0, undoSum = 0, undo2 = 0, undo2Sum = 0;
    double arcDecr = 0;
    int res;

    if (m_pThread != NULL)
    {
        g_oMeshSemaphore.P();
        m_pThread->bValid = false;
        g_oMeshSemaphore.V();
        m_pThread = NULL;
    }

    m_isSimplifiedFlag = false; // ueber isSimplified-Funktion externe zugriffen auf pm verweigern
    m_simpOptionsChanged = false;

    if ((getThreshold()) > 0 && (!getPenalizeBordersFlag()))
        cout  << endl << "svt_mesh> WARNING: Positive threshold without boundary penalty is critical! " << endl << endl;
    resetSimplification();

    start = svt_getElapsedTime();
    initQuadricSimplification();
    finish = svt_getElapsedTime();
    cout << "svt_mesh> Init time: " << (finish - start) / 1000. << "s" << endl;

    printf("svt_mesh> Simplify:   0%%"); fflush(stdout);
    while (!m_edgeHeapSet.empty())
    {
        // kante mit geringsten kosten holen und entfernen
        // danach gibt es noch immer zeiger auf bereits geloeschte kante in vertex array!!!
        edge = m_edgeHeapSet.getFirstEdge(itFront);

        // Progressive mesh erweitern
        addPm(edge);

        // kante zusammenziehen
        start = svt_getElapsedTime();

        if ((res = edgeContract(edge, itFront)) != 0)
        {
            if (res == 1)
            {
                undo++; undoSum++;
                // eine endlosschleife koennte sonst entstehen, wenn am ende nur noch normalenspringende
                // kantenkontrationen im heap sind; dann wird der faktor eigenstaendig heruntergesetzt
                if (undo >100)
                {
                    m_penalizeSwitchArc -= 0.1;
                    arcDecr +=0.1;
                }
            }
            else // res == 2
            {
                undo2++; undo2Sum++;
                // eine endlosschleife entsteht sonst insbesondere zum ende hin, wenn am ende nur noch dreiecke aufeinderfallen
                // kantenkontrationen im heap sind; dann wird die ueberpruefung eigenstaendig ausgeschaltet
                if (undo2 >10)
                    m_penalizeIdentityFlag = false;
            }
            edgeContractUndo(edge, itFront);
        }
        else
        {
            undo = 0;
            undo2 = 0;
        }

        finish = svt_getElapsedTime();
        duration += (finish - start) / 1000.;

        // ein if, da sonst die ausgabe zuviel zeit kostet!
        if ( (pp = ((int) (100. - (double)(getVerNumber()) / origVerNr))) > p)
        {
            p = pp;
            printf("\b\b\b\b%3d%%", p); fflush(stdout);
        }
    }
    printf("\b\b\b\b100%%\n");

    if (m_penalizeSwitchFlag)
        cout << "svt_mesh> Penalized " << undoSum << " edges because of normal switching";
    if (arcDecr > 0)
        cout << "; arc penalty decreased gradually by " << arcDecr << " up to " << m_penalizeSwitchArc ;
    if (m_penalizeSwitchFlag)
        cout << "." << endl;

    if (m_penalizeIdentityFlag || (undo2Sum > 0))
    {
        cout << "svt_mesh> Penalized " << ((undo2Sum > 11)?undo2Sum-11:undo2Sum) << " triangles because of identity." << endl;
        m_penalizeIdentityFlag = true;
    }

    cout << "svt_mesh> Quadrics during simplification: " << m_edgeHeapSet.getSingularQuadricNumber() << " singular, " << m_edgeHeapSet.getRegularQuadricNumber() << " regular" << endl;

    cout << "svt_mesh> Simplification time: " << duration << "s" << endl;

    start = svt_getElapsedTime();
    // ausgabe des rueckbaus, da resetmesh bei grossen netzen einige zeit beansprucht
    cout << "svt_mesh> Simplification reset... ";
    resetAfterSimplification();
//wurde frueher auch hier gemacht, aber bei reinen simplifizierungen grosser netze muss nicht 
//zurueckgebaut werden bevor eine vereinfachte stufe eingestellt wird 
//     // aufbau des originalnetzes / set the mesh to maximal vertex number
//     resetMesh();
    if (anz != -1)
        setMeshToVerNumber(anz); // set number of vertices, but updateSimplify may change it
                                 // use simplifyToVerNumber() to set to a fix vertex number
    cout << "finished!" << endl;
    finish = svt_getElapsedTime();
    cout << "svt_mesh> Reset time: " << (finish - start) / 1000.  << "s" << endl;

    m_isSimplifiedFlag = true; // ueber isSimplified-Funktion externe zugriffen auf pm wieder freigeben

    setRuleVertexNumbers(getVerNumber());

    // create mesh simplification thread
    m_pThread = new simplifyMeshThread;
    m_pThread->pMesh = this;
    m_pThread->bValid = true;
    svt_createThread( svt_simplificationThread, m_pThread );
}
/**
 * check, if the  mesh is already simplified
 * \return if the mesh is already simplified
 */ 
bool svt_mesh::isSimplified() { return (!m_pm.empty() && m_isSimplifiedFlag); }
/**
 * check, if options for simplification would changed after simplification was performed
 * \return if an option has changed
 */ 
bool svt_mesh::simpOptionsChanged() { return m_simpOptionsChanged; }

/**
 * undo the contraction
 * \param edge edge to reinsert in edge heap
 * \param itFront iterator to erased edge if edge heap
 */
void svt_mesh::edgeContractUndo(svt_edge& edge, svt_edgeHeapSet::iterator& itFront)
{
    list<svt_edgeHeapSet::iterator>::iterator it;
    svt_edgeHeapSet::iterator it3;

    // contract rueckgaengig machen
    m_pm.split(*this);
    // pm-stufe aus pm-liste erntfernen
    m_pm.delLast();
    // kanteniterator von beiden punkten der kante entfernen
    it = m_oVertices[edge.v1].edge.begin();
    while (it != m_oVertices[edge.v1].edge.end())
        if ((*it) == itFront) // nicht auf (*it3)->v1 zugr., da speicher freigegeben
            it = m_oVertices[edge.v1].edge.erase(it);
        else
            ++it;
    it = m_oVertices[edge.v2].edge.begin();
    while (it != m_oVertices[edge.v2].edge.end())
        if ((*it) == itFront)
            it = m_oVertices[edge.v2].edge.erase(it);
        else
            ++it;
    // kante bestrafen
    if (edge.cost > 0)
        edge.cost *= m_switchingIdentityPenalizeFactor;
    else
        edge.cost += m_switchingIdentityPenalizeAdd;
    // kante neu in kantenheap einfuegen
    it3 = m_edgeHeapSet.addEdgeSorted(edge);
    // verweis auf die kante in die edge-liste beider knoten einfuegen
    m_oVertices[edge.v1].edge.push_front(it3);
    m_oVertices[edge.v2].edge.push_front(it3);
}

/**
 * update detail level to anz vertices without changing simplification rule
 * \param anz number of vertices the mesh should have
 * \return if the mesh has been changed
 */
// could not be called outside of mesh, because of side effects with thread updatesimplify
bool svt_mesh::setMeshToVerNumber(int anz)
{
    int nr = anz - getVerNumber();
    int nr2 = nr;

    if (nr > 0)
        while ((nr > 0) && m_pm.split(*this))
            nr--;
    else if (nr < 0)
        while ((nr < 0) && m_pm.contract(*this))
            nr++;
    if (nr != nr2)
        return true;
    else
        return false;
}

/**
 * update detail level for achieving boundary condition
 * \return if something was updated
 */
bool svt_mesh::updateSimplify()
{  
    static bool justSimplifying = false;
    static svt_semaphore oSimplifySema;
    bool changed = false;
    float tol_fak = m_fAdaptionTolerance;
    float speed_fak = m_fAdaptionStep;

    oSimplifySema.P();

    if (isSimplified() && !justSimplifying)
    {
        justSimplifying = true;
        if (m_SimplifyRule == 2)
        {
            changed = setMeshToVerNumber(getRuleVertexNumbers());
        } else if (m_SimplifyRule == 0) {
            int i, fps = svt_getFPS();
            float range;
            if (fps < (1 - tol_fak)*m_RuleMinimalFPS)
            {
                range = speed_fak * getVerNumber() * (m_RuleMinimalFPS-fps) /
                    (m_RuleMinimalFPS);
                for (i = 0; i < range; i++)
                    if (m_pm.contract(*this))
                        changed = true;
            }
            else if (fps > (1 + tol_fak)*m_RuleMinimalFPS)
            {
                range = speed_fak * getVerNumber() * (fps-m_RuleMinimalFPS) /
                    (m_RuleMinimalFPS);
                for (i = 0; i < range; i++)
                    if (m_pm.split(*this))
                        changed = true;
            }
        } else if (m_SimplifyRule == 3) {
            int i, fus = 1; // m_pForceTool->getFUS();m_pScene->getFUS(); (aber szene nicht immer verfuegbar!!!) getFUS(); //svt_ // HIER MUSS FUNKTION DIE FUS ZURUECKGIBT STEHEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            float range;
            if (fus != 1)
            {
                if (fus < (1 - tol_fak)*m_RuleMinimalFUS)
                {
                    range = speed_fak * getVerNumber() * (m_RuleMinimalFUS-fus) /
                        (m_RuleMinimalFUS);
                    for (i = 0; i < range; i++)
                        if (m_pm.contract(*this))
                            changed = true;
                }
                else if (fus > (1 + tol_fak)*m_RuleMinimalFUS)
                {
                    range = speed_fak * getVerNumber() * (fus-m_RuleMinimalFUS) /
                        m_RuleMinimalFUS;
                    for (i = 0; i < range; i++)
                        if (m_pm.split(*this))
                            changed = true;
                }
            }
        }
        justSimplifying = false;
    }

    oSimplifySema.V();

    return changed;
}

/**
 * clear temporary data for simplification
 */
void svt_mesh::resetAfterSimplification()
{
    // Quadrikenzaehler zuruecksetzen
    m_edgeHeapSet.setRegularQuadricNumber(0);
    m_edgeHeapSet.setSingularQuadricNumber(0);
    // edgeheap wegschmeissen
    m_edgeHeapSet.clear();
    // kantenliste und quadric in knoten wegschmeissen bzw auf 0 setzen
    for (unsigned int i=0; i<m_oVertices.size(); i++)
    {
        m_oVertices[i].edge.clear();
        m_oVertices[i].quadric.clear();
    }
}

/**
 * set the mesh to maximal vertex number and clear progressive mesh
 */
void svt_mesh::resetSimplification()
{
    // progressive mesh bis zum originalmesh verfeinern
    resetMesh();
    // pm loeschen
    m_pm.clearAll();
}

/**
 * save the progressive mesh to file
 * \param saveFile filename to save progressive mesh to
 * \return if saving was successful
 */ 
bool svt_mesh::savePm(const char *saveFile) { return m_pm.save(*this, saveFile); }
/**
 * read a progressive mesh from file
 * \param readFile filename to read progressive mesh from
 * \return if reading was successful
 */ 
bool svt_mesh::readPm(const char *readFile)
{
    if (!m_pm.read(*this, readFile))
    {
        fprintf(stderr, "svt_mesh> Error: File %s could not be opened.\n", readFile);
        return false;
    }
    else
        return true; 
}

/**
 * set the mesh to maximal vertex number
 */
void svt_mesh::resetMesh()
{
    while (m_pm.split(*this))
        ;
}

/**
 * add a detail level to progressive mesh
 * \param edge edge to be inserted because of contraction
 */
void svt_mesh::addPm(svt_edge& edge)
{
    svt_pm_elem pm; // fuer pm aufbau, wird am ende an m_pm angehaengt
    svt_pm_tri t; // dreieck fuer pm aufbau
    list<int>::iterator it, it2; 
    int i;

    pm.v.v = edge.v1;
    pm.v = m_oVertices[edge.v1];

    pm.v_merge = edge.pkt;

    pm.v_new.v = edge.v2;
    pm.v_new = m_oVertices[edge.v2];

    // dreiecke bei denen v1 zu v2 bzw v zu v_new wird
    for(it = m_oVertices[edge.v2].tri.begin(); it != m_oVertices[edge.v2].tri.end(); ++it)
    {
        pm.t_switch.push_back(*it);
    }

    // neu entstehende dreiecke suchen, also die beide knoten enthalten
    for(it = m_oVertices[edge.v2].tri.begin(); it != m_oVertices[edge.v2].tri.end(); ++it)
        for(it2 = m_oVertices[edge.v1].tri.begin(); it2 != m_oVertices[edge.v1].tri.end(); ++it2)
            if (*it == *it2)
            {
                t.t = (*it);
                for (i=0; i<3; i++)
                    t.ver[i] = m_oTriangles[*it].ver[i];

                pm.t_switch.remove(*it);
                pm.t_new.push_back(t);
            }

    m_pm.add(pm);
}

/**
 * contract edge to simplify mesh and update all data structures
 * \param edge edge to contract
 * \param itFront iterator to erased edge if edge heap
 * \return 0 if contraction was ok, 1 if contraction causes normal switching, 2 if contraction causes triangle identity
 */
int svt_mesh::edgeContract(svt_edge& edge, svt_edgeHeapSet::iterator& itFront)
{
    int i;
    list<int>::iterator it, it2;
    list<svt_edgeHeapSet::iterator>::iterator it3, it4;
    int delNum = 0; // anzahl gemeinsamer dreiecke
    list<unsigned int> delPktList; // pkte nicht v1, v2 aus zu loeschenden dreiecken
    svt_vector3<double> norm;
    bool normSwitch = false;

    // knotenzahl um 1 herunterzaehlen, dreieckszahl wird in delTriangle richtig belegt
    m_oVertices.setNumber(m_oVertices.getNumber() - 1);
    
    // knotenposition von v1 aendern
    m_oVertices[edge.v1].pkt = edge.pkt;

    // dreiecke von v2 mit v1 belegen
    for (it = m_oVertices[edge.v2].tri.begin(); 
         it != m_oVertices[edge.v2].tri.end(); ++it)
        for (i = 0; i < 3; i++)
            if (m_oTriangles[*it].ver[i] == edge.v2)
                m_oTriangles[*it].ver[i] = edge.v1;

    // dreiecke von v2 auf v1 uebertragen
    m_oVertices[edge.v1].tri.splice(m_oVertices[edge.v1].tri.end(), m_oVertices[edge.v2].tri); 

    // sortieren, dann gleiche dreiecke beider knoten herausnehmen 
    m_oVertices[edge.v1].tri.sort();
    it = m_oVertices[edge.v1].tri.begin();
    while (it != m_oVertices[edge.v1].tri.end())
    {
        it2 = it; ++it2;
        if ((it2 != m_oVertices[edge.v1].tri.end()) && ((*it) == (*it2))) // dreieck von beiden knoten
        {
            for (i = 0; i < 3; i++)
                // pkt nicht v1, v2 aus zu loeschenden dreiecken
                if ((m_oTriangles[*it].ver[i] != edge.v1) && (m_oTriangles[*it].ver[i] != edge.v2))
                {
                    delPktList.push_back(m_oTriangles[*it].ver[i]);
                    m_oVertices[m_oTriangles[*it].ver[i]].tri.remove(*it); // geloeschte dreiecke aus liste des 3. punktes loeschen
                }
            delTriangle(*it);
            delNum++;
            it = m_oVertices[edge.v1].tri.erase(it, ++it2); // nochmal ++, da oben offenes intervall
        } 
        else 
        {
            if (m_penalizeSwitchFlag)
            {
                norm = m_oTriangles[*it].norm;
                calcTriNormal(*it); // dreiecksnormalen neu berechnen
                // normalenflip wird untersucht; da .norm normalisiert ergibt sich -1..1
                if ((norm * m_oTriangles[*it].norm) < m_penalizeSwitchArc) // 0 waere 90 grad; -0.5 135 grad wechsel
                    normSwitch = true;
            } 
            else
                calcTriNormal(*it); // dreiecksnormalen neu berechnen
            ++it;
        }
    }

    // normale am knoten anpassen 
    calcVerNormal(edge.v1);
    // Texturkoordinate mitteln
    for (i = 0; i < 2; i++)
        m_oVertices[edge.v1].tex[i] = (m_oVertices[edge.v1].tex[i] + m_oVertices[edge.v2].tex[i])/2.;
    // Farbwerte mitteln
    if (getVertexColored())
    {
        m_oVertices[edge.v1].col.setR((m_oVertices[edge.v1].col.getR()+m_oVertices[edge.v2].col.getR())/2.);
        m_oVertices[edge.v1].col.setG((m_oVertices[edge.v1].col.getG()+m_oVertices[edge.v2].col.getG())/2.);
        m_oVertices[edge.v1].col.setB((m_oVertices[edge.v1].col.getB()+m_oVertices[edge.v2].col.getB())/2.);
    }

    // untersuchung ob zwei dreiecke genau aufeinanderliegen
    if (m_penalizeIdentityFlag)
        for (it = m_oVertices[edge.v1].tri.begin(); it != m_oVertices[edge.v1].tri.end(); ++it)
            for (((it2 = it), ++it2); it2 != m_oVertices[edge.v1].tri.end(); ++it2)
                if (((m_oTriangles[*it].ver[0] == m_oTriangles[*it2].ver[0]) || 
                     (m_oTriangles[*it].ver[0] == m_oTriangles[*it2].ver[1]) || 
                     (m_oTriangles[*it].ver[0] == m_oTriangles[*it2].ver[2])) && 
                    ((m_oTriangles[*it].ver[1] == m_oTriangles[*it2].ver[0]) || 
                     (m_oTriangles[*it].ver[1] == m_oTriangles[*it2].ver[1]) || 
                     (m_oTriangles[*it].ver[1] == m_oTriangles[*it2].ver[2])) && 
                    ((m_oTriangles[*it].ver[2] == m_oTriangles[*it2].ver[0]) || 
                     (m_oTriangles[*it].ver[2] == m_oTriangles[*it2].ver[1]) || 
                     (m_oTriangles[*it].ver[2] == m_oTriangles[*it2].ver[2])))
                    return 2;

    if (normSwitch) // wenn normale sich um bestimmten winkel gedreht hat hier herausspringen
        return 1;

    // quadric von v2 zu v1 hinzuaddieren
    m_oVertices[edge.v1].quadric += m_oVertices[edge.v2].quadric;

    // Kantenbehandlung -----------------------------------------------
    // gerade behandelnde kante und damit zu loeschende kante 
    // aus kantenliste der knoten entfernen
    it3 = m_oVertices[edge.v1].edge.begin();
    while (it3 != m_oVertices[edge.v1].edge.end())
        if ((*it3) == itFront) // nicht auf (*it3)->v1 zugreifen, da speicher schon freigegeben
            // // andersherum muss nicht ueberpfueft werden, da kanten einmalig und gleich
            it3 = m_oVertices[edge.v1].edge.erase(it3);
        else if ((((*it3)->second.v1 == edge.v1) && ((*it3)->second.v2 == edge.v2)) || 
                 (((*it3)->second.v1 == edge.v2) && ((*it3)->second.v2 == edge.v1)))
        {
            // kante aus liste des anderen knoten in kante loeschen
            // daher muss weiter unen auch nicht nochmal nach diesen knotenkombinationen gesucht werden
            it4 = m_oVertices[edge.v2].edge.begin();
            while (it4 != m_oVertices[edge.v2].edge.end())
                if ((*it3) == (*it4))
                    it4 = m_oVertices[edge.v2].edge.erase(it4);
                else
                    ++it4;
            m_edgeHeapSet.erase(*it3);
            it3 = m_oVertices[edge.v1].edge.erase(it3);
        }
        else
            ++it3;

    it3 = m_oVertices[edge.v2].edge.begin();
    while (it3 != m_oVertices[edge.v2].edge.end())
        if ((*it3) == itFront)
            it3 = m_oVertices[edge.v2].edge.erase(it3);
        else 
        {
            // kanten werden geloescht, die nach verschmelzung uebereinanderliegen
            if ((find(delPktList.begin(), delPktList.end(), (*it3)->second.v1) != delPktList.end()) || 
                (find(delPktList.begin(), delPktList.end(), (*it3)->second.v2) != delPktList.end()))
            {
                // kante aus liste des anderen knoten in kante loeschen
                if (((*it3)->second.v1) == edge.v2)
                {
                    it4 = m_oVertices[(*it3)->second.v2].edge.begin();
                    while (it4 != m_oVertices[(*it3)->second.v2].edge.end())
                        if ((*it3) == (*it4))
                            it4 = m_oVertices[(*it3)->second.v2].edge.erase(it4);
                        else
                            ++it4;
                }
                else
                {
                    it4 = m_oVertices[(*it3)->second.v1].edge.begin();
                    while (it4 != m_oVertices[(*it3)->second.v1].edge.end())
                        if ((*it3) == (*it4))
                            it4 = m_oVertices[(*it3)->second.v1].edge.erase(it4);
                        else
                            ++it4;
                }
                m_edgeHeapSet.erase(*it3);
                it3 = m_oVertices[edge.v2].edge.erase(it3);
            }
            else
            {
                if ((*it3)->second.v1 == edge.v2)
                    (*it3)->second.v1 = edge.v1;
                if ((*it3)->second.v2 == edge.v2)
                    (*it3)->second.v2 = edge.v1;
                ++it3;
            }
        }
    // aneinanderhaengen der kantenlisten
    m_oVertices[edge.v1].edge.splice(m_oVertices[edge.v1].edge.end(), m_oVertices[edge.v2].edge);
    // alle kanten neu berechnen und einsortieren
    m_edgeHeapSet.reCalcInsert(edge.v1, m_oVertices);

    return 0;
}

/**
 * initialize data structures for simplification
 */
void svt_mesh::initQuadricSimplification()
{
    // unsortierte edges einsammeln
    svt_edgeHeap edgeHeap;
    unsigned int anz;
    unsigned int sing; // anzahl singulaerer quadriken waehrend initialisierung

    cout << "svt_mesh> Simplification initialization: quadrics..." << flush;
    initAllQuadric();
    cout << " edges " << flush;
    edgeHeap.init(m_oVertices,  m_oTriangles, m_edge_threshold);
    // wenn gewuenscht Rander bestrafen
    if (m_penalizeBordersFlag)
    {
        cout << "... penalize..." << flush;
        anz = penalizeBorders(edgeHeap);
        cout << "\b\b\b " << anz << " border edges...";
    }
    cout << " target and cost (" << flush;
    sing = edgeHeap.computeAllTargetCost(m_oVertices);
    cout << sing << " singular quadrics, " << edgeHeap.size()-sing << " regular quadrics)..." << flush;
    // alle Kanten Elemente sortieren, bei kostenaenderung muss das veraenderte element neu einsortiert bzw in liste verschoben werden
    cout << " sort..." << flush;
    m_edgeHeapSet.set(edgeHeap, m_oVertices);
    cout << "sorted quadrics, " << m_edgeHeapSet.size() << flush;
    edgeHeap.clear(); // weitere zugriffe auf kanten geschieht ueber m_edgeHeapSet; da nun lokal, wuerde sowieso am ende der funkton freigegeben
    cout << " finished!" << endl;
}

/**
 * initialize quadrics at vertices for simplification
 */
void svt_mesh::initAllQuadric()
{
    unsigned int i;
    int j;
    double d;
    svt_quadric q;

    for (i = 0; i < m_oTriangles.size(); i++)
    { 
        // Abstand des Dreiecks vom Nullpunkt (Normale ist bereits normalisiert)
        d = - m_oTriangles[i].norm * m_oVertices[m_oTriangles[i].ver[0]].pkt;

        q.init(m_oTriangles[i].norm, d);
        // hier koennte eine gewichtung der quadric ueber die flaechengroesse und den anliegenden
        // winkel eines jeden knotens erfolgen!!!
        for (j = 0; j < 3; j++)
            m_oVertices[m_oTriangles[i].ver[j]].quadric += q;
    }
}

/**
 * penalize vertices at borders for simplification
 * \param edgeHeap reference to edge heap
 * \return number of penalized borders
 */
unsigned int svt_mesh::penalizeBorders(svt_edgeHeap& edgeHeap)
{
    svt_edgeHeap::iterator it;
    list<int>::iterator it2;
    unsigned int i, anz, t=0, penAnz = 0;
    svt_quadric q;
    svt_vector3<double> vc;
    double d;

    for (it = edgeHeap.begin(); it != edgeHeap.end(); ++it)
    {
        anz = 0;
        for (it2 = m_oVertices[it->v1].tri.begin(); it2 != m_oVertices[it->v1].tri.end(); ++it2)
            for (i = 0; i < 3; i++)
                if (m_oTriangles[*it2].ver[i] == it->v2)
                {
                    anz++;
                    t = *it2;
                }
        if (anz == 1) // ein dreieck grenzt an kante an
        {
            // normalenvektor einer ebene senkrecht auf dreieck durch kante
            // t ist auf jeden fall gesetzt, da anz nicht 0
            vc = crossProduct(m_oTriangles[t].norm, m_oVertices[it->v1].pkt - m_oVertices[it->v2].pkt);
            vc.normalize();
            // Abstand des Dreiecks vom Nullpunkt (Normale ist bereits normalisiert)
            d = - vc * m_oVertices[it->v1].pkt;
            //quadrik berechnen und auf knotenquadriken gewichtet draufaddieren
            q.init(vc, d);
            q *= m_borderPenalizeFactor; //bestrafung
            m_oVertices[it->v1].quadric += q;
            m_oVertices[it->v2].quadric += q;
            penAnz++;
        }
    }
    return penAnz;
}

/**
 * enable smooth crossover between detail levels
 */ 
void svt_mesh::enableGeomorph() { m_pm.enableGeomorph(); }
/**
 * disable smooth crossover between detail levels
 */ 
void svt_mesh::disableGeomorph() { m_pm.disableGeomorph(); }

/**
 * enable display of vertex numbers in visualization
 */
void svt_mesh::enableShowVertexNumbers()
{ 
    m_showVertexNumbers = true;
}

/**
 * disable display of vertex numbers in visualization
 */
void svt_mesh::disableShowVertexNumbers()
{ 
    m_showVertexNumbers = false; 
}
/**
 * get status of displaying of vertex numbers in visualization
 * \return if display of vertex numbers is enabled
 */ 
bool svt_mesh::getShowVertexNumbers() { return m_showVertexNumbers; }

/**
 * set rule for adaptive change of detail level
 * \param sel rule for adaptive change of detail level (0: fps, 1: local error, 2: explicit vertex number, 3: fus)
 */ 
void svt_mesh::setSimplifyRule(int sel) { m_SimplifyRule = sel; }
/**
 * set minimal fps for rule for adaptive change of detail level
 * \param fps frames per second
 */ 
void svt_mesh::setRuleMinimalFPS(int fps) { m_RuleMinimalFPS = fps; }
/**
 * set minimal fus for rule for adaptive change of detail level
 * \param fus force updates per second
 */ 
void svt_mesh::setRuleMinimalFUS(int fus) { m_RuleMinimalFUS = fus; }
/**
 * set maximal local error for rule for adaptive change of detail level
 * \param error local error
 */ 
void svt_mesh::setRuleLocalError(float error) { m_RuleLocalError = error; }
/**
 * set exact number of vertices for rule for adaptive change of detail level
 * \param nr number of vertices in simplified mesh
 */ 
void svt_mesh::setRuleVertexNumbers(int nr) { m_RuleVertexNumbers = nr; }
/**
 * get rule for adaptive change of detail level
 * \return rule for adaptive change of detail level (0: fps, 1: local error, 2: explicit vertex number, 3: fus)
 */ 
int svt_mesh::getSimplifyRule() { return m_SimplifyRule; }
/**
 * get minimal fps for rule for adaptive change of detail level
 * \return frames per second
 */ 
int svt_mesh::getRuleMinimalFPS() { return m_RuleMinimalFPS; }
/**
 * get minimal fus for rule for adaptive change of detail level
 * \return force updates per second
 */ 
int svt_mesh::getRuleMinimalFUS() { return m_RuleMinimalFUS; }
/**
 * get maximal local error for rule for adaptive change of detail level
 * \return local error
 */ 
float svt_mesh::getRuleLocalError() { return m_RuleLocalError; }
/**
 * get exact number of vertices for rule for adaptive change of detail level
 * \return number of vertices in simplified mesh
 */ 
int svt_mesh::getRuleVertexNumbers() { return m_RuleVertexNumbers; }

/**
 * simplify exact to a number of vertices
 * \param i number of vertices in simplified mesh
 * \return if the detail level changed
 */ 
bool svt_mesh::simplifyToVerNumber(unsigned int i)
{
    bool b;
    setRuleVertexNumbers(i);
    setSimplifyRule(2);
    g_oMeshSemaphore.P();
    b = updateSimplify();
    g_oMeshSemaphore.V();
    return b; 
}

/**
 * set flag to penalize vertices at border edges
 * \param b if vertices at border edges should penalized
 */ 
void svt_mesh::setPenalizeBordersFlag(bool b) 
{ 
    if (b != getPenalizeBordersFlag())
        m_simpOptionsChanged = true; 
    m_penalizeBordersFlag = b; 
}
/**
 * get flag to penalize vertices at border edges
 * \return if vertices at border edges should penalized
 */ 
bool svt_mesh::getPenalizeBordersFlag() { return m_penalizeBordersFlag; }
/**
 * set flag to penalize triangle identity
 * \param b if triangle identity should penalized
 */ 
void svt_mesh::setPenalizeIdentityFlag(bool b) 
{ 
    if (b != getPenalizeIdentityFlag())
        m_simpOptionsChanged = true; 
    m_penalizeIdentityFlag = b; 
}
/**
 * get flag to penalize triangle identity
 * \return if triangle identity should penalized
 */ 
bool svt_mesh::getPenalizeIdentityFlag() { return m_penalizeIdentityFlag; }
/**
 * set flag to penalize normal switching
 * \param b if normal switching should penalized
 */ 
void svt_mesh::setPenalizeSwitchFlag(bool b)
{ 
    if (b != getPenalizeSwitchFlag())
        m_simpOptionsChanged = true; 
    m_penalizeSwitchFlag = b; 
}
/**
 * get flag to penalize normal switching
 * \return if normal switching should penalized
 */ 
bool svt_mesh::getPenalizeSwitchFlag() { return m_penalizeSwitchFlag; }
/**
 * set arc for penalizing normal switching
 * \param d arc for penalizing normal switching
 */ 
void svt_mesh::setPenalizeSwitchArc(double d) 
{ 
    if (d != getPenalizeSwitchArc())
        m_simpOptionsChanged = true; 
    m_penalizeSwitchArc = d; 
}
/**
 * get arc for penalizing normal switching
 * \return arc for penalizing normal switching
 */ 
double svt_mesh::getPenalizeSwitchArc() { return m_penalizeSwitchArc; }

/**
 * set threshold for adding virtual edges
 * \param t threshold
 */ 
void svt_mesh::setThreshold(double t) 
{ 
    if (t != getThreshold())
        m_simpOptionsChanged = true; 
    m_edge_threshold = t; 
}
/**
 * get threshold for adding virtual edges
 * \return threshold
 */ 
double svt_mesh::getThreshold() { return m_edge_threshold; }

/**
 * get maximal global error from original mesh to the current detail level
 * \return maximal global error
 */ 
double svt_mesh::getGlobalErrorMax() { return m_globalErrorMax; }
/**
 * get average squered global error from original mesh to the current detail level
 * \return average squered global error
 */ 
double svt_mesh::getGlobalErrorAvg() { return m_globalErrorAvg; }
/**
 * get average global error from original mesh to the current detail level
 * \return average global error
 */ 
double svt_mesh::getGlobalErrorSum() { return m_globalErrorSum; }
/**
 * set maximal global error from original mesh to the current detail level
 * \param err maximal global error
 */ 
void svt_mesh::setGlobalErrorMax(double err) { m_globalErrorMax = err; }
/**
 * set average squered global error from original mesh to the current detail level
 * \param err average squered global error
 */ 
void svt_mesh::setGlobalErrorAvg(double err) { m_globalErrorAvg = err; }
/**
 * set average global error from original mesh to the current detail level
 * \param err average global error
 */ 
void svt_mesh::setGlobalErrorSum(double err) { m_globalErrorSum = err; }

/**
 * set the adaption tolerance
 * \param fAdaptionTolerance new tolerance factor
 */
void svt_mesh::setAdaptionTolerance(double fAdaptionTolerance)
{
    m_fAdaptionTolerance = fAdaptionTolerance;
};
/**
 * get the adaption tolerance
 * \return tolerance factor
 */
double svt_mesh::getAdaptionTolerance()
{
    return m_fAdaptionTolerance;
};
/**
 * set the adaption step
 * \param fAdaptionTolerance new adaption step
 */
void svt_mesh::setAdaptionStep(double fAdaptionStep)
{
    m_fAdaptionStep = fAdaptionStep;
};
/**
 * get the adaption step
 * \return adaption step
 */
double svt_mesh::getAdaptionStep()
{
    return m_fAdaptionStep;
};

/**
 * set the adaption time
 * \param fAdaptionTime new adaption time (millisecond)
 */
void svt_mesh::setAdaptionTime(int iAdaptionTime)
{
    m_iAdaptionTime = iAdaptionTime;
};
/**
 * get the adaption time
 * \return adaption time (milliseconds)
 */
int svt_mesh::getAdaptionTime()
{
    return m_iAdaptionTime;
};

/**
 * calculate global erros between original and simplified mesh
 * \param all if global errors to all detail levels should be calculate
 * \param saveFile file to write global errors to all detail levels to
 */
void svt_mesh::calcGlobalError(bool all, const char *saveFile)// (bool all = false, const char *saveFile = "", int step = 1)
{
    unsigned int i, j, k;
    // vektor, der anzeigt, ob vertex im vereinbfachten netz enthalten
    vector<bool> in(getVerOrigNumber(), false);
    double minDist, minDistSum1, minDistSum2, minDistMax1, minDistMax2, minDistSqrSum1, minDistSqrSum2;

    m_isSimplifiedFlag = false; // gesetzt, damit automatischer timer zb bei fps und fus
                                // nicht waehrend berechnung eine netzveraenderung vornimmt!!!

    // komplette kopieerstellung des netzes
    svt_mesh orig_mesh = *this;

    // aufbau des originalnetzes ueber pm
    orig_mesh.resetMesh();

    minDistSum1 = 0.; minDistSum2 = 0.; minDistMax1 = 0.; minDistMax2 = 0.; minDistSqrSum1 = 0.; minDistSqrSum2 = 0.;

    // in erstellen
    for (j=0; j<m_oTriangles.size(); j++)
        if (m_oTriangles[j].in)
            for (k=0; k<3; k++)
                in[m_oTriangles[j].ver[k]] = true;

    // minimaler abstand der knoten des originalnetzes zu den dreiecken des vereinfachten netzes
    for (i=0; i<orig_mesh.getVerOrigNumber(); i++)
    {
        minDist = FLT_MAX;
        for (j=0; j<m_oTriangles.size(); j++)
            if (m_oTriangles[j].in)
                minDist = svt_min(minDist, distQuadTriVer(j, (*(orig_mesh.getVertexArray()))[i].pkt));
        if (minDist != FLT_MAX)
        {
            minDistSqrSum1 += minDist;
            minDist = sqrt(minDist); // sqrt, da distQuadTriVer abstandsquad zurueckgibt, .distance aber richtigen abst.
            minDistSum1 += minDist;
            minDistMax1 = svt_max(minDistMax1, minDist);
        }
    }
    cout << "svt_mesh> Minimal distance from the original vertices (" << orig_mesh.getVerOrigNumber() << ") to the simplified mesh:" << endl;
    cout << "svt_mesh>    Maximal distance        : " << minDistMax1 << "," << endl;
    cout << "svt_mesh>    Average squared distance: " << minDistSqrSum1/orig_mesh.getVerOrigNumber() << "," << endl;
    cout << "svt_mesh>    Average distance        : " << minDistSum1/orig_mesh.getVerOrigNumber() << endl;
    // minimaler abstand der knoten des vereinfachten netzes zu den dreiecken des originalnetzes
    for (i=0; i< getVerOrigNumber(); i++)
        if (in[i])
        {
            minDist = FLT_MAX;
            for (j=0; j<orig_mesh.getTriOrigNumber(); j++)
                minDist = svt_min(minDist, orig_mesh.distQuadTriVer(j, m_oVertices[i].pkt));
            if (minDist != FLT_MAX)
            {
                minDistSqrSum2 += minDist;
                minDist = sqrt(minDist); // sqrt, da distQuadTriVer abstandsquad zurueckgibt, .distance aber richtigen abst.
                minDistSum2 += minDist;
                minDistMax2 = svt_max(minDistMax2, minDist);
            }
        }
    cout << "svt_mesh> Minimal distance from the vertices of the simplified mesh (" << getVerNumber() << ") to the original mesh:" << endl;
    cout << "svt_mesh>     Maximal distance        : " << minDistMax2 << "," << endl;
    cout << "svt_mesh>     Average squared distance: " << minDistSqrSum2/orig_mesh.getVerOrigNumber() << "," << endl;
    cout << "svt_mesh>     Average distance        : " << minDistSum2/orig_mesh.getVerOrigNumber() << endl;
    minDistSum2 = (minDistSum1+minDistSum2)/(orig_mesh.getVerOrigNumber()+getVerNumber());
    minDistSqrSum2 = (minDistSqrSum1+minDistSqrSum2)/(orig_mesh.getVerOrigNumber()+getVerNumber());
    minDistMax2 = svt_max(minDistMax1, minDistMax2);
    cout << "svt_mesh> Altogether:" << endl;
    cout << "svt_mesh>     Maximal distance        : " << minDistMax2 << "," << endl;
    cout << "svt_mesh>     Average squared distance: " << minDistSqrSum2 << "," << endl;
    cout << "svt_mesh>     Average distance        : " << minDistSum2 << endl;
    setGlobalErrorMax(minDistMax2);
    setGlobalErrorAvg(minDistSqrSum2);
    setGlobalErrorSum(minDistSum2);

    if (all) // wenn fuer alle vereinfachunsstufe fehler berechnet werden soll
    {
        unsigned int verNum = getVerNumber();
        // ausgabe einer fehlerdatei
        ofstream errFp(saveFile);
        errFp.setf(ios::fixed);
        errFp.precision(20);
        vector<bool> changed(getVerOrigNumber(), false);
        list<svt_pm_elem>::const_iterator aktPm;

        // durch diese netzaenderungen des originalnetzes kann es zu einem flackern des bildes kommen
        // wenn display-liste andauend erneuert wird, was zb bei angeschaltetem font der fall ist
        while (m_pm.split(*this))
            ;

        // in erstellen
        for (j=0; j<m_oVertices.size(); j++)
            in[j] = true;

        do
        {
            minDistSum1 = 0.; minDistSum2 = 0.; minDistMax1 = 0.; minDistMax2 = 0.; minDistSqrSum1 = 0.; minDistSqrSum2 = 0.;

            aktPm = m_pm.getAkt();
            if (aktPm != m_pm.end())
            {
                in[aktPm->v_new.v] = false;
                changed[aktPm->v_new.v] = true;
                changed[aktPm->v.v] = true;
            }

            // minimaler abstand der knoten des originalnetzes zu den dreiecken des vereinfachten netzes
            for (i=0; i<orig_mesh.getVerOrigNumber(); i++)
            {
                if (changed[i]) // nur die veranderten knoten liegen nicht mehr genau auf dem netz
                {
                    minDist = FLT_MAX;
                    for (j=0; j<m_oTriangles.size(); j++)
                        if (m_oTriangles[j].in)
                            minDist = svt_min(minDist, distQuadTriVer(j, (*(orig_mesh.getVertexArray()))[i].pkt));
                    if (minDist != FLT_MAX)
                    {
                        minDistSqrSum1 += minDist;
                        minDist = sqrt(minDist); // sqrt, da distQuadTriVer abstandsquad zurueckgibt, .distance aber richtigen abst.
                        minDistSum1 += minDist;
                        minDistMax1 = svt_max(minDistMax1, minDist);
                    }
                }
            }
            // minimaler abstand der knoten des vereinfachten netzes zu den dreiecken des originalnetzes
            for (i=0; i< getVerOrigNumber(); i++)
                if (in[i] && changed[i]) // ua. nur die veranderten knoten liegen nicht mehr genau auf dem netz
                {
                    minDist = FLT_MAX;
                    for (j=0; j<orig_mesh.getTriOrigNumber(); j++)
                        minDist = svt_min(minDist, orig_mesh.distQuadTriVer(j, m_oVertices[i].pkt));
                    if (minDist != FLT_MAX)
                    {
                        minDistSqrSum2 += minDist;
                        minDist = sqrt(minDist); // sqrt, da distQuadTriVer abstandsquad zurueckgibt, .distance aber richtigen abst.
                        minDistSum2 += minDist;
                        minDistMax2 = svt_max(minDistMax2, minDist);
                    }
                }
            errFp << orig_mesh.getVerOrigNumber() << " " << getVerNumber();
            errFp << " " << minDistMax1 << " " << minDistSqrSum1/orig_mesh.getVerOrigNumber() << " " << minDistSum1/orig_mesh.getVerOrigNumber();
            errFp << " " << minDistMax2 << " " << minDistSqrSum2/getVerNumber() << " " << minDistSum2/getVerNumber();
            minDistSum2 = (minDistSum1+minDistSum2)/(orig_mesh.getVerOrigNumber()+getVerNumber());
            minDistMax2 = svt_max(minDistMax1, minDistMax2);
            minDistSqrSum2 = (minDistSqrSum1+minDistSqrSum2)/(orig_mesh.getVerOrigNumber()+getVerNumber());
            errFp << " " << minDistMax2 << " " << minDistSqrSum2  << " " << minDistSum2 << endl;
        } while (m_pm.contract(*this));
        // netz wieder in den urspruenglichen zustand zuruecksetzen
        verNum = verNum-getVerNumber();
        while ((verNum > 0) && m_pm.split(*this))
            verNum--;

    }

    m_isSimplifiedFlag = true; // gesetzt, damit automatischer timer zb bei fps und fus
    // wieder netzveraenderung vornehmen kann!!!
}

/**
 * quadric distance from a vertex to a triangle
 * \param t triangle number in mesh
 * \param v vertex vector
 * \return quadric distance from the vertex v to the triangle number t
 */
double svt_mesh::distQuadTriVer(int t, svt_vector3<double>& v)
{
    svt_vector3<double> vp, v1, v2;
    double v1v2, v1vp, v2vp, v1v1, v2v2;
    double fak1, fak2;
    double abs;

	// m_oVertices[m_oTriangles[t].ver[0]].p auf den Nullpunkt verschieben
    v1 = m_oVertices[m_oTriangles[t].ver[1]].pkt - m_oVertices[m_oTriangles[t].ver[0]].pkt;
    v2 = m_oVertices[m_oTriangles[t].ver[2]].pkt - m_oVertices[m_oTriangles[t].ver[0]].pkt;
    vp = v - m_oVertices[m_oTriangles[t].ver[0]].pkt;

	// Projektionsfaktoren fak1, fak2 auf die dreiecksseiten bestimmen
    v1v2 = v1 * v2;
    v1vp = v1 * vp;
    v2vp = v2 * vp;
    v1v1 = v1 * v1;
    v2v2 = v2 * v2;
    fak2 = v1v2*v1v2 - v1v1*v2v2; // als Zwischenspeicher missbraucht
    fak1 = (v1v2*v2vp - v2v2*v1vp) / fak2;
    fak2 = (v1v2*v1vp - v1v1*v2vp) / fak2;

    if ((fak1 < 0) || (fak2 < 0) || ((fak1 + fak2) > 1)) // keine senkrechte projektion auf dreieck moeglich
    {
        // senkrechte projektion auf kante, sonst eckpunktabstand nehmen
        fak1 = v1vp / v1v1; // projektion von vp auf v1
        if (fak1 <= 0)
            abs = vp.lengthSq();
        else if (fak1 >= 1)
            abs = vp.distanceSq(v1);
        else
            abs = vp.distanceSq(fak1 * v1);
        fak2 = v2vp / v2v2; // projektion von vp auf v2
        if (fak2 <= 0)
            abs = svt_min(abs, vp.lengthSq());
        else if (fak1 >= 1)
            abs = svt_min(abs, vp.distanceSq(v2));
        else
            abs = svt_min(abs, vp.distanceSq(fak2 * v2));
        // fuer 3. kante muss m_oVertices[m_oTriangles[t].ver[1]].pkt in den nullpunkt verschoben werden
        v1 = m_oVertices[m_oTriangles[t].ver[2]].pkt - m_oVertices[m_oTriangles[t].ver[1]].pkt;
        vp = v - m_oVertices[m_oTriangles[t].ver[1]].pkt;
        v1vp = v1 * vp;
        v1v1 = v1 * v1;
        fak1 = v1vp / v1v1; // projektion von vp auf 3. kante
        if (fak1 <= 0)
            abs = svt_min(abs, vp.lengthSq());
        else if (fak1 >= 1)
            abs = svt_min(abs, vp.distanceSq(v1));
        else
            abs = svt_min(abs, vp.distanceSq(fak1 * v1));
        return abs;
    }
    else
    {
        v1 = fak1*v1 + fak2*v2; // missbrauch von v1 zur speicherplatzersparnis
        return vp.distanceSq(v1);
    }

    return 0;
}

/**
 * calculate the normal vectors for a triangle
 * \param oTriangle the triangle
 * \return the normal vectors
 */
Triangle svt_mesh::calcTriNormal(const Triangle &oTriangle)
{
    int k, j;
    svt_vector3<double> vt1, vt2, vc;
    Triangle oNormals;
     
    for (k = 0; k < 3; k++)
    {
        vt1[k] = oTriangle.m_fP[1][k] - oTriangle.m_fP[0][k];
        vt2[k] = oTriangle.m_fP[2][k] - oTriangle.m_fP[0][k];
    }
    vc = crossProduct(vt1, vt2);

    // normalize normal at triangle
    vc.normalize();
    for (k = 0; k < 3; k++) // Normalenindex 
        for (j = 0; j < 3; j++) // Normalenindex 
            oNormals.m_fP[j][k] = vc[k];

    return oNormals;
}


/**
 * calculate the normal for a triangle
 * \param t triangle number in mesh
 */
void svt_mesh::calcTriNormal(int t)
{
    svt_vector3<double> vt1, vt2, vc;
     
    vt1 = m_oVertices[m_oTriangles[t].ver[0]].pkt - m_oVertices[m_oTriangles[t].ver[2]].pkt;
    vt2 = m_oVertices[m_oTriangles[t].ver[1]].pkt - m_oVertices[m_oTriangles[t].ver[2]].pkt;
    m_oTriangles[t].norm = crossProduct(vt1, vt2).normalize();
}

/**
 * calculate the normal for a vertex
 * \param v vertex number in mesh
 */
void svt_mesh::calcVerNormal(int v)
{
    list<int>::iterator it;

    for (it = m_oVertices[v].tri.begin(); it != m_oVertices[v].tri.end(); ++it)
        m_oVertices[v].norm += m_oTriangles[*it].norm;

    // normalize normal at vertex
    m_oVertices[v].norm.normalize();
}

/**
 * print the name of the object
 */
void svt_mesh::printName()
{
    cout << "Mesh with " << getTriNumber() << " triangles."<< endl;
}
