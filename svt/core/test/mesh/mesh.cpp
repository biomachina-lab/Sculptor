#include <svt_scenegraph.h>
#include <svt_light.h>
#include <svt_mesh.h>
#include <svt_rectangle.h>
#include <svt_stlString.h>
#include <time.h>

//#include <svt_core.h>
//#include <svt_sphere.h>
#include <svt_pipette.h>
//#include <svt_pos.h>
#include <svt_sphere.h>
#include <svt_move_tool.h>

#include <svt_scene.h>
#include <svt_system.h>
#include <svt_init.h>
// fuer svt_sleep
#include <svt_time.h>

// fuer situs daten mit marching cube
#include <situs.h>
#include <svt_volume_mc_cline.h>

// globale Variable, ob Animation aufgezeichnet werden soll
bool animate = false;
bool exitProg = false;

double g_threshold;

// from situs package (volio.cpp)
bool readvol(char *vol_file, MYFLT *m_fWidth, MYFLT *m_fGridX, MYFLT *m_fGridY, MYFLT *m_fGridZ, int *m_iExtX, int *m_iExtY, int *m_iExtZ, MYFLT **phi);
void writevol(char *vol_file, MYFLT m_fWidth, MYFLT m_fGridX, MYFLT m_fGridY, MYFLT m_fGridZ, int m_iExtX, int m_iExtY, int m_iExtZ, MYFLT *phi);

void printfVerNumber(svt_mesh* pMesh)
{
    printf("Change vertex number with key/page up/down/left/right, if you have simplified it!\n");
    printf("Actual vertex number: %6d", pMesh->getVerNumber());
}

void printHelp(svt_mesh* pMesh)
{
    // information
    printf("Key usage:\n");
    printf("  1,2,3 - select moving object (complete scene, plane, light)\n");
    printf("  4 - reset transformation of all objects\n");
    printf("  0 - show and calculate separated mesh by plane\n");
    printf("  6,7,8,9 - rotate plane around x and y axis\n");
//     printf("  1,2,3,4,5,6 - moves vertices of clipping plane\n");
    printf("  +,-,,,. - moves clipping plane fast and slow in z direction\n");
    printf("  e,E - toggle if only triangles will be separeted, which have a similar normal than the separating plane\n");
//     printf("  f,F - swap clipping plane\n");
//     printf("  b,B - search for clipping plane\n");
    printf("  j,J - print info about components\n");
    printf("  i,I - disable components\n");
    printf("  c,C - switch background color to black/white\n");
    printf("  a,A - save simplify-animated screenshots\n");
    printf("  d,D - save single screenshot\n");
    printf("  s,S - switch illumination between flat/goraud\n");
    printf("  w,W - switch visualization between wireframe/shape\n");
    printf("  g,G - toggle between one and two sided lightning\n");
    printf("  t-k, z-o, u-l - move light in x, y, z direction\n");
    printf("  p,P - print mesh triangles and vertices to stdout\n");
    printf("  v,V - show triangle and vertex indices of mesh\n");
    printf("  y,Y - save mesh in xml-yafray format to svt_mesh.xml\n");
    printf("  r,R - save mesh in povray format to svt_mesh.pov\n");
    printf("  n,N - save mesh without simplification\n");
    printf("  m,M - save mesh with actual simplification level\n");
    printf("  h,H - print this help message\n");
    printf("  x,X,q,Q - quit program\n\n");
    printfVerNumber(pMesh);
}

// class lightsourceSphere: public svt_lightsource
// {
// protected:
//     svt_sphere* light_sphere;
// public:
//     lightsourceSphere(svt_pos* pPos, bool bInfinite): svt_lightsource(pPos, bInfinite)
//     {
//         light_sphere = new svt_sphere(pPos, new svt_properties(new svt_color(1.,1.,1.)), .1);
//     }
// //     /**
// //      * need this function due to some technical reasons (opengl doesnt like some vertex array functions inside a display list)
// //      */
// //     void outputGL()
// //     {
// //         svt_node::outputGL();
// //     }    
//     void drawGL()
//     {
//     GLUquadricObj* m_pQuadric;
// m_pQuadric = gluNewQuadric();
//         svt_lightsource::drawGL();
// //         cout << endl << getTransformation()->getTranslationX() <<endl;
// //         cout << light_sphere->getTransformation()->getTranslationX() <<endl;
// //         light_sphere->setTransformation(getTransformation());
// //         cout << light_sphere->getTransformation()->getTranslationX() <<endl;
//         applyGL();
//         gluSphere(m_pQuadric,0.1, 16, 16);

