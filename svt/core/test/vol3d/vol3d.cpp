#include <svt_scenegraph.h>
#include <svt_core.h>
#include <svt_init.h>
#include <svt_scene.h>
#include <svt_volume_3dtextures.h>
#include <svt_pipette.h>
#include <svt_move_tool.h>

// vol3dtext_scene class
class vol3dtext_scene : public svt_scene
{
protected:
    svt_scenegraph m_oGraph;
    svt_volume<Real32> m_oTestData;
    svt_volume_3dtextures<Real32> m_oVol3D;

public:
    vol3dtext_scene() : svt_scene(),
	m_oTestData(50,60,50),
        m_oVol3D( m_oTestData )
    {
    };

public:
    /**
     * build the scene
     */
    void buildScene()
    {
	// calc the random 3d volume data
	calcTestData();

	// insert the 3d texture volume object
        m_oVol3D.recalcTextures();
        m_oGraph.add(&m_oVol3D);

        // create a tool
        svt_move_tool* pTool = new svt_move_tool("Pointer", &m_oVol3D);
        svt_properties* pRed = new svt_properties(new svt_color(1.0f,0.0f,0.0f));
        pTool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), pRed));
        pTool->setCenterMode(true);
        m_oGraph.add(pTool);

        // Lighting
        svt_light* pLight = new svt_light();
        pLight->add(new svt_lightsource(new svt_pos(5.0f,1.0f,1.0f)));
        pLight->add(new svt_lightsource(new svt_pos(0.0f,2.0f,0.0f)));
        setLight(pLight);

        // put the sg into the scene
        setSceneGraph(&m_oGraph);
    };

    /**
     * Calculate some noisy test data
     */
    void calcTestData()
    {
	int iX, iY, iZ;
        int iSizeX = (int) m_oTestData.getSizeX();
        int iSizeY = (int) m_oTestData.getSizeY();
	int iSizeZ = (int) m_oTestData.getSizeZ();

        for(iX=0; iX<iSizeX; iX++)
            for(iY=0; iY<iSizeY; iY++)
                for(iZ=0; iZ<iSizeZ; iZ++)
                {
                    m_oTestData.setAt( iX, iY, iZ, rand() % 256 );
                }
    }
};

// main
int main(int argc, char *argv[])
{
    // init svt
    svt_init(argc,argv);

    // create a new scene object
    vol3dtext_scene oScene;
    // and build the scene
    oScene.buildScene();
    
    // ...and now start svt
    svt_start(&oScene);
    
    return 0;
}
