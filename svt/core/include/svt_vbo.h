/***************************************************************************
                          svt_vbo.h
                          ---------
    begin                : 11/16/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VBO_H
#define SVT_VBO_H

#include <svt_types.h>

#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
#define GL_ELEMENT_ARRAY_BUFFER_ARB 0x8893

typedef void      (APIENTRY * SVT_PFNGLBINDBUFFERARBPROC)    (GLenum target, GLuint buffer);
typedef void      (APIENTRY * SVT_PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void      (APIENTRY * SVT_PFNGLGENBUFFERSARBPROC)    (GLsizei n, GLuint *buffers);
typedef void      (APIENTRY * SVT_PFNGLBUFFERDATAARBPROC)    (GLenum target, int size, const GLvoid *data, GLenum usage);
typedef void      (APIENTRY * SVT_PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef GLvoid *  (APIENTRY * SVT_PFNGLMAPBUFFERARBPROC)     (GLenum target, GLenum access);
typedef GLboolean (APIENTRY * SVT_PFNGLUNMAPBUFFERARBPROC)   (GLenum target);
typedef GLboolean (APIENTRY * SVT_PFNGLISBUFFERARBPROC)      (GLuint buffer);

/**
 * Vertex-Buffer Object (VBO)
 * \author Stefan Birmanns
 */
class DLLEXPORT svt_vbo
{
protected:

    static SVT_PFNGLGENBUFFERSARBPROC  glGenBuffersARB;
    static SVT_PFNGLBINDBUFFERARBPROC  glBindBufferARB;
    static SVT_PFNGLBUFFERDATAARBPROC  glBufferDataARB;
    static SVT_PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
    static SVT_PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
    static SVT_PFNGLMAPBUFFERARBPROC   glMapBufferARB;
    static SVT_PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
    static SVT_PFNGLISBUFFERARBPROC    glIsBufferARB;

    GLuint m_iBuffer;
    bool m_bBufferGenerated;


public:

    svt_vbo();
    virtual ~svt_vbo();

    void genBuffer( );

    void deleteBuffer( );

    bool isBuffer( );

    void bindBuffer( GLenum target );

    void bufferData( GLenum target, int size, GLvoid *data, GLenum usage );

    void bufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data );

    void releaseBuffer( GLenum target );

    void * mapBuffer(GLenum target, GLenum access);

    bool unmapBuffer(GLenum target);
};

#endif
