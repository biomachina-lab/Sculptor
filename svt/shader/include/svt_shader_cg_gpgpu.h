/*-*-c++-*-*****************************************************************
                          svt_shader_cg_gpgpu
                          ---------------
    begin                : 25/07/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_GPGPU_H
#define __SVT_SHADER_CG_GPGPU_H

#include <svt_basics.h>
#include <svt_shader_cg.h>
#include <svt_opengl.h>

class DLLEXPORT svt_shader_cg_gpgpu : svt_shader_cg
{

 protected:

    GLuint m_iInputTexID;

    CGparameter m_pcgInputTex;

 public:

    /**
     * Constructor
     * \param bFile if true the following pointers are actually filenames, if false the pointers point to the actual vertex/fragment shader source code
     * \param pVertex pointer to char with filename of vertex shader sourcecode
     * \param pFragment pointer to char with filename of fragment shader sourcecode
     */
    svt_shader_cg_gpgpu(bool bFile, const char* pVertex, const char* pFragment, CGprofile pVertProfile = CG_PROFILE_UNKNOWN, CGprofile pFragProfile = CG_PROFILE_UNKNOWN);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg_gpgpu();


    /**
     * set the current state of parameters
     */
    virtual void setStateFragParams();

    /**
     * add input texture for the fragment shader
     */
    virtual void addInputTex(GLuint iInputTexID);

    /**
     * Unbind shader
     */
    virtual void unbind();

    /**
     * Bind shader
     */
    virtual void bind();
    virtual void printInfoLog();
};

#endif
