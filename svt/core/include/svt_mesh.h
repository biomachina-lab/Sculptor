/***************************************************************************
                          svt_mesh
			  --------
    begin                : 20.01.2003
    author               : Maik Boltes, Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_MESH_H
#define SVT_MESH_H

#include <svt_stlList.h>
#include <svt_stlVector.h>

#include <svt_core.h>
#include <svt_init.h>
#include <svt_node.h>
#include <svt_vertinterl.h>
#include <svt_vector3.h>
#include <svt_iostream.h>

#include <svt_vertex_array.h>
#include <svt_edge_heap.h>
#include <svt_edge_heap_set.h>
#include <svt_pm.h>
#include <svt_ply.h>

class svt_mesh;

// special struct used as argument in the thread creation for the mesh simplification. 
// Acts as connecting element between thread and mesh object.
typedef struct
{
    svt_mesh* pMesh;
    bool bValid;
} simplifyMeshThread;

/** 
  * triangle
  *@author Maik Boltes
  */
class svt_triangle_type {       	   // Dreiecksstruktur
public:
    /**
     * Constructor
     */
    svt_triangle_type()
    {
        in = true;
    }
    UInt32 ver[3];            // Eckpunktnummern (beginnt bei 0, in smf-datei ab 1)
    svt_vector3<double> norm; //  Normale der Dreiecksflaeche
    bool in;                  //  zeigt an, ob Dreieck noch im (vereinfachten) Netz
};

/**
 * stream out triangle
 * \param s output stream
 * \param t triangle
 * \return output stream 
 */
inline ostream& operator<< (ostream& s, const svt_triangle_type& t) {
    int i;
    s << "ver( ";
    for (i= 0; i<3; i++)
        s << t.ver[i] << " ";
    s << "), norm( ";
    for (i= 0; i<3; i++)
        s << t.norm[i] << " ";
    s << "), in: " << t.in;
//      s << ", chg: " << t.chg;
    return s;
}

typedef vector<svt_triangle_type> svt_triangle_array;

// !!!! Waehrend der Berechnung der Vereinfachung darf keine Kopie vom Netz erstellt werden, 
//      da sonst Zeiger undefiniert sind (Vertex, Edge) !!!!

/**
 * \brief Triangle mesh
 *@author Maik Boltes
 * \author Maik Boltes
 *
 * Triangle mesh
 *
 * \par no copie alowed during simplification, because of undefined iterators and pointers in the vertex- and edge- datastructure
 */
class DLLEXPORT svt_mesh : public svt_node
{
    friend class svt_pm; // greift auf m_oVertices und m_oTriangles zu
public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_mesh(svt_pos* pPos =NULL, svt_properties* pProp =NULL);

    /**
     * Destructor
     */ 
    virtual ~svt_mesh();

    /**
     * compute the size of the surface of the mesh
     * \return size of the surface of the mesh
     */ 
    double surfaceSize();
        
    /**
     * switch the circular direction of vertices for switch the normal direction for reading PLY file
     * \return bool if switch is active
     */
    bool switchNormalDirectionForReadingPly();

    /**
     * set status for deleting unused vertices while reading smf and ply files
     * \return bool true, if unused vertices will be deleted after reading smf and ply files
     */
    bool setDelUnusedVertices(bool b);

    /**
     * set the flag that the vertices have their own color
     * \param b true, if vertices have their own color
     */
    void setVertexColored(bool b);
    /**
     * get the flag that shows, if the vertices have their own color
     */
    bool getVertexColored();

    /**
     * set the flag that shows, if surface should be lightend from both sides
     */
    void setLightTwoSide(bool b);
    /**
     * get the flag that shows, if surface should be lightend from both sides
     */
    bool getLightTwoSide();
    /**
     * set the flag that shows, if surface should be culled
     */
    void setCulling(bool b);
    /**
     * get the flag that shows, if surface should be culled
     */
    bool getCulling();
    /**
     * set the flag that shows, if normals should be switched for visu
     */
    void setSwitchNormals(bool b);
    /**
     * get the flag that shows, if normals should be switched for visu
     */
    bool getSwitchNormals();
    
