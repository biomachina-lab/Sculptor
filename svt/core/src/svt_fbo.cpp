/***************************************************************************
                          svt_fbo.cpp
                          ---------
    begin                : 24/02/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_types.h>
#include <svt_fbo.h>


SVT_PFNGLCLAMPCOLORARBPROC                          svt_fbo::glClampColorARB                          = NULL;
SVT_PFNGLDRAWBUFFERSARBPROC                         svt_fbo::glDrawBuffersARB                         = NULL;
SVT_PFNGLISFRAMEBUFFEREXTPROC                       svt_fbo::glIsFramebufferEXT                       = NULL;
SVT_PFNGLGENFRAMEBUFFERSEXTPROC                     svt_fbo::glGenFramebuffersEXT                     = NULL;
SVT_PFNGLBINDFRAMEBUFFEREXTPROC                     svt_fbo::glBindFramebufferEXT                     = NULL;
SVT_PFNGLGENRENDERBUFFERSEXTPROC                    svt_fbo::glGenRenderbuffersEXT                    = NULL;
SVT_PFNGLBINDRENDERBUFFEREXTPROC                    svt_fbo::glBindRenderbufferEXT                    = NULL;
SVT_PFNGLDELETERENDERBUFFERSEXTPROC                 svt_fbo::glDeleteRenderbuffersEXT                 = NULL;
SVT_PFNGLRENDERBUFFERSTORAGEEXTPROC                 svt_fbo::glRenderbufferStorageEXT                 = NULL;
SVT_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                svt_fbo::glFramebufferTexture2DEXT                = NULL;
SVT_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              svt_fbo::glCheckFramebufferStatusEXT              = NULL;
SVT_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             svt_fbo::glFramebufferRenderbufferEXT             = NULL;
SVT_PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC svt_fbo::glGetFramebufferAttachmentParameterivEXT = NULL;
SVT_PFNGLDELETEFRAMEBUFFERSEXTPROC                  svt_fbo::glDeleteFramebuffersEXT                  = NULL;

bool svt_fbo::m_bFBOSupported = true;
bool svt_fbo::m_bMRTSupported = true;

svt_fbo::svt_fbo() :
    m_iBuffer( 0 ),
    m_aMRT( NULL ),
    m_iMRTSize( 0 ),
    m_fR( 0.0f ),
    m_fG( 0.0f ),
    m_fB( 0.0f ),
    m_fA( 1.0f ),
    m_fD( 1.0f ),
    m_fS( 0.0f ),
    m_iNumColorAttachments( 0 ),
    m_bColorAttachment( false ),
    m_bDepthAttachment( false ),
    m_bStencilAttachment( false )
{

#ifndef MACOSX

    // check for availability of framebuffer objects
    if (!svt_checkEXT_FBO())
    {
	svtout << "Framebuffer objects not supported" << endl;

#if defined (DEBUG) || defined (EXPERIMENTAL)
	printf("Framebuffer objects not supported\n");
#endif

	m_bFBOSupported = false;
        return;
    }


    // check for availability of multiple render targets
    if (!svt_isExtensionSupported("GL_ARB_draw_buffers") && !svt_isExtensionSupported("GL_ATI_draw_buffers"))
    {
	svtout << "Multiple render targets not supported" << endl;

#if defined (DEBUG) || defined (EXPERIMENTAL)
	printf("Multiple render targets not supported\n");
#endif

	m_bMRTSupported = false;
    }

#endif


    if (glGenFramebuffersEXT == NULL)
    {
        glGenFramebuffersEXT =                     (SVT_PFNGLGENFRAMEBUFFERSEXTPROC)                     svt_lookupFunction("glGenFramebuffersEXT");
        glBindFramebufferEXT =                     (SVT_PFNGLBINDFRAMEBUFFEREXTPROC)                     svt_lookupFunction("glBindFramebufferEXT");
        glFramebufferTexture2DEXT =                (SVT_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)                svt_lookupFunction("glFramebufferTexture2DEXT");
        glCheckFramebufferStatusEXT =              (SVT_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)              svt_lookupFunction("glCheckFramebufferStatusEXT");
        glGetFramebufferAttachmentParameterivEXT = (SVT_PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) svt_lookupFunction("glGetFramebufferAttachmentParameterivEXT");
        glDeleteFramebuffersEXT =                  (SVT_PFNGLDELETEFRAMEBUFFERSEXTPROC)                  svt_lookupFunction("glDeleteFramebuffersEXT");
        glIsFramebufferEXT =                       (SVT_PFNGLISFRAMEBUFFEREXTPROC)                       svt_lookupFunction("glIsFramebufferEXT");

        glGenRenderbuffersEXT =                    (SVT_PFNGLGENRENDERBUFFERSEXTPROC)                    svt_lookupFunction("glGenRenderbuffersEXT");
        glBindRenderbufferEXT =                    (SVT_PFNGLBINDRENDERBUFFEREXTPROC)                    svt_lookupFunction("glBindRenderbufferEXT");
        glRenderbufferStorageEXT =                 (SVT_PFNGLRENDERBUFFERSTORAGEEXTPROC)                 svt_lookupFunction("glRenderbufferStorageEXT");
        glFramebufferRenderbufferEXT =             (SVT_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)             svt_lookupFunction("glFramebufferRenderbufferEXT");
        glDeleteRenderbuffersEXT =                 (SVT_PFNGLDELETERENDERBUFFERSEXTPROC )                svt_lookupFunction("glDeleteRenderbuffersEXT");

	glClampColorARB  =  (SVT_PFNGLCLAMPCOLORARBPROC)  svt_lookupFunction("glClampColorARB");
    }

    if (m_bMRTSupported && glDrawBuffersARB == NULL)
	glDrawBuffersARB =  (SVT_PFNGLDRAWBUFFERSARBPROC) svt_lookupFunction("glDrawBuffersARB");

}


svt_fbo::~svt_fbo()
{
    if (m_bFBOSupported && getIsBuffer())
        deleteBuffer();

    if (m_aMRT != NULL)
	delete[] m_aMRT;
}


void svt_fbo::setDrawBuffers(int iSize, int * aMRT)
{
    if (m_bMRTSupported)
    {

	if (m_aMRT != NULL)
	    delete [] m_aMRT;

	m_aMRT = new GLenum[iSize];
	m_iMRTSize = iSize;

	if (aMRT == NULL)
	{
	    for (int i=0; i<m_iMRTSize; ++i)
		m_aMRT[i] = (GLenum) ((int)GL_COLOR_ATTACHMENT0_EXT + i);
	}
	else
	{
	    for (int i=0; i<m_iMRTSize; ++i)
		m_aMRT[i] = (GLenum)aMRT[i];
	}

	glDrawBuffersARB(iSize, (const GLenum *) m_aMRT);
    }
}


void svt_fbo::genBuffer()
{
    if (getIsBuffer())
       return;

    if (m_bFBOSupported)
    {
        glGenFramebuffersEXT(1, &m_iBuffer);

#if defined (DEBUG) || defined (EXPERIMENTAL)
        svt_checkGLErrors( "svt_fbo: glGenFramebuffersEXT()" );
#endif
    }
}


void svt_fbo::deleteBuffer()
{
    if (getIsBuffer())
    {
        glDeleteFramebuffersEXT(1, &m_iBuffer);

#if defined (DEBUG) || defined (EXPERIMENTAL)
        svt_checkGLErrors( "svt_fbo: glDeleteFramebuffersEXT()" );
#endif
    }
}


bool svt_fbo::getIsBuffer()
{
    GLboolean b = GL_FALSE;

    if (m_bFBOSupported)
    {
        b = glIsFramebufferEXT(m_iBuffer);

#if defined (DEBUG) || defined (EXPERIMENTAL)
        svt_checkGLErrors( "svt_fbo: glIsFramebufferEXT()" );
#endif
    }

    return (b == GL_TRUE);
}


bool svt_fbo::getIsComplete()
{
    GLenum iFBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch(iFBOstatus)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            //svtout << "GL_FRAMEBUFFER_COMPLETE_EXT" << endl;
            //printf("GL_FRAMEBUFFER_COMPLETE_EXT\n");
            return true;
            break; 
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            svtout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << endl;
            //printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            svtout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << endl;
            //printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            svtout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
            //printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            svtout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
            //printf("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            svtout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
            //printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            svtout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
            //printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            svtout << "GL_FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
            //printf("GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");
            break;
        default:
            svtout << "Framebuffer not complete, unknown reason" << endl;
            //printf("Framebuffer not complete, unknown reason\n");
            break;
    }

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::getIsComplete");
#endif

    return false;
}


void svt_fbo::bindBuffer()
{
    if (m_bFBOSupported)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iBuffer);

#if defined (DEBUG) || defined (EXPERIMENTAL)
        svt_checkGLErrors("svt_fbo::bindBuffer");
#endif
    }
}


void svt_fbo::bindDisplayFramebuffer()
{
    if (m_bFBOSupported)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

#if defined (DEBUG) || defined (EXPERIMENTAL)
        svt_checkGLErrors( "svt_fbo: bindDisplayFramebuffer" );
#endif
    }
}


void svt_fbo::attachColorTexture(GLuint iName)
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + m_iNumColorAttachments, GL_TEXTURE_2D, iName, 0);

    ++m_iNumColorAttachments;
    m_bColorAttachment = true;

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::attachColorTexture");
#endif
}


void svt_fbo::releaseColorAttachments()
{
    for (GLuint i=0; i<m_iNumColorAttachments; ++i)
    {
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0, 0);
    }

    m_iNumColorAttachments = 0;
    m_bColorAttachment = false;

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::releaseColorAttachments");
#endif
}


void svt_fbo::attachDepthTexture(GLuint iName)
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, iName, 0);
    m_bDepthAttachment = true;

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::attachDepthTexture");
#endif
}


void svt_fbo::releaseDepthAttachment()
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
    m_bDepthAttachment = false;

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::releaseDepthAttachment");
#endif
}


void svt_fbo::attachStencilTexture(GLuint iName)
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, iName, 0);
    m_bStencilAttachment = true;

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::attachStencilTexture");
#endif
}


void svt_fbo::releaseStencilAttachment()
{
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, 0, 0, 0);
    m_bStencilAttachment = false;

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo::releaseStencilAttachment");
#endif
}


void svt_fbo::clearAttachments()
{
    GLbitfield iBits = 0;

    if (m_bColorAttachment)
    {
        iBits = iBits | GL_COLOR_BUFFER_BIT;
        glClearColor(m_fR, m_fG, m_fB, m_fA);
    }
    if (m_bDepthAttachment)
    {
        iBits = iBits | GL_DEPTH_BUFFER_BIT;
        glClearDepth(m_fD);
    }
    if (m_bStencilAttachment)
    {
        iBits = iBits | GL_STENCIL_BUFFER_BIT;
        glClearStencil(m_fS);
    }

    glClear(iBits);
}


void svt_fbo::setClearColor(Real32 fR, Real32 fG, Real32 fB, Real32 fA)
{
    m_fR = (GLfloat)fR;
    m_fG = (GLfloat)fG;
    m_fB = (GLfloat)fB;
    m_fA = (GLfloat)fA;
}


void svt_fbo::setClearStencil(Real32 fS)
{
    m_fS = (GLfloat)fS;
}


void svt_fbo::setClearDepth(Real32 fD)
{
    m_fD = (GLfloat)fD;
}


void svt_fbo::setColorClamping(unsigned int iClampMode)
{
    //GLint i;
    //glGetIntegerv(GL_RGBA_FLOAT_MODE_ARB, &i);
    //svtout << "float color clamping: " << i << endl;

    glClampColorARB(GL_CLAMP_FRAGMENT_COLOR, (GLenum)iClampMode);
    glClampColorARB(GL_CLAMP_VERTEX_COLOR,   (GLenum)iClampMode);
    glClampColorARB(GL_CLAMP_READ_COLOR,     (GLenum)iClampMode);

#if defined (DEBUG) || defined (EXPERIMENTAL)
    svt_checkGLErrors("svt_fbo_ao::setColorClamping");
#endif
}


bool svt_fbo::getFBOSupported()
{
    return m_bFBOSupported;
}


bool svt_fbo::getMRTSupported()
{
    return m_bMRTSupported;
}
