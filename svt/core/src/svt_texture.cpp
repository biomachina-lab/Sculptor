/***************************************************************************
                          svt_texture.cpp  -  description
                             -------------------
    begin                : Mon Jul 03 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_texture.h>
#include <svt_file_utils.h>
#include <svt_pic_reader.h>
#include <svt_window.h>
#include <svt_init.h>
#include <svt_opengl.h>
#include <svt_iostream.h>
#include <svt_cmath.h>
// clib includes
#include <string.h>

/**
 * Constructor
 * inits variables, you need to call LoadFile later
 */
svt_texture::svt_texture(const char *pFilename) :  svt_contextData()
{
    // init vars
    m_pData = NULL;
    m_pTexHandle = NULL;
    m_pTexValid = NULL;
    m_iInternType = GL_RGB;
    m_iDataType = GL_RGB;
    m_pReader = NULL;
    m_bAutoCoords=false;

    // init filter
    setMinFilter(SVT_FILTER_LINEAR);
    setMagFilter(SVT_FILTER_LINEAR);

    // create arrays
    m_pTexHandle = new int[svt_getWindowsNum()];
    m_pTexValid  = new bool[svt_getWindowsNum()];

    if (m_pTexHandle == NULL || m_pTexValid == NULL)
        svt_fatalError("svt_texture: memory management error!");

    // reset all texture handles
    int i;
    for(i=0; i<svt_getWindowsNum(); i++)
    {
        m_pTexHandle[i] = -1;
        m_pTexValid[i] = false;
    }

    // Load Texture image?
    if (pFilename != NULL)
	LoadFile(pFilename);
}
svt_texture::~svt_texture()
{
    if (m_pData)
        delete[] m_pData;
    if (m_pReader)
        delete m_pReader;
}

void svt_texture::setDataRGBA(const char* pData, int iSizeX, int iSizeY)
{
    // free old texture data
    if (m_pData != NULL)
        delete m_pData;

    // new size
    m_iSizeX = (unsigned long int)iSizeX;
    m_iSizeY = (unsigned long int)iSizeY;

    // reset all texture handles
    for(int i=0; i<svt_getWindowsNum(); i++)
        m_pTexValid[i] = false;

    // Set the data
    m_pData = (unsigned char*)pData;

    // create the texture
    m_iDataType = m_iInternType = GL_RGBA;

    // set filters
    setMinFilter(SVT_FILTER_LINEAR);
    setMagFilter(SVT_FILTER_LINEAR);
}

/**
 * set the minification filter method
 * \param iMinFilter minification filter (e.g. SVT_FILTER_LINEAR)
 */
void svt_texture::setMinFilter(int iMinFilter)
{
    m_iMinFilter = iMinFilter;
};
/**
 * get the minification filter method
 * \return minification filter (e.g. SVT_FILTER_LINEAR)
 */
int svt_texture::getMinFilter() const
{
    return m_iMinFilter;
};
/**
 * set the magnification filter method
 * \param iMagFilter magnification filter (e.g. SVT_FILTER_LINEAR)
 */
void svt_texture::setMagFilter(int iMagFilter)
{
    m_iMagFilter = iMagFilter;
};
/**
 * get the magnification filter method
 * \return magnification filter (e.g. SVT_FILTER_LINEAR)
 */
int svt_texture::getMagFilter() const
{
    return m_iMagFilter;
};

bool svt_texture::LoadFile(const char* pFilename)
{
    // free old texture data
    if (m_pData != NULL)
        delete m_pData;

    // reset all texture handles
    for(int i=0; i<svt_getWindowsNum(); i++)
        m_pTexValid[i] = false;

    // now read the picture
    m_pReader = new svt_pic_reader((char *)pFilename);
    m_pData = m_pReader->getData();

    // error?
    if (m_pData == NULL)
	return false;

    // set the size
    m_iSizeX = m_pReader->getSizeX();
    m_iSizeY = m_pReader->getSizeY();

    // create the texture
    m_iInternType = m_iDataType = GL_RGB;

    // set filters
    setMinFilter(SVT_FILTER_LINEAR);
    setMagFilter(SVT_FILTER_LINEAR);

    return true;
}

/**
 * BindTexture - sets the objects texture to current
 */
