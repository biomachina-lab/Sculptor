/***************************************************************************
                          svt_scene.cpp  -  description
                             -------------------
    begin                : Wed Feb 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_scene.h>
#include <svt_scenegraph.h>
#include <svt_init.h>
#include <svt_light.h>
#include <svt_defs.h>
#include <svt_window.h>
#include <svt_file_utils.h>
#include <svt_opengl.h>
#include <svt_clipplane.h>
#include <svt_fbo_ao.h>
#include <svt_shader_cg_ao.h>
#include <svt_shader_manager.h>
// clib includes FIXME
#include <stdlib.h>

bool bFirstRun = true;

#ifdef WIN32
  #ifndef STATICQT
  extern "C" DLLEXPORT BOOL WINAPI DllMain( HANDLE hModule, DWORD dwFunction, LPVOID lpNot )
  {
    svt_registerCout( &std::cout );
    return true;
  };
  #endif
#endif

/**
 * Constructor
 */
svt_scene::svt_scene()
    : m_oAliasOBJ( ),
      m_iPass( SVT_PASS_OPAQUE ),
      m_bCappedClip( false ),
      m_bSelection( false ),
      m_bFogEnabled( true ),
      m_fFogMode( GL_LINEAR ),
      m_fFogCoordSrc( GL_FRAGMENT_DEPTH ),
      m_fFogStart( 1.0f ),
      m_fFogEnd( 25.0f ),
      m_fFogZoomOffset( 0.0f ),
      m_fFogDensity( 0.2f ),
#ifdef EXPERIMENTAL
      m_bAOEnabled( true ),
#else
      m_bAOEnabled( false ),
#endif
      m_iAOMethod( SVT_AO_LINE_BASED ),
      m_fAOIntensity( 0.7f ),
      m_fAORadiusScaling( 0.55f ),
      m_pFBO_AO( NULL ),
      m_pShaderManager( NULL )
{
    m_pSceneMat = new svt_matrix4f("Scene Matrix");
    m_pSceneMat->setToId();

    setSceneGraph(NULL);
    setLight(NULL);

    m_aFogColor = new GLfloat[4];
    m_aFogColor[0] = 0.15f;
    m_aFogColor[1] = 0.15f;
    m_aFogColor[2] = 0.15f;
    m_aFogColor[3] = 1.0f;

    // init the shader manager
    m_pShaderManager = new svt_shader_manager();

    // init the FBO for ambient occlusion
    m_pFBO_AO = new svt_fbo_ao();

    // switch AO off if not supported
    if ( !m_pFBO_AO->getAOSupported() )
    {
	setAOEnabled( false );
    }
    else
    {
	setAOMethod(m_iAOMethod);
	setAOIntensity(m_fAOIntensity);
	setAOSampleRadiusScaling(m_fAORadiusScaling);

	// switch AO off by default for certain hardware/software
	if ( svt_getRenderer() == SVT_RENDERER_MESA    ||
             svt_getRenderer() == SVT_RENDERER_UNKNOWN ||
             svt_getRenderer() == SVT_RENDERER_ATI     ||
             svt_getRenderer() == SVT_RENDERER_INTEL )
        {
	    setAOEnabled( false );
        }
    }
}
/**
 * Destructor
 */
svt_scene::~svt_scene()
{
    if (m_pSceneMat != NULL)
        delete m_pSceneMat;

    if (m_pLight != NULL)
        delete m_pLight;

    if (getSceneGraph() != NULL)
        delete(getSceneGraph());

    if (m_aFogColor != NULL)
	delete[] m_aFogColor;

    if (m_pFBO_AO != NULL)
	delete m_pFBO_AO;

    if (m_pShaderManager != NULL)
        delete m_pShaderManager;
}

/**
 * returns the current scenegraph
 */
svt_scenegraph* svt_scene::getSceneGraph()
{
    return m_pSceneGraph;
}
/**
 * set the current scenegraph
 * \param pSceneGraph pointer to svt_scenegraph object
 */
void svt_scene::setSceneGraph(svt_scenegraph* pSceneGraph)
{
    m_pSceneGraph = pSceneGraph;
}

/**
 * Get the scene opengl drawing semaphore
 */
svt_semaphore& svt_scene::getSceneSema()
{
    return m_oSceneSema;
};

/**
 * draw the scene (for internal use only). This function redraws the scene by the use of the opengl routines.
 */
