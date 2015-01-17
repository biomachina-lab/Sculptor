/***************************************************************************
                          svt_shader_cg_gpgpu
                          ---------------
    begin                : 25/07/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_shader_cg_gpgpu.h>
#include <svt_iostream.h>


/**
 * Constructor
 * \param bFile if true the following pointers are actually filenames, if false the pointers point to the actual vertex/fragment shader source code
 * \param pVertex pointer to char with filename of vertex shader sourcecode
 * \param pFragment pointer to char with filename of fragment shader sourcecode
 */
svt_shader_cg_gpgpu::svt_shader_cg_gpgpu(bool bFile, const char* pVertex, const char* pFragment, CGprofile pVertProfile, CGprofile pFragProfile) : svt_shader_cg(bFile, pVertex, pFragment, pVertProfile, pFragProfile)
{
    m_pcgInputTex = cgGetNamedParameter(m_pFragProg, "InputTex");
}

/**
 * Destructor
 */
svt_shader_cg_gpgpu::~svt_shader_cg_gpgpu()
{
}


/**
 * set the current state of parameters
 */
void svt_shader_cg_gpgpu::setStateFragParams()
{
    if(m_pcgInputTex != NULL)
    {
	cgGLSetTextureParameter(m_pcgInputTex, m_iInputTexID);
	cgGLEnableTextureParameter(m_pcgInputTex);
    }
}

/**
 * Unbind shader
 */
void svt_shader_cg_gpgpu::unbind()
{
    if (m_bVertex && m_pVertProfile != CG_PROFILE_UNKNOWN)
	cgGLDisableProfile(m_pVertProfile);
    if (m_bFragment && m_pFragProfile != CG_PROFILE_UNKNOWN)
    {
        cgGLDisableTextureParameter(m_pcgInputTex);
	cgGLDisableProfile(m_pFragProfile);
    }
};



/**
 * add input texture for the fragment shader
 */
void svt_shader_cg_gpgpu::addInputTex(GLuint iInputTexID)
{
    m_iInputTexID = iInputTexID;
}

/**
 * Bind shader
 */
void svt_shader_cg_gpgpu::bind()
{
    if (m_bVertex && m_pVertProfile != CG_PROFILE_UNKNOWN)
    {
	cgGLBindProgram(m_pVertProg);

        setStateVertParams();

	cgGLEnableProfile(m_pVertProfile);
	printInfoLog();
    }
    if (m_bFragment && m_pFragProfile != CG_PROFILE_UNKNOWN)
    {
	cgGLBindProgram(m_pFragProg);

	printInfoLog();

	setStateFragParams();

	cgGLEnableProfile(m_pFragProfile);
	printInfoLog();
    }
};

/**
 * print the log file associated with this shader
 */
void svt_shader_cg_gpgpu::printInfoLog()
{
    CGerror err = cgGetError();

    while(err != CG_NO_ERROR)
    {
	svtout << "  CG error: " << cgGetErrorString(err) << endl;
	err = cgGetError();
    }
}
