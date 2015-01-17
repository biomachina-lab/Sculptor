/***************************************************************************
                          svt_shader_cg
                          -------------
    begin                : 08/03/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_shader_cg.h>

CGcontext svt_shader_cg::m_pContext = NULL;

/**
 * Constructor
 * \param bFile if true the following pointers are actually filenames, if false the pointers point to the actual vertex/fragment shader source code
 * \param pVertex pointer to char with filename of vertex shader sourcecode
 * \param pFragment pointer to char with filename of fragment shader sourcecode
 * \param pVertProfile determines a specific vertex profile. If not given or set to CG_PROFILE_UNKNOWN, latest profile will be determined
 * \param pFragProfile determines a specific fragment profile. If not given or set to CG_PROFILE_UNKNOWN, latest profile will be determined
 */
svt_shader_cg::svt_shader_cg(bool bFile, const char* pVertex, const char* pFragment, CGprofile pVertProfile, CGprofile pFragProfile) :
    m_bVertex( false ),
    m_bFragment( false )
{
    // are shaders allowed at all?
    if (svt_getAllowShaderPrograms() == false)
	return;

    if (pVertex != NULL)
	m_bVertex = true;

    if (pFragment != NULL)
	m_bFragment = true;

    // is there already a context?
    if (m_pContext == NULL)
    {
	m_pContext = cgCreateContext();
	printInfoLog();
    }

    // get the profiles right
    if (m_bVertex)
    {
	if (pVertProfile == CG_PROFILE_UNKNOWN)
	    m_pVertProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	else
	    m_pVertProfile = pVertProfile;

	printInfoLog();

        if (m_pVertProfile != CG_PROFILE_UNKNOWN)
        {
//#define DEBUG_SVT_SHADER_CG
#ifdef DEBUG_SVT_SHADER_CG
            printf("%s%s\n", "Cg Vertex-Profile: ",   cgGetProfileString( m_pVertProfile ));
	    svtout <<        "Cg Vertex-Profile: " << cgGetProfileString( m_pVertProfile ) << endl;
#endif
            cgGLSetOptimalOptions(m_pVertProfile);
            printInfoLog();
        }
	else
	{
            svtout <<      "Error: Was not able to initialize Cg vertex shader profile..." << endl;
	    m_bVertex = false;
#ifdef DEBUG_SVT_SHADER_CG
	    printf("%s\n", "Error: Was not able to initialize Cg vertex shader profile...");
#endif
	}

    }
    if (m_bFragment)
    {
	if (pFragProfile == CG_PROFILE_UNKNOWN)
	    m_pFragProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	else
	    m_pFragProfile = pFragProfile;

	printInfoLog();

        if (m_pFragProfile != CG_PROFILE_UNKNOWN)
        {
#ifdef DEBUG_SVT_SHADER_CG
            printf("%s%s\n", "Cg Fragment-Profile: ",   cgGetProfileString( m_pFragProfile ));
            svtout <<        "Cg Fragment-Profile: " << cgGetProfileString( m_pFragProfile ) << endl;
#endif
            cgGLSetOptimalOptions(m_pFragProfile);
            printInfoLog();
        }
	else
	{
	    m_bFragment = false;
            svtout <<      "Error: Was not able to initialize Cg fragment shader profile..." << endl;
#ifdef DEBUG_SVT_SHADER_CG
	    printf("%s\n", "Error: Was not able to initialize Cg fragment shader profile...");
#endif
	}
    }

    if (bFile)
    {
	if (m_bVertex && m_pVertProfile != CG_PROFILE_UNKNOWN)
	{
#ifdef DEBUG_SVT_SHADER_CG
            printf("%s%s\n", "Vertex program: ",   pVertex);
            svtout <<        "Vertex program: " << pVertex << endl;
#endif
	}

	if (m_bFragment && m_pFragProfile != CG_PROFILE_UNKNOWN)
	{
 #ifdef DEBUG_SVT_SHADER_CG
            printf("%s%s\n", "Fragment program: ",   pFragment);
	    svtout <<        "Fragment program: " << pFragment << endl;
 #endif
	}
    }

    // load vertex program
    if (m_bVertex && m_pVertProfile != CG_PROFILE_UNKNOWN)
    {
#ifdef DEBUG_SVT_SHADER_CG
	printf("%s\n", "Create vertex program");
	svtout <<      "Create vertex program" << endl;
#endif

	// Load and compile the vertex program from pVertex
        if (bFile)
	    m_pVertProg = cgCreateProgramFromFile(m_pContext, CG_SOURCE, pVertex, m_pVertProfile, NULL, NULL);
        else
	    m_pVertProg = cgCreateProgram(m_pContext, CG_SOURCE, pVertex, m_pVertProfile, NULL, NULL);

	printInfoLog();

	if (!cgIsProgramCompiled(m_pVertProg))
	{
#ifdef DEBUG_SVT_SHADER_CG
	    printf("%s\n", "Compile vertex program");
	    svtout <<      "Compile vertex program" << endl;
#endif
	    cgCompileProgram(m_pVertProg);
	    printInfoLog();
	}

	// load the vertex program.
#ifdef DEBUG_SVT_SHADER_CG
	printf("%s\n", "Load vertex program into card");
	svtout <<      "Load vertex program into card" << endl;
#endif
	cgGLLoadProgram(m_pVertProg);

	printInfoLog();
    }

    // load fragment program
    if (m_bFragment && m_pFragProfile != CG_PROFILE_UNKNOWN)
    {
#ifdef DEBUG_SVT_SHADER_CG
	printf("%s\n", "Create fragment program");
	svtout <<      "Create fragment program" << endl;
#endif

	// Load and compile the fragment program from pFragment
        if (bFile)
	    m_pFragProg = cgCreateProgramFromFile(m_pContext, CG_SOURCE, pFragment, m_pFragProfile, NULL, NULL);
        else
	    m_pFragProg = cgCreateProgram(m_pContext, CG_SOURCE, pFragment, m_pFragProfile, NULL, NULL);

	printInfoLog();

	if (!cgIsProgramCompiled(m_pFragProg))
	{
#ifdef DEBUG_SVT_SHADER_CG
	    printf("%s\n", "Compile fragment program");
	    svtout <<      "Compile fragment program" << endl;
#endif
	    cgCompileProgram(m_pFragProg);
	    printInfoLog();
	}

	// load the fragment program.
#ifdef DEBUG_SVT_SHADER_CG
	printf("%s\n", "Load fragment program into card");
	svtout <<      "Load fragment program into card" << endl;
#endif
	cgGLLoadProgram(m_pFragProg);

	printInfoLog();
    }

    // parameter
    if (m_bVertex)
	m_pModelViewProj = cgGetNamedParameter(m_pVertProg, "ModelViewProj");
    else
	m_pModelViewProj = NULL;

    if (m_bVertex || m_bFragment)
        printInfoLog();
};


