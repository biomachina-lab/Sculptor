
#include <svt_core.h>
#include <svt_init.h>
#include <svt_scene.h>
#include <svt_container.h>
#include <svt_move_tool.h>
#include <svt_pipette.h>

class container_scene : public svt_scene
{
public:
    container_scene() : svt_scene() { };

public:
    // build the scene
    void buildScene()
    {
        // create scenegraph
        svt_scenegraph* pGraph = new svt_scenegraph();

        // create svt_basic_container
	svt_container* pContainer = new svt_container();
	svt_c3f_v3f oVecCV_A, oVecCV_B, oVecCV_C;
	svt_color oCol(0.0,0.0,0.8f);

	Real32 fRadius = 3;
	svt_color * pColor = new svt_color(1,0,0);
	svt_color * pColor2 = new svt_color(0,1,0);
	svt_color * pColor3 = new svt_color(0,0,1);
    
	svt_vector4<Real32> pTail;
	svt_vector4<Real32> pHead;
	Real32 fRho = 0.70;
	
	//x
	pTail.set(0*1.e-2,0*1.e-2,0*1.e-2);
	pHead.set(40*1.e-2,0*1.e-2, 0*1.e-2);
    
	pContainer->addSphere(pTail.x(), pTail.y(), pTail.z(), fRadius*1.e-2,*pColor);
	pContainer->addCylinder(pTail, pTail + fRho*(pHead - pTail), fRadius*1.e-2,*pColor,16);
	pContainer->addCone(pTail + fRho*(pHead - pTail), pHead, fRadius*2.5e-2,*pColor,16);
	
	//y
	pTail.set(0*1.e-2,0*1.e-2,0*1.e-2);
	pHead.set(0*1.e-2,40*1.e-2, 0*1.e-2);
    
	pContainer->addSphere(pTail.x(), pTail.y(), pTail.z(), fRadius*1.e-2,*pColor2);
	pContainer->addCylinder(pTail, pTail + fRho*(pHead - pTail), fRadius*1.e-2,*pColor2,16);
	pContainer->addCone(pTail + fRho*(pHead - pTail), pHead, fRadius*2.5e-2,*pColor2,16);
	
	//z
	pTail.set(0*1.e-2,0*1.e-2,0*1.e-2);
	pHead.set(0*1.e-2,0*1.e-2, 40*1.e-2);
	
	pContainer->addSphere(pTail.x(), pTail.y(), pTail.z(), fRadius*1.e-2,*pColor3);
	pContainer->addCylinder(pTail, pTail + fRho*(pHead - pTail), fRadius*1.e-2,*pColor3,16);
	pContainer->addCone(pTail + fRho*(pHead - pTail), pHead, fRadius*2.5e-2,*pColor3,16);
	
	
	oCol.set( 1.0f, 0.0f, 0.0f );
	pContainer->addSphere( 0.0, 0.0, 0.0, 0.05f, oCol );

        pGraph->add( pContainer );

        // create a lightsource
	svt_light* pLight = new svt_light();
	svt_lightsource* m_pLightSource = new svt_lightsource(new svt_pos(0.0f,0.0f,2.0f));
	m_pLightSource->setInfinite( false );
        pLight->add( m_pLightSource );
        setLight( pLight );

        // create property
        svt_properties* pWhite = new svt_properties( new svt_color(1.0f,1.0f,1.0f) );

        // create a tool
        svt_move_tool* pTool = new svt_move_tool("Pointer", pContainer);
        pTool->setCenterMode(true);
        pTool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), pWhite));
        pGraph->add(pTool);

        setSceneGraph(pGraph);
    };

    /**
     * print out frame per second counter
     */
    virtual void drawGL()
    {
        //printf("fps: %d   \r", svt_getFPS());

        svt_scene::drawGL();
    }

    /**
     * apply some opengl settings here
     */
    virtual void prepareGL()
    {
        // no local lighting and two side lighting
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

        Real32 m_aMatSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        Real32 m_aMatShininess[1] = { 20.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, m_aMatSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, m_aMatShininess);
    };
};

// main
int main(int argc, char *argv[])
{
    // init svt
    svt_init(argc,argv);

    // create a new scene object
    container_scene* pScene = new container_scene();
    // and build the scene
    pScene->buildScene();

    // ...and now start svt
    svt_start(pScene);
    
    return 0;
}
