/***************************************************************************
                          svt_opengl
			  ----------
    begin                : 09.10.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#define GLX_GLXEXT_LEGACY

///////////////////////////////////////////////////////////////////////////////
// GetProcAddress for the different platforms
///////////////////////////////////////////////////////////////////////////////

// clib includes
#include <string.h>
#include <stdio.h>
// svt includes
#include <svt_opengl.h>
#include <svt_system.h>
#include <svt_opengl.h>
#include <svt_iostream.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
#ifndef MACOSX
#include <GL/glx.h>
#endif
#endif

#ifdef MACOSX
#import <mach-o/dyld.h>
void* NSGLGetProcAddress(const char *name)
{
    NSSymbol symbol;
    char *symbolName;
    // Prepend a '_' for the Unix C symbol mangling convention
    symbolName = (char*)malloc (strlen (name) + 2);
    strcpy(symbolName + 1, name);
    symbolName[0] = '_';
    symbol = NULL;

    if (NSIsSymbolNameDefined (symbolName))
        symbol = NSLookupAndBindSymbol (symbolName);
    free (symbolName);

    return symbol ? NSAddressOfSymbol (symbol) : NULL;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// SVT code
///////////////////////////////////////////////////////////////////////////////

int g_iEXT_3DT = -1;
int g_iEXT_CVA = -1;
int g_iEXT_VBO = -1;
int g_iEXT_FBO = -1;
int g_iOpenGLRenderer = -1;

// are vertex/fragment shader programs allowed?
bool g_bAllowShaderPrograms = true;

// is ambient occlusion allowed?
bool g_bAllowAmbientOcclusion = true;

#define OGL cout << svt_trimName("svt_opengl")

/**
 * This function will find the pointer to a function of an opengl extension.
 * Attention: This function does not work for the Qt backend, Qt 3.3.x has no function GetProcAddress.
 * \param pFuncName pointer to the name of the function to look for
 */
DLLEXPORT void* svt_lookupFunction(const char *pFuncName)
{
    void *pPointer = NULL;

#ifdef WIN32
    pPointer = (void*)wglGetProcAddress(pFuncName);
#endif

#ifdef LINUX
#ifndef MACOSX
    pPointer = (void*)glXGetProcAddressARB( (const GLubyte*)pFuncName );
#endif
#endif

#ifdef MACOSX
    pPointer = (void*)NSGLGetProcAddress( pFuncName );
#endif

    return pPointer;
};

/**
 * check if an extension is supported by this system. Attention: This is context specific information. I.e. you must call this function for every context!!!
 * \param pExtension pointer to array of char with the name of the extension
 * \return true if the extension is supported
 */
DLLEXPORT bool svt_isExtensionSupported(const char *pExtension)
{
    const GLubyte *pExtensions = NULL;
    const GLubyte *pStart;
    GLubyte *pWhere, *pTerminator;

    // Extension names should not have spaces.
    pWhere = (GLubyte *) strchr(pExtension, ' ');
    if (pWhere || *pExtension == '\0')
        return false;
    pExtensions = glGetString(GL_EXTENSIONS);

    // It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc.
    pStart = pExtensions;
    for (;;) {
        pWhere = (GLubyte *) strstr((const char *) pStart, pExtension);
        if (!pWhere)
            break;
        pTerminator = pWhere + strlen(pExtension);
        if (pWhere == pStart || *(pWhere - 1) == ' ')
            if (*pTerminator == ' ' || *pTerminator == '\0')
                return true;
        pStart = pTerminator;
    }
    return false;
}

/**
 * check the current opengl errors
 * \param pText name of function/module that calls this function.
 * \return bool if false there were no errors, if true there were.
 */
bool svt_checkGLErrors(const char* pText)
{
    GLenum iEnm = glGetError();

    if (iEnm == GL_NO_ERROR)
	return false;

    while(iEnm != GL_NO_ERROR)
    {
	cout << pText << endl;
	cout << (char*) gluErrorString(iEnm) << endl;

	iEnm = glGetError();
    }

    return true;
};

/**
 * Checks the most important extensions and writes about the result a short status message to stdout
 */