void svt_scene::drawGL()
{
    m_oSceneSema.P();

    GLuint aSelBuffer[512];

    if (svt_getSamples( svt_getCurrentWindow() ) > 0)
        glEnable(GL_MULTISAMPLE);

    if (bFirstRun == true)
    {
        GLint iValue;
        glGetIntegerv( GL_STENCIL_BITS, &iValue );
        svtout << "OpenGL: Stencil-buffer has " << iValue << " bits" << endl;
        glGetIntegerv( GL_DEPTH_BITS, &iValue );
        svtout << "OpenGL: Depth-buffer has " << iValue << " bits" << endl;
        GLint iR, iG, iB, iA;
        glGetIntegerv( GL_ALPHA_BITS, &iA );
        glGetIntegerv( GL_RED_BITS, &iR );
        glGetIntegerv( GL_GREEN_BITS, &iG );
        glGetIntegerv( GL_BLUE_BITS, &iB );
        svtout << "OpenGL: RGBA bits: (" << iR << ", " << iG << ", " << iB << ", " << iA << ")" << endl;

        if (svt_getSamples( svt_getCurrentWindow() ) > 0)
        {
            GLint iBufs;
            GLint iSamples;
            glGetIntegerv(GL_SAMPLE_BUFFERS, &iBufs);
            glGetIntegerv(GL_SAMPLES, &iSamples);
            svtout << "OpenGL: Multisample: (" << iBufs << " buffers and " << iSamples << " samples)" << endl;
        }

        bFirstRun = false;
    }

    //
    // Selection Mode?
    //
    if (m_bSelection)
    {
        glSelectBuffer(512, aSelBuffer);
        glRenderMode( GL_SELECT );
        glInitNames();
        glPushName( 0 );


        GLint viewport[4];
        glMatrixMode(GL_PROJECTION);
	glPushMatrix();
        glLoadIdentity();

	glGetIntegerv(GL_VIEWPORT,viewport);
        gluPickMatrix(svt_getMouseX(),viewport[3]-svt_getMouseY(),5,5,viewport);
        svt_setPerspective(0);
	//gluPerspective(45,ratio,0.1,1000);
	glMatrixMode(GL_MODELVIEW);
    }

    //
    // Ambient Occlusion: if it is enabled, redirect all OpenGL drawing into the dedicated AO framebuffer
    //
    if ( m_bAOEnabled && svt_getAllowAmbientOcclusion() && svt_getAllowShaderPrograms() )
    {
	// pass the background color to the AO framebuffer
	svt_color* pColor = svt_getBackgroundColor();
	m_pFBO_AO->setClearColor(pColor->getR(), pColor->getG(), pColor->getB(), 1.0f);
	m_pFBO_AO->beginAO(svt_getWindowWidth(svt_getCurrentWindow()), svt_getWindowHeight(svt_getCurrentWindow()));
    }

    //
    // 1st pass: draw all opaque objects (selection will be done in svt_node!)
    //

    m_iPass = SVT_PASS_OPAQUE;

    // prepare all opengl settings
    prepareGL();
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glEnable( GL_LIGHTING );
    glDisable( GL_STENCIL_TEST );
    glEnable( GL_DEPTH_TEST );
    glDepthMask( GL_TRUE );
    glDepthFunc(GL_LESS );

    // apply the scene matrix
    m_pSceneMat->applyToGL();

    // draw the lightsources
    if (getLight() != NULL)
        getLight()->draw(SVT_MODE_GL);
    else
        glDisable(GL_LIGHTING);

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
        m_pSceneGraph->draw(SVT_MODE_GL);

    //
    // 2nd pass: draw all transparent objects into the depth buffer only
    //

    m_iPass = SVT_PASS_TRANSPARENT;

    // prepare all opengl settings
    prepareGL();

    // render only into depth buffer
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

    // apply the scene matrix
    m_pSceneMat->applyToGL();

    // draw the lightsources
    if (getLight() != NULL)
        getLight()->draw(SVT_MODE_GL);
    else
        glDisable(GL_LIGHTING);

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
        m_pSceneGraph->draw(SVT_MODE_GL);

    //
    // 3rd pass: draw all transparent objects into framebuffer, but with depth test set to equal
    //

    m_iPass = SVT_PASS_TRANSPARENT;

    // prepare all opengl settings
    prepareGL();

    // render into framebuffer
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthFunc( GL_EQUAL );

    // apply the scene matrix
    m_pSceneMat->applyToGL();

    // draw the lightsources
    if (getLight() != NULL)
        getLight()->draw(SVT_MODE_GL);
    else
        glDisable(GL_LIGHTING);

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
        m_pSceneGraph->draw(SVT_MODE_GL);

    glDepthFunc(GL_LESS);

    //
    // Ambient Occlusion: now compute the AO and send the final image to the windows system frame
    // buffer
    //
    if ( m_bAOEnabled && svt_getAllowAmbientOcclusion() && svt_getAllowShaderPrograms() )
    {
	m_pFBO_AO->finishAO();
    }

    //
    // 4th pass: clipplane rectangles
    //
    m_iPass = SVT_PASS_CLIPPLANE_RECTANGLE;

    if (m_pSceneGraph != NULL)
        m_pSceneGraph->draw(SVT_MODE_GL);



    /*
    if (m_bCappedClip && m_pSceneGraph != NULL && svt_clipplane::getNumUsedClipPlanes() > 0)
    {
        m_iPass = SVT_PASS_OPAQUE;

        //
        // Check if we actually have a stencil buffer
        //
        GLint iValue;
        glGetIntegerv( GL_STENCIL_BITS, &iValue );
        if (iValue == 0)
        {
            m_oSceneSema.V();
            return;
        }

        //
        // First Pass: Render Back Faces
        //
        glClear( GL_DEPTH_BUFFER_BIT );
        glDepthFunc(GL_LESS);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glEnable( GL_CULL_FACE );
        glCullFace(GL_FRONT);
        prepareGL();
        m_pSceneMat->applyToGL();
        m_pSceneGraph->draw(SVT_MODE_GL);

        glDepthFunc( GL_EQUAL );
        glDepthMask( GL_FALSE );
        glEnable(GL_STENCIL_TEST);
        glClear(GL_STENCIL_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glStencilFunc(GL_ALWAYS, 1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        glEnable( GL_CULL_FACE );
        glCullFace(GL_FRONT);
        prepareGL();
        m_pSceneMat->applyToGL();
        m_pSceneGraph->draw(SVT_MODE_GL);

        //
        // Second Pass: Render Front Faces
        //
        glDepthFunc( GL_LESS );
        glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
        glCullFace(GL_BACK);
        prepareGL();
        m_pSceneMat->applyToGL();
        m_pSceneGraph->draw(SVT_MODE_GL);

        m_iPass = SVT_PASS_CLIP;

        //
        // Third Pass: Render Into Stencil Mask
        //
        glEnable( GL_STENCIL_TEST );
        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
        glClear( GL_DEPTH_BUFFER_BIT );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LESS );
        glDepthMask( GL_TRUE );
        glDisable( GL_CULL_FACE );
        glStencilFunc(GL_NOTEQUAL, 0, ~0);
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
        prepareGL();
        m_pSceneMat->applyToGL();
        m_pSceneGraph->draw(SVT_MODE_GL);
    }
    */

    if (m_bSelection)
    {
        glFlush();
        int iHits = glRenderMode( GL_RENDER );
        m_bSelection = false;

        unsigned int iCount = 0;
        unsigned int iNames = 0;
        float fMinZ, fMaxZ;
        float fAbsMin = 1.0E10;
        m_iSelectedNodeID = -1;
        int iID;
        for(int i=0; i<iHits; i++)
        {
            iNames = aSelBuffer[iCount++];
            fMinZ = (float)(aSelBuffer[iCount++])/0x7fffffff;
            fMaxZ = (float)(aSelBuffer[iCount++])/0x7fffffff;

            if (fMinZ < fAbsMin)
            {
                for(unsigned int j=0; j<iNames; j++)
                {
                    iID = aSelBuffer[iCount++];

                    if (m_aSelectionArray.size() == 0)
                        m_iSelectedNodeID = iID;
                    else
                        if ( find(m_aSelectionArray.begin(), m_aSelectionArray.end(), iID ) != m_aSelectionArray.end() )
                            m_iSelectedNodeID = iID;
                }

                fAbsMin = fMinZ;

            } else
                iCount += iNames;

        }

        if (m_iSelectedNodeID != -1)
        {
            m_pSelectedNode = NULL;
            m_pSceneGraph->findSelected();
        }
    }

    m_oSceneSema.V();
}
/**
 * prepare the opengl before drawing the scene. svt_scene does nothing here, but if you wish to change some global opengl settings, you should rewrite the function in your application scene class.
 */
