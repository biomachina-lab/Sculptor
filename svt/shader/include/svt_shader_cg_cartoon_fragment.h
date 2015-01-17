/*-*-c++-*-*****************************************************************
                          svt_shader_cg_cartoon_fragment
                          ---------------
    begin                : 09/04/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SHADER_CG_CARTOON_FRAGMENT_H
#define SVT_SHADER_CG_CARTOON_FRAGMENT_H

#include <svt_shader_cg.h>
#include <svt_vector3.h>

// GeForce4 Ti4200: arbvp1 
//                  fp20

// GeForceFX 5200:  arbvp1 
//                  fp30


/**
 * \brief This is a class derived from svt_shader_cg for the cartoon and newtube fragment shader.
 *
 * It is supposed to be used only for the cartoon/newtube fragment shader program. The vertex
 * shaders pass some parameters to the fragment shaders for lighting computation. Both have their
 * own class (svt_shader_cg_cartoon and svt_shader_cg_newtube).
 * \author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_cartoon_fragment : public svt_shader_cg
{


 protected:

    
    CGparameter m_cgMV;
    CGparameter m_cgViewPos;
    CGparameter m_cgLighting;
    CGparameter m_cgSpecularColor;
    CGparameter m_cgFogColor;
    CGparameter m_cgFogDensity;
    CGparameter	m_cgFogStart;
    CGparameter	m_cgFogEnd;
    CGparameter	m_cgFogZoomOffset;

    GLfloat * m_aFogColor;
    GLfloat m_fFogDensity;
    GLfloat m_fFogStart;
    GLfloat m_fFogEnd;
    GLfloat m_fFogZoomOffset;
    GLint m_iFogMode;

    svt_vector3< Real32 > m_oViewPos;
    svt_vector3< Real32 > m_oLighting;
    svt_vector3< Real32 > m_oSpecularColor;


 public:


    /**
     * Constructor
     * \param iFogMode this is a GL_ENUM that determines the fog type to be used. Can be
     * - GL_FALSE
     * - GL_LINEAR
     * - GL_EXP
     * - GL_EXP2
     * all other parameter are same with svt_shader_cg
     */
    svt_shader_cg_cartoon_fragment(GLint iFogMode, bool bFile, const char * pTubeFrag);
    /**
     * Destructor
     */
    virtual ~svt_shader_cg_cartoon_fragment();

    /**
     * set the current state of parameters
     */
    virtual void setStateFragParams();

    /**
     * set viewing position, which is used as the light position
     * \param oViewPos, an svt_vector3<Real32> with the 3D viewing position
     */
    virtual void setViewPos(svt_vector3<Real32> oViewPos);

    /**
     * set lighting parameters
     * \param oLighting svt_vector3<Real32> with
     * - x ambient coefficient
     * - y diffuse coefficient
     * - z specular shininess
     * \param oSpecularColor svt_vector3<Real32> with specular color
     */
    virtual void setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor);

    /**
     * set fog color
     * \param aFogColor pointer to GLfloat with the fog color values
     */
    void setFogColor(GLfloat * aFogColor);

    /**
     * set fog density
     * \param fFogDensity float with the fog density value
     */
    void setFogDensity(GLfloat fFogDensity);

    /**
     * set fog start
     * \param iFogStart float the fog start value
     */
    void setFogStart(GLfloat fFogStart);

    /**
     * set fog end
     * \param iFogEnd float the fog end value
     */
    void setFogEnd(GLfloat fFogEnd);

    /**
     * 
     * \param
     */
    void setFogZoomOffset(GLfloat fZoomOffset);
};

#endif // SVT_SHADER_CG_CARTOON_FRAGMENT_H