DLLEXPORT void svt_checkEXT()
{
    OGL << "OpenGL: Vendor  : " << glGetString(GL_VENDOR) << endl;                         // company responsible for this GL implementation
    OGL << "OpenGL: Renderer: " << glGetString(GL_RENDERER) << endl;                       // hardware (graphics card)
    OGL << "OpenGL: Version : " << glGetString(GL_VERSION) << endl;                        // version or release number
    //OGL << "OpenGL: GLSL    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;       // GLSL version
    OGL << "Cg    : Version : " << cgGetString(CG_VERSION) << endl;                        // Cg version

    OGL << "OpenGL: Found the following extensions:";

    if (svt_checkEXT_3DT())
	cout << " 3dt";
    if (svt_checkEXT_VBO())
	cout << " vbo";
    if (svt_checkEXT_FBO())
	cout << " fbo";

    cout << endl;
}

/**
 * Are vertex buffer objects supported?
 */
DLLEXPORT bool svt_checkEXT_VBO()
{


#ifndef MACOSX

    int i = svt_getRenderer();

    // for the time being, disable VBOs if the graphics card is an Intel or an ATI one
    if (i == SVT_RENDERER_ATI || i == SVT_RENDERER_INTEL)
    {
	g_iEXT_VBO = 0;
	return false;
    }

#endif
	

    // otherwise, actually check if VBOs are supported:

    if (g_iEXT_VBO == -1)
    {
	if (svt_isExtensionSupported("GL_ARB_vertex_buffer_object") || svt_isExtensionSupported("GL_EXT_vertex_buffer_object"))
	    g_iEXT_VBO = 1;
	else
	    g_iEXT_VBO = 0;
    }

    if (g_iEXT_VBO == 1)
	return true;
    else
        return false;
}

/**
 * Are framebuffer objects supported?
 */
DLLEXPORT bool svt_checkEXT_FBO()
{
    if (g_iEXT_FBO == -1)
    {
	if (svt_isExtensionSupported("GL_EXT_framebuffer_object") || svt_isExtensionSupported("GL_ARB_framebuffer_object"))
	    g_iEXT_FBO = 1;
	else
	    g_iEXT_FBO = 0;
    }

    if (g_iEXT_FBO == 1)
	return true;
    else
        return false;
}

/**
 * Are 3D textures supported?
 */
DLLEXPORT bool svt_checkEXT_3DT()
{
#ifndef MACOSX
    if (g_iEXT_3DT == -1)
    {
	if (svt_isExtensionSupported("GL_EXT_texture3D"))
	    g_iEXT_3DT = 1;
	else
	    g_iEXT_3DT = 0;
    }

    if (g_iEXT_3DT == 1)
	return true;
    else
        return false;
#else

    return true;

#endif
}

/**
 * Enable/Disable VBO
 * \param bDisable if true VBO support is disabled...
 */
DLLEXPORT void svt_disableEXT_VBO(bool bDisable)
{
    if (bDisable)
	g_iEXT_VBO = 0;
    else
	g_iEXT_VBO = -1;
}

/**
 * Enable/Disable FBO
 * \param bDisable if true FBO support is disabled...
 */
DLLEXPORT void svt_disableEXT_FBO(bool bDisable)
{
    if (bDisable)
	g_iEXT_FBO = 0;
    else
	g_iEXT_FBO = -1;
}

/**
 * Check for the OpenGL renderer. Return:
 * - SVT_RENDERER_UNKNOWN
 * - SVT_RENDERER_MESA
 * - SVT_RENDERER_NVIDIA
 * - SVT_RENDERER_ATI
 * - SVT_RENDERER_INTEL
 */