void svt_scene::prepareGL()
{
}

/**
 * print the iv code (OpenInventor) for the object
 */
void svt_scene::outputIV()
{
    cout << "#Inventor V2.0 ascii" << endl;

    cout << "Separator {" << endl;
    cout << "  Info {" << endl;
    cout << "    string \" SVT (Scientific Visualization Toolkit) scene.\"" << endl;
    cout << "  }" << endl;

    cout << "  PerspectiveCamera {" << endl;
    cout << "    position 0.0 0.0 1.1" << endl;
    cout << "    nearDistance 0.9" << endl;
    cout << "    farDistance 100.0" << endl;
    cout << "    heightAngle 0.756667" << endl;
    cout << "  }" << endl;
    cout << endl;

    // draw the lightsources
    if (getLight() != NULL)
        getLight()->draw(SVT_MODE_IV);

    // draw a room
    cout << endl;
    cout << "  Separator {" << endl;
    cout << "    Info {" << endl;
    cout << "      string \" Room \"" << endl;
    cout << "    }" << endl;
    cout << "    Coordinate3 {" << endl;
    cout << "      point [" << endl;
    cout << "        5.0 -5.0 -5.0," << endl;
    cout << "       -5.0 -5.0 -5.0," << endl;
    cout << "       -5.0 -5.0  5.0," << endl;
    cout << "        5.0 -5.0  5.0," << endl;

    cout << "        5.0  5.0 -5.0," << endl;
    cout << "        5.0  5.0  5.0," << endl;
    cout << "       -5.0  5.0  5.0," << endl;
    cout << "       -5.0  5.0 -5.0 ]" << endl;
    cout << "    }" << endl;
    cout << "    Material {" << endl;
    cout << "      diffuseColor 0.7 0.7 0.7" << endl;
    cout << "      shininess 0.0" << endl;
    cout << "    }" << endl;
    cout << "    IndexedFaceSet {" << endl;
    cout << "      coordIndex [" << endl;
    cout << "        0, 1, 2, 3, -1," << endl;
    cout << "        0, 1, 7, 4, -1," << endl;
    cout << "        2, 3, 5, 6, -1," << endl;
    cout << "        0, 3, 5, 4, -1," << endl;
    cout << "        7, 6, 2, 1, -1 ]" << endl;
    cout << "    }" << endl;
    cout << "    Material {" << endl;
    cout << "      emissiveColor 1.0 1.0 1.0" << endl;
    cout << "      diffuseColor  1.0 1.0 1.0" << endl;
    cout << "      shininess 0.0" << endl;
    cout << "    }" << endl;
    cout << "    IndexedFaceSet {" << endl;
    cout << "      coordIndex [" << endl;
    cout << "        4, 5, 6, 7, -1 ]" << endl;
    cout << "    }" << endl;
    cout << "  }" << endl;
    cout << endl;

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
	m_pSceneGraph->draw(SVT_MODE_IV);

    cout << "}" << endl;
}

