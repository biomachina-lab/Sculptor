#include <svt_scenegraph.h>
#include <svt_light.h>
#include <svt_core.h>
#include <svt_init.h>
#include <svt_scene.h>
#include <svt_extrusion.h>
#include <svt_move_tool.h>
#include <svt_pipette.h>

// extrude_scene class
class extrude_scene : public svt_scene
{
protected:

    svt_scenegraph* m_pGraph;
    svt_extrusion* m_pExtrusion;

public:

    extrude_scene() : svt_scene() { };

public:

    /**
     * build the scene
     */
    void buildScene()
    {
	svt_properties* pWhite = new svt_properties(new svt_color(0.8f,0.8f,0.8f));
	svt_properties* pRed = new svt_properties(new svt_color(0.8f,0.0f,0.0f));
        m_pGraph = new svt_scenegraph();

	// create points
	int iAlpha;
	svt_array<svt_vec4real32> aPoints;
        svt_array<svt_vec4real32> aPath;

	//for(iAlpha=0; iAlpha<360; iAlpha+=10)
	//    aPoints.addElement( svt_vec4real32(cos(deg2rad((float)iAlpha))*0.02f, sin(deg2rad((float)iAlpha))*0.02f, 0.0f) );

	aPoints.addElement( svt_vec4real32( 0.0f, 0.0f, 0.0f ) );
	aPoints.addElement( svt_vec4real32( 0.1f, 0.0f, 0.0f ) );
	aPoints.addElement( svt_vec4real32( 0.1f, 0.1f, 0.0f ) );
	aPoints.addElement( svt_vec4real32( 0.0f, 0.1f, 0.0f ) );

	for(iAlpha=0; iAlpha<390; iAlpha+=2)
            aPath.addElement( svt_vec4real32(cos(deg2rad((float)iAlpha))*0.1f, iAlpha*0.001f, sin(deg2rad((float)iAlpha))*0.1f ) );

	// create extrusion object
	m_pExtrusion = new svt_extrusion();
	m_pExtrusion->setPath( aPath );
	m_pExtrusion->setShape( aPoints );
        m_pExtrusion->extrude();
        m_pExtrusion->setProperties( pRed );
        m_pGraph->add(m_pExtrusion);

        // lighting
	svt_light* light = new svt_light();
        light->add(new svt_lightsource(new svt_pos(0.0f,0.0f,2.0f)));
        setLight(light);

        // create a tool
        svt_move_tool* pTool = new svt_move_tool("Pointer", m_pExtrusion);
        pTool->setCenterMode(true);
        pTool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), pWhite));
        m_pGraph->add(pTool);

        setSceneGraph(m_pGraph);
    };
};

// main
int main(int argc, char *argv[])
{
    // init svt
    svt_init(argc,argv);

    // create a new scene object
    extrude_scene* pScene = new extrude_scene();
    // and build the scene
    pScene->buildScene();

    // ...and now start svt
    svt_start(pScene);
    
    return 0;
}
