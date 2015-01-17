/*-*-c++-*-*****************************************************************
                          svt_shader_cg_toon
                          ---------------
    begin                : 09/12/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_TOON_H
#define __SVT_SHADER_CG_TOON_H

// svt includes
#include <svt_shader_cg_toon.h>
#include <svt_shader_manager.h>
#include <svt_opengl.h>
#include <svt_shader_cg.h>
#include <svt_vector3.h>
#include <svt_matrix.h>

// GeForce4 Ti: arbvp1 
//              fp20

/**
 * This is a class derived from svt_shader_cg for the toon mode shader and their parameters. It also
 * handles the van der Waals spheres.
 * \brief This is a class derived from svt_shader_cg for the toon mode shader and their parameters
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_toon : public svt_shader_cg
{


protected:


    enum {SVT_FOG_NONE,
	  SVT_FOG_LINEAR,
	  SVT_FOG_EXP,
	  SVT_FOG_EXP2};


    CGparameter m_cgMVP;
    CGparameter m_cgMV;
    CGparameter m_cgViewpos;
    CGparameter m_cgLightpos;
    CGparameter m_cgLighting;
    CGparameter m_cgSpecularColor;
    CGparameter m_cgViewport;
    CGparameter m_cgUp;
    CGparameter m_cgCelmap;
    CGparameter m_cgZ1Z2;
    CGparameter m_cgFogColor;
    CGparameter m_cgFogDensity;
    CGparameter m_cgFogStart;
    CGparameter m_cgFogEnd;
    CGparameter m_cgFogZoomOffset;

    unsigned int m_iCelmapHandle;
    Real32 m_fZ1, m_fZ2;
    unsigned int m_iFogMode;
    GLfloat * m_aFogColor;
    GLfloat m_fFogDensity;
    GLfloat m_fFogStart;
    GLfloat m_fFogEnd;
    GLfloat m_fFogZoomOffset;

    svt_vector3< Real32 > m_oViewpos;
    svt_vector3< Real32 > m_oLightpos;
    svt_vector4< Real32 > m_oViewport;
    svt_vector3< Real32 > m_oUp;

    svt_vector3<Real32> m_oLighting;
    svt_vector3<Real32> m_oSpecularColor;


 public:


    /**
     * Constructor
     */
    svt_shader_cg_toon(bool bFile, const char * pToonVert, const char * pToonFrag,
		       GLfloat fFogMode = GL_FALSE,
		       CGprofile pVertProfile = CG_PROFILE_UNKNOWN,
		       CGprofile pFragProfile = CG_PROFILE_UNKNOWN);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg_toon();

//     /**
//      * generate the luminance map with which the spheres will be shaded
//      * \param iSize the side length in pixel of the luminance map (default 64)
//      */
//     void genLuminanceMap(unsigned int iNumColorBins = 0, unsigned int iSize = 64);

//     /**
//      * generate a depth map for approximate depth correction of the spheres
//      * \param iSize the side length in pixel of the depth map (default 64)
//      */
//     void genDepthMap(unsigned int iSize = 64);

    /**
     * generate the 1D luminance texture that is used for cel shading
     * \param iCels the number of cels
     */
    void genCelmap(unsigned int iCels = 3);

    /**
     * set the current state of parameters
     */
    virtual void setStateVertParams();

    /**
     * set the current state of parameters
     */
    virtual void setStateFragParams();

    /**
     * set two values that are used to compute the window z from eye z
     * \param fZ1 used to compute the window z from eye z
     * \param fZ1 used to compute the window z from eye z
     */
    void setZParams(Real32 fZ1, Real32 fZ2);

    /**
     * set viewing position
     * \param oViewpos, an svt_vector3<Real32> with the 3D position of the viewpoint
     */
    void setViewpos(svt_vector3< Real32 > oViewpos);

    /**
     * set light position
     * \param oLightpos, an svt_vector3<Real32> with the 3D position of the light
     */
    void setLightpos(svt_vector3< Real32 > oLightpos);

    /**
     * set the lighting parameters
     * \param svt_vector3<Real32>:
     * - first component is ambient coefficient
     * - second component is diffuse coefficient
     * - third component is shininess
     * \param svt_vector3<Real32> with the specular color. set all components to 0.0f for no specular color
     */
    void setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor);

    /**
     * set viewport parameters
     * \param oViewpos, an svt_vector3<Real32> with the OpenGL viewport parameters
     */
    void setViewport(svt_vector4< Real32 > oViewport);

    /**
     * set current camera up vector
     * \param oUp current camera up vector
     */
    void setUp(svt_vector3<Real32> oUp);

    /**
     * 
     * \param
     */
    void setFogColor(GLfloat * aFogColor);

    /**
     * 
     * \param
     */
    void setFogDensity(GLfloat fFogDensity);

    /**
     * 
     * \param
     */
    void setFogStart(GLfloat fFogStart);

    /**
     * 
     * \param
     */
    void setFogEnd(GLfloat fFogEnd);

    /**
     * 
     * \param
     */
    void setFogZoomOffset(GLfloat fZoomOffset);
};
#endif