    /**
     * set color of mesh object
     * \param r red value between 0 and 1
     * \param g green value between 0 and 1
     * \param b blue value between 0 and 1
     */
    void setColor(float r, float g, float b);
    /**
     * get red color fraction of mesh object
     * \return red color fraction of mesh object between 0 and 1
     */ 
    float getColorR();
    /**
     * get green color fraction of mesh object
     * \return green color fraction of mesh object between 0 and 1
     */ 
    float getColorG();
    /**
     * get blue color fraction of mesh object
     * \return blue color fraction of mesh object between 0 and 1
     */ 
    float getColorB();

    /**
     * set transparency level of mesh object
     * \param f transpyrency level between 0 and 1
     */ 
    void setTransparency(float f);
    /**
     * set how transparency of mesh object should done
     * \param b set stripple on
     */ 
    void setStipple(bool b);

    /**
     * init storage and data for special marching cube mesh generation
     * \param l, x, y dimension of volume
     * \param fastMesh shows if the mesh is generated fast and not that exact
     */ 
    void initMarchingCube(int l, int x, int y, bool fastMesh);
    /**
     * free storage for special marching cube mesh generation
     */ 
    void freeMarchingCube();
protected:
    /**
     * checks, if oTriVer is already in voxel voxel
     * \param voxel voxel index from which up the next 4 triangles will be analysed
     * \param oTriVer vertex which is searched in voxel
     */ 
    int checkVoxelForVertex(int voxel, const Vector3f &oTriVer);
public:
    /**
     * add a triangle to the mesh
     * special to triangles from marching cube algorithm
     * assuming, that the voxels will be traversed in correct sequential order
     * \param oTriangle new triangle object
     * \param oNormals normal vectors for the vertices of new triangle
     * \param x, y, z voxel index
     * \param nr number of triangle in voxel (0..3)
     */ 
    void addTriangle(const Triangle &oTriangle, const Triangle &oNormals, int x, int y, int z, int nr);

    /**
     * add a triangle to the mesh
     * \param oTriangle new Triangle object
     * \param oNormals normal vectors for the new triangle
     */
    void addTriangle(const Triangle &oTriangle, const Triangle &oNormals);

    /**
     * add a triangle with its vertices to the mesh
     * \param oTriangle new Triangle object
     */
    void addTriangle(const Triangle &oTriangle);

    /**
     * add a triangle with its vertices to the mesh
     * \param oTriangle new Triangle object
     * \param oTexTriangle texture coordinates for the triangle
     */
    void addTriangle(const Triangle &oTriangle, const TexTriangle &oTexTriangle);

    /**
     * add a triangle with its vertices to the mesh
     * \param oTriangle new Triangle object
     * \param oNormals normal vectors for the new triangle
     * \param oTexTriangle texture coordinates for the triangle
     */
    void addTriangle(const Triangle &oTriangle, const Triangle &oNormals, const TexTriangle &oTexTriangle);

    /**
     * add a triangle to the mesh
     * \param ver pointer to three vertex numbers
     * \return true if triangle could added
     */ 
    bool addTriangle(UInt32 *ver);

    /**
     * activate a triangle
     * \param i activate triangle number i for visualization
     */ 
    void addTriangle(int i);

    /**
     * deactivate a triangle
     * \param i deactivate triangle number i for visualization
     */ 
    void delTriangle(int i);

    /**
     * reserve vertices in vertex vector for performace enhancement
     * \param n number of vertices in vertex vector
     */  
    void reserveVertex(int n);
    /**
     * reserve triangles in triangle vector for performace enhancement
     * \param n number of triangles in triangle vector
     */  
    void reserveTriangle(int n);

    /**
     * resize vertex vector 
     * \param n new number of vertices in vertex vector
     */  
    void resizeVertex(int n);
    /**
     * resize triangle vector 
     * \param n new number of triangles in triangle vector
     */  
    void resizeTriangle(int n);

