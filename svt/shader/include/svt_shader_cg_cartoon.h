/*-*-c++-*-*****************************************************************
                          svt_shader_cg_cartoon
                          ---------------
    begin                : 05/01/2007
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SHADER_CG_CARTOON_H
#define SVT_SHADER_CG_CARTOON_H

#include <svt_opengl.h>
#include <svt_shader_cg.h>
#include <svt_vector3.h>
#include <svt_vector4.h>


// GeForce4 Ti4200: arbvp1 
//                  fp20

// GeForceFX 5200:  arbvp1 
//                  fp30


/**
 * \brief This is a class derived from svt_shader_cg for the cartoon vertex shader.
 *
 * It is supposed top be used only for the vertex shader programs. The fragment shaders have their own exclusive class (svt_shader_cg_cartoon_fragment)
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_cartoon : public svt_shader_cg
{


 protected:


    CGparameter m_cgMV;
    CGparameter m_cgMVP;
    CGparameter m_cgHeight;
    CGparameter m_cgViewPos;
    CGparameter m_cgLighting;
    CGparameter m_cgSpecularColor;

    svt_matrix4< Real32 > m_oMVP;
    svt_vector3< Real32 > m_oViewPos;
    svt_vector3< Real32 > m_oLighting;
    svt_vector3< Real32 > m_oSpecularColor;
    Real32  m_fHeight;
    bool m_bFog;


 public:


    /**
     * Constructor
     */
    svt_shader_cg_cartoon(bool bFile, const char * pCartoonVert, const char * pCartoonFrag = NULL, bool bFog = false);
    /**
     * Destructor
     */
    virtual ~svt_shader_cg_cartoon();

    /**
     * set the current state of parameters
     */
    virtual void setStateVertParams();

    /**
     * set the height of the tube strip
     * \param fHeight the height of the tube strip
     */
    void setHeight(Real32 fHeight);

    /**
     * set viewing position, which is used as the light position
     * \param oViewPos, an svt_vector3<Real32> with the 3D viewing position
     */
    void setViewPos(svt_vector3<Real32> oViewPos);

    /**
     * set lighting parameters
     * \param oLighting svt_vector3<Real32> with
     * - x ambient coefficient
     * - y diffuse coefficient
     * - z specular coefficient
     * \param oSpecularColor svt_vector3<Real32> with specular color
     */
    virtual void setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor);
};

#endif // SVT_SHADER_CG_CARTOON_H