/**
 * Destructor
 */
svt_shader_cg::~svt_shader_cg()
{
    if (m_bFragment)
        cgDestroyProgram( m_pFragProg );

    if (m_bVertex)
        cgDestroyProgram( m_pVertProg );
};


/**
 * set the current state of parameters
 */
void svt_shader_cg::setStateVertParams()
{
    if(m_pModelViewProj != NULL)
	cgGLSetStateMatrixParameter(m_pModelViewProj,
				    CG_GL_MODELVIEW_PROJECTION_MATRIX,
				    CG_GL_MATRIX_IDENTITY);
}


/**
 * set the current state of parameters
 */
void svt_shader_cg::setStateFragParams()
{
}


/**
 * Bind shader
 */
void svt_shader_cg::bind()
{
    if (m_bVertex)
    {
	cgGLBindProgram(m_pVertProg);

        setStateVertParams();

	cgGLEnableProfile(m_pVertProfile);
	printInfoLog();
    }
    if (m_bFragment)
    {
	cgGLBindProgram(m_pFragProg);

	setStateFragParams();

	cgGLEnableProfile(m_pFragProfile);
	printInfoLog();
    }
};


/**
 * Unbind shader
 */
void svt_shader_cg::unbind()
{
    if (m_bVertex)
	cgGLDisableProfile(m_pVertProfile);
    if (m_bFragment)
	cgGLDisableProfile(m_pFragProfile);
};


/**
 * add parameter to vertex shader
 * \param pParam pointer to char with the parameter name
 */
void svt_shader_cg::addVertParam(const char* pParam)
{
    if (m_bVertex)
        cgGetNamedParameter(m_pVertProg, pParam);
}
/**
 * add parameter to fragment shader
 * \param pParam pointer to char with the parameter name
 */
void svt_shader_cg::addFragParam(const char* pParam)
{
    if (m_bFragment)
        cgGetNamedParameter(m_pFragProg, pParam);
}

/**
 * print the log file associated with this shader
 */
void svt_shader_cg::printInfoLog()
{
    CGerror err = cgGetError();

    while(err != CG_NO_ERROR)
    {
	svtout << cgGetErrorString(err) << endl;
	printf("%s\n", cgGetErrorString(err));
	err = cgGetError();
    }
}