    /**
     * add a vertex to the mesh
     * \param pkt vertex vector to be inserted
     * \return number of vertex in the mesh
     */ 
    int addVertex(const svt_vector3<double> &pkt);
    /**
     * add a vertex to the mesh
     * \param pkt vertex vector to be inserted
     * \param col color for vertex
     * \return number of vertex in the mesh
     */ 
    int addVertex(const svt_vector3<double> &pkt, const svt_color &col);
    /**
     * add a vertex to the mesh
     * \param pkt vertex vector to be inserted
     * \param norm normal vector for vertex
     * \param tex texture coordinate for vertex
     * \return number of vertex in the mesh
     */ 
    int addVertex(const svt_vector3<double> &pkt, const svt_vector3<double> &norm, const Point2f &tex);
    /**
     * add a vertex to the mesh
     * \param pkt vertex point to be inserted
     * \return number of vertex in the mesh
     */ 
    int addVertex(const Point3f &pkt);
    /**
     * add a vertex to the mesh
     * \param pkt vertex point to be inserted
     * \param norm normal vector for vertex
     * \param tex texture coordinate for vertex
     * \return number of vertex in the mesh
     */ 
    int addVertex(const Point3f &pkt, const Point3f &norm, const Point2f &tex);

    /**
     * get flag for searching for identical vertex in mesh for inserting a vertex
     * \return if the comparison should done
     */ 
    bool getCompareFlag();
    /**
     * set flag for searching for identical vertex in mesh for inserting a vertex
     * \param b set if the comparison should done
     */ 
    void setCompareFlag(bool b);

    /**
     * get number of remapped vertices
     * \return number of remapped vertices
     */ 
    int getRemappedVertices();

    /**
     * calculate the minimum and maximum expansion of the mesh and set internal variables
     */ 
    void calcMinMax();
    /**
     * calculate the minimum and maximum expansion of all vertices in the mesh and set internal variables (also vertices not in the detail level)
     */ 
    void calcMinMaxAll();

    /**
     * get size of the mesh in x direction
     * \return mesh size in x direction
     */ 
    float getSizeX();
    /**
     * get size of the mesh in y direction
     * \return mesh size in y direction
     */ 
    float getSizeY();
    /**
     * get size of the mesh in z direction
     * \return mesh size in z direction
     */ 
    float getSizeZ();

    /**
     * get minimum of the mesh in x direction
     * \return mesh minimum in x direction
     */ 
    float getMinX();
    /**
     * get minimum of the mesh in y direction
     * \return mesh minimum in y direction
     */ 
    float getMinY();
    /**
     * get minimum of the mesh in z direction
     * \return mesh minimum in z direction
     */ 
    float getMinZ();
    /**
     * get maximum of the mesh in x direction
     * \return mesh maximum in x direction
     */ 
    float getMaxX();
    /**
     * get maximum of the mesh in y direction
     * \return mesh maximum in y direction
     */ 
    float getMaxY();
    /**
     * get maximum of the mesh in z direction
     * \return mesh maximum in z direction
     */ 
    float getMaxZ();

    /**
     * set minimum of the mesh in x direction
     * \param t minimum of the mesh in x direction
     */ 
    void setMinX(float t);
    /**
     * set minimum of the mesh in y direction
     * \param t minimum of the mesh in y direction
     */ 
    void setMinY(float t);
    /**
     * set minimum of the mesh in z direction
     * \param t minimum of the mesh in z direction
     */ 
    void setMinZ(float t);
    /**
     * set maximum of the mesh in x direction
     * \param t maximum of the mesh in x direction
     */ 
    void setMaxX(float t);
    /**
     * set maximum of the mesh in y direction
     * \param t maximum of the mesh in y direction
     */ 
    void setMaxY(float t);
    /**
     * set maximum of the mesh in y direction
     * \param t maximum of the mesh in y direction
     */ 
    void setMaxZ(float t);

    /**
     * get the expansion of all vertices in x direction (also vertices not in the detail level)
     * \return expansion of all vertices in x direction
     */ 
    float getSizeXAll();
    /**
     * get the expansion of all vertices in y direction (also vertices not in the detail level)
     * \return expansion of all vertices in y direction
     */ 
    float getSizeYAll();
    /**
     * get the expansion of all vertices in z direction (also vertices not in the detail level)
     * \return expansion of all vertices in z direction
     */ 
    float getSizeZAll();
    
