/***************************************************************************
                          svt_fbo_ao.cpp
                          ---------
    begin                : 24/02/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_shader_manager.h>
#include <svt_shader_cg_ao.h>
#include <svt_iostream.h>
#include <svt_fbo_ao.h>
#include <svt_matrix4.h>
#include <svt_init.h>
#include <svt_clipplane.h>

int   svt_fbo_ao::m_iMaxClipPlanes = -1;
bool* svt_fbo_ao::m_aClipPlanes    = NULL;

/**
 * Constructor
 */
svt_fbo_ao::svt_fbo_ao()
    : svt_fbo(),
    m_bAOSupported( false ),
    m_iFBOwidth( 0 ),
    m_iFBOheight( 0 ),
    m_iColortexture( 0 ),
    m_iAOtexture1( 0 ),
    m_iAOtexture2( 0 ),
    m_iDepthtexture( 0 ),
#ifdef MACOSX
    m_bAttachmentsSameFormat( true ),
#else
    m_bAttachmentsSameFormat( false ),
#endif
    m_fIntensity( 1.0f ),
    m_fRadius(1.0f),
    m_pAOShaderPass1( NULL ),
    m_pAOShaderPass1_PointBased( NULL ),
    m_pAOShaderPass1_LineBased( NULL ),
    m_pAOShaderPass2( NULL ),
    m_pAOShaderPass3( NULL ),
    m_iAOMethod( SVT_AO_LINE_BASED ),
    m_bShaderInitialized( false )
{
    // check if AO supported and init the framebuffer
    checkAOSupported();
}

/**
 * Destructor
 */
svt_fbo_ao::~svt_fbo_ao()
{
    if (m_iColortexture != 0)
    {
        glDeleteTextures(1, &m_iColortexture);
        glDeleteTextures(1, &m_iAOtexture1);
        glDeleteTextures(1, &m_iAOtexture2);
        glDeleteTextures(1, &m_iDepthtexture);
    }
}


/**
 * This is the method that needs to be called when this framebuffer should start collecting
 * depth and color for all geometry that should undergo ambient occlusion. Its arguments are the
 * width and height of the OpenGL window.
 * \param iFBOwidth integer with the current width (in pixel) of the OpenGL window
 * \param iFBOheight integer with the current height (in pixel) of the OpenGL window
 */
