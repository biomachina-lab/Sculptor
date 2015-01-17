#include <svt_scenegraph.h>
#include <svt_light.h>
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

        for(Real32 fX=-0.4f; fX<=0.4f; fX+=0.2f)
            for(Real32 fY=-0.4f; fY<=0.4f; fY+=0.2f)
                for(Real32 fZ=-0.4f; fZ<=0.4f; fZ+=0.2f)
		{
                    oVecCV_A[0] = 0.0f;
                    oVecCV_A[1] = 0.0f;
                    oVecCV_A[2] = 1.0f;
		    oVecCV_A[3] = fX;
		    oVecCV_A[4] = fY;
		    oVecCV_A[5] = fZ;

		    oVecCV_B = oVecCV_A;
                    oVecCV_B[0] = 1.0f;
                    oVecCV_B[1] = 0.0f;
                    oVecCV_B[2] = 0.0f;

		    if (fX < 0.4f)
			oVecCV_B[3] = fX+0.2f;
		    if (fY < 0.4f)
			oVecCV_B[4] = fY+0.2f;
		    if (fZ < 0.4f)
			oVecCV_B[5] = fZ+0.2f;

		    pContainer->addLine( oVecCV_A, oVecCV_B );

                    if ( fX == fY && fX == fZ && fX < 0.4f )
		    {
                        oCol.set( 0.7f, 0.0f, 0.0f );
                        pContainer->addCylinder(svt_vector4<Real32>(fX, fY, fZ), svt_vector4<Real32>(fX+0.2f, fY+0.2f, fZ+0.2f), 0.015f, oCol);
                        //(*pContainer) << new svt_basic_quad(fX-0.035f, fY-0.035f, fZ-0.035f, fX+0.035f, fY+0.035f, fZ+0.035f, 0.4f - fX, 0.4f + fY, 0.4f - fZ);
		    } else {

                        oCol.set( 0.4f - fX, 0.4f + fY, 0.4f - fZ );
			pContainer->addSphere( fX, fY, fZ, 0.05f, oCol );
		    }

                    oCol.set( 0.0f, 0.0f, 0.8f );
		    if (fX == 0.0f && fZ == 0 && fY < 0.4f)
			pContainer->addCylinder(svt_vector4<Real32>(fX, fY, fZ), svt_vector4<Real32>(fX, fY+0.2f, fZ), 0.015f, oCol);
                    if (fY == 0.0f && fZ == 0 && fX < 0.4f)
			pContainer->addCylinder(svt_vector4<Real32>(fX, fY, fZ), svt_vector4<Real32>(fX+0.2f, fY, fZ), 0.015f, oCol);
		    if (fX == 0.0f && fY == 0 && fZ < 0.4f)
			pContainer->addCylinder(svt_vector4<Real32>(fX, fY, fZ), svt_vector4<Real32>(fX, fY, fZ+0.2f), 0.015f, oCol);
                }

        oCol.set( 1.0f, 0.0f, 0.0f );
	pContainer->addSphere( 0.0, 0.0, 0.0, 0.05f, oCol );

        // add container to graph
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