    /**
     * get minimum of all vertices in x direction (also vertices not in the detail level)
     * \return minimum of all vertices in x direction
     */ 
    float getMinXAll();
    /**
     * get minimum of all vertices in y direction (also vertices not in the detail level)
     * \return minimum of all vertices in y direction
     */ 
    float getMinYAll();
    /**
     * get minimum of all vertices in z direction (also vertices not in the detail level)
     * \return minimum of all vertices in z direction
     */ 
    float getMinZAll();
    /**
     * get maximum of all vertices in x direction (also vertices not in the detail level)
     * \return maximum of all vertices in x direction
     */ 
    float getMaxXAll();
    /**
     * get maximum of all vertices in y direction (also vertices not in the detail level)
     * \return maximum of all vertices in y direction
     */ 
    float getMaxYAll();
    /**
     * get maximum of all vertices in z direction (also vertices not in the detail level)
     * \return maximum of all vertices in z direction
     */ 
    float getMaxZAll();

    /**
     * set miminum of all vertices in x direction (also vertices not in the detail level)
     * \param miminum of all vertices in x direction
     */ 
    void setMinXAll(float t);
    /**
     * set miminum of all vertices in y direction (also vertices not in the detail level)
     * \param miminum of all vertices in y direction
     */ 
    void setMinYAll(float t);
    /**
     * set miminum of all vertices in z direction (also vertices not in the detail level)
     * \param miminum of all vertices in z direction
     */ 
    void setMinZAll(float t);
    /**
     * set maximum of all vertices in x direction (also vertices not in the detail level)
     * \param maximum of all vertices in x direction
     */ 
    void setMaxXAll(float t);
    /**
     * set maximum of all vertices in y direction (also vertices not in the detail level)
     * \param maximum of all vertices in y direction
     */ 
    void setMaxYAll(float t);
    /**
     * set maximum of all vertices in z direction (also vertices not in the detail level)
     * \param maximum of all vertices in z direction
     */ 
    void setMaxZAll(float t);

    /**
     * get the number of triangles in the mesh; must not be equivalent to .size()
     * \return number of triangles in the mesh
     */
    unsigned int getNumber();

    /**
     * get the number of triangles in the mesh; must not be equivalent to .size()
     * \return number of triangles in the mesh
     */
    unsigned int getTriNumber();

    /**
     * get the number of triangles in the original mesh
     * \return number of triangles in the original mesh
     */
    unsigned int getTriOrigNumber();

    /**
     * set the number of triangles in the mesh
     * \param i number of triangles in the mesh
     */
    void setTriNumber(unsigned int i);

    /**
     * get the number of vertices in the mesh; must not be equivalent to .size()
     * \return number of vertices in the mesh
     */
    unsigned int getVerNumber();

    /**
     * set the number of vertices in the mesh
     * \param i number of vertices in the mesh
     */
    void setVerNumber(unsigned int i);

    /**
     * get the number of vertices in the original mesh
     * \return number of vertices in the original mesh
     */
    unsigned int getVerOrigNumber();

    /**
     * get the pointer to the vertex array
     * \return pointer to the vertex array
     */
    svt_vertex_array * getVertexArray();

    /**
     * get the pointer to the triangle array
     * \return pointer to the triangle array
     */
    svt_triangle_array * getTriangleArray();

    /**
     * delete all triangles in the mesh, and therefor all vertices and the progressive mesh
     */ 
    void deleteTriangles();

    /**
     * delete all triangles in the mesh, and therefor all vertices and the progressive mesh
     */ 
    void deleteMesh();

    /**
     * draw the mesh
     */
    virtual void drawGL();

    /**
     * draw the mesh in povray
     */
    virtual void drawPOV();
    /**
     * draw the mesh (OpenInventor)
     * Attention: only correct results with not simplified mesh
     */
    virtual void drawIV();
    /**
     * draw the mesh as yafray xml script
     * Attention: only correct results with not simplified mesh
     */
    virtual void drawYafray();
    /**
     * draw mesh in alias/wavefront obj format
     */
    virtual void drawOBJ();

    /**
     * normalize mesh to expansion of -1..1 in maximal direction
     * \return scaling factor
     */ 
    double normalizeData();

