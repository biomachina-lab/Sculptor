/***************************************************************************
                          svt_texture_3d.cpp
			  ------------------
    begin                : 09.10.01
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_texture_3d.h>
#include <svt_file_utils.h>
#include <svt_pic_reader.h>
#include <svt_window.h>
#include <svt_init.h>
#include <svt_opengl.h>
#include <svt_time.h>
#include <svt_opengl.h>
// clib includes
#include <string.h>
#include <svt_cmath.h>
#include <svt_iostream.h>

// defines for the 3d texture extension

typedef GLvoid (APIENTRY *GLTEXIMAGE3DEXTFUNCPTR)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
GLTEXIMAGE3DEXTFUNCPTR glTexImage3DPtr = NULL;
#define GL_TEXTURE_3D 0x806F

/**
 * Constructor
 */
svt_texture_3d::svt_texture_3d() :
    m_pcData( NULL ),
    m_piTexHandle( NULL ),
    m_pbTexValid( NULL ),
    m_iSizeX( 0 ),
    m_iSizeY( 0 ),
    m_iSizeZ( 0 ),
    m_iInternType( GL_RGB),
    m_iDataType( GL_RGB ),
    m_iMinFilter( SVT_FILTER_LINEAR ),
    m_iMagFilter( SVT_FILTER_LINEAR )
{
    // create arrays
    m_piTexHandle = new GLuint[svt_getWindowsNum()];
    m_pbTexValid  = new bool[svt_getWindowsNum()];

    if (m_piTexHandle == NULL || m_pbTexValid == NULL)
	svt_fatalError("svt_texture_3d::svt_texture3d()> memory management error!");

    // reset all texture handles
    int i;
    for(i=0; i<svt_getWindowsNum(); i++)
    {
        m_piTexHandle[i] = 0;
        m_pbTexValid[i] = false;
    }

    glTexImage3DPtr = NULL;
}
svt_texture_3d::~svt_texture_3d()
{
    if (m_pcData != NULL)
        delete [] m_pcData;

    deleteTextureHandles();
}

/**
 * bindTexture() binds the image to the current texture so the next OpenGL commands in this context are going to use it.
 */
void svt_texture_3d::bindTexture()
{
    // if  we dont have any texture data, then quit
    if (m_pcData == NULL)
        return;

    svt_win_handle win = svt_getCurrentWindow();

    if (!m_pbTexValid[win-1])
        createTextureInContext(win);

    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, getMagFilter());
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, getMinFilter());

    if (m_pbTexValid[win-1])
        glBindTexture(GL_TEXTURE_3D, m_piTexHandle[win-1]);
};

/**
 * apply to GL (calls bind texture)
 */
void svt_texture_3d::applyGL()
{
    bindTexture();
};

/**
 * Set the 3D texture to a new RGBA data block.
 * Attention: This function will delete the old data block!!!
 */
void svt_texture_3d::setDataRGBA(const unsigned char* pcData, int iSizeX, int iSizeY, int iSizeZ)
{
    //svtout << "DEBUG setDataRGBA()- Size: ("<< iSizeX << "," << iSizeY << "," << iSizeZ << ")" << endl;

    // free old texture data
    if (m_pcData != NULL)
        delete m_pcData;

    // new size
    m_iSizeX = (unsigned long int)iSizeX;
    m_iSizeY = (unsigned long int)iSizeY;
    m_iSizeZ = (unsigned long int)iSizeZ;

    // reset all texture handles
    deleteTextureHandles();

    // set the data
    m_pcData = (unsigned char*)pcData;

    // data type
    m_iInternType = GL_RGBA;
    m_iDataType = GL_RGBA;

    // set filters
    m_iMinFilter = m_iMagFilter = SVT_FILTER_LINEAR;
}

/**
 * set the minification filter method
 */
void svt_texture_3d::setMinFilter(int iMinFilter)
{
    m_iMinFilter = iMinFilter;
};
/**
 * get the minification filter method
 */
int svt_texture_3d::getMinFilter() const
{
    return m_iMinFilter;
};
/**
 * set the magnification filter method
 */
void svt_texture_3d::setMagFilter(int iMagFilter)
{
    m_iMagFilter = iMagFilter;
};
/**
 * get the magnification filter method
 */
int svt_texture_3d::getMagFilter() const
{
    return m_iMagFilter;
};

/**
 * get opengl ID of texture object
 */
GLuint svt_texture_3d::getTextureID(int iWin) const
{
    return m_piTexHandle[iWin-1];
};
/**
 * get opengl ID of texture object
 */
GLuint svt_texture_3d::getTextureID() const
{
    svt_win_handle iWin = svt_getCurrentWindow();

    return m_piTexHandle[iWin-1];
};

///////////////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////////////

/**
 * for internal use only: create the texture in a opengl context
 */
void svt_texture_3d::createTextureInContext(svt_win_handle iWin)
{
    svt_checkGLErrors("svt_texture_3d::CreateTextureInContext()> Begin>");

    if (m_pbTexValid[iWin-1] == true)
        glDeleteTextures(1,(GLuint*)&m_piTexHandle[iWin-1]);

    glGenTextures(1, (GLuint*)&m_piTexHandle[iWin-1]);
    glBindTexture(GL_TEXTURE_3D, m_piTexHandle[iWin-1]);

#ifndef MACOSX

    if (glTexImage3DPtr == NULL)
    {

	// dont know why, but windows is not reporting this extension, but most drivers are supporting it. Therefore we are not checking it under windows
#ifndef WIN32
        if (svt_isExtensionSupported("GL_EXT_texture3D"))
#endif

            glTexImage3DPtr = (GLTEXIMAGE3DEXTFUNCPTR) svt_lookupFunction((const char*)"glTexImage3D");
    }

    if (glTexImage3DPtr == NULL)
    {
	cout << "svt_texture_3d::createTextureInContext()> All by your opengl implementation supported extensions:" << endl;
	cout << endl << glGetString(GL_EXTENSIONS) << endl << endl;
	svt_fatalError("svt_texture_3d:>createTextureInContext()> GL_EXT_texture3D opengl extension not supported by your hardware.");
    }

    (*glTexImage3DPtr)((ENUM_GLENUM)GL_TEXTURE_3D, 0, m_iInternType, m_iSizeX, m_iSizeY, m_iSizeZ, 0, (ENUM_GLENUM)m_iDataType, (ENUM_GLENUM)GL_UNSIGNED_BYTE, (const void*)m_pcData);

    svt_checkGLErrors("svt_texture_3d::CreateTextureInContext()> End>");
    m_pbTexValid[iWin-1] = true;

#else

    glTexImage3D((ENUM_GLENUM)GL_TEXTURE_3D, 0, m_iInternType, m_iSizeX, m_iSizeY, m_iSizeZ, 0, (ENUM_GLENUM)m_iDataType, (ENUM_GLENUM)GL_UNSIGNED_BYTE, (const void*)m_pcData);

    svt_checkGLErrors("svt_texture_3d::CreateTextureInContext()> End>");
    m_pbTexValid[iWin-1] = true;

#endif
}

/**
 * for internal use only: delete context-specific texture handle information
 */
void svt_texture_3d::deleteTextureHandles()
{
    int i;
    for(i=0; i<svt_getWindowsNum(); i++)
        m_pbTexValid[i] = false;
};