void svt_texture::BindTexture()
{
    // errors?
    GLenum iEnm = glGetError();
    while(iEnm != GL_NO_ERROR)
    {
        cout << "svt_texture: error before binding texture!" << endl;
        cout << (char*) gluErrorString(iEnm) << endl;

        iEnm = glGetError();
    }


    // if we dont have any texture data, then quit
    if (m_pData == NULL)
	return;

    svt_win_handle iWin = svt_getCurrentWindow();

    if (!m_pTexValid[iWin-1])
	create(iWin);
    else
	glBindTexture(GL_TEXTURE_2D, m_pTexHandle[iWin-1]);

    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, getMagFilter());
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, getMinFilter());

    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    // errors?
    iEnm = glGetError();
    while(iEnm != GL_NO_ERROR)
    {
        cout << "svt_texture: error during binding texture!" << endl;
        cout << (char*) gluErrorString(iEnm) << endl;

        iEnm = glGetError();
    }
}

/**
 * for internal use only: create the texture in a opengl context
 * \param iWin context number (same as window number)
 */
void svt_texture::create(svt_win_handle iWin)
{
    // errors?
    GLenum iEnm = glGetError();
    while(iEnm != GL_NO_ERROR)
    {
        cout << "svt_texture: create()!" << endl;
        cout << (char*) gluErrorString(iEnm) << endl;

        iEnm = glGetError();
    }

    svt_contextData::create(iWin);

    if (m_pTexHandle[iWin-1] != -1)
	glDeleteTextures(1,(GLuint*)&m_pTexHandle[iWin-1]);

    glGenTextures(1, (GLuint*)&m_pTexHandle[iWin-1]);

    // errors?
    iEnm = glGetError();
    while(iEnm != GL_NO_ERROR)
    {
        cout << "svt_texture: error during glGenTextures()!" << endl;
        cout << (char*) gluErrorString(iEnm) << endl;

        iEnm = glGetError();
    }

    glBindTexture(GL_TEXTURE_2D, m_pTexHandle[iWin-1]);

    // set filtering styles
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, getMagFilter());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, getMinFilter());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    // clear error state
    while(glGetError() != GL_NO_ERROR)
    {

    };

    // create the texture
    glTexImage2D((ENUM_GLENUM)GL_TEXTURE_2D, 0, m_iInternType, m_iSizeX, m_iSizeY, 0, (ENUM_GLENUM)m_iDataType, (ENUM_GLENUM)GL_UNSIGNED_BYTE, m_pData);

    // errors?
    iEnm = glGetError();
    while(iEnm != GL_NO_ERROR)
    {
        cout << "svt_texture: error during texture creation!" << endl;
        cout << (char*) gluErrorString(iEnm) << endl;

        iEnm = glGetError();
    }

    m_pTexValid[iWin-1] = true;
}

/**
 * for internal use only: destroy the texture in a opengl context
 * \param iWin context number (same as window number)
 */
void svt_texture::destroy(int iWin)
{
    if (m_pTexHandle[iWin-1] != -1)
        glDeleteTextures(1,(GLuint*)&m_pTexHandle[iWin-1]);

    m_pTexHandle[iWin-1] = -1;

    svt_contextData::destroy(iWin);
}

void svt_texture::applyGL()
{
    BindTexture();

    // enable auto texture coordinate generation if requested
    if (m_bAutoCoords)
    {
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

        glTexGenfv(GL_S, GL_OBJECT_PLANE, m_oPlaneS.c_data());
        glTexGenfv(GL_T, GL_OBJECT_PLANE, m_oPlaneT.c_data());

        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
    }
    else
    {
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    }
}


void svt_texture::autoCoordGen(const svt_vector4<Real32> oPlaneS, const svt_vector4<Real32> oPlaneT)
{
    m_bAutoCoords=true;
    m_oPlaneS=oPlaneS;
    m_oPlaneT=oPlaneT;
}

void svt_texture::autoCoordGen(Real32 xMin, Real32 xMax, Real32 yMin, Real32 yMax)
{
    m_bAutoCoords=true;
    m_oPlaneT.set(1.0/(xMax-xMin), 0, 0,-xMin/(xMax-xMin) );
    m_oPlaneS.set(0, -1.0/(yMax-yMin), 0, yMax/(yMax-yMin) );
}