    /**
     * calculate all triangles for each vertex which contain the triangle
     */ 
    void calcAllVerInTri();

    /**
     * calculate the number of mesh components
     * \param verb verbose mode: print out number of triangles in every component
     * \return number of components
     */ 
    int calcCompNumber(bool verb = false);

    /**
     * disable mesh components with triangles less than triAnz
     * \param triAnz number of triangles up to which the components will be deleted
     * \return number of disabled components
     */
    int disableComp(int triAnz);

    /**
     * get the number of mesh components
     * \return number of components
     */ 
    int getCompNumber();

    /**
     * searches for triangle in mesh which lies in outer envelope and near a plane in mesh
     * \param triangle
     * \return triangle index
     */ 
    int searchSnuggleTriangle(svt_vector3<double> oTriangle[3]);

    /**
     * color codes mesh separated through triangle plane
     * \param oTriangle triangle at which the mesh will be separated by color
     * \param checkNorm if true, then only triangles will be separeted, which have e similar normal than the separating triangle
     * \return percentage of separated mesh
     */ 
    double splitAtTriangle(svt_vector3<double> oTriangle[3], bool checkNorm = false);

    /**
     * calculate the number of edges with one, two or more adjacent triangles and save in internal variables
     * \return number of all edges
     */ 
    int calcEdgeNumber();

    /**
     * get the number of edges with one adjacent triangle
     * \return number of edges with one adjacent triangle
     */ 
    int getEdgeNumber1();
    /**
     * get the number of edges with two adjacent triangles
     * \return number of edges with two adjacent triangles
     */ 
    int getEdgeNumber2();
    /**
     * get the number of edges with more than two adjacent triangles
     * \return number of edges with more than two adjacent triangles
     */ 
    int getEdgeNumber3();

    /**
     * calculate all normals at triangles and vertices
     */ 
    void calcAllNormals();

    /**
     * delete all vertices, which are not used from the triangles
     */ 
    void delUnusedVertices();

    // methods for simplification

    /**
     * simplify the mesh and save the result to a progressive mesh
     * reset the mesh to anz vertices after simplification 
     * or with anz=-1 leaves the mesh completely simplified
     * \param anz number of vertices the mesh should have or with anz=-1 leave the mesh completely simplified
     */ 
    void simplify(int anz = -1);
    /**
     * check, if the  mesh is already simplified
     * \return if the mesh is already simplified
     */ 
    bool isSimplified();
    /**
     * check, if options for simplification would changed after simplification was performed
     * \return if an option has changed
     */ 
    bool simpOptionsChanged();
protected:
    //bool setDetailLevel(int level);
    /**
     * update detail level to anz vertices without changing simplification rule
     * \param anz number of vertices the mesh should have
     * \return if the mesh has been changed
     */
    bool setMeshToVerNumber(int anz);
public:
    /**
     * update detail level for achieving boundary condition
     * \return if something was updated
     */ 
    bool updateSimplify();
    /**
     * clear temporary data for simplification
     */ 
    void resetAfterSimplification();
    /**
     * set the mesh to maximal vertex number and clear progressive mesh
     */ 
    void resetSimplification();

    /**
     * save the progressive mesh to file
     * \param saveFile filename to save progressive mesh to
     * \return if saving was successful
     */ 
    bool savePm(const char *saveFile);
    /**
     * read a progressive mesh from file
     * \param readFile filename to read progressive mesh from
     * \return if reading was successful
     */ 
    bool readPm(const char *readFile);

    /**
     * read PLY file
     * \param readFile pointer to filename
     * \return bool if reading was successful
     */ 
    bool readPly(const char *readFile);

    /**
     * calculates field with new vertex numbers for vertices in current mesh
     * \param field with new vertex numbers for vertices in current mesh
     * \return int vertex number realy within triangles
     */
    int calcVerTab(int* verTab);
    /**
     * save SMF file from mesh with current simplification level
     * \param saveFile pointer to filename
     * \return bool if writing was successful
     */
    bool saveSimplSmf(const char *saveFile);
    /**
     * save SMF file from mesh
     * \param readFile pointer to filename
     * \return bool if writing was successful
     */
    bool saveSmf(const char *saveFile);
    /**
     * read SMF file
     * \param readFile pointer to filename
     * \return bool if reading was successful
     */ 
    bool readSmf(const char *readFile);

