/***************************************************************************
                          svt_texture_3d
			  --------------
    begin                : 09.10.01
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TEXTURE_3D_H
#define SVT_TEXTURE_3D_H

#define MAX_EXT_LEN 10 // max length of filename extensions

#define SVT_FILTER_LINEAR  GL_LINEAR
#define SVT_FILTER_NEAREST GL_NEAREST

/**
 * 3D texture class.
 * Attention: This class uses the 3d_texture extension. This is not available on all opengl implementations/hardware.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_texture_3d
{

private:

    unsigned char *m_pcData;    // points to the raw image data
    GLuint* m_piTexHandle;      // the texture handle (for each opengl context one!)
    bool* m_pbTexValid;         // is the handle valid, or must we recreate the texture?

    unsigned long int m_iSizeX; // x resolution
    unsigned long int m_iSizeY; // y resolution
    unsigned long int m_iSizeZ; // y resolution

    int m_iInternType;          // what should the internal data format of the texture be (typically GL_RGB, GL_RGBA, but many more possible and useful, see reference manual)
    int m_iDataType;            // how is the data stored in your data (typically GL_RGB or GL_RGBA)

    int m_iMinFilter;
    int m_iMagFilter;

public:

    /**
     * Constructor
     */
    svt_texture_3d();
    virtual ~svt_texture_3d();

    /**
     * bindTexture() binds the image to the current texture so the next OpenGL commands in this context are going to use it.
     */
    void bindTexture();

    /**
     * apply to GL (calls bind texture)
     */
    void applyGL();

    /**
     * setDataRGBA creates a texture out of the data (in RGBA bytes) and the sizes \
     */
    void setDataRGBA(const unsigned char* pcData, int iSizeX, int iSizeY, int iSizeZ);

    /**
     * set the minification filter method
     */
    void setMinFilter(int iMinFilter);
    /**
     * get the minification filter method
     */
    int getMinFilter() const;
    /**
     * set the magnification filter method
     */
    void setMagFilter(int iMagFilter);
    /**
     * get the magnification filter method
     */
    int getMagFilter() const;

    /**
     * get opengl ID of texture object
     */
    GLuint getTextureID(int iWin) const;

    /**
     * get opengl ID of texture object
     */
    GLuint getTextureID() const;
 
protected:

    /**
     * for internal use only: create the texture in a opengl context
     */
    void createTextureInContext(int win);

    /**
     * for internal use only: delete context-specific texture handle information
     */
    void deleteTextureHandles();
};

#endif