void svt_fbo_ao::beginAO(int iFBOwidth, int iFBOheight)
{
    // on the first run, initialize the shaders
    if (!m_bShaderInitialized && m_bAOSupported)
    {
        svt_shader_manager* pShaderManager = svt_getScene()->getShaderManager();
        m_pAOShaderPass1_PointBased = (svt_shader_cg_ao_pass1*)(pShaderManager->getAOShader(svt_shader_manager::SVT_SHADER_AO_PASS1_POINT_BASED));
        m_pAOShaderPass1_LineBased = (svt_shader_cg_ao_pass1*)(pShaderManager->getAOShader(svt_shader_manager::SVT_SHADER_AO_PASS1_LINE_BASED));
        m_pAOShaderPass2 = (svt_shader_cg_ao_pass2*)(pShaderManager->getAOShader(svt_shader_manager::SVT_SHADER_AO_PASS2));
        m_pAOShaderPass3 = (svt_shader_cg_ao_pass3*)(pShaderManager->getAOShader(svt_shader_manager::SVT_SHADER_AO_PASS3));
        switch (m_iAOMethod)
        {
            case SVT_AO_POINT_BASED:
                m_pAOShaderPass1 = m_pAOShaderPass1_PointBased;
                break;
            default:
                m_pAOShaderPass1 = m_pAOShaderPass1_LineBased;
                break;
        }
        m_pAOShaderPass1_PointBased->setAOIntensity(m_fIntensity);
        m_pAOShaderPass1_LineBased->setAOIntensity(m_fIntensity);
        m_pAOShaderPass1_PointBased->setAOSampleRadiusScaling(m_fRadius);
        m_pAOShaderPass1_LineBased->setAOSampleRadiusScaling(m_fRadius);
        m_bShaderInitialized = true;
    }

    bindBuffer();

    // if size of the canvas changed, re-init all attachments
    if (m_iFBOwidth != (GLsizei)iFBOwidth || m_iFBOheight != (GLsizei)iFBOheight)
    {
        m_iFBOwidth  = (GLsizei)iFBOwidth;
        m_iFBOheight = (GLsizei)iFBOheight;

        initAttachments();
    }

    Real32 fR = m_fR;
    Real32 fG = m_fG;
    Real32 fB = m_fB;

    // clear all color buffers
    int aMRT[] = { (int)GL_COLOR_ATTACHMENT1_EXT, (int)GL_COLOR_ATTACHMENT2_EXT };
    setDrawBuffers(2, aMRT);
    setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glColorMask( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
    clearAttachments();


    // redirect OpenGL color output into the first color attachment
    setDrawBuffers(1);

    setClearColor(fR, fG, fB, 0.0f);
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    clearAttachments();

    // for floats, not clamping is default behavior, so shouldn't be necessary
    //setColorClamping((unsigned int) GL_FALSE);
}

/**
 * This is the method that needs to be called when collecting data is done and ambient
 * occlusion is to be calculated and applied. Results are written to the window system
 * framebuffer for display
 */
void svt_fbo_ao::finishAO()
{
    // disable clip planes
    if ( svt_clipplane::getNumUsedClipPlanes() > 0)
    {
        if (m_iMaxClipPlanes == -1)
        {
            m_iMaxClipPlanes = svt_clipplane::getNumClipPlanes();
            m_aClipPlanes = svt_clipplane::getClipOcc();
        }

        for (GLint i=0; i<m_iMaxClipPlanes; ++i)
            if ( m_aClipPlanes[i] )
                glDisable(GL_CLIP_PLANE0 + i);
    }

    // disable blending
    glDisable(GL_BLEND);

    // disable depth testing
    glDisable(GL_DEPTH_TEST);


    // get the current projection matrix
    svt_matrix4< Real32 > oP;
    oP.loadProjectionMatrix();



    // set MVP matrix to identity, so we can easily draw
    // a full screen quad for running the ao post processing
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // following lines for debugging purposes
    //bindDisplayFramebuffer();
    //glEnable(GL_TEXTURE_2D);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glBindTexture(GL_TEXTURE_2D, m_iColortexture);
    //glBindTexture(GL_TEXTURE_2D, m_iDepthtexture);


    //
    // pass 1
    //
    // the pass1 shader checks around a sample point (the backprojected pixel) for cavity-like
    // geometry. result is written into GL_COLOR_ATTACHMENT1 (m_iAOtexture1)
    //
    int aMRT[] = { (int)GL_COLOR_ATTACHMENT1_EXT };
    setDrawBuffers(1, aMRT);

    glColorMask( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
    glDepthMask(GL_FALSE);


    m_pAOShaderPass1->setP1P2(oP[0][0], oP[1][1]);
    m_pAOShaderPass1->setABCD(oP[0][2], oP[1][2], oP[2][2], oP[2][3]);
    m_pAOShaderPass1->setFBOsize(m_iFBOwidth, m_iFBOheight);
    m_pAOShaderPass1->setDepthTexture(m_iDepthtexture);


    m_pAOShaderPass1->bind();
    glBegin(GL_QUADS);
        glVertex2i(-1, -1);
        glVertex2i( 1, -1);
        glVertex2i( 1,  1);
        glVertex2i(-1,  1);
//      glTexCoord2i(0, 0); glVertex2i(-1, -1);
//      glTexCoord2i(1, 0); glVertex2i( 1, -1);
//      glTexCoord2i(1, 1); glVertex2i( 1,  1);
//      glTexCoord2i(0, 1); glVertex2i(-1,  1);
    glEnd();
    m_pAOShaderPass1->unbind();


    // following lines for debugging purposes
    //bindDisplayFramebuffer();
    //glEnable(GL_TEXTURE_2D);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glBindTexture(GL_TEXTURE_2D, m_iColortexture);
    //glBindTexture(GL_TEXTURE_2D, m_iAOtexture1);
    //glBindTexture(GL_TEXTURE_2D, m_iDepthtexture);



    //
    // pass 2
    //
    // the pass2 shader blurres the ao coefficients in x-direction;
    // result is written into GL_COLOR_ATTACHMENT2 (m_iAOtexture2)
    //
    aMRT[0] = (int)GL_COLOR_ATTACHMENT2_EXT;
    setDrawBuffers(1, aMRT);

    m_pAOShaderPass2->setFBOsize(m_iFBOwidth, m_iFBOheight);
    m_pAOShaderPass2->setDepthTexture(m_iDepthtexture);
    m_pAOShaderPass2->setAOTexture(m_iAOtexture1);

    m_pAOShaderPass2->bind();
    glBegin(GL_QUADS);
        glVertex2i(-1, -1);
        glVertex2i( 1, -1);
        glVertex2i( 1,  1);
        glVertex2i(-1,  1);
//      glTexCoord2i(0, 0); glVertex2i(-1, -1);
//      glTexCoord2i(1, 0); glVertex2i( 1, -1);
//      glTexCoord2i(1, 1); glVertex2i( 1,  1);
//      glTexCoord2i(0, 1); glVertex2i(-1,  1);
    glEnd();
    m_pAOShaderPass2->unbind();



    //
    // pass 3
    //
    // the pass3 shader blurres the ao coefficients in y-direction;
    // the color modulated with the result is written into the window system framebuffer
    //
    bindDisplayFramebuffer();

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask(GL_TRUE);

    // re-enable depth testing
    glEnable(GL_DEPTH_TEST);


    // following lines for debugging purposes
    //glEnable(GL_TEXTURE_2D);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glBindTexture(GL_TEXTURE_2D, m_iColortexture);
    //glBindTexture(GL_TEXTURE_2D, m_iAOtexture1);
    //glBindTexture(GL_TEXTURE_2D, m_iAOtexture2);
    //glBindTexture(GL_TEXTURE_2D, m_iDepthtexture);


    // the pass3 shader blurres the ao coefficients in y-direction and modulates RGB with the
    // result. It writes into the window system framebuffer
    //
    m_pAOShaderPass3->setFBOsize(m_iFBOwidth, m_iFBOheight);
    m_pAOShaderPass3->setDepthTexture(m_iDepthtexture);
    m_pAOShaderPass3->setColorTexture(m_iColortexture);
    m_pAOShaderPass3->setAOTexture(m_iAOtexture2);


    m_pAOShaderPass3->bind();
    glBegin(GL_QUADS);
        glVertex2i(-1, -1);
        glVertex2i( 1, -1);
        glVertex2i( 1,  1);
        glVertex2i(-1,  1);
//      glTexCoord2i(0, 0); glVertex2i(-1, -1);
//      glTexCoord2i(1, 0); glVertex2i( 1, -1);
//      glTexCoord2i(1, 1); glVertex2i( 1,  1);
//      glTexCoord2i(0, 1); glVertex2i(-1,  1);
    glEnd();
    m_pAOShaderPass3->unbind();


    // restore MVP matrix
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // re-enable clipplanes
    for (GLint i=0; i<m_iMaxClipPlanes; ++i)
    {
        if (m_aClipPlanes[i])
            glEnable(GL_CLIP_PLANE0 + i);
    }
}

/**
 * This method checks if the size of the attached textures needs to be adjusted (according to
 * the OpenGL window size) and does that if necessary. On first run, it generate the OpenGL
 * texture objects.
 */
void svt_fbo_ao::initAttachments()
{
    bool bFirstRun = false;

    if (m_iColortexture == 0)
    {
        glGenTextures(1, &m_iColortexture);
        bFirstRun = true;
    }
    glBindTexture(GL_TEXTURE_2D, m_iColortexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iFBOwidth, m_iFBOheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    if (bFirstRun)
        glGenTextures(1, &m_iAOtexture1);
    glBindTexture(GL_TEXTURE_2D, m_iAOtexture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (m_bAttachmentsSameFormat)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iFBOwidth, m_iFBOheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, 1, m_iFBOwidth, m_iFBOheight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    if (bFirstRun)
        glGenTextures(1, &m_iAOtexture2);
    glBindTexture(GL_TEXTURE_2D, m_iAOtexture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (m_bAttachmentsSameFormat)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iFBOwidth, m_iFBOheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, 1, m_iFBOwidth, m_iFBOheight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    if (bFirstRun)
        glGenTextures(1, &m_iDepthtexture);
    glBindTexture(GL_TEXTURE_2D, m_iDepthtexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_iFBOwidth, m_iFBOheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    // attach the textures to the framebuffer
    if (bFirstRun)
    {
        attachColorTexture(m_iColortexture); // GL_COLOR_ATTACHMENT0
        attachColorTexture(m_iAOtexture1);   // GL_COLOR_ATTACHMENT1
        attachColorTexture(m_iAOtexture2);   // GL_COLOR_ATTACHMENT2

        attachDepthTexture(m_iDepthtexture);

        if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
        {
            // driver/card does not like attachments with different formats.
            // so make all attachments same format. needs more memory, but works
            m_bAttachmentsSameFormat = true;

            releaseColorAttachments();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, m_iAOtexture1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iFBOwidth, m_iFBOheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, m_iAOtexture2);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iFBOwidth, m_iFBOheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

            attachColorTexture(m_iColortexture); // GL_COLOR_ATTACHMENT0
            attachColorTexture(m_iAOtexture1);   // GL_COLOR_ATTACHMENT1
            attachColorTexture(m_iAOtexture2);   // GL_COLOR_ATTACHMENT2
        }
    }

    getIsComplete();
}

/**
 * Check if ambient occlusion is supported (i.e., all OpenGL functionality is present).
 */
bool svt_fbo_ao::checkAOSupported()
{
    // if those are now allowed, don't even think about it
    if ( !svt_getAllowAmbientOcclusion() || !svt_getAllowShaderPrograms() )
    {
        m_bAOSupported = false;
        return false;
    }

    // check for required OpenGL extensions
    if ( getFBOSupported() && getMRTSupported() && svt_isExtensionSupported("GL_ARB_texture_non_power_of_two") )
        m_bAOSupported = true;

    // same attachment format is safer for some implementations
    if ( svt_getRenderer() == SVT_RENDERER_MESA || svt_getRenderer() == SVT_RENDERER_INTEL )
        m_bAttachmentsSameFormat = true;

    // generate the OpenGL framebuffer object - does nothing if already initialized
    genBuffer();

    return m_bAOSupported;
}

/**
 * Get if ambient occlusion is supported (i.e., all OpenGL functionality is present).
 */
bool svt_fbo_ao::getAOSupported()
{
    return m_bAOSupported;
}

/**
 * Set the value for the intensity of ambient occlusion. Set to zero for no intensity (like
 * switching AO off), and to one for full intensity. Values greater than one are also possible
 * \param fIntensity float with the desired intensity value
 */
void svt_fbo_ao::setAOIntensity(Real32 fIntensity)
{
    m_fIntensity = fIntensity;

    if (!m_bShaderInitialized)
        return;

    m_pAOShaderPass1_PointBased->setAOIntensity(fIntensity);
    m_pAOShaderPass1_LineBased->setAOIntensity(fIntensity);
}

/**
 * Set the scaling of the radius of the view space sampling sphere (see svt_shader_cg_ao_pass1)
 * \param fRadius float with the desired radius scaling factor
 */
void svt_fbo_ao::setAOSampleRadiusScaling(Real32 fRadius)
{
    m_fRadius = fRadius;

    if (!m_bShaderInitialized)
        return;

    m_pAOShaderPass1_PointBased->setAOSampleRadiusScaling(fRadius);
    m_pAOShaderPass1_LineBased->setAOSampleRadiusScaling(fRadius);
}

/**
 * Set ambient occlusion method
 * \param iAOMethod enum that determines AO mode to be used
 */
void svt_fbo_ao::setAOMethod(int iAOMethod)
{
    m_iAOMethod = iAOMethod;

    if ( !m_bShaderInitialized )
        return;

    switch (m_iAOMethod)
    {
        case SVT_AO_POINT_BASED:
        {
            m_pAOShaderPass1 = m_pAOShaderPass1_PointBased;
            break;
        }
        case SVT_AO_LINE_BASED:
        {
            m_pAOShaderPass1 = m_pAOShaderPass1_LineBased;
            break;
        }
    }
}
/**
 * Set ambient occlusion method
 * \return int enum with the current AO mode
 */
int svt_fbo_ao::getAOMethod()
{
    return m_iAOMethod;;
}
