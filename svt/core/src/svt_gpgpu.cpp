/***************************************************************************
                          svt_gpgpu
                          ---------------------
    begin                : 07/21/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_gpgpu.h>
#include <svt_iostream.h>
#include <svt_opengl.h>
#include <svt_time.h>

SVT_PFNGLGENFRAMEBUFFERS        svt_gpgpu::glGenFramebuffersEXT        = NULL;
SVT_PFNGLBINDFRAMEBUFFER        svt_gpgpu::glBindFramebufferEXT        = NULL;
SVT_PFNGLDELETEFRAMEBUFFERS     svt_gpgpu::glDeleteFramebuffersEXT     = NULL;
SVT_PFNGLFRAMEBUFFERTEXTURE2D   svt_gpgpu::glFramebufferTexture2DEXT   = NULL;
SVT_PFNGLCHECKFRAMEBUFFERSTATUS svt_gpgpu::glCheckFramebufferStatusEXT = NULL;

/**
* Constructor 
*/
svt_gpgpu::svt_gpgpu()
{
    if (glGenFramebuffersEXT == NULL)
    {
	glGenFramebuffersEXT =        (SVT_PFNGLGENFRAMEBUFFERS)        svt_lookupFunction("glGenFramebuffersEXT");
	glBindFramebufferEXT =        (SVT_PFNGLBINDFRAMEBUFFER)        svt_lookupFunction("glBindFramebufferEXT");
	glDeleteFramebuffersEXT =     (SVT_PFNGLDELETEFRAMEBUFFERS)     svt_lookupFunction("glDeleteFramebuffersEXT");
	glFramebufferTexture2DEXT =   (SVT_PFNGLFRAMEBUFFERTEXTURE2D)   svt_lookupFunction("glFramebufferTexture2DEXT");
	glCheckFramebufferStatusEXT = (SVT_PFNGLCHECKFRAMEBUFFERSTATUS) svt_lookupFunction("glCheckFramebufferStatusEXT");
    }

    if (svt_isExtensionSupported("GL_ARB_texture_float") || svt_isExtensionSupported("GL_ATI_texture_float"))
    {
        m_iIntexInternalFormat = 0x8815; // GL_RGB32F_ARB    == RGB_FLOAT32_ATI       == 0x8815
	m_iOutexInternalFormat = 0x8815; // LUMINANCE32F_ARB == LUMINANCE_FLOAT32_ATI == 0x8818
	m_iIntexTarget = GL_TEXTURE_1D;
	m_iOutexTarget = GL_TEXTURE_2D;
	m_iIntexFormat = GL_RGB;
	m_iOutexFormat = GL_RGB;
    }
//     else 
//     if (svt_isExtensionSupported("GL_NV_float_buffer"))
//     {
//         m_iIntexInternalFormat = 0x8889; // FLOAT_RGB32_NV == 0x8889
//  	m_iOutexInternalFormat = 0x8885; // FLOAT_R32_NV   == 0x8885
// 	m_iIntexTarget = GL_TEXTURE_1D;
// 	m_iOutexTarget = 0x84F5;         // GL_TEXTURE_RECTANGLE_ARB == 0x84F5
// 	m_iIntexFormat = GL_RGB;
// 	m_iOutexFormat = GL_LUMINANCE;
//     }
    else
    {
	m_iIntexInternalFormat = GL_RGB;
	m_iOutexInternalFormat = GL_LUMINANCE;
    }

    m_iFB = 0;
}


/**
* Destructor
*/
svt_gpgpu::~svt_gpgpu()
{
    glDeleteFramebuffersEXT(1, &m_iFB);
}


/**
*
*/
void svt_gpgpu::initFBO()
{
}


/**
 * 
 */