    /**
     * add a detail level to progressive mesh
     * \param edge edge to be inserted because of contraction
     */ 
    void addPm(svt_edge& edge);
    /**
     * set the mesh to maximal vertex number
     */ 
    void resetMesh();

    /**
     * contract edge to simplify mesh and update all data structures
     * \param edge edge to contract
     * \param itFront iterator to erased edge if edge heap
     * \return 0 if contraction was ok, 1 if contraction causes normal switching, 2 if contraction causes triangle identity
     */ 
    int edgeContract(svt_edge& edge, svt_edgeHeapSet::iterator& itFront);
    /**
     * undo the contraction
     * \param edge edge to reinsert in edge heap
     * \param itFront iterator to erased edge if edge heap
     */ 
    void edgeContractUndo(svt_edge& edge, svt_edgeHeapSet::iterator& itFront);
    /**
     * initialize data structures for simplification
     */ 
    void initQuadricSimplification();
    /**
     * initialize quadrics at vertices for simplification
     */ 
    void initAllQuadric();
    /**
     * penalize vertices at borders for simplification
     * \param edgeHeap reference to edge heap
     * \return number of penalized borders
     */ 
    unsigned int penalizeBorders(svt_edgeHeap& edgeHeap);

    /**
     * enable smooth crossover between detail levels
     */ 
    void enableGeomorph();
    /**
     * disable smooth crossover between detail levels
     */ 
    void disableGeomorph();

    /**
     * enable display of vertex numbers in visualization
     */ 
    void enableShowVertexNumbers();
    /**
     * disable display of vertex numbers in visualization
     */ 
    void disableShowVertexNumbers();
    /**
     * get status of displaying of vertex numbers in visualization
     * \return if display of vertex numbers is enabled
     */ 
    bool getShowVertexNumbers();

    /**
     * set rule for adaptive change of detail level
     * \param sel rule for adaptive change of detail level (0: fps, 1: local error, 2: explicit vertex number, 3: fus)
     */ 
    void setSimplifyRule(int sel);
    /**
     * set minimal fps for rule for adaptive change of detail level
     * \param fps frames per second
     */ 
    void setRuleMinimalFPS(int fps);
    /**
     * set minimal fus for rule for adaptive change of detail level
     * \param fus force updates per second
     */ 
    void setRuleMinimalFUS(int fus);
    /**
     * set maximal local error for rule for adaptive change of detail level
     * \param error local error
     */ 
    void setRuleLocalError(float error);
    /**
     * set exact number of vertices for rule for adaptive change of detail level
     * \param nr number of vertices in simplified mesh
     */ 
    void setRuleVertexNumbers(int nr);
    /**
     * get rule for adaptive change of detail level
     * \return rule for adaptive change of detail level (0: fps, 1: local error, 2: explicit vertex number, 3: fus)
     */ 
    int getSimplifyRule();
    /**
     * get minimal fps for rule for adaptive change of detail level
     * \return frames per second
     */ 
    int getRuleMinimalFPS();
    /**
     * get minimal fus for rule for adaptive change of detail level
     * \return force updates per second
     */ 
    int getRuleMinimalFUS();
    /**
     * get maximal local error for rule for adaptive change of detail level
     * \return local error
     */ 
    float getRuleLocalError();
    /**
     * get exact number of vertices for rule for adaptive change of detail level
     * \return number of vertices in simplified mesh
     */ 
    int getRuleVertexNumbers();

    /**
     * simplify exact to a number of vertices
     * \param i number of vertices in simplified mesh
     * \return if the detail level changed
     */ 
    bool simplifyToVerNumber(unsigned int i);

