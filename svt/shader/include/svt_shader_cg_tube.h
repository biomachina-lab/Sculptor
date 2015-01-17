/*-*-c++-*-*****************************************************************
                          svt_shader_cg_tube
                          ---------------
    begin                : 05/01/2007
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_TUBE_H
#define __SVT_SHADER_CG_TUBE_H

#include <svt_shader_cg.h>
#include <svt_vector3.h>



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////svt_shader_cg_tube/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
 * \brief This is a class derived from svt_shader_cg for the tube mode shader and their
 * parameters.
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_tube : public svt_shader_cg
{


 protected:


    CGparameter m_cgMV;
    CGparameter m_cgMVP;
    CGparameter m_cgViewPos;
    CGparameter m_cgLighting;
    CGparameter m_cgSpecularColor;
    CGparameter m_cgAngle;
    CGparameter m_cgSpan;

    svt_vector3<Real32> m_oViewPos;
    svt_vector3<Real32> m_oLighting;
    svt_vector3<Real32> m_oSpecularColor;
    Real32 m_iAngle;
    Real32 m_iSpan;
    bool m_bFog;


 public:


    /**
     * Constructor
     */
    svt_shader_cg_tube(bool bFile, const char * pTubeVert, const char * pTubeFrag,
		       bool bFog = false,
		       CGprofile pVertProfile = CG_PROFILE_UNKNOWN,
		       CGprofile pFragProfile = CG_PROFILE_UNKNOWN);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg_tube();

    /**
     * Set the width in radian of a quad strip. This is 2*PI divided by the number of corners of the tube's cross section
     * \param oSpan the width of a tube's quad strip (in radian)
     */
    void setSpan(Real32 oSpan);

    /**
     * Set which quad should be drawn
     * \param oAngle the angular position of a quad strip of the tube (in radian)
     */
    void setAngle(Real32 oAngle);

    /**
     * set the current state of parameters
     */
    virtual void setStateVertParams();

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
     * - z specular coefficient
     * \param oSpecularColor svt_vector3<Real32> with specular color
     */
    virtual void setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor);
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////svt_shader_cg_tube_helixsheet/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
 * \brief This is a class derived from svt_shader_cg_tube. It is used for shading the round helices
 * and sheets.
 */
class DLLEXPORT svt_shader_cg_tube_helixsheet : public svt_shader_cg_tube
{


 protected:


    CGparameter m_cgR1;
    CGparameter m_cgR2;

    Real32 m_fR1;
    Real32 m_fR2;


 public:


    /**
     * Constructor
     */
    svt_shader_cg_tube_helixsheet(bool bFile, const char * pTubeVert, const char * pTubeFrag,
				  bool bFog = false,
				  CGprofile pVertProfile = CG_PROFILE_UNKNOWN,
				  CGprofile pFragProfile = CG_PROFILE_UNKNOWN);

    /**
     * set radius in one direction
     */
    void setR1(Real32 fR1);

    /**
     * set radius in the other direction
     */
    void setR2(Real32 fR2);
};

#endif