/**
 * print the povray code for the scene
 * \param bRadiosity if true radiosity is enable, if false not
 * \param width sets the width of the generated output window
 * \param height sets the height of the generated output window
 */
void svt_scene::outputPOV(bool bRadiosity, int width, int height)
{
    cout << "// POVRAY Code generated by SVT" << endl;

    cout << "camera { location 1.5*z right -" << (float)width / (float)height << "*x look_at 0 }" << endl;
    cout << endl;

    svt_color* pColor = svt_getBackgroundColor();

    cout << "background {" << endl;
    cout << "  color rgb< " << pColor->getR() << ", " << pColor->getB() << ", " << pColor->getG() << " >" << endl;
    cout << "}" << endl;
    cout << endl;

    if (bRadiosity)
    {
	cout << "global_settings {" << endl;
	cout << "  radiosity {" << endl;
	cout << "  brightness 1.3" << endl;
	cout << "  count 200" << endl;
	cout << "  error_bound 0.01" << endl;
	cout << "  gray_threshold 0.0" << endl;
	cout << "  low_error_factor 0.01" << endl;
	cout << "  minimum_reuse 0.015" << endl;
	cout << "  nearest_count 10" << endl;
	cout << "  recursion_limit 1" << endl;
	cout << "  adc_bailout 0.01" << endl;
	cout << "  max_sample 0.5" << endl;
	cout << "  media off" << endl;
	cout << "  normal off" << endl;
	cout << "  always_sample 1" << endl;
	cout << "  pretrace_start 0.08" << endl;
	cout << "  pretrace_end 0.01" << endl;
	cout << "  }" << endl;
	cout << "}" << endl;
	cout << endl;
    }

    // draw the lightsources
    if (getLight() != NULL)
        getLight()->draw(SVT_MODE_POV);

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
	m_pSceneGraph->draw(SVT_MODE_POV);
}