//         //light_sphere->applyGL();
//         light_sphere->drawGL();
//     }
// };

void planeTrans(svt_matrix4f *trans, svt_vector3<double> &vec, svt_vector3<double> &res)
{
    int i;

    for (i=0; i < 3; i++)
        res[i] = 0;
    for (i=0; i < 4; i++)
        for (int j=0; j < 3; j++)
            res[i] += (*trans)(j, i)*vec[j];
    for (i=0; i < 3; i++)
        res[i] += (*trans)(3, i);
}

// mesh_scene class
class mesh_scene : public svt_scene
{
protected:
    svt_mesh* m_pMesh;
    svt_lightsource* m_pSource;
    svt_rectangle *m_pPlane;
    svt_move_tool* m_tool;
    svt_node* m_light_cont;
    svt_pos* m_pLightPos;

public:
    mesh_scene(svt_mesh &mesh) : svt_scene() 
    {
        m_pMesh = &mesh;
    };

public:

    void buildScene()
    {
        m_pSceneGraph = new svt_scenegraph();

        svt_color* yellow = new svt_color(1.0f,1.0f,0.0f);
        m_pMesh->getProperties()->setSmooth(false);
        m_pMesh->getProperties()->setColor(yellow);

//         mat->setToId();
//         // Rotate
// //         mat->postAddRotation(1,-PI/2.); // Rotation around y-axis (x-axis: 0)
// //         mat->postAddRotation(2,PI/2.); // Rotation around z-axis
//          mat->setTranslationX(-250);
//          mat->setTranslationY(-250);
//          mat->setTranslationZ(-32);
//          //         mat->preAddScale(10);
//         m_pMesh->setTransformation(mat);

//         svt_matrix4f* pMatrix = pNode->getTransformation();
//         pMatrix->setToId();
//         svt_vector4f oVec = pDoc->getRealPos();
//         oVec.multScalar( 1.0E8f );
//         pMatrix->setTranslationX( -oVec.x() );
//         pMatrix->setTranslationY( -oVec.y() );
//         pMatrix->setTranslationZ( -oVec.z() );

//         svt_vector4<Real32> oOrigin( oVec.x(), oVec.y(), oVec.z() );

        // create a tool
        m_tool = new svt_move_tool("Pointer", m_pSceneGraph); // m_pMesh, m_pSceneGraph
        //m_tool->setCenterMode(true);
//         svt_vector4<Real32> oOrigin( 250., 250., 32. );
//         tool->setPivotPoint( oOrigin );
// //         svt_matrix4f* pMatrix = tool->getGlobalMatrix();
// //         pMatrix->preAddScale(10);
// //         tool->setGlobalMatrix(pMatrix);
        m_pSceneGraph->add(m_tool);

        // Lighting
        //        m_light_cont = new svt_node(new svt_pos(0.0f,2.0f,4.0f));
        //svt_sphere* light_sphere = new svt_sphere(new svt_pos(0.0f,0.0f,0.0f), new svt_properties(new svt_color(1.,1.,1.)), .1);
        //        m_light_cont->add(light_sphere);
        svt_light* light = new svt_light();
        m_pLightPos = new svt_pos(0.0f,2.0f,4.0f); // muss identisch mit naechster zeile sein
        m_pSource = new svt_lightsource(new svt_pos(0.0f,2.0f,4.0f), true); // true fuer unendlich weit weg //lightsourceSphere
        light->add(m_pSource);
        //        m_light_cont->add(m_pSource);
        setLight(light);
        //        m_pSceneGraph->add(m_pSource);
        //m_pSource->svt_node::add(light_sphere);

        // put mesh into the sg
        m_pSceneGraph->add(m_pMesh);

        // plane
        svt_properties* pPropPlane = new svt_properties(new svt_color(0.9,0.9,1.));
        pPropPlane->setLighting(false);
        pPropPlane->getColor()->setTransparency(50./100.);
//         pPropPlane->setSolid(true);
//         pPropPlane->setWireframe(getProperties()->getWireframe());
        m_pPlane = new svt_rectangle(new svt_pos(0.,0.,0.), pPropPlane,3.,3.);    
        m_pSceneGraph->add(m_pPlane);
    };

