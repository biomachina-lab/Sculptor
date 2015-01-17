/***************************************************************************
                          svt_adv_force_calc.h  -  description
                             -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ADV_FORCE_CALC_H
#define SVT_ADV_FORCE_CALC_H

#include <svt_core.h>
#include <svt_types.h>

/**
 * A force calculation class. Please see svt_adv_force_tool for more information.
 *
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_adv_force_calc
{
private:
    Real32 m_fForceX;
    Real32 m_fForceY;
    Real32 m_fForceZ;
    Real32 m_fTorqueX;
    Real32 m_fTorqueY;
    Real32 m_fTorqueZ;

public:
    /**
     * Constructor
     */
    svt_adv_force_calc();
    /**
     * Destructor
     */
    virtual ~svt_adv_force_calc() {};

    /**
     * calculate forces. You should inherit from this class and make a real implementation of this function.
     */
    virtual void calculateForces();

    /**
     * get x axis force
     * \return x axis force
     */
    Real32 getForceX();
    /**
     * get y axis force
     * \return y axis force
     */
    Real32 getForceY();
    /**
     * get z axis force
     * \return z axis force
     */
    Real32 getForceZ();
    /**
     * set x axis force
     * \param fForceX x axis force
     */
    void setForceX(Real32 fForceX);
    /**
     * set y axis force
     * \param fForceY y axis force
     */
    void setForceY(Real32 fForceY);
    /**
     * set z axis force
     * \param fForceZ z axis force
     */
    void setForceZ(Real32 fForceZ);

    /**
     * get x axis torque force
     * \return x axis torque force
     */
    Real32 getTorqueForceX();
    /**
     * get y axis torque force
     * \return y axis torque force
     */
    Real32 getTorqueForceY();
    /**
     * get z axis torque force
     * \return z axis torque force
     */
    Real32 getTorqueForceZ();
    /**
     * set x axis torque force
     * \param fForceX x axis torque force
     */
    void setTorqueForceX(Real32 fForceX);
    /**
     * set y axis torque force
     * \param fForceY y axis torque force
     */
    void setTorqueForceY(Real32 fForceY);
    /**
     * set z axis torque force
     * \param fForceZ z axis torque force
     */
    void setTorqueForceZ(Real32 fForceZ);
};

#endif