/**
 * print the yafray code for the scene
 */
void svt_scene::outputYafray()
{
    cout << "<!-- Yafray XML - Generated by SVT -->" << endl;
    cout << endl;
    cout << "<scene>" << endl;
    cout << endl;
    cout << "<shader type = \"generic\" name = \"svt.mat\">" << endl;
    cout << "  <attributes>" << endl;
    cout << "    <color r=\"0.750000\" g=\"0.750000\" b=\"0.800000\" />" << endl;
    cout << "    <specular r=\"0.000000\" g=\"0.000000\" b=\"0.000000\" />"  << endl;
    cout << "    <reflected r=\"0.000000\" g=\"0.000000\" b=\"0.000000\" />" << endl;
    cout << "    <transmitted r=\"0.000000\" g=\"0.000000\" b=\"0.000000\" />" << endl;
    cout << "    </attributes>" << endl;
    cout << "</shader>" << endl;
    cout << endl;
    cout << "<camera name=\"Camera\" resx=\"800\" resy=\"600\" focal=\"1.0\" >" << endl;
    cout << "  <from x=\"0.0\" y=\"0.0\" z=\"3.0\" />" << endl;
    cout << "  <to x=\"0.0\" y=\"0.0\" z=\"-1.0\" />" << endl;
    cout << "  <up x=\"0.0\" y=\"1.0\" z=\"1.0\" />" << endl;
    cout << "</camera>" << endl;
    cout << endl;

    //cout << "  PerspectiveCamera {" << endl;
    //cout << "    position 0.0 0.0 1.1" << endl;
    //cout << "    nearDistance 0.9" << endl;
    //cout << "    farDistance 100.0" << endl;
    //cout << "    heightAngle 0.756667" << endl;
    //cout << "  }" << endl;
    //cout << endl;

    // draw the lightsources
    if (getLight() != NULL)
        getLight()->draw(SVT_MODE_YAFRAY);

    // draw a floor
    cout << "<object name=\"Floor\" shader_name=\"svt.mat\" caus_IOR=\"1.5\" recv_rad=\"on\" emit_rad=\"on\" shadow=\"on\">" << endl;
    cout << "  <attributes>" << endl;
    cout << "  </attributes>" << endl;
    cout << "  <mesh>" << endl;
    cout << "  <points>" << endl;
    cout << "    <p x=\"-5.0\" y=\"-0.5\" z=\"-5.0\"/>" << endl;
    cout << "    <p x=\"5.0\" y=\"-0.5\" z=\"-5.0\"/>" << endl;
    cout << "    <p x=\"5.0\" y=\"-0.5\" z=\"5.0\"/>" << endl;
    cout << "    <p x=\"-5.0\" y=\"-0.5\" z=\"5.0\"/>" << endl;
    cout << "  </points>" << endl;
    cout << "  <faces>" << endl;
    cout << "    <f a=\"0\" b=\"1\" c=\"2\" />" << endl;
    cout << "    <f a=\"0\" b=\"2\" c=\"3\" />" << endl;
    cout << "  </faces>" << endl;
    cout << "  </mesh>" << endl;
    cout << "</object>" << endl;
    cout << endl;

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
	m_pSceneGraph->draw(SVT_MODE_YAFRAY);

    cout << endl;
    cout << "<render camera_name=\"Camera\" AA_passes=\"2\" AA_minsamples=\"2\" AA_pixelwidth=\"1.500000\" AA_threshold=\"0.040000\" raydepth=\"5\" bias=\"0.300000\" indirect_samples=\"1\" gamma=\"1.000000\" exposure=\"0.000000\">" << endl;
    cout << "  <outfile value=\"mesh.tga\"/>" << endl;
    //cout << "  <save_alpha value=\"on\"/>" << endl;
    cout << "</render>" << endl;
    cout << endl;
    cout << "</scene>" << endl;
}

/**
 * Output the scene in the alias/wavefront format
 * \param pFilename filename to which the ascii output should be sent
 */
