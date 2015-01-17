#include <svt_core.h>
#include <svt_scene.h>
#include <svt_sphere.h>
#include <svt_pipette.h>
#include <svt_pos.h>
#include <svt_box.h>
#include <svt_move_tool.h>
#include <svt_sky.h>

#include <svt_scene.h>
#include <svt_init.h>

// sky_scene class
class sky_scene : public svt_scene
{

public:
    sky_scene() : svt_scene() { };

public:

    /** build the scene */
    virtual void keyPressed(unsigned char key, int x, int y) {
        cout << "sky_scene::keyPressed()  YEAH YEAH YEAH" << endl;
        svt_scene::keyPressed(key, x, y);
    }

    void buildScene()
    {
        m_pSceneGraph = new svt_scenegraph();

        // construct empty node
        svt_node* empty = new svt_node();
        svt_matrix4f* mat = new svt_matrix4f();
        mat->setToId();
        // mat->postAddRotation(0,(90.0f*PI)/180.0f);
        empty->setTransformation(mat);
        m_pSceneGraph->add(empty);

        // construct the sky
        svt_sky* pSky = new svt_sky(empty, 6, 3);
        pSky->setTexture(new svt_texture("../../../../data/bmp/sky.bmp"));
        // pSky->getProperties()->setLighting(true);
        // pSky->getProperties()->setWireframe(true);
        // pSky->getProperties()->setNormals(true);

        svt_properties* red = new svt_properties(new svt_color(1.0f,0.0f,0.0f));
        svt_sphere* oSphere = new svt_sphere(new svt_pos(-0.5f,-0.5f, -1.5f), red, 1.0);

        pSky->add(oSphere);

        // create a tool
        svt_move_tool* tool = new svt_move_tool("Pointer", pSky);
        tool->setGlobalMatrix(mat);
        tool->setCenterMode(true);
        tool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), red));
        m_pSceneGraph->add(tool);

        // Lighting
        svt_light* light = new svt_light();
        svt_lightsource* source = new svt_lightsource(new svt_pos(0.0f,1.0f,2.0f));
        // source->setInfinite(true);
        light->add(source);
        // light->add(new svt_lightsource(new svt_pos(0.0f,-2.0f,1.0f)));
        setLight(light);

        // put the sg into the scene
        // setSceneGraph(graph);
    };
};

// main
int main(int argc, char *argv[])
{
    // init svt
    svt_init(argc,argv);

    // create a new scene object
    sky_scene oScene;
    // tool_scene* scene = new sky_scene();

    // and build the scene
    oScene.buildScene();

    // ...and now start svt
    svt_start(&oScene);

    return 0;
}