    /**
     * set flag to penalize vertices at border edges
     * \param b if vertices at border edges should penalized
     */ 
    void setPenalizeBordersFlag(bool b);
    /**
     * get flag to penalize vertices at border edges
     * \return if vertices at border edges should penalized
     */ 
    bool getPenalizeBordersFlag();
    /**
     * set flag to penalize triangle identity
     * \param b if triangle identity should penalized
     */ 
    void setPenalizeIdentityFlag(bool b);
    /**
     * get flag to penalize triangle identity
     * \return if triangle identity should penalized
     */ 
    bool getPenalizeIdentityFlag();
    /**
     * set flag to penalize normal switching
     * \param b if normal switching should penalized
     */ 
    void setPenalizeSwitchFlag(bool b);
    /**
     * get flag to penalize normal switching
     * \return if normal switching should penalized
     */ 
    bool getPenalizeSwitchFlag();
    /**
     * set arc for penalizing normal switching
     * \param d arc for penalizing normal switching
     */ 
    void setPenalizeSwitchArc(double d);
    /**
     * get arc for penalizing normal switching
     * \return arc for penalizing normal switching
     */ 
    double getPenalizeSwitchArc();

    /**
     * set threshold for adding virtual edges
     * \param t threshold
     */ 
    void setThreshold(double t);
    /**
     * get threshold for adding virtual edges
     * \return threshold
     */ 
    double getThreshold();

    /**
     * calculate global erros between original and simplified mesh
     * \param all if global errors to all detail levels should be calculate
     * \param saveFile file to write global errors to all detail levels to
     */ 
    void calcGlobalError(bool all = false, const char *saveFile = ""); //, int step = 1
    /**
     * quadric distance from a vertex to a triangle
     * \param t triangle number in mesh
     * \param v vertex vector
     * \return quadric distance from the vertex v to the triangle number t
     */ 
    double distQuadTriVer(int t, svt_vector3<double>& v);
    /**
     * get maximal global error from original mesh to the current detail level
     * \return maximal global error
     */ 
    double getGlobalErrorMax();
    /**
     * get average squered global error from original mesh to the current detail level
     * \return average squered global error
     */ 
    double getGlobalErrorAvg();
    /**
     * get average global error from original mesh to the current detail level
     * \return average global error
     */ 
    double getGlobalErrorSum();
    /**
     * set maximal global error from original mesh to the current detail level
     * \param err maximal global error
     */ 
    void setGlobalErrorMax(double err);
    /**
     * set average squered global error from original mesh to the current detail level
     * \param err average squered global error
     */ 
    void setGlobalErrorAvg(double err);
    /**
     * set average global error from original mesh to the current detail level
     * \param err average global error
     */ 
    void setGlobalErrorSum(double err);

    /**
     * set the adaption tolerance
     * \param fAdaptionTolerance new tolerance factor
     */
    void setAdaptionTolerance(double fAdaptionTolerance);
    /**
     * get the adaption tolerance
     * \return tolerance factor
     */
    double getAdaptionTolerance();
    /**
     * set the adaption step
     * \param fAdaptionTolerance new adaption step
     */
    void setAdaptionStep(double fAdaptionStep);
    /**
     * get the adaption step
     * \return adaption step
     */
    double getAdaptionStep();

    /**
     * set the adaption time
     * \param fAdaptionTime new adaption time (millisecond)
     */
    void setAdaptionTime(int iAdaptionTime);
    /**
     * get the adaption time
     * \return adaption time (milliseconds)
     */
    int getAdaptionTime();

    /**
     * print the name of the object
     */
    void printName();

protected:

    /**
     * visit all triangles which can be reached from a triangle
     * \param t triangle number from which all neighbour triangles will be visited
     * \param visit vector who shows which triangles already have been visited
     * \return number of triangles in component
     */ 
    int compTest(int t, vector<bool> &visit);

    /**
     * calculate the normal vectors for a triangle
     * \param oTriangle the triangle
     * \return the normal vectors
     */
    Triangle calcTriNormal(const Triangle & oTriangle);

    /**
     * calculate the normal for a triangle
     * \param t triangle number in mesh
     */ 
    void calcTriNormal(int t);

    /**
     * calculate the normal for a vertex
     * \param v vertex number in mesh
     */ 
    void calcVerNormal(int v);

    /****** CONSTANTS ***********************/

    // factor for penalizing border edges
    float m_borderPenalizeFactor;
    // factor for penalizing triangle identity and normal switching
    float m_switchingIdentityPenalizeFactor;
    // value for penalizing triangle identity and normal switching, if quadric is zero
    float m_switchingIdentityPenalizeAdd;

