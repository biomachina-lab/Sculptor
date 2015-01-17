/***************************************************************************
                          svt_shader_cg_volume_fast
                          -------------------------
    begin                : 12/22/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_VOLUME_FAST_H
#define __SVT_SHADER_CG_VOLUME_FAST_H

#include <svt_shader_cg.h>
#include <svt_vector3.h>
#include <svt_matrix4.h>

class DLLEXPORT svt_shader_cg_volume_fast : public svt_shader_cg
{
protected:

    CGparameter m_pTexMat;
    CGparameter m_pVolume;
    CGparameter m_pTransfer;
    CGparameter m_pModelViewInv;
    CGparameter m_pSliceDistance;
    CGparameter m_pF_ModelViewProj;

    int m_iVolumeID;
    int m_iTransferID;
    float m_fSliceDistance;
    svt_matrix4<Real32> m_oModelView;

public:
    /**
     * Constructor
     */
    svt_shader_cg_volume_fast();
    virtual ~svt_shader_cg_volume_fast(){};

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
     * set modelview matrix
     * \param oModelView svt_matrix4<Real32> modelview matrix object
     */
    void setModelViewMatrix(svt_matrix4<Real32> oModelView);
};

#endif
