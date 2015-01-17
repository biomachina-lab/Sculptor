/***************************************************************************
                          svt_texture.h  -  description
                             -------------------
    begin                : Mon Jul 03 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TEXTURE_H
#define SVT_TEXTURE_H

#define SVT_FILTER_LINEAR  GL_LINEAR
#define SVT_FILTER_NEAREST GL_NEAREST

#include <svt_contextData.h>
#include <svt_types.h>
#include <svt_vector4.h> 

class svt_pic_reader;

/** Texture class
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_texture : public svt_contextData
{
private:
    // type of image data stored (usually GL_RGB/GL_RGBA)
    int m_iInternType;
    int m_iDataType;
    // x res
    unsigned long int m_iSizeX;
    // y res
    unsigned long int m_iSizeY;
    // points to the raw image data
    unsigned char *m_pData;
    // the texture handle (for each opengl context one!)
    int* m_pTexHandle;
    // is the handle valid, or must we recreate the texture?
    bool* m_pTexValid;

    int m_iMinFilter;
    int m_iMagFilter;

    bool m_bAutoCoords;

    svt_vector4<Real32> m_oPlaneS;
    svt_vector4<Real32> m_oPlaneT;

    svt_pic_reader* m_pReader;

public:
    /**
     * Constructor
     * svt_texture creates a instance with no image loaded, you can call LoadFile later
     * to load one - or use svt_texture(filename) instead
     */
    svt_texture(const char *pFilename =NULL);
    virtual ~svt_texture();

    /**
     * BindTexture binds the image to the current texture so we can render with it
     */
    void BindTexture();

    /**
     * LoadFile loads filename over the current file in the instance and creates a texture
     */
    bool LoadFile(const char *filename);
    /**
     * setDataRGBA creates a texture out of the data (in RGBA bytes) and the sizes
     */
    void setDataRGBA(const char* data, int iSizeX, int iSizeY);

    /**
     * set the minification filter method
     * \param iMinFilter minification filter (e.g. SVT_FILTER_LINEAR)
     */
    void setMinFilter(int iMinFilter);
    /**
     * get the minification filter method
     * \return minification filter (e.g. SVT_FILTER_LINEAR)
     */
    int getMinFilter() const;
    /**
     * set the magnification filter method
     * \param iMagFilter magnification filter (e.g. SVT_FILTER_LINEAR)
     */
    void setMagFilter(int iMagFilter);
    /**
     * get the magnification filter method
     * \return magnification filter (e.g. SVT_FILTER_LINEAR)
     */
    int getMagFilter() const;

    /**
     * for internal use only: create the texture in a opengl context
     * \param iWin context number (same as window number)
     */
    virtual void create(int iWin);

    /**
     * for internal use only: destroy the texture in a opengl context
     * \param iWin context number (same as window number)
     */
    virtual void destroy(int iWin);

    void applyGL();

    /**
     * query if texture coordinates are generated automatically
     */
    bool autoCoordGen() const {return m_bAutoCoords;}

    /**
     * switch on/off automatic texture coordinate generation.
     * NOTE: if argument is true, the previously specified settings
     *       are used. Therefore, this method should only be used to toggle
     *       automatic gerneration.
     */
    void autoCoordGen(bool b) {m_bAutoCoords=b;}

    /**
     * switch on automatic coordinate refering to the given plane parameters
     * (See Reference Manual for "glTexGen" for the plane's meaning)
     */
    void autoCoordGen(const svt_vector4<Real32> oPlaneS, const svt_vector4<Real32> oPlaneT);

    /**
     * switch on automatic coordinate generation for height fields.
     * NOTE: y-axis will be mirrored!
     */
    void autoCoordGen(Real32 xMin, Real32 xMax, Real32 yMin, Real32 yMax);


    /**
     * get x-size of the texture
     */
    unsigned long sizeX() const {return m_iSizeX;}

    /**
     * get y-size of the texture
     */
    unsigned long sizeY() const {return m_iSizeY;}

    /**
     * get raw data pointer
     */
    const unsigned char* data() const {return m_pData;}
};

#endif
