/***************************************************************************
                          svt_opengl.h  -  description
                             -------------------
    begin                : 09.10.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_OPENGL_H
#define SVT_OPENGL_H

#include <svt_system.h>

#ifndef GL_DRAW_FRAMEBUFFER_EXT
#define GL_DRAW_FRAMEBUFFER_EXT 0x8CA9
#endif

#ifndef GL_FOG_COORD
#define GL_FOG_COORD 0x8451
#endif

#ifndef GL_FOG_COORD_SRC
#define GL_FOG_COORD_SRC 0x8450
#endif

#ifdef WIN32
#ifndef GLintptr
typedef ptrdiff_t GLintptr;
#endif
#ifndef GLsizeiptr
typedef ptrdiff_t GLsizeiptr;
#endif
#endif

#ifndef GL_FRAGMENT_DEPTH
#define GL_FRAGMENT_DEPTH 0x8452
#endif

#ifndef GL_TEXTURE_RECTANGLE_ARB
#define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#ifndef GL_COLOR_ATTACHMENT0_EXT
#define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#endif
#ifndef GL_COLOR_ATTACHMENT1_EXT
#define GL_COLOR_ATTACHMENT1_EXT 0x8CE1
#endif
#ifndef GL_COLOR_ATTACHMENT2_EXT
#define GL_COLOR_ATTACHMENT2_EXT 0x8CE2
#endif

#ifndef GL_FRAMEBUFFER_EXT
#define GL_FRAMEBUFFER_EXT 0x8D40
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT 0x8CD6
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT 0x8CD7
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT 0x8CD9
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT 0x8CDA
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT 0x8CDB
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT 0x8CDC
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#endif

#ifndef GL_FRAMEBUFFER_UNSUPPORTED_EXT
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT 0x8CDD
#endif

#ifndef GL_DEPTH_ATTACHMENT_EXT
#define GL_DEPTH_ATTACHMENT_EXT 0x8D00
#endif

#ifndef GL_STENCIL_ATTACHMENT_EXT
#define GL_STENCIL_ATTACHMENT_EXT 0x8D20
#endif

#ifndef GL_DEPTH_TEXTURE_MODE
#define GL_DEPTH_TEXTURE_MODE 0x884B
#endif

#ifndef GL_DYNAMIC_DRAW_ARB
#define GL_DYNAMIC_DRAW_ARB 0x88E8
#endif

// OpenGL 3.0 defines
#ifndef GL_VERSION_3_0
#define GL_CLAMP_VERTEX_COLOR                              0x891A
#define GL_CLAMP_FRAGMENT_COLOR                            0x891B
#define GL_CLAMP_READ_COLOR                                0x891C
#endif

// OpenGL includes
#ifdef MACOSX
#include <gl.h>
#include <glu.h>
#include <glext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

// Cg includes
#ifdef MACOSX
  #include <cg.h>
  #include <cgGL.h>
#else
  #include <Cg/cg.h>
  #include <Cg/cgGL.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

// OpenGL typedefs
typedef void (APIENTRY * SVT_PFNGLDRAWARRAYSEXTPROC)        (GLenum, GLint, GLsizei);
typedef void (APIENTRY * SVT_PFNGLDRAWELEMENTSEXTPROC)      (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
#ifndef WIN32
typedef void (APIENTRY * SVT_PFNGLDRAWRANGEELEMENTSEXTPROC) (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);
typedef void (APIENTRY * SVT_PFNGLMULTIDRAWELEMENTSEXTPROC) (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);
typedef void (APIENTRY * SVT_PFNGLMULTIDRAWARRAYSEXTPROC)   (GLenum, GLint *, GLsizei *, GLsizei);
#endif

enum { SVT_RENDERER_UNKNOWN,
       SVT_RENDERER_MESA,
       SVT_RENDERER_NVIDIA,
       SVT_RENDERER_ATI,
       SVT_RENDERER_INTEL};

/**
 * This function will find the pointer to a function of an opengl extension.
 * Attention: This function does not work for the Qt backend, Qt 3.3.x has no function GetProcAddress.
 * \param pFuncName pointer to the name of the function to look for
 */
DLLEXPORT void* svt_lookupFunction(const char *pFuncName);

/**
 * check if an extension is supported by this system. Attention: This is context specific information. I.e. you must call this function for every context!!!
 * \param pExtension pointer to array of char with the name of the extension
 * \return true if the extension is supported
 */
DLLEXPORT bool svt_isExtensionSupported(const char *pExtension);

/**
 * check the current opengl errors
 * \param pText name of function/module that calls this function.
 * \return bool if false there were no errors, if true there were.
 */
DLLEXPORT bool svt_checkGLErrors(const char* pText);

/**
 * Checks the most important extensions and writes about the result a short status message to stdout
 */
DLLEXPORT void svt_checkEXT();

/**
 * Are vertex buffer objects supported?
 */
DLLEXPORT bool svt_checkEXT_VBO();
/**
 * Enable/Disable VBO
 * \param bDisable if true VBO support is disabled...
 */
DLLEXPORT void svt_disableEXT_VBO(bool bDisable);

/**
 * Are framebuffer objects supported?
 */
DLLEXPORT bool svt_checkEXT_FBO();
/**
 * Enable/Disable FBO
 * \param bDisable if true FBO support is disabled...
 */
DLLEXPORT void svt_disableEXT_FBO(bool bDisable);

/**
 * Are 3D textures supported?
 */
DLLEXPORT bool svt_checkEXT_3DT();

/**
 * Check for the OpenGL renderer. Return:
 * - SVT_RENDERER_UNKNOWN
 * - SVT_RENDERER_MESA
 * - SVT_RENDERER_NVIDIA
 * - SVT_RENDERER_ATI
 * - SVT_RENDERER_INTEL
 */
DLLEXPORT int svt_getRenderer();

/**
 * get if ambient occlusion can be used. Solves problems on some hardware (old Intel chipsets in particular)
 * \return bool indicates if ambient occlusion allowed or not
 */
DLLEXPORT bool svt_getAllowAmbientOcclusion();
/**
 * set if ambient occlusion can be used. Solves problems on some hardware (old Intel chipsets in particular)
 * \param bAO indicates if ambient occlusion allowed or not
 */
DLLEXPORT void svt_setAllowAmbientOcclusion(bool bAO);

/**
 * get if the use of vertex and fragment shader programs is ok
 * \return bool indicates if use of shader programs is allowed or not
 */
DLLEXPORT bool svt_getAllowShaderPrograms();
/**
 * set if the use of vertex and fragment shader programs is ok
 * \param bShader true to allow vertex and fragment shader programs, false to suppress their usage
 */
DLLEXPORT void svt_setAllowShaderPrograms(bool bShader);

#endif
