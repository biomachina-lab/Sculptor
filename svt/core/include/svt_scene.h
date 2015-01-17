/***************************************************************************
                          svt_scene.h  -  description
                             -------------------
    begin                : Wed Feb 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SCENE_H
#define SVT_SCENE_H

// svt includes
class svt_scenegraph;
class svt_light;
#include <svt_aliasobj.h>
#include <svt_semaphore.h>
class svt_shader_manager;
class svt_fbo_ao;
class svt_node;

enum
{
    SVT_PASS_TRANSPARENT,
    SVT_PASS_OPAQUE,
    SVT_PASS_CLIPPLANE_RECTANGLE
};

/**
 * This is the container class for a vr scene
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_scene {

protected:
    svt_scenegraph* m_pSceneGraph;
    svt_light* m_pLight;
    svt_matrix4f *m_pSceneMat;

    svt_aliasobj m_oAliasOBJ;

    int m_iPass;
    bool m_bCappedClip;

    bool m_bSelection;
    svt_node* m_pSelectedNode;
    int m_iSelectedNodeID;
    vector< int > m_aSelectionArray;

    svt_semaphore m_oSceneSema;

    // fog parameters
    bool m_bFogEnabled;
    GLfloat m_fFogMode;
    GLfloat m_fFogCoordSrc;
    GLfloat m_fFogStart;
    GLfloat m_fFogEnd;
    GLfloat m_fFogZoomOffset;
    GLfloat m_fFogDensity;
    GLfloat * m_aFogColor;

    // ambient occlusion parameters
    bool m_bAOEnabled;
    int m_iAOMethod;
    Real32 m_fAOIntensity;
    Real32 m_fAORadiusScaling;
    svt_fbo_ao * m_pFBO_AO;

    // the central object for getting shader programs
    svt_shader_manager* m_pShaderManager;

public:
    /**
     * Constructor
     */
    svt_scene();

    virtual ~svt_scene();

    /**
     * get the current scene graph
     */
    svt_scenegraph* getSceneGraph();
    /**
     * set the current scene graph
     * \param pSceneGraph pointer to svt_scenegraph object
     */
    void setSceneGraph(svt_scenegraph* pSceneGraph);

    /**
     * draw the scene (for internal use only). This function redraws the scene by the use of the opengl routines.
     */
    virtual void drawGL();
    /**
     * prepare the opengl before drawing the scene. svt_scene does nothing here, but if you wish to change some global opengl settings, you should rewrite the function in your application scene class.
     */
    virtual void prepareGL();

    /**
     * print the povray code for the scene
     * \param bRadiosity if true radiosity is enable, if false not (default: false)
     */
    virtual void outputPOV(bool bRadiosity =false, int width=800, int height=600);
    /**
     * print the iv code (OpenInventor) for the scene
     */
    virtual void outputIV();
    /**
     * print the yafray code for the scene
     */
    virtual void outputYafray();
    /**
     * Output the scene in the alias/wavefront format
     * \param pFilename filename to which the ascii output should be sent
     */
    virtual void outputOBJ(const char* pFilename);
    /**
     * Output the materials used in the scene in the alias/wavefront format.
     * \param pFilename filename to which the ascii output should be sent
     * One has to call outputOBJ first so that the material arrays are generated.
     */
    virtual void outputMAT(const char* pFilename);

    /**
     * draw the scene in a specific mode (call this routine if you want to dump the scene in e.g. VRML or PovRay format)
     * \param iMode the mode (e.g. SVT_MODE_VRML)
     */
    void draw(int iMode);

    /**
     * Get the scene opengl drawing semaphore
     */
    svt_semaphore& getSceneSema();

    /**
     * what should happen when the window is resized?
     */
    virtual void resizeScene(int xpos, int ypos, int width, int height);

    /**
     * what should happen, when the user presses a key?
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void keyPressed(unsigned char key, int x, int y);
    /**
     * a special key (e.g. cursor keys) pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void specialKeyPressed(int key, int x, int y);
    /**
     * a special key (e.g. cursor keys) released
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void specialKeyReleased(int key, int x, int y);
    /**
     * mouse moved  - with mouse key pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void mouseActiveMoved(int x, int y);
    /**
     * mouse moved  - without mouse key pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void mousePassiveMoved(int x, int y);
    /**
     * left mouse button pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void leftMouseButton(int x, int y);
    /**
     * right mouse button pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void rightMouseButton(int x, int y);

    /**
     * up mouse wheel button pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void upMouseWheel(int, int);
    /**
     * down mouse wheel button pressed
     * Overload this function if you want to adjust the behaviour of your scene.
     */
    virtual void downMouseWheel(int, int);

    /**
     * set light
     */
    inline void setLight(svt_light* _tlight){ m_pLight = _tlight; };
    /** get light */
    inline svt_light* getLight( void ){ return m_pLight; };

    /**
     * build the scene. You must overload this function with your own code to build the scene.
     */
    virtual void buildScene() = 0;

    /**
     * call all recalc functions of the nodes
     */
    void recalcAll();
    /**
     * the scene recalc function (like in the nodes). If you return true, the nodes will get calculated, if false not.
     */
    virtual bool recalc();

    /**
     * get global scene matrix
     * \return pointer to svt_matrix4f object
     */
    svt_matrix4f* getTransformation();
    /**
     * set global scene matrix
     * \param pSceneMat pointer to svt_matrix4f object
     */
    void setTransformation(svt_matrix4f* pSceneMat);

    /**
     * Get Alias/Wavefront support object
     * \return reference to alias support object
     */
    svt_aliasobj& getAliasOBJ();

    /**
     * Get the current pass information
     */
    int getPass();

    /**
     * Set if the clipped surfaces should be closed or not (default: true)
     * \param bCappedClip if true the surfaces are capped
     */
    void setCappedClip( bool bCappedClip );
    /**
     * Get if the clipped surfaces should be closed or not
     * \return true if the surfaces are capped
     */
    bool getCappedClip();

    /**
     * Set the scene into selection rendering mode
     * \param bSelection if true the scene is only rendered to find the svt node onto which the user has clicked.
     */
    void setSelection( bool bSelection );
    /**
     * Get the state of the scene - selection rendering mode
     * \return if true the scene is only rendered to find the svt node onto which the user has clicked.
     */
    bool getSelection();

    /**
     * Set the selected node - function is only used internally
     * \param pNode pointer to svt_node object
     */
    void setSelectedNode( svt_node* pNode );
    /**
     * Get the selected node
     * \return pointer to svt_node object
     */
    svt_node* getSelectedNode( );

    /**
     * Get the id of the selected node
     * \return number/id of the selected node
     */
    int getSelectedNodeID();
    /**
     * Set the id of the selected node
     * \param iID number/id of the selected node
     */
    void setSelectedNodeID( int iID );

    /**
     * Set the selection id array. If this function is used, the selection mechanism will only detect if the user clicks on an svt_node with an ID in the array, all the other svt_nodes are then ignored.
     * If the array is empty, all objects are investigated again.
     * \param aSelectionArray stl vector with the ids
     */
    void setSelectionArray( vector< int >& aSelectionArray );
    /**
     * Return a pointer to the central shader manager, where objects can request shader programs.
     */
    svt_shader_manager* getShaderManager();
    /**
     * \name Fog Methods
     * Methods for setting and getting fog parameters. All fog parameters are stored here
     * and should be polled from here.
     */
    //@{
    /**
     * Enable or disable fog
     * \param bFog true for enabling fog, false for disabling fog
     */
    void setFogEnabled(bool bFog);
    /**
     * Get if fog is enabled
     * \return bool true for enabled fog, false for disabled fog
     */
    bool getFogEnabled();
    /**
     * Change a fog parameter.
     * \param eParameter the OpenGL enum for the respective fog parameter that is to be set. Can be one of
     * - GL_FOG_MODE
     * - GL_FOG_DENSITY
     * - GL_FOG_START
     * - GL_FOG_END
     * - GL_FOG_COORD_SRC
     * - GL_FOG_COLOR
     * - GL_LINEAR a helper to set the value for fog zoom offset (needed by our shader programs)
     * \param pValue pointer to a GLfloat with the actual value(s)
     */
    void setFogParam(GLenum eParameter, GLfloat * pValue);
    /**
     * Get a fog parameter.
     * \param eParameter the OpenGL enum for the respective fog parameter. See setFogParam()
     * \param pValue pointer to a GLfloat where the value(s) will be written to
     */
    void getFogParam(GLenum eParameter, GLfloat * pValue);
    //@}
    /**
     * \name Ambient Occlusion Methods
     * Methods for setting and getting ambient occlusion parameters. All ambient occlusion
     * parameters are stored here and should be polled from here.
     */
    //@{
    /**
     * Get if ambient occlusion is supported at all (certain OpenGL and hardware requirements must be met)
     * \return bool true if ambient occlusion is possible, false if not.
     */
    bool getAOSupported();
    /**
     * Enable or disable ambient occlusion.
     * \param bAO true for enabling ambient occlusion, false for disabling ambient occlusion
     */
    void setAOEnabled(bool bAO);
    /**
     * Get if ambient occlusion is enabled.
     * \return bFog true for enabled ambient occlusion, false for disabled ambient occlusion
     */
    bool getAOEnabled();
    /**
     * Set the radius of the sphere in which the ambient occlusion fragment shader samples the view
     * space. Increase the value if you want bigger creases/pockets to be darkened, decrease for
     * samller ones.
     * \param fRadius float that determines the view space ambient occluder sample sphere
     */
    void setAOSampleRadiusScaling(Real32 fRadius);
    /**
     * Get the radius of the sphere in which the ambient occlusion fragment shader samples the view
     * space.
     * \return Real32
     */
    Real32 getAOSampleRadiusScaling();
    /**
     * Set a factor between 0 and 1 that is used to modulate the ambient occlusion intensity
     * \param fIntensity float that representes AO intensity
     */
    void setAOIntensity(Real32 fIntensity);
    /**
     * Get the ambient occlusion intensity factor
     * \return Real32
     */
    Real32 getAOIntensity();
    /**
     * Set ambient occlusion method
     * \param iAOMethod enum that determines AO mode to be used
     */
    void setAOMethod(int iAOMethod);
    /**
     * Set ambient occlusion method
     * \return int enum with the current AO mode
     */
    int getAOMethod();
    /**
     * Set if ambient occlusion is allowed. if true, this method inits the ambient occlusion
     * framebuffer and sets ambient occlusion parameters
     */
    void setAllowAmbientOcclusion(bool bAllow);
    //@}
};

#endif
