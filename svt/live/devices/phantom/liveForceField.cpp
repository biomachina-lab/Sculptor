/***************************************************************************
                          liveForceField.cpp  -  description
                          -------------------
    begin                : 20.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <liveForceField.h>
#include <iostream.h>

liveForceField::liveForceField() : gstForceField()
{
    m_fForceX = 0.0f;
    m_fForceY = 0.0f;
    m_fForceZ = 0.0f;

    m_fTorqueForceX = 0.0f;
    m_fTorqueForceY = 0.0f;
    m_fTorqueForceZ = 0.0f;
}

gstVector liveForceField::calculateForceFieldForce(gstPHANToM *phantom, gstVector &rTorques)
{
    rTorques.setx(m_fTorqueForceX);
    rTorques.sety(m_fTorqueForceY);
    rTorques.setz(m_fTorqueForceZ);

    // konstante Kraft in X-Richtung
    //return gstVector( 1.0f, 0.0f, 0.0f );

    return gstVector(m_fForceX / 100.0f, m_fForceY / 100.0f, m_fForceZ / 100.0f);
}

void liveForceField::setForces(Real32 fForceX, Real32 fForceY, Real32 fForceZ)
{
    m_fForceX = fForceX;
    m_fForceY = fForceY;
    m_fForceZ = fForceZ;
}

void liveForceField::setForceX(Real32 fForceX)
{
    m_fForceX = fForceX;
}

void liveForceField::setForceY(Real32 fForceY)
{
    m_fForceY = fForceY;
}

void liveForceField::setForceZ(Real32 fForceZ)
{
    m_fForceZ = fForceZ;
}

void liveForceField::setTorqueForceX(Real32 fTorqueX)
{
    m_fTorqueForceX = fTorqueX;
};

void liveForceField::setTorqueForceY(Real32 fTorqueY)
{
    m_fTorqueForceY = fTorqueY;
};

void liveForceField::setTorqueForceZ(Real32 fTorqueZ)
{
    m_fTorqueForceZ = fTorqueZ;
};
