/***************************************************************************
                          liveForceField.h  -  description
                          -------------------
    begin                : 20.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_FORCEFIELD_H
#define LIVE_FORCEFIELD_H

#include <live.h>

// ghost includes
#include <stdlib.h>
#include <gstBasic.h>
#include <gstPHANToM.h>
#include <gstScene.h>
#include <gstSeparator.h>
#include <gstForceField.h>

/**
 * liveForceField
 */
class liveForceField : public gstForceField {

protected:
    Real32 m_fForceX;
    Real32 m_fForceY;
    Real32 m_fForceZ;
    Real32 m_fTorqueForceX;
    Real32 m_fTorqueForceY;
    Real32 m_fTorqueForceZ;

public:
    liveForceField();

    gstVector calculateForceFieldForce(gstPHANToM *phantom, gstVector &torques);

    void setForces(Real32 fForceX, Real32 fForceY, Real32 fForceZ);
    void setForceX(Real32 fForceX);
    void setForceY(Real32 fForceY);
    void setForceZ(Real32 fForceZ);
    void setTorqueForceX(Real32 fTorqueX);
    void setTorqueForceY(Real32 fTorqueY);
    void setTorqueForceZ(Real32 fTorqueZ);
};

#endif
