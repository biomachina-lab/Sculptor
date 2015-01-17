/***************************************************************************
                          svt_adv_force_calc.cpp  -  description
                             -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_adv_force_calc.h>

/**
 * Constructor
 */
svt_adv_force_calc::svt_adv_force_calc()
{
}

/**
 * calculate forces. You should inherit from this class and make a real implementation of this function.
 */
void svt_adv_force_calc::calculateForces()
{
}

/**
 * get x axis force
 * \return x axis force
 */
Real32 svt_adv_force_calc::getForceX()
{
    return m_fForceX;
}
/**
 * get y axis force
 * \return y axis force
 */
Real32 svt_adv_force_calc::getForceY()
{
    return m_fForceY;
}
/**
 * get z axis force
 * \return z axis force
 */
Real32 svt_adv_force_calc::getForceZ()
{
    return m_fForceZ;
}
/**
 * set x axis force
 * \param fForceX x axis force
 */
void svt_adv_force_calc::setForceX(Real32 fForceX)
{
    m_fForceX = fForceX;
}
/**
 * set y axis force
 * \param fForceY y axis force
 */
void svt_adv_force_calc::setForceY(Real32 fForceY)
{
    m_fForceY = fForceY;
}
/**
 * set z axis force
 * \param fForceZ z axis force
 */
void svt_adv_force_calc::setForceZ(Real32 fForceZ)
{
    m_fForceZ = fForceZ;
}
/**
 * get x axis torque force
 * \return x axis torque force
 */
Real32 svt_adv_force_calc::getTorqueForceX()
{
    return m_fTorqueX;
}
/**
 * get y axis torque force
 * \return y axis torque force
 */
Real32 svt_adv_force_calc::getTorqueForceY()
{
    return m_fTorqueY;
}
/**
 * get z axis torque force
 * \return z axis torque force
 */
Real32 svt_adv_force_calc::getTorqueForceZ()
{
    return m_fTorqueZ;
}
/**
 * set x axis torque force
 * \param fForceX x axis torque force
 */
void svt_adv_force_calc::setTorqueForceX(Real32 fForceX)
{
    m_fTorqueX = fForceX;
}
/**
 * set y axis torque force
 * \param fForceY y axis torque force
 */
void svt_adv_force_calc::setTorqueForceY(Real32 fForceY)
{
    m_fTorqueY = fForceY;
}
/**
 * set z axis torque force
 * \param fForceZ z axis torque force
 */
void svt_adv_force_calc::setTorqueForceZ(Real32 fForceZ)
{
    m_fTorqueZ = fForceZ;
}
