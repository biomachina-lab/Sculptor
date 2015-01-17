/*-*-c++-*-*****************************************************************
                          svt_shader_glsl
                          ---------------
    begin                : Oct 27, 2009
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SHADER_GLSL_H
#define SVT_SHADER_GLSL_H

#include <svt_opengl.h>

#ifndef GLchar
#define GLchar char
#endif

typedef GLuint (APIENTRY * SVT_PFNGLCREATEPROGRAM)     ();
typedef void   (APIENTRY * SVT_PFNGLDELETEPROGRAM)     (GLuint iHandle);
typedef void   (APIENTRY * SVT_PFNGLGETPROGRAM)        (GLuint iHandle, GLenum iName, GLint * pParam);
typedef void   (APIENTRY * SVT_PFNGLGETPROGRAMINFOLOG) (GLuint iHandle, GLsizei iMaxLength, GLsizei * iLength, GLchar * pMessage);

/**
 * OpenGL Shading Language. This class encapsulates the handling of GLSL so that within SVT, we can
 * use it in a unified way and hide the implementation/version specific details here.
 * This class should encapsulate an OpenGL Program Object.
 * \brief A class encapsulating a GLSL program object.
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_glsl
{

protected:

    static SVT_PFNGLCREATEPROGRAM glCreateProgramSVT;
    static SVT_PFNGLDELETEPROGRAM glDeleteProgramSVT;
    static SVT_PFNGLGETPROGRAM glGetProgramSVT;
    static SVT_PFNGLGETPROGRAMINFOLOG glGetProgramInfoLogSVT;

    GLuint iHandle;

public:

    /**
     * Constructor
      */
    svt_shader_glsl();

    /**
     * Destructor
     */
    virtual ~svt_shader_glsl();

    /**
     * print the log file associated with this shader
     */
    void printInfoLog();
};

#endif