    /****** VARIABLES ***********************/

    // true, if the circular direction of vertices will switch for switching the normal direction for reading PLY file
    bool m_bSwitchNormalDirectionForReadingPly;
    // true, if unused vertices will be deleted after reading smf and ply files
    bool m_bDelUnusedVertices;

    // tolerance in the adaption
    double m_fAdaptionTolerance;
    // adaption step size
    double m_fAdaptionStep;
    // simplify mesh thread
    simplifyMeshThread* m_pThread;
    // adaption time
    int m_iAdaptionTime;

    // triangle vector
    svt_triangle_array m_oTriangles;
    // vertex vector
    svt_vertex_array m_oVertices;
    // edge heap
    svt_edgeHeapSet m_edgeHeapSet;
    // progressive mesh list
    svt_pm m_pm;

    // number of visable triangles
    int m_triNumber;

    // number of mesh components
    int m_compNumber;

    // edges with one adjacent triangle
    int m_edgeNumber1;
    // edges with two adjacent triangles
    int m_edgeNumber2;
    // edges with more than two adjacent triangles
    int m_edgeNumber3;

    // flag if the vertices have their own color (default: false)
    bool m_bVertexColored;

    // flag for showing vertex numbers in visualization
    bool m_showVertexNumbers;

    // flag for successfully loaded font
    bool m_LoadFontOK;

    // flag if surface should be lightend from both sides
    bool m_lightTwoSide;
    // flag if surface should be culled
    bool m_culling;
    // flag if normals should be switched for visu
    bool m_switchNormals;

    // flag if the mesh is already simplified
    bool m_isSimplifiedFlag;
    // flag if simplify option was changed
    bool m_simpOptionsChanged;

    // rule for mesh simplification for adaptive level of detail
    // 0: minimal FPS
    // 1: maximal local error
    // 2: vertex number
    // 3: minimal FUS
    int m_SimplifyRule;
    // minimal FPS for adaptive level of detail
    int m_RuleMinimalFPS;
    // minimal FUS for adaptive level of detail
    int m_RuleMinimalFUS;
    // maximal local error for adaptive level of detail
    float m_RuleLocalError;
    // vertex number for adaptive level of detail
    int m_RuleVertexNumbers;

    // flag for penalizing vertices at border edges
    bool m_penalizeBordersFlag;
    // flag for penalizing triangle identity
    bool m_penalizeIdentityFlag;
    // flag for penalizing normal switching
    bool m_penalizeSwitchFlag;
    // arc from which on penalizing normal switching
    // -1 := 180 grad; 1 := 0 grad between cosinus of normal vectors of triangles
    double m_penalizeSwitchArc;

    // threshold for virtual edges
    double m_edge_threshold;

    // maximal global error
    double m_globalErrorMax;
    // average squered global error
    double m_globalErrorAvg;
    // average global error
    double m_globalErrorSum;

    // minimal value for vertices inside the mesh in x direction
    float m_fMinX;
    // minimal value for vertices inside the mesh in y direction
    float m_fMinY;
    // minimal value for vertices inside the mesh in z direction
    float m_fMinZ;
    // maximal value for vertices inside the mesh in x direction
    float m_fMaxX;
    // maximal value for vertices inside the mesh in y direction
    float m_fMaxY;
    // maximal value for vertices inside the mesh in z direction
    float m_fMaxZ;

    // temporary matrix for marching cube triangle insertion
    // triangle indices of the current and the previous layer of volume
    int *m_pMcTriMat;
    // dimension of mc volume
    int m_iMcXdim;
    int m_iMcYdim;

    // vertex buffer data for the rendering
    bool m_bVAR_Generated;
    vector< svt_n3f_v3f > m_aVAR_Verts;
    vector< unsigned int > m_aVAR_Faces;
};

/**
 * stream out triangle vector
 * \param s output stream
 * \param t triangle vector
 * \return output stream 
 */
inline ostream& operator<< (ostream& s, const svt_triangle_array& t) {
    unsigned int i;
    
    for (i=0; i < t.size(); i++)
        s << "Triangle " << i << ": " << t[i] << endl;
    return s;
}

#endif