void svt_scene::outputOBJ(const char* pFilename)
{
    ofstream oFile(pFilename);
    streambuf* pBuffer = NULL;

    if (oFile.is_open())
    {
        pBuffer = cout.rdbuf();
	cout.rdbuf(oFile.rdbuf());
    }

    char pFilewopath[1024];
    svt_stripPath( (char*)pFilename, pFilewopath );
    char pFileonly[1024];
    svt_stripExtension( pFilewopath, pFileonly );
    cout << "mtllib " << pFileonly << ".mtl" << endl;

    m_oAliasOBJ.reset();

    // reset the alias obj
    svt_matrix4<Real32> oMat;
    oMat.scale( 100.0 );
    m_oAliasOBJ.pushMatrix( oMat );

    // draw the complete scenegraph
    if (m_pSceneGraph != NULL)
	m_pSceneGraph->draw(SVT_MODE_OBJ);

    m_oAliasOBJ.drawOBJ();

    if (oFile.is_open())
    {
        cout.rdbuf( pBuffer );
        oFile.close();
    }
};
/**
 * Output the materials used in the scene in the alias/wavefront format
 * One has to call outputOBJ first so that the material arrays are generated.
 * \param pFilename filename to which the ascii output should be sent
 */
void svt_scene::outputMAT(const char* pFilename)
{
    ofstream oFile(pFilename);
    streambuf* pBuffer = NULL;

    if (oFile.is_open())
    {
        pBuffer = cout.rdbuf();
	cout.rdbuf(oFile.rdbuf());
    }

    m_oAliasOBJ.drawMAT();

    if (oFile.is_open())
    {
        cout.rdbuf( pBuffer );
        oFile.close();
    }
};

/**
 * draw the scene in a specific mode (call this routine if you want to dump the scene in e.g. VRML or PovRay format)
 * \param iMode the mode (e.g. SVT_MODE_VRML)
 */
void svt_scene::draw(int iMode)
{
    switch(iMode)
    {
    case SVT_MODE_IV:
	outputIV();
        break;

    case SVT_MODE_YAFRAY:
	outputYafray();
        break;

    case SVT_MODE_POV:
	outputPOV(800, 600);
        break;

    default:
    case SVT_MODE_GL:
	prepareGL();
	drawGL();
	break;
    }
}

/**
 * what should happen when the window is resized?
 */
void svt_scene::resizeScene(int, int, int, int)
{
}

/**
 * key event
 */
void svt_scene::keyPressed(unsigned char key, int, int)
{

    switch(key)
    {
    case 'S':
    case 's':
        svt_saveScreenshot();
        break;

    case 'I':
    case 'i':
        draw(SVT_MODE_IV);
        break;

    default:
        break;
    }
}
/**
 * special key event
 */
void svt_scene::specialKeyPressed(int key, int, int)
{
    switch(key)
    {
        // here one can react if the user presses one of the special keys, e.g. page-up, etc.
    // case SVT_KEY_RIGHT:
    //    break;
    // case SVT_KEY_LEFT:
    //    break;
    // case SVT_KEY_UP:
    //    break;
    // case SVT_KEY_DOWN:
    //    break;
    // case SVT_KEY_PAGE_UP:
    //    break;
    // case SVT_KEY_PAGE_DOWN:
    //    break;
    }
}
/**
 * special key event
 */
void svt_scene::specialKeyReleased(int key, int, int)
{
    switch(key)
    {
        // here one can react if the user presses one of the special keys, e.g. page-up, etc.
    // case SVT_KEY_RIGHT:
    //    break;
    // case SVT_KEY_LEFT:
    //    break;
    // case SVT_KEY_UP:
    //    break;
    // case SVT_KEY_DOWN:
    //    break;
    // case SVT_KEY_PAGE_UP:
    //    break;
    // case SVT_KEY_PAGE_DOWN:
    //    break;
    }
}

/**
 * mouse moved
 */
void svt_scene::mouseActiveMoved(int, int)
{
}
/**
 * mouse moved
 */
void svt_scene::mousePassiveMoved(int, int)
{
}

/**
 * left mouse button pressed
 */
void svt_scene::leftMouseButton(int, int)
{
}
/**
 * right mouse button pressed
 */
void svt_scene::rightMouseButton(int, int)
{
}

/**
 * up mouse wheel button pressed
 */
void svt_scene::upMouseWheel(int, int)
{
}
/**
 * down mouse wheel button pressed
 */
void svt_scene::downMouseWheel(int, int)
{
}

