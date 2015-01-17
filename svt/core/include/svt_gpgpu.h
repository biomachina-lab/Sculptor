/***************************************************************************
                          svt_gpgpu
                          ---------------------
    begin                : 07/21/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_GPGPU_H
#define SVT_GPGPU_H

#include <svt_basics.h>
#include <svt_opengl.h>
#include <svt_types.h>
#include <svt_shader_cg_gpgpu.h>
#include <svt_stlVector.h>

typedef void   (APIENTRY * SVT_PFNGLGENFRAMEBUFFERS)       (GLsizei n, GLuint *framebuffers);
typedef void   (APIENTRY * SVT_PFNGLBINDFRAMEBUFFER)       (GLenum target, GLuint framebuffer);
typedef void   (APIENTRY * SVT_PFNGLDELETEFRAMEBUFFERS)    (GLsizei n, const GLuint *framebuffers);
typedef void   (APIENTRY * SVT_PFNGLFRAMEBUFFERTEXTURE2D)  (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum (APIENTRY * SVT_PFNGLCHECKFRAMEBUFFERSTATUS)(GLenum target);

class DLLEXPORT svt_gpgpu
{

 protected:

    static SVT_PFNGLGENFRAMEBUFFERS        glGenFramebuffersEXT;
    static SVT_PFNGLBINDFRAMEBUFFER        glBindFramebufferEXT;
    static SVT_PFNGLDELETEFRAMEBUFFERS     glDeleteFramebuffersEXT;
    static SVT_PFNGLFRAMEBUFFERTEXTURE2D   glFramebufferTexture2DEXT;
    static SVT_PFNGLCHECKFRAMEBUFFERSTATUS glCheckFramebufferStatusEXT;

    // are framebuffer objects supported?
    bool m_bFBOSupported;

    // internalFormat for textures
    GLint m_iIntexInternalFormat;
    GLint m_iOutexInternalFormat;
    GLint m_iIntexTarget;
    GLint m_iOutexTarget;
    GLint m_iIntexFormat;
    GLint m_iOutexFormat;

    // svt_shader_cg object
    svt_shader_cg_gpgpu * m_pShader;

    // Framebuffer ID
    GLuint m_iFB;

    // max size for textures (reported by the GL)
    GLint m_iMaxTexSize;

    // size for input textures
    GLint m_iTexSize;

    // GL texture names
    vector< GLuint > aTexNames;

    // putput texture id
    GLuint m_iOutputTexID;


 public:

    /**
     * Constructor 
     */
    svt_gpgpu();
    /**
     * Destructor 
     */
    virtual ~svt_gpgpu();

    /**
     * 
     */
    bool checkFramebufferStatus();

    /**
     * 
     */
    void compute(Real32 * pResult);

    /**
     * 
     */
    void setInputData(Real32* pInputData, unsigned int iSize);

    /**
     *
     */
    void addShader(bool bFile, char * pVertex, char * pFragment, CGprofile pVertProfile = CG_PROFILE_UNKNOWN, CGprofile pFragProfile = CG_PROFILE_UNKNOWN);

    /**
     * check if texture values (used as input and output) behave like full floats or are clamped to the 0..1 range
     * \return bool false if floats are clamped to the 0..1 range, true if not
     */
    bool checkUnclampedFloats();

 protected:

    /**
     * 
     */
    void initFBO();

    /**
     *
     */
    void releaseFBO();

};

#endif