void svt_gpgpu::setInputData(Real32* pInputData, unsigned int iSize)
{
    m_iTexSize = iSize;

    GLuint iInputTexID;

    glGenTextures(1, &iInputTexID);
    glBindTexture(m_iIntexTarget, iInputTexID);

    svt_checkGLErrors("setInputData glBindTexture: ");

    glTexImage1D(m_iIntexTarget,
		 0,
		 m_iIntexInternalFormat,
		 m_iTexSize,
		 0,
		 m_iIntexFormat,
		 GL_FLOAT,
		 pInputData);

    svt_checkGLErrors("setInputData glTexImage1D: ");

    glTexParameteri(m_iIntexTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_iIntexTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(m_iIntexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(m_iIntexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    svt_checkGLErrors("setInputData glTexImage1D: ");

    m_pShader->addInputTex(iInputTexID);
}


/**
*
*/
void svt_gpgpu::compute(Real32 * pResult)
{
    unsigned int iTime = svt_getToD();

//     glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_iMaxTexSize);
//     cout << "Maximum Texture Size: " << m_iMaxTexSize << "\n";

    glGenTextures(1, &m_iOutputTexID);
    glBindTexture(m_iOutexTarget, m_iOutputTexID);
    svt_checkGLErrors("compute glBindTexture: ");
    glTexImage2D(m_iOutexTarget,
		 0,
		 m_iOutexInternalFormat,
		 m_iTexSize,
		 m_iTexSize,
		 0,
		 m_iOutexFormat,
		 GL_FLOAT,
		 0);

    svt_checkGLErrors("compute glTexImage2D: ");
    glTexParameteri(m_iOutexTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_iOutexTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(m_iOutexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(m_iOutexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


    if (m_iFB == 0)
	glGenFramebuffersEXT(1, &m_iFB);
    glBindFramebufferEXT(0x8D40, m_iFB);// 0x8D40 == GL_FRAMEBUFFER_EXT

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, (Real32)m_iTexSize, 0.0f, (Real32)m_iTexSize);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, m_iTexSize, m_iTexSize);

    glFramebufferTexture2DEXT(0x8D40, // 0x8D40 == GL_FRAMEBUFFER_EXT,
			      0x8CE0, // 0x8CE0 == GL_COLOR_ATTACHMENT0_EXT,
			      m_iOutexTarget,
			      m_iOutputTexID,
			      0);

    svt_checkGLErrors("glFramebufferTexture2DEXT: ");


    printf ("svt_gpgpu:: msec for setting up FBO: %li\n", svt_getToD() - iTime); // --------------- time
    checkFramebufferStatus();


    iTime = svt_getToD(); // --------------- time


    m_pShader->bind();

    if (m_iOutexTarget == GL_TEXTURE_2D)
    {
	glBegin(GL_QUADS);			   
	    glTexCoord2f(0.0, 0.0); 	   
	    glVertex2f(0.0, 0.0);		   
	    glTexCoord2f(1.0, 0.0); 	   
	    glVertex2f(m_iTexSize, 0.0);	   
	    glTexCoord2f(1.0, 1.0);		   
	    glVertex2f(m_iTexSize, m_iTexSize);
	    glTexCoord2f(0.0, 1.0); 	   
	    glVertex2f(0.0, m_iTexSize);	   
	glEnd();                               
    }
    else if (m_iOutexTarget == GL_TEXTURE_RECTANGLE_ARB)
    {
	glBegin(GL_QUADS);			   
	    glTexCoord2f(0, 0);
	    glVertex2f(0.0, 0.0);		   
	    glTexCoord2f(m_iTexSize, 0); 	   
	    glVertex2f(m_iTexSize, 0.0);	   
	    glTexCoord2f(m_iTexSize, m_iTexSize);		   
	    glVertex2f(m_iTexSize, m_iTexSize);
	    glTexCoord2f(0, m_iTexSize); 	   
	    glVertex2f(0, m_iTexSize);	   
	glEnd();                               
    }

    m_pShader->unbind();


    printf ("svt_gpgpu:: msec for computing: %li\n", svt_getToD() - iTime);


    iTime = svt_getToD();

//     glGetTexImage(m_iOutexTarget, // works fine, too, but takes almost 100% more time than ReadPixels
// 		  0,
// 		  m_iOutexFormat,
// 		  GL_FLOAT,
// 		  pResult);
    glReadBuffer(0x8CE0); // 0x8CE0 == GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, m_iTexSize, m_iTexSize, m_iOutexFormat, GL_FLOAT, pResult);

    printf ("svt_gpgpu:: msec for downloading data: %li\n", svt_getToD() - iTime);

    releaseFBO();
}

/**
*
*/
void svt_gpgpu::releaseFBO()
{
    glBindFramebufferEXT(0x8D40, 0); // 0x8D40 == GL_FRAMEBUFFER_EXT,
}

/**
*
*/
bool svt_gpgpu::checkFramebufferStatus()
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(0x8D40); // 0x8D40 == GL_FRAMEBUFFER_EXT,
    switch(status)
    {
        case 0x8CD5:
	    return true;
        case 0x8CD6:
	    cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n";
	    return false;
	case 0x8CD7:
	    cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n";
	    return false;
	case 0x8CD9:
	    cout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n";
	    return false;
	case 0x8CDA:
	    cout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n";
	    return false;
	case 0x8CDB:
	    cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n";
	    return false;
	case 0x8CDC:
	    cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n";
	    return false;
	case 0x8CDD:
	    cout << "GL_FRAMEBUFFER_UNSUPPORTED_EXT\n";
	    return false;
    }
    return false;
}

/**
 *
 */
bool svt_gpgpu::checkUnclampedFloats()
{
    if (m_iIntexInternalFormat != GL_RGB)
	return true;
    else
	return false;
}


/**
 * 
 */
void svt_gpgpu::addShader(bool bFile, char * pVertex, char * pFragment, CGprofile pVertProfile, CGprofile pFragProfile)
{
    m_pShader = new svt_shader_cg_gpgpu(bFile, pVertex, pFragment, pVertProfile, pFragProfile);
}