/**
 * recalc the whole scene (incl. the scenegraph)
 */
void svt_scene::recalcAll()
{
    if (recalc() == false)
        return;

    if (m_pSceneGraph != NULL)
        m_pSceneGraph->recalcAll();
};
/**
 * recalc this scene only (without scenegraph)
 */
bool svt_scene::recalc()
{
    return true;
};

/**
 * get global scene matrix
 * \return pointer to svt_matrix4f object
 */
svt_matrix4f* svt_scene::getTransformation()
{
    return m_pSceneMat;
}
/**
 * set global scene matrix
 * \param pSceneMat pointer to svt_matrix4f object
 */
void svt_scene::setTransformation(svt_matrix4f* pSceneMat)
{
    m_pSceneMat = pSceneMat;
}

/**
 * Get Alias/Wavefront support object
 * \return reference to alias support object
 */
svt_aliasobj& svt_scene::getAliasOBJ()
{
    return m_oAliasOBJ;
}

/**
 * Get the current pass information
 */
int svt_scene::getPass()
{
    return m_iPass;
};

/**
 * Set if the clipped surfaces should be closed or not (default: true)
 * \param bCappedClip if true the surfaces are capped
 */
void svt_scene::setCappedClip( bool bCappedClip )
{
    m_bCappedClip = bCappedClip;
    svt_redraw();
};
/**
 * Get if the clipped surfaces should be closed or not
 * \return true if the surfaces are capped
 */
bool svt_scene::getCappedClip()
{
    return m_bCappedClip;
};

/**
 * Set the scene into selection rendering mode
 * \param bSelection if true the scene is only rendered to find the svt node onto which the user has clicked.
 */
void svt_scene::setSelection( bool bSelection )
{
    m_bSelection = bSelection;
};
/**
 * Get the state of the scene - selection rendering mode
 * \return if true the scene is only rendered to find the svt node onto which the user has clicked.
 */
bool svt_scene::getSelection()
{
    return m_bSelection;
};

/**
 * Set the selected node - function is only used internally
 * \param pNode pointer to svt_node object
 */
void svt_scene::setSelectedNode( svt_node* pNode )
{
    m_pSelectedNode = pNode;
};
/**
 * Get the selected node
 * \return pointer to svt_node object
 */
svt_node* svt_scene::getSelectedNode( )
{
    return m_pSelectedNode;
};
/**
 * Get the id of the selected node
 * \return number/id of the selected node
 */
int svt_scene::getSelectedNodeID()
{
    return m_iSelectedNodeID;
}
/**
 * Set the id of the selected node
 * \param iID number/id of the selected node
 */
void svt_scene::setSelectedNodeID( int iID )
{
    m_iSelectedNodeID = iID;
}
/**
 * Set the selection id array. If this function is used, the selection mechanism will only detect if the user clicks on an svt_node with an ID in the array, all the other svt_nodes are then ignored.
 * If the array is empty, all objects are investigated again.
 * \param aSelectionArray stl vector with the ids
 */
void svt_scene::setSelectionArray( vector< int >& aSelectionArray )
{
    m_aSelectionArray = aSelectionArray;
};
/**
 *
 */
svt_shader_manager* svt_scene::getShaderManager()
{
    return m_pShaderManager;
}
/**
 * enable or disable fog
 * \param bFog true for enabling fog, false for disabling fog
 */
void svt_scene::setFogEnabled(bool bFog)
{
    m_bFogEnabled = bFog;
}
/**
 * get if fog is enabled
 * \param bFog true to enable fog, false to disable fog
 */
bool svt_scene::getFogEnabled()
{
    return m_bFogEnabled;
}
/**
 * Change a fog parameter. See the OpenGL reference for explanations.
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
void svt_scene::setFogParam(GLenum eParameter, GLfloat * pValue)
{
    switch (eParameter)
    {
	case GL_FOG_MODE:
	    m_fFogMode = *pValue;
	    break;
	case GL_FOG_DENSITY:
	    m_fFogDensity = *pValue;
	    break;
	case GL_FOG_START:
	    m_fFogStart = *pValue;
	    break;
	case GL_FOG_END:
	    m_fFogEnd = *pValue;
	    break;
	case GL_FOG_COORD_SRC:
	    m_fFogCoordSrc = *pValue;
	    break;
	case GL_FOG_COLOR:
	    memcpy(m_aFogColor, pValue, sizeof(GLfloat)*4);
	    break;
	case GL_LINEAR:
	    m_fFogZoomOffset = *pValue;
	    break;
	default:
	    break;
    }
}
/**
 * Get a fog parameter.
 * \param eParameter the opengl enum for the respective fog parameter. See setFogSetting()
 * \param pValue pointer to a GLfloat where the value(s) will be written to
 */