    void keyPressed(unsigned char key, int, int)
    {
        ofstream oFile;
	ofstream oFileOBJ;
	ofstream oFileMAT;
        streambuf* pBuffer = cout.rdbuf();
        static int bestTri;
        static double epsStep = ((m_pMesh->getMaxX() - m_pMesh->getMinX())+(m_pMesh->getMaxY() - m_pMesh->getMinY())+(m_pMesh->getMaxZ() - m_pMesh->getMinZ()))/100.;
        static svt_vector3<double> oTriangle[3];
        static svt_vector3<double> tmp;
        static svt_vector3<double> norm;
        static double edgeStep = 0.;
        static bool checkNorm = false;
        svt_matrix4f *trans = m_pPlane->getTransformation();
        svt_vector3<double> e1(1, 0, 0);
        svt_vector3<double> e2(0, 1, 0);
        svt_vector3<double> e3(0, 0, 0);
        
        switch(key)
        {
        case '1':
            m_tool->setMoveNode(m_pSceneGraph);
            break;
        case '2':
            m_tool->setMoveNode(m_pPlane);
            break;
        case '3':
            m_tool->setMoveNode(m_pSource);
            break;
        case '4':
            m_pSceneGraph->getTransformation()->setToId();
            m_pPlane->getTransformation()->setToId();
            m_pSource->getTransformation()->setFromTranslation(m_pLightPos->getX(), m_pLightPos->getY(), m_pLightPos->getZ());
            m_pMesh->rebuildDL();
            break;
//         case '1':
//             oTriangle[0] -= edgeStep*norm; oTriangle[1] += edgeStep*norm; oTriangle[2] += edgeStep*norm; 
//             break;
//         case '2':
//             oTriangle[0] += edgeStep*norm; oTriangle[1] -= edgeStep*norm; oTriangle[2] -= edgeStep*norm; 
//             break;
//         case '3':
//             oTriangle[0] += edgeStep*norm; oTriangle[1] -= edgeStep*norm; oTriangle[2] += edgeStep*norm; 
//             break;
//         case '4':
//             oTriangle[0] -= edgeStep*norm; oTriangle[1] += edgeStep*norm; oTriangle[2] -= edgeStep*norm; 
//             break;
//         case '5':
//             oTriangle[0] += edgeStep*norm; oTriangle[1] += edgeStep*norm; oTriangle[2] -= edgeStep*norm;
//             break;
//         case '6':
//             oTriangle[0] -= edgeStep*norm; oTriangle[1] -= edgeStep*norm; oTriangle[2] += edgeStep*norm;
//             break;
        case '6':
            m_pPlane->getTransformation()->postAddRotation(0, .001);
            m_pMesh->rebuildDL();
            break;
        case '7':
            m_pPlane->getTransformation()->postAddRotation(0, -.001);
            m_pMesh->rebuildDL();
            break;
        case '8':
            m_pPlane->getTransformation()->postAddRotation(1, .001);
            m_pMesh->rebuildDL();
            break;
        case '9':
            m_pPlane->getTransformation()->postAddRotation(1, -.001);
            m_pMesh->rebuildDL();
            break;
        case '+':
            m_pPlane->getTransformation()->setTranslationZ(m_pPlane->getTransformation()->getTranslationZ()+.01);
            m_pMesh->rebuildDL();
            //oTriangle[0] -= epsStep*norm; oTriangle[1] -= epsStep*norm; oTriangle[2] -= epsStep*norm; 
            break;
        case '-':
            m_pPlane->getTransformation()->setTranslationZ(m_pPlane->getTransformation()->getTranslationZ()-.01);
            m_pMesh->rebuildDL();
            //oTriangle[0] += epsStep*norm; oTriangle[1] += epsStep*norm; oTriangle[2] += epsStep*norm; 
            break;
        case '.':
            m_pPlane->getTransformation()->setTranslationZ(m_pPlane->getTransformation()->getTranslationZ()+.0005);
            m_pMesh->rebuildDL();
            //oTriangle[0] -= epsStep/30.*norm; oTriangle[1] -= epsStep/30.*norm; oTriangle[2] -= epsStep/30.*norm; 
            break;
        case ',':
            m_pPlane->getTransformation()->setTranslationZ(m_pPlane->getTransformation()->getTranslationZ()-.0005);
            m_pMesh->rebuildDL();
            //oTriangle[0] += epsStep/30.*norm; oTriangle[1] += epsStep/30.*norm; oTriangle[2] += epsStep/30.*norm; 
            break;
        case '0': 
            cout << endl;
            planeTrans(trans, e1, oTriangle[1]);
            planeTrans(trans, e2, oTriangle[0]);
            planeTrans(trans, e3, oTriangle[2]);
            cout << "Percentage of separated red vertices: " << 100*m_pMesh->splitAtTriangle(oTriangle, checkNorm) << endl;
            m_pMesh->rebuildDL();
            printfVerNumber(m_pMesh);
            break;
//         case 'b':
//         case 'B':
//             cout << endl;
//             cout << "Search for clipping plane... " << (bestTri = m_pMesh->searchSnuggleTriangle(oTriangle)) << endl;
//             norm = crossProduct(oTriangle[0] - oTriangle[2], oTriangle[1] - oTriangle[2]).normalize();
//             edgeStep = ((oTriangle[1] - oTriangle[0]).length() + (oTriangle[2] - oTriangle[1]).length() + (oTriangle[0] - oTriangle[2]).length())/3000.;
//             oTriangle[0] -= epsStep*norm; oTriangle[1] -= epsStep*norm; oTriangle[2] -= epsStep*norm;
//             cout << "Percentage of separated red vertices: " << 100*m_pMesh->splitAtTriangle(oTriangle, checkNorm) << endl;
//             m_pMesh->rebuildDL();
//             printfVerNumber(m_pMesh);
//             break;
        case 'e':
        case 'E':
            checkNorm = !checkNorm;
            break;
//         case 'f':
//         case 'F':
//             tmp = oTriangle[0];
//             oTriangle[0] = oTriangle[1];
//             oTriangle[1] = tmp;
//             break;
        case 'j':
        case 'J':
            cout << endl;
            cout << "Number of components in the possibly simplified mesh: " << m_pMesh->calcCompNumber(true) << endl;
            printfVerNumber(m_pMesh);
            break;
        case 'h':
        case 'H':
            cout << endl;
            printHelp(m_pMesh);
            break;
        case 'i':
        case 'I':
            int disAnz;
            cout << endl << "Insert number of triangles for deleting a component: ";
            cin >> disAnz;
            cout << "  Number of disabled components with triangles lower equal " << disAnz << " triangles: " << m_pMesh->disableComp(disAnz) << endl;
            m_pMesh->rebuildDL();
            cout << "  Now simplified meshes are not allowed to change vertex number anymore (save mesh with 'm')." << endl;
            printfVerNumber(m_pMesh);
            break;
        case 'n':
        case 'N':
                m_pMesh->saveSmf("svt_mesh_complete.smf");
            break;
        case 'm':
        case 'M':
            {
                char zahl[12] = "";
                sprintf(zahl, "%d", (int)m_pMesh->getVerNumber());
                string smfSimpl_str(zahl);
                string smfSimpl_str2("svt_mesh_");
                smfSimpl_str.append("v.smf");
                m_pMesh->simplifyToVerNumber((int)m_pMesh->getVerNumber()); // stellt zusaetzlich die Regel auf exakte knotenwerte
                m_pMesh->saveSimplSmf((smfSimpl_str2 + smfSimpl_str).c_str());
                //m_pMesh->saveSimplSmf((file.substr(0, sz-4) + smfSimpl_str).c_str());
            }
            break;
        case 'c':
        case 'C':
            // switch color between white and black
            {
            svt_color* pColor = svt_getBackgroundColor();
            if (pColor == NULL){
                pColor = new svt_color();
                svt_setBackgroundColor(pColor);
            }
            if (pColor->getR() == 0.)
            {
                pColor->setR(1.);
                pColor->setG(1.);
                pColor->setB(1.);
            }
            else
            {
                pColor->setR(0.);
                pColor->setG(0.);
                pColor->setB(0.);
            }
            }            
            break;
        case 'a':
        case 'A':
            // animate screenshots
            animate = !animate;
            break;
        case 'd':
        case 'D':
            svt_saveScreenshot(NULL);
            break;
        case 's':
        case 'S':
            // toggle 
            m_pMesh->getProperties()->setSmooth(!m_pMesh->getProperties()->getSmooth());
            m_pMesh->rebuildDL();
            break;
        case 'w':
        case 'W':
            m_pMesh->getProperties()->setWireframe(!m_pMesh->getProperties()->getWireframe());
            m_pMesh->rebuildDL();
            break;
        case 'g':
        case 'G':
            m_pMesh->setLightTwoSide(!m_pMesh->getLightTwoSide());
            m_pMesh->rebuildDL();
            break;
        case 'p':
        case 'P':
            cout << endl << "Mesh:" << endl;
            cout << *(m_pMesh->getTriangleArray()) << endl;
            cout << *(m_pMesh->getVertexArray()) << endl;
            break;
        case 'v':
        case 'V':
            // tuts nicht
            if (m_pMesh->getShowVertexNumbers())
            {
                cout << "Display Vertex Numbers off." << endl;
                m_pMesh->disableShowVertexNumbers();
                m_pMesh->rebuildDL();
            }
            else
            {
                cout << "Display Vertex Numbers on." << endl;
                m_pMesh->enableShowVertexNumbers();
                m_pMesh->rebuildDL();
            }
            break;
        case 'y':
        case 'Y':
            oFile.open("svt_mesh.xml");
            cout.rdbuf(oFile.rdbuf());
            outputYafray();
            cout.rdbuf(pBuffer);
            oFile.close();
            break;
        case 'r':
        case 'R':
            oFile.open("svt_mesh.pov");
            cout.rdbuf(oFile.rdbuf());
            outputPOV(); // true as parameter would turn on radiosity which takes a long time to generate picture
            cout.rdbuf(pBuffer);
            oFile.close();
	    break;
        case 't':
        case 'T':
            (m_pSource->getTransformation())->postAddTranslation(.1, 0, 0);
            m_pMesh->rebuildDL();
            break;
        case 'k':
        case 'K':
            (m_pSource->getTransformation())->postAddTranslation(-.1, 0, 0);
            m_pMesh->rebuildDL();
            break;
        case 'z':
        case 'Z':
            (m_pSource->getTransformation())->postAddTranslation(0, .1, 0);
            m_pMesh->rebuildDL();
            break;
        case 'o':
        case 'O':
            (m_pSource->getTransformation())->postAddTranslation(0, -.1, 0);
            m_pMesh->rebuildDL();
            break;
        case 'u':
        case 'U':
            (m_pSource->getTransformation())->postAddTranslation(0, 0, .1);
            m_pMesh->rebuildDL();
            break;
        case 'l':
        case 'L':
            (m_pSource->getTransformation())->postAddTranslation(0, 0, -.1);
            m_pMesh->rebuildDL();
            break;
        case 'x':
        case 'X':
        case 'q':
        case 'Q':
            exitProg = true;
            break;
        }
    };