DLLEXPORT int svt_getRenderer()
{
    // if renderer already determined, don't check again
    if (g_iOpenGLRenderer != -1)
        return g_iOpenGLRenderer;



    // get the OpenGL renderer string
    unsigned char * C = (unsigned char*) glGetString(GL_RENDERER);

    if (C == NULL)
    {
        g_iOpenGLRenderer = SVT_RENDERER_UNKNOWN;
        return g_iOpenGLRenderer;
    }
    int l = strlen( (char*) C );

    // create a new signed char string that is liked by strstr (add space for the terminating NULL)
    char * cRenderer = new char[l+1];

    // convert the OpenGL renderer string to lower case and throw numbers away
    for (int i=0; i<l; ++i)
        if (isalpha( (int) C[i] ))
            cRenderer[i] = (char) tolower( (int) C[i] );
        else
            cRenderer[i] = ' ';
    cRenderer[l] = 0;



    // also check for the vendor string
    C = (unsigned char*) glGetString(GL_VENDOR);
    l = strlen( (char*) C );

    // create a new signed char string that is liked by strstr (add space for the terminating NULL)
    char * cVendor = new char[l+1];

    // convert the OpenGL vendor string to lower case and throw numbers away
    for (int i=0; i<l; ++i)
	if (isalpha( (int) C[i] ))
	    cVendor[i] = (char) tolower( (int) C[i] );
	else
	    cVendor[i] = ' ';
    cVendor[l] = 0;



    g_iOpenGLRenderer = SVT_RENDERER_UNKNOWN;

    if (strstr(cRenderer, "mesa") != NULL || strstr(cVendor, "mesa") != NULL)
    {
        g_iOpenGLRenderer = SVT_RENDERER_MESA;
    }

    else if (strstr(cRenderer, "nvidia") != NULL || strstr(cRenderer, "geforce") != NULL || strstr(cRenderer, "quadro") || strstr(cRenderer, "riva tnt") ||
	strstr(cVendor,   "nvidia") != NULL)
    {
        g_iOpenGLRenderer = SVT_RENDERER_NVIDIA;
    }

    else if (strstr(cRenderer, "ati") != NULL || strstr(cRenderer, "radeon") != NULL ||
	strstr(cVendor,   "ati") != NULL)
    {
        g_iOpenGLRenderer = SVT_RENDERER_ATI;
    }

    else if (strstr(cRenderer, "intel") != NULL || strstr(cRenderer, " gma") != NULL ||
	strstr(cVendor,   "intel") != NULL)
    {
        g_iOpenGLRenderer = SVT_RENDERER_INTEL;
    }


// FIXME at some point, we should introduce a commandline parameter for different levels of debugging output verbosity
//     switch (g_iOpenGLRenderer)
//     {
//      case SVT_RENDERER_UNKNOWN: svtout << "SVT_RENDERER_UNKNOWN" << endl; break;
//      case SVT_RENDERER_MESA   : svtout << "SVT_RENDERER_MESA"    << endl; break;
//      case SVT_RENDERER_NVIDIA : svtout << "SVT_RENDERER_NVIDIA"  << endl; break;
//      case SVT_RENDERER_ATI    : svtout << "SVT_RENDERER_ATI"     << endl; break;
//      case SVT_RENDERER_INTEL  : svtout << "SVT_RENDERER_INTEL"   << endl; break;
//     }            

    delete[] cRenderer;

    return g_iOpenGLRenderer;
}

/**
 * get if the use of vertex and fragment shader programs is ok
 */
DLLEXPORT bool svt_getAllowShaderPrograms()
{
    return g_bAllowShaderPrograms;
}
/**
 * set if the use of vertex and fragment shader programs is ok
 * \param bShader true to allow vertex and fragment shader programs, false to suppress their usage
 */
DLLEXPORT void svt_setAllowShaderPrograms(bool bShader)
{
    g_bAllowShaderPrograms = bShader;
}

/**
 *  * get if ambient occlusion can be used. Solves problems on some hardware (old Intel chipsets in particular)
 *   * \return bool indicates if ambient occlusion allowed or not
 *    */
DLLEXPORT bool svt_getAllowAmbientOcclusion()
{
    return g_bAllowAmbientOcclusion;
}
/**
 *  * set if ambient occlusion can be used. Solves problems on some hardware (old Intel chipsets in particular)
 *   * \param bAO indicates if ambient occlusion allowed or not
 *    */
DLLEXPORT void svt_setAllowAmbientOcclusion(bool bAO)
{
    g_bAllowAmbientOcclusion = bAO;
}

