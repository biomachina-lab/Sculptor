/***************************************************************************
                          svt_shader_cg
                          ---------------
    begin                : 08/03/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_H
#define __SVT_SHADER_CG_H

#include <svt_opengl.h>

#ifndef CWD
# define CWD ""
#endif


class DLLEXPORT svt_shader_cg
{
protected:

    CGprogram m_pVertProg;
    CGprogram m_pFragProg;
    CGprofile m_pVertProfile;
    CGprofile m_pFragProfile;

    CGparameter m_pModelViewProj;

    static CGcontext m_pContext;

    bool m_bVertex;
    bool m_bFragment;

public:
    /**
     * Constructor
     * \param bFile if true the following pointers are actually filenames, if false the pointers point to the actual vertex/fragment shader source code
     * \param pVertex pointer to char with filename of vertex shader sourcecode
     * \param pFragment pointer to char with filename of fragment shader sourcecode
     * \param pVertProfile determine a specific vertex profile. If not given or set to CG_PROFILE_UNKNOWN, latest profile will be determined
     * \param pFragProfile determine a specific fragment profile. If not given or set to CG_PROFILE_UNKNOWN, latest profile will be determined
      */
    svt_shader_cg(bool bFile=true, const char* pVertex=NULL, const char* pFragment=NULL, CGprofile pVertProfile = CG_PROFILE_UNKNOWN, CGprofile pFragProfile = CG_PROFILE_UNKNOWN);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg();

    /**
     * set the current state of parameters
     */
    virtual void setStateVertParams();

    /**
     * set the current state of parameters
     */
    virtual void setStateFragParams();

    /**
     * Bind shader
     */
    virtual void bind();
    /**
     * Unbind shader
     */
    virtual void unbind();

    /**
     * add parameter to vertex shader
     * \param pParam pointer to char with the parameter name
     */
    void addVertParam(const char* pParam);
    /**
     * add parameter to fragment shader
     * \param pParam pointer to char with the parameter name
     */
    void addFragParam(const char* pParam);

    /**
     * print the log file associated with this shader
     */
    void printInfoLog();
};

#endif