void svt_scene::getFogParam(GLenum eParameter, GLfloat * pValue)
{
    switch (eParameter)
    {
	case GL_FOG_MODE:
	    *pValue = m_fFogMode;
	    break;
	case GL_FOG_DENSITY:
	    *pValue = m_fFogDensity;
	    break;
	case GL_FOG_START:
	    *pValue = m_fFogStart;
	    break;
	case GL_FOG_END:
	    *pValue = m_fFogEnd;
	    break;
	case GL_FOG_COORD_SRC:
	    *pValue = m_fFogCoordSrc;
	    break;
	case GL_FOG_COLOR:
	    memcpy(pValue, m_aFogColor, sizeof(GLfloat)*4);
	    break;
	case GL_LINEAR:
	    *pValue = m_fFogZoomOffset;
	    break;
	default:
	    break;
    }
}


/**
 * Get if ambient occlusion is supported at all (certain OpenGL and hardware requirements must be met)
 * \return bool true if ambient occlusion is possible, false if not.
 */
bool svt_scene::getAOSupported()
{
    return m_pFBO_AO->getAOSupported();
}

/**
 * Enable or disable ambient occlusion.
 * \param bAO true for enabling ambient occlusion, false for disabling ambient occlusion
 */
void svt_scene::setAOEnabled(bool bAO)
{
    m_bAOEnabled = (bAO && getAOSupported());
}

/**
 * Get if ambient occlusion is enabled.
 * \return bFog true for enabled ambient occlusion, false for disabled ambient occlusion
 */
bool svt_scene::getAOEnabled()
{
    return m_bAOEnabled;
}

/**
 * Set the radius of the sphere in which the ambient occlusion fragment shader samples the view
 * space. Increase the value if you want bigger creases/pockets to be darkened, decrease for
 * samller ones.
 * \param fRadius float that determines the size of the view space ambient occlusion sampling sphere
 */
void svt_scene::setAOSampleRadiusScaling(Real32 fRadius)
{
    m_fAORadiusScaling = fRadius;

    if ( m_pFBO_AO->getAOSupported() )
	m_pFBO_AO->setAOSampleRadiusScaling(m_fAORadiusScaling);
}
/**
 * Get the radius of the sphere in which the ambient occlusion fragment shader samples the view
 * space.
 * \return Real32
 */
Real32 svt_scene::getAOSampleRadiusScaling()
{
        return m_fAORadiusScaling;
}

/**
 * Set a factor between 0 and 1 that is used to modulate the ambient occlusion intensity
 * \param fIntensity float that representes AO intensity
 */
void svt_scene::setAOIntensity(Real32 fIntensity)
{
    m_fAOIntensity = fIntensity;
    if ( m_pFBO_AO->getAOSupported() )
        m_pFBO_AO->setAOIntensity(m_fAOIntensity);
}
/**
 * Get the ambient occlusion intensity factor
 * \return Real32
 */
Real32 svt_scene::getAOIntensity()
{
    return m_fAOIntensity;
}

/**
 * Set ambient occlusion method
 * \param iAOMethod enum that determines AO mode to be used
 */
void svt_scene::setAOMethod(int iAOMethod)
{
    m_iAOMethod = iAOMethod;

    if ( m_pFBO_AO->getAOSupported() )
        m_pFBO_AO->setAOMethod(iAOMethod);
}
/**
 * Set ambient occlusion method
 * \return int enum with the current AO mode
 */
int svt_scene::getAOMethod()
{
    return m_iAOMethod;;
}

/**
 *
 */
void svt_scene::setAllowAmbientOcclusion(bool bAllowAmbientOcclusion)
{
    // if AO is to be allowed, check for AO support. this will also init the shaders. if that
    // succeeded, set the ambient occlusion parameters
    if ( bAllowAmbientOcclusion )
    {
        if ( m_pFBO_AO->checkAOSupported() )
        {
            setAOSampleRadiusScaling( getAOSampleRadiusScaling() );
            setAOIntensity( getAOIntensity() );
            setAOMethod( getAOMethod() );
        }
    }

    svt_redraw();
}
