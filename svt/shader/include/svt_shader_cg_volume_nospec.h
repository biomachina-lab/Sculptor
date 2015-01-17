/***************************************************************************
                          svt_shader_cg_volume_nospec
                          ---------------------------
    begin                : 05/09/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_VOLUME_NOSPEC_H
#define __SVT_SHADER_CG_VOLUME_NOSPEC_H

#include <svt_shader_cg.h>
#include <svt_vector3.h>
#include <svt_matrix4.h>

class DLLEXPORT svt_shader_cg_volume_nospec : public svt_shader_cg
{
protected:

    CGparameter m_pTexMat;
    CGparameter m_pVolume;
    CGparameter m_pTransfer;
    CGparameter m_pModelViewInv;
    CGparameter m_pSliceDistance;
    CGparameter m_pLightPosition;
    CGparameter m_pDiffuse;
    CGparameter m_pAmbient;
    CGparameter m_pShading;

    int m_iVolumeID;
    int m_iTransferID;
    float m_fSliceDistance;
    svt_vector3<Real32> m_vLightPosition;
    svt_matrix4<Real32> m_oModelView;

    Real32 m_fDiffuse;
    Real32 m_fAmbient;
    Real32 m_fShininess;

public:
    /**
     * Constructor
     */
    svt_shader_cg_volume_nospec();
    virtual ~svt_shader_cg_volume_nospec(){};

    /**
     * set the current state of parameters
     */
    virtual void setStateVertParams();

    /**
     * set the current state of parameters
     */
    virtual void setStateFragParams();

    /**
     * set volume ID
     * \param iVolumeID texture ID of 3D volume map
     */
    void setVolumeID(int iVolumeID);
    /**
     * set transfer ID
     * \param iTransferID texture ID of transfer map
     */
    void setTransferID(int iTransferID);

    /**
     * set slice thickness
     * \param fSliceDistance distance between slices
     */
    void setSliceDistance(float fSliceDistance);

    /**
     * set light position
     * \param vLightPosition svt_vector3<Real32> with the 3D position of the light source
     */
    void setLightPosition(svt_vector3<Real32> vLightPosition);

    /**
     * set modelview matrix
     * \param oModelView svt_matrix4<Real32> modelview matrix object
     */
    void setModelViewMatrix(svt_matrix4<Real32> oModelView);

    /**
     * set lighting params
     * \param fDiffuse diffuse lighting factor (0..10)
     * \param fAmbient ambient light contribution (0..1)
     */
    void setLightingParams( Real32 fDiffuse, Real32 fAmbient );
};

#endif