    void specialKeyPressed(int key, int, int)
    {
        switch(key)
        {
        case SVT_KEY_RIGHT:
            //m_pSceneMat->preAddRotation(1,0.01f);
            m_pMesh->simplifyToVerNumber(m_pMesh->getVerNumber()+1);
            break;

        case SVT_KEY_LEFT:
            //m_pSceneMat->preAddRotation(1,-0.01f);
            m_pMesh->simplifyToVerNumber(m_pMesh->getVerNumber()-1);
            break;

        case SVT_KEY_UP:
            m_pMesh->simplifyToVerNumber(m_pMesh->getVerNumber()+m_pMesh->getVerOrigNumber()/100);
            //m_pSceneMat->postAddTranslation(0.0f, 0.0f, 0.01f);
            break;

        case SVT_KEY_DOWN:
            m_pMesh->simplifyToVerNumber(m_pMesh->getVerNumber()-m_pMesh->getVerOrigNumber()/100);
            //m_pSceneMat->postAddTranslation(0.0f, 0.0f, -0.01f);
            break;

        case SVT_KEY_PAGE_UP:
            //m_pSceneMat->postAddTranslation(0.0f, 0.01f, 0.0f);
            m_pMesh->simplifyToVerNumber(m_pMesh->getVerNumber()+m_pMesh->getVerOrigNumber()/10);
            break;

        case SVT_KEY_PAGE_DOWN:
            //m_pSceneMat->postAddTranslation(0.0f, -0.01f, 0.0f);
            m_pMesh->simplifyToVerNumber(m_pMesh->getVerNumber()-m_pMesh->getVerOrigNumber()/10);
            break;
        }   
        m_pMesh->rebuildDL();
        printf("\b\b\b\b\b\b%6d", m_pMesh->getVerNumber()); 
    };
};

