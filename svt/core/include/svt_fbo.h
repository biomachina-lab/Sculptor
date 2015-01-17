/*-*-c++-*-*****************************************************************
                          svt_fbo.h
                          ---------
    begin                : 24/02/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FBO_H
#define SVT_FBO_H


#include <svt_types.h>


typedef void      (APIENTRY * SVT_PFNGLCLAMPCOLORARBPROC)                          (GLenum target, GLenum clamp);
typedef void      (APIENTRY * SVT_PFNGLDRAWBUFFERSARBPROC)                         (GLsizei n, const GLenum *bufs);
typedef GLboolean (APIENTRY * SVT_PFNGLISFRAMEBUFFEREXTPROC)                       (GLuint framebuffer);
typedef void      (APIENTRY * SVT_PFNGLGENFRAMEBUFFERSEXTPROC)                     (GLsizei n, GLuint *framebuffers);
typedef void      (APIENTRY * SVT_PFNGLBINDFRAMEBUFFEREXTPROC)                     (GLenum target, GLuint framebuffer);
typedef void      (APIENTRY * SVT_PFNGLGENRENDERBUFFERSEXTPROC)                    (GLsizei n, GLuint *renderbuffers);
typedef void      (APIENTRY * SVT_PFNGLBINDRENDERBUFFEREXTPROC)                    (GLenum target, GLuint renderbuffer);
typedef void      (APIENTRY * SVT_PFNGLDELETEFRAMEBUFFERSEXTPROC)                  (GLsizei n, const GLuint *framebuffers);
typedef void      (APIENTRY * SVT_PFNGLDELETERENDERBUFFERSEXTPROC)                 (GLsizei n, const GLuint *renderbuffers);
typedef void      (APIENTRY * SVT_PFNGLRENDERBUFFERSTORAGEEXTPROC)                 (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void      (APIENTRY * SVT_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)                (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum    (APIENTRY * SVT_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)              (GLenum target);
typedef void      (APIENTRY * SVT_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)             (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void      (APIENTRY * SVT_PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) (GLenum target, GLenum attachment, GLenum pname, GLint *params);


/**
 * This class has the basic framebuffer functionality. It handles only 2d texture attachments, which
 * should be sufficient for most cases. If more complex operations are needed, like 3d textures,
 * derive a class from this one and rewrite and/or add methods. Also, this class is not supposed to
 * set up the texture objects. You have to do that in your code, and then pass the OpenGL names of
 * them to this class using the attach*Texture(..) methods.
 * \brief A class that encapsulates a framebuffer object (FBO)
 * @author Manuel Wahle
 */
class DLLEXPORT svt_fbo
{

protected:

    static SVT_PFNGLCLAMPCOLORARBPROC                          glClampColorARB;
    static SVT_PFNGLDRAWBUFFERSARBPROC                         glDrawBuffersARB;
    static SVT_PFNGLISFRAMEBUFFEREXTPROC                       glIsFramebufferEXT;
    static SVT_PFNGLGENFRAMEBUFFERSEXTPROC                     glGenFramebuffersEXT;
    static SVT_PFNGLBINDFRAMEBUFFEREXTPROC                     glBindFramebufferEXT;
    static SVT_PFNGLGENRENDERBUFFERSEXTPROC                    glGenRenderbuffersEXT;
    static SVT_PFNGLBINDRENDERBUFFEREXTPROC                    glBindRenderbufferEXT;
    static SVT_PFNGLDELETEFRAMEBUFFERSEXTPROC                  glDeleteFramebuffersEXT;
    static SVT_PFNGLDELETERENDERBUFFERSEXTPROC                 glDeleteRenderbuffersEXT;
    static SVT_PFNGLRENDERBUFFERSTORAGEEXTPROC                 glRenderbufferStorageEXT;
    static SVT_PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                glFramebufferTexture2DEXT;
    static SVT_PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              glCheckFramebufferStatusEXT;
    static SVT_PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             glFramebufferRenderbufferEXT;
    static SVT_PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;

    static bool m_bFBOSupported;
    static bool m_bMRTSupported;
    GLuint m_iBuffer;

    GLenum * m_aMRT;
    GLsizei m_iMRTSize;

    GLfloat m_fR, m_fG, m_fB, m_fA;
    GLfloat m_fD;
    GLfloat m_fS;

    GLuint m_iNumColorAttachments;
    bool   m_bColorAttachment;
    bool   m_bDepthAttachment;
    bool   m_bStencilAttachment;

public:

