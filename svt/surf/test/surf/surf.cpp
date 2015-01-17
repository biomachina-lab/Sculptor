/***************************************************************************
                          surf (test program)
                          ---------------------
    begin                : 10/08/2010
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_pdb.h>
#include <svt_system.h>
#include <svt_iostream.h>
#include <svt_core.h>
#include <svt_init.h>
#include <svt_scene.h>
#include <svt_move_tool.h>
#include <svt_pipette.h>

#define SURF cout << svt_trimName("surf_test")

/**
 * Simple svt scene class
 */
class surf_scene : public svt_scene
{
protected:

    svt_scenegraph* graph;
    svt_pdb& m_rSurf;

public:

    /**
     * Constructor
     */
    surf_scene(svt_pdb& rSurf) : svt_scene(),
        m_rSurf( rSurf )
    {
    };

    /**
     * Build the scene
     */
    void buildScene()
    {
        //
        // Create new scenegraph
        //
        graph = new svt_scenegraph();

        //
        // Add the surf object to the scenegraph
        //
        graph->add( &m_rSurf );

        //
        // Add lightsource
        //
        svt_light* light = new svt_light();
        light->add(new svt_lightsource(new svt_pos(0.0f,0.0f,2.0f)));
        setLight(light);

        //
        // Create a tool, so one can move the thing around
        //
        svt_move_tool* tool = new svt_move_tool("Pointer", &m_rSurf);
        tool->setCenterMode(true);
        tool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f) ));
        graph->add(tool);

        //
        // And tell svt that this is our scenegraph
        //
        setSceneGraph(graph);
    };
};

/**
 * Main program that loads the pdb file and starts the svt scene
 */
int main(int argc, char *argv[])
{
    //
    // Init svt
    //
    svt_init(argc,argv);

    //
    // Now load the data
    //
    SURF << "Loading pdb file: " << argv[1] << endl;
    svt_pdb oPDB( argv[1], SVT_PDB, NULL, SVT_NONE );
    oPDB.setDisplayMode( SVT_PDB_SURF );
    oPDB.rebuildDL();

    //
    // Create the svt_surf object
    //
    //svt_surf oSurf( oPDB );
    //if (argc > 2)
    //    oSurf.setProbeRadius( atof(argv[2] ) );

    //
    // Create a new scene object and build the scene
    //
    surf_scene oScene( oPDB );
    oScene.buildScene();

    //
    // ...and now start svt
    //
    svt_start(&oScene);
    
    return 0;
}