// main
int main(int argc, char *argv[])
{

    if (argc > 1)
    {
        svt_mesh* pMesh = new svt_mesh();
        bool readOk;
        double t;
        int pen;
        int comp;
        clock_t start, finish;

        string file(argv[1]);
        string pm_str("pm");
        unsigned int sz = file.length();

        char fileToAnim[256];

        if ((sz > 2) && (file.substr(sz-3, sz-1) == "smf"))
        {
            if ((argc > 5) && (sscanf(argv[5], "%d", &comp) == 1) && ((comp == 0) || (comp == 1)))
            {
                pMesh->setCompareFlag((bool) comp);
            } else  if (argc > 5) {
                cout << "Warning: Comparison of identical vertices con only by 0 (do not compare - default) or 1 (compare)!" << endl;
            }
            
            start = clock();

            //int i;cin>>i; // um zu stoppen und hauptspeicherplatz ablesen
            // pMesh->setDelUnusedVertices(true); // for cow which has 1 unused vertices
            if (readOk = pMesh->readSmf(argv[1]))
            {    
                // rueck um 0, aber schlecht fuer flaechenberechnungen
                cout << "Normalizing Data with Scaling factor of: " << pMesh->normalizeData() << endl;

                finish = clock();
                cout << "Time for reading SMF: " << (double)(finish - start) / CLOCKS_PER_SEC  << "s" << endl;
                if ((argc > 2) && (sscanf(argv[2], "%lf", &t) == 1))
                {
                    cout << "Set threshold to: " << t << endl;
                    pMesh->setThreshold(t);
                    g_threshold = t;
                }
                if ((argc > 4) && (sscanf(argv[4], "%d", &pen) == 1) && ((pen >= 0) && (pen <= 7)))
                {
                    cout << "Set penalty to: " << pen << " (";
                    if (!(pen&4))
                    {
                        pMesh->setPenalizeBordersFlag(false);
                        cout << "border off, ";
                    } else {
                        cout << "border on, ";
                    }
                    if (!(pen&2))
                    {
                        pMesh->setPenalizeIdentityFlag(false);
                        cout << "identity off, ";
                    } else {
                        cout << "identity on, ";
                    }
                    if (!(pen&1))
                    {
                        pMesh->setPenalizeSwitchFlag(false);
                        cout << "switch off)" << endl;
                    } else {
                        cout << "switch on)" << endl;
                    }
                } else  if (argc > 4) {
                    cout << "Warning: Penalty must be between 0 and 7!" << endl;
                }

                // so wuerde das progrssive mesh auf Datei ausgegeben werden:
                //start = clock();
                //if (pMesh->savePm((file.substr(0, sz-3) + pm_str).c_str()))
                //{
                //    finish = clock();
                //    cout << "Time for saving PM: " << (double)(finish - start) / CLOCKS_PER_SEC  << "s" << endl;
                //} else {
                //    cout << "Error writing PM!" << endl;
                //}

                // alle 100 vereinfachungsstufen fehler berechnen
                //for (int i = 500; i>=0; i-=50)
                //{
                //    pMesh->simplifyToVerNumber(i);
                //    pMesh->calcGlobalError();
                //}
                //pMesh->simplifyToVerNumber(177);
                //pMesh->calcGlobalError();
            }
        }
        else if ((sz > 1) && (file.substr(sz-2, sz-1) == "pm"))
        {
            start = clock();

            if (readOk = pMesh->readPm(argv[1]))
            {
                finish = clock();
                cout << "Time for reading PM: " << (double)(finish - start) / CLOCKS_PER_SEC  << "s" << endl;

                pMesh->resetSimplification();
                // auch die PM Daten koennen, muessten aber nicht, 
                // erneut simplifiziert werden (zB um andere Parameter zu testen):
                if ((argc > 2) && (sscanf(argv[2], "%lf", &t) == 1))
                {
                    cout << "Set threshold to: " << t << endl;
                    pMesh->setThreshold(t);
                    g_threshold = t;
                }
                if ((argc > 4) && (sscanf(argv[4], "%d", &pen) == 1) && ((pen >= 0) && (pen <= 7)))
                {
                    cout << "Set penalty to: " << pen << " (";
                    if (!(pen&4))
                    {
                        pMesh->setPenalizeBordersFlag(false);
                        cout << "border off, ";
                    } else {
                        cout << "border on, ";
                    }
                    if (!(pen&2))
                    {
                        pMesh->setPenalizeIdentityFlag(false);
                        cout << "identity off, ";
                    } else {
                        cout << "identity on, ";
                    }
                    if (!(pen&1))
                    {
                        pMesh->setPenalizeSwitchFlag(false);
                        cout << "switch off)" << endl;
                    } else {
                        cout << "switch on)" << endl;
                    }
                } else if (argc > 4) {
                    cout << "Warning: Penalty must be between 0 and 7!" << endl;
                }
            }
        }
        else if ((sz > 1) && (file.substr(sz-3, sz-1) == "ply"))
        {
            start = clock();

            //pMesh->switchNormalDirectionForReadingPly();
            //pMesh->setDelUnusedVertices(true);
            //int i;cin>>i; // um zu stoppen und hauptspeicherplatz ablesen
            if (readOk = pMesh->readPly(argv[1]))
            {
                finish = clock();
                cout << "Time for reading PLY: " << (double)(finish - start) / CLOCKS_PER_SEC  << "s" << endl;
                if ((argc > 2) && (sscanf(argv[2], "%lf", &t) == 1))
                {
                    cout << "Set threshold to: " << t << endl;
                    pMesh->setThreshold(t);
                    g_threshold = t;
                }
                if ((argc > 4) && (sscanf(argv[4], "%d", &pen) == 1) && ((pen >= 0) && (pen <= 7)))
                {
                    cout << "Set penalty to: " << pen << " (";
                    if (!(pen&4))
                    {
                        pMesh->setPenalizeBordersFlag(false);
                        cout << "border off, ";
                    } else {
                        cout << "border on, ";
                    }
                    if (!(pen&2))
                    {
                        pMesh->setPenalizeIdentityFlag(false);
                        cout << "identity off, ";
                    } else {
                        cout << "identity on, ";
                    }
                    if (!(pen&1))
                    {
                        pMesh->setPenalizeSwitchFlag(false);
                        cout << "switch off)" << endl;
                    } else {
                        cout << "switch on)" << endl;
                    }
                } else  if (argc > 4) {
                    cout << "Warning: Penalty must be between 0 and 7!" << endl;
                }

                // so wuerde das progrssive mesh auf Datei ausgegeben werden:
                //start = clock();
                //if (pMesh->savePm((file.substr(0, sz-3) + pm_str).c_str()))
                //{
                //    finish = clock();
                //    cout << "Time for saving PM: " << (double)(finish - start) / CLOCKS_PER_SEC  << "s" << endl;
                //} else {
                //    cout << "Error writing PM!" << endl;
                //}

                // alle 100 vereinfachungsstufen fehler berechnen
                //for (int i = 500; i>=0; i-=50)
                //{
                //    pMesh->simplifyToVerNumber(i);
                //    pMesh->calcGlobalError();
                //}
                //pMesh->simplifyToVerNumber(177);
                //pMesh->calcGlobalError();
            }
        }
        else if ((sz > 1) && (file.substr(sz-5, sz-1) == "situs"))
        {
            svt_volume_mc_cline<Real64>* m_pVolumeMC;
            svt_volume<Real64> m_cVolume;

            m_cVolume.loadSitus((char*)argv[1]);
            readOk = true;

            m_pVolumeMC = new svt_volume_mc_cline<Real64>(m_cVolume);

            if ((argc > 2) && (sscanf(argv[2], "%lf", &t) == 1))
            {
                cout << "Set isolevel to: " << t << endl;
                m_pVolumeMC->setIsolevel(t); // threshold wird als isoleval missbraucht
                g_threshold = t;
            }
            else
            {
                cout << "Set isolevel to: 9. (default)" << endl;
                m_pVolumeMC->setIsolevel(9.);
            }
            m_pVolumeMC->setStep(1);
            m_pVolumeMC->recalcMesh(); // generate mesh
//             cout << *(m_pVolumeMC->getMesh()->getTriangleArray()) << endl;
//             cout << *(m_pVolumeMC->getMesh()->getVertexArray()) << endl;

            pMesh = m_pVolumeMC->getMesh();
            readOk = true;
        }
        else 
        {
            cout << "Please specify a .smf, .pm, .ply or .situs file!";
            readOk = false;
        }

        if (readOk)
        {
            if ((argc > 3) && (sscanf(argv[3], "%lf", &t) == 1))
            {
                pMesh->simplify();
                pMesh->simplifyToVerNumber((int)t);
//                 // Fehlerberechnung tut es im Moment nicht ?!
//                 if (t<0)
//                 {
//                     cout << "Calculate global error for every level and write the result to globalError.txt!" << endl;
//                     pMesh->calcGlobalError(true,"globalError.txt");
//                 } 
//                 else 
//                 {
//                     cout << "Number of vertices for error calculation: " << t << endl;
//                     pMesh->simplifyToVerNumber(t);
//                     if (pMesh->getVerNumber() != t)
//                         cout << "  to small; minimal vertex number set to: " << pMesh->getVerNumber() << endl;
//                     pMesh->calcGlobalError();
//                 }
            }
            cout << "Number of triangles in the possibly simplified mesh : " << pMesh->getTriNumber() << endl;
            cout << "Number of edges in the possibly simplified mesh     : " << pMesh->calcEdgeNumber() << endl;
            cout << "    Edges with 1 adjacent triangle                  : " << pMesh->getEdgeNumber1() << endl;
            cout << "    Edges with 2 adjacent triangles                 : " << pMesh->getEdgeNumber2() << endl;
            cout << "    Edges with more than 2 adjacent triangles       : " << pMesh->getEdgeNumber3() << endl;
            pMesh->calcMinMax();
            cout << "Size of the possibly simplified mesh in x: " << pMesh->getSizeX()
                 << " (min: " << pMesh->getMinX() << ", max: " << pMesh->getMaxX() << ")" << endl;
            cout << "                                     in y: " << pMesh->getSizeY()
                 << " (min: " << pMesh->getMinY() << ", max: " << pMesh->getMaxY() << ")" << endl;
            cout << "                                     in z: " << pMesh->getSizeZ()
                 << " (min: " << pMesh->getMinZ() << ", max: " << pMesh->getMaxZ() << ")" << endl;

            cout << "Surface size: " << pMesh->surfaceSize() << endl << endl;

//             cout << *(pMesh->getVertexArray()) << endl;

            // init svt
            svt_init(1, argv); //argc,argv,(char **) NULL
            // create a new scene object
            mesh_scene oScene(*pMesh);
            // tool_scene* scene = new mesh_scene();
        
            // and build the scene
            oScene.buildScene();

//             // transformation matrix
//             svt_matrix4f* pSceneMat = oScene.getTransformation();
//             pSceneMat->setTranslationX(-250);
//             pSceneMat->setTranslationY(-250);
//             pSceneMat->setTranslationZ(-32);
//             oScene.setTransformation(pSceneMat);

            printHelp(pMesh);

            // set scene
            svt_setScene(&oScene);

            pMesh->rebuildDL();
            // main loop
            int oldVerNumber=pMesh->getVerOrigNumber()+1;
            while(!exitProg)
            {
                svt_system_update();
                if (animate && (pMesh->getVerNumber() < oldVerNumber))
                {
                    sprintf(fileToAnim,"%07i.tga",(oldVerNumber=pMesh->getVerNumber()));
                    cout << "Save Screenshot to file " << fileToAnim << ".\n";
                    svt_setStop(true);
                    svt_saveScreenshot(fileToAnim);
                    svt_setStop(false);
                    pMesh->simplifyToVerNumber(pMesh->getVerNumber()/1.01); //logarithmische verkleinerung
                    pMesh->rebuildDL();
                } else {
                    animate = false;
                    oldVerNumber=pMesh->getVerOrigNumber()+1;
                }
            }
            // folgende zeile laesst einen nicht mehr in mainloop eingreifen
            //svt_start(&oScene);
        }
//         {
//             // animate screenshots
//             char file[256];
//             char wait;
//             cout << endl;
//             while (pMesh->getVerNumber()>2)
//             {
//                 sprintf(file,"svt_screenshot%07i.tga",pMesh->getVerNumber());
//                 cout << "Save Screenshot to file " << file << endl;
//                 //svt_setStop(true);
//                 svt_saveScreenshot(file);
//                 pMesh->rebuildDL();
//                 //for (int i=0; i<100000;i++)
//                 //    svt_sleep(150);
//                 //svt_sleep(1500000);
//                 //svt_setStop(false);
//                 cin >> wait;
//                 pMesh->simplifyToVerNumber(pMesh->getVerNumber()/2);
//                 //svt_redraw();
//             }
//         }
    }
    else
        cout << "Usage: mesh file.smf | file.pm | file.ply | file.situs [threshold | isolevel] [verNumberToSimplify | -1] [penalty] [compare]";
    
    return 0;
}
