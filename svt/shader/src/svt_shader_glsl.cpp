/***************************************************************************
                          svt_shader_glsl
                          ---------------
    begin                : Oct 27, 2009
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


#include <svt_shader_glsl.h>
#include <svt_iostream.h>

SVT_PFNGLCREATEPROGRAM     svt_shader_glsl::glCreateProgramSVT     = NULL;
SVT_PFNGLDELETEPROGRAM     svt_shader_glsl::glDeleteProgramSVT     = NULL;
SVT_PFNGLGETPROGRAM        svt_shader_glsl::glGetProgramSVT        = NULL;
SVT_PFNGLGETPROGRAMINFOLOG svt_shader_glsl::glGetProgramInfoLogSVT = NULL;

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

/**
 * Constructor
 */
svt_shader_glsl::svt_shader_glsl()
{
    if (glCreateProgramSVT == NULL)
    {
	if (svt_isExtensionSupported("GL_ARB_shader_objects"))
	{
		glCreateProgramSVT     = (SVT_PFNGLCREATEPROGRAM)     svt_lookupFunction("glCreateProgramObjectARB");
		glDeleteProgramSVT     = (SVT_PFNGLDELETEPROGRAM)     svt_lookupFunction("glDeleteProgramARB");
		glGetProgramSVT        = (SVT_PFNGLGETPROGRAM)        svt_lookupFunction("glGetProgramARB");
		glGetProgramInfoLogSVT = (SVT_PFNGLGETPROGRAMINFOLOG) svt_lookupFunction("glGetProgramInfoLogARB");
	}
	else
	{
	    svtout << "Use of GLSL shaders not possbile!"<< endl;
	}
    }

    if (glCreateProgramSVT != NULL)
	iHandle = glCreateProgramSVT();
    else
	iHandle = 0;
}


/**
 * Destructor
 */
svt_shader_glsl::~svt_shader_glsl()
{
    glDeleteProgramSVT(iHandle);
}

/**
 * print the log file associated with this shader
 */
void svt_shader_glsl::printInfoLog()
{
    GLint iLength;

    glGetProgramSVT(iHandle, GL_INFO_LOG_LENGTH, &iLength);

    char pMessage[iLength+1];

    glGetProgramInfoLogSVT(iHandle, iLength+1, &iLength, pMessage);

    //svtout << pMessage) << endl;
    printf("%s\n", pMessage);
}
