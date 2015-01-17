#include <svt_scenegraph.h>
#include <svt_core.h>
#include <svt_light.h>
#include <svt_init.h>
#include <svt_scene.h>
#include <svt_sphere.h>
#include <svt_box.h>
#include <svt_move_tool.h>
#include <svt_pipette.h>

// ball_scene class
class ball_scene : public svt_scene
{
protected:

    svt_scenegraph* graph;

public:

    ball_scene() : svt_scene() { };

public:

    /** build the scene */
    void buildScene()
    {
        graph = new svt_scenegraph();

        // create some object properties
        svt_properties* red = new svt_properties(new svt_color(1.0f,0.0f,0.0f));
        svt_properties* blue = new svt_properties(new svt_color(0.0f,0.0f,1.0f));
        svt_properties* blue_wireframe = new svt_properties(new svt_color(0.0f,0.0f,1.0f));
        blue_wireframe->setWireframe(true);
        svt_properties* green = new svt_properties(new svt_color(0.0f,1.0f,0.0f));
        green->setWireframe(true);
        green->setLighting(false);
	green->setSolid(false);

	svt_properties* white = new svt_properties(new svt_color(0.8f,0.8f,0.8f));
	white->getColor()->setShininess(40.0f);
	white->getColor()->setAmbient( 0.0f, 0.0f, 0.0f, 1.0f );
        white->getColor()->setDiffuse( 0.85f, 0.0f, 0.0f, 1.0f );
	white->getColor()->setSpecular( 1.00f, 0.0f, 0.0f, 1.0f );

        // create some objects
        graph->add(new svt_sphere(new svt_pos(0.0f,-0.5f,-1.0f), blue, 0.1f));
        graph->add(new svt_sphere(new svt_pos(0.0f,0.0f,-0.8f), blue_wireframe, 0.1f));
        graph->add(new svt_sphere(new svt_pos(0.0f,0.0f,-0.4f), red, 0.1f));
        graph->add(new svt_sphere(new svt_pos(0.0f,0.2f,-0.6f), red, 0.1f));
        graph->add(new svt_sphere(new svt_pos(0.0f,-0.2f,-0.6f), green, 0.1f));
        graph->add(new svt_box(new svt_pos(-0.5f,-0.5f, -0.5f), new svt_pos(1.0f, 1.0f, 1.0f), green));

	svt_sphere* sphere = new svt_sphere(new svt_pos(0.3f,0.0f,-0.1f), white, 0.25f, 16, 16);
        //sphere->setTexture(new svt_texture("../../../../data/tga/earth.tga"));
        graph->add(sphere);

        svt_light* light = new svt_light();
        light->add(new svt_lightsource(new svt_pos(0.0f,0.0f,2.0f)));
        setLight(light);

        // create a tool
        svt_move_tool* tool = new svt_move_tool("Pointer", sphere);
        tool->setCenterMode(true);
        tool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), white));
        graph->add(tool);

        svt_properties::setDefWireframeColor(1.0,1.0,1.0);

        setSceneGraph(graph);
    };
};

// main
int main(int argc, char *argv[])
{
    // init svt
    svt_init(argc,argv);

    // create a new scene object
    ball_scene* scene = new ball_scene();
    // and build the scene
    scene->buildScene();

    // set background color white
    // svt_color* white=new svt_color(1.0,1.0,1.0);
    // svt_setBackgroundColor(white);

    // ...and now start svt
    svt_start(scene);
    
    return 0;
}