    /**
     * Constructor
     */
    svt_fbo();

    /**
     * Destructor
     */
    virtual ~svt_fbo();

    /**
     * \name Methods for setting up the framebuffer object
     * These methods deal with setting up and binding the framebuffer
     */
    //@{
    /**
     * Generates an actual OpenGL framebuffer object. Do nothing if already initialized
     */
    void genBuffer();

    /**
     * Binds the framebuffer object, so that GL drawing is done into this framebuffer object. See
     * also bindDisplayFramebuffer()
     */
    virtual void bindBuffer();

    /**
     * Binds the framebuffer that is provided by the window system. So calling this method
     * redirects rendering back to the screen, and this framebuffer will be left alone
     */
    void bindDisplayFramebuffer();

    /**
     * Checks if the OpenGL framebuffer object exists. See also getIsComplete()
     */
    bool getIsBuffer();

    /**
     * Checks if the framebuffer object is complete. If the framebuffer is not complete, prints out
     * an error message indicating what is wrong
     */
    bool getIsComplete();

    /**
     * Deletes the actual OpenGL framebuffer object
     */
    void deleteBuffer();

    /**
     * Sets the render targets. Does not need to be called if there is only one color attachment,
     * which is OpenGL default. Omitting the second parameter will result in binding the first iSize
     * color attachments. If you want specific color attachment(s) to be bound, specify their
     * number in the first parameter and an array with the corresponding GLenums in the second
     * parameter.
     *
     * Examples:
     * - setDrawBuffers(3);\n binds the first three color attachments
     * - int aMRT[] = { (int)GL_COLOR_ATTACHMENT0_EXT, (int)GL_COLOR_ATTACHMENT2_EXT };
     * setDrawBuffers(2, aMRT);\n binds the first and the third color attachment
     *
     * \param iSize an integer with the number of render targets
     * \param aMRT an array with the desired color attachments. the elements must be the GL_COLOR_ATTACHMENTi_EXT
     */
    virtual void setDrawBuffers(int iSize, int * aMRT = NULL);

    //@}


    /**
     * \name Methods for attachment manipulation
     * These methods deal with attaching, releasing, and clearing color, depth, and stencil
     * texture attachments
     */
    //@{
    /**
     * Sets the value used to clear the depth attachment
     * \param fD Real32 for depth clear value
     */
    void setClearDepth(Real32 fD);

    /**
     * Sets the value used to clear the stencil attachment
     * \param fS Real32 for stencil clear value
     */
    void setClearStencil(Real32 fS);

    /**
     * Sets the values used to clear the color attachment
     * \param fR Real32 for red clear value
     * \param fG Real32 for green clear value
     * \param fB Real32 for blue clear value
     * \param fA Real32 for alpha clear value
     */
    virtual void setClearColor(Real32 fR, Real32 fG, Real32 fB, Real32 fA);

    /**
     * Clear attachments. only bound attachments are cleared (call setDrawBuffers(n) to bind all n color attachments).
     */
    virtual void clearAttachments();

    /**
     * Attaches a color texture to the framebuffer. The texture must 2d and already have been
     * generated
     * \param iName the name of the OpenGL texture object that should be used as color
     * texture
     */
    virtual void attachColorTexture(GLuint iName);

    /**
     * Releases all color attachments from the framebuffer
     */
    virtual void releaseColorAttachments();

    /**
     * Attaches a depth texture to the framebuffer, so that depth values will be rendered into the
     * framebuffer. The texture must 2d and already have been generated
     * \param iName the name of the OpenGL texture object that should be used as depth texture
     */
    virtual void attachDepthTexture(GLuint iName);

    /**
     * Releases the depth attachment from the framebuffer
     */
    virtual void releaseDepthAttachment();

    /**
     * Attaches a depth texture to the framebuffer, so that depth values will be rendered into the
     * framebuffer. The texture must 2d and already have been generated
     * \param iName the name of the OpenGL texture object that should be used as stencil texture
     */
    virtual void attachStencilTexture(GLuint iName);

    /**
     * Releases the stencil attachment from the framebuffer
     */
    virtual void releaseStencilAttachment();
    //@}


    /**
     * Set the color texture clamping behavior. For float textures, not clamping at all is OpenGL default behavior
     */
    void setColorClamping(unsigned int iClampMode);

    /**
     * Check if frame buffer objects are supported at all
     */
    bool getFBOSupported();

    /**
     * Check if multiple render targets are supported
     */
    bool getMRTSupported();
};

#endif
