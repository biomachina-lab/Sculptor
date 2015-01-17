/***************************************************************************
                          svt_force_tool.h  -  description
                             -------------------
    begin                : 21.06.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FORCE_TOOL_H
#define SVT_FORCE_TOOL_H

#include <svt_core.h>
#include <svt_node.h>
#include <svt_move_tool.h>
#include <svt_channel.h>
#include <svt_vector4f.h>
#include <svt_semaphore.h>

/** A universal force tool. The force tool can output forces to the user (see the functions: setForces() and setTorqueForces()).
 * The class will search for <name>Device, <name>ForceXChannel, <name>ForceYChannel, <name>ForceZChannel, <name>TorqueForceXChannel, <name>TorqueForceYChannel, <name>TorqueForceZChannel in the config file. As this class is inherited from svt_tool it will also search for: <name>MatrixChannel and <name>ButtonChannel.
 * This is just a very simple force class. If you want to do more advanced things, you should have a look at the svt_adv_force_tool.
 *
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_force_tool : public svt_move_tool
{
protected:

    svt_device*  m_pLiveDevice;
    svt_channel* m_pLiveMatrixChannel;
    svt_channel* m_pLiveButtonChannel;

    svt_device*  m_pMouseDevice;
    svt_channel* m_pMouseMatrixChannel;
    svt_channel* m_pMouseButtonChannel;

    svt_channel* m_pForceXChannel;
    svt_channel* m_pForceYChannel;
    svt_channel* m_pForceZChannel;
    svt_channel* m_pTorqueXChannel;
    svt_channel* m_pTorqueYChannel;
    svt_channel* m_pTorqueZChannel;
    svt_channel* m_pForceXTempChannel;
    svt_channel* m_pForceYTempChannel;
    svt_channel* m_pForceZTempChannel;
    svt_channel* m_pTorqueXTempChannel;
    svt_channel* m_pTorqueYTempChannel;
    svt_channel* m_pTorqueZTempChannel;

    svt_vector4f m_oForceVec;
    svt_vector4f m_oTorqueVec;

    int m_iOldSec;
    int m_iFUS;
    int m_iFUSCounter;

    svt_semaphore m_oSema;

public:
    /**
     * Constructor
     * \param pName the name of the force device
     */
    svt_force_tool(const char* pName);
    virtual ~svt_force_tool(){};

    /**
     * Switch to the mouse for input
     */
    void switchToMouse();

    /**
     * Switch to the LIVE device for input
     */
    void switchToLIVE();

    /**
     * set the x,y,z forces
     * \param fForceX force in x axis direction
     * \param fForceY force in y axis direction
     * \param fForceZ force in z axis direction
     */
    void setForces(float fForceX, float fForceY, float fForceZ);
    /**
     * set the torque forces
     * \param fTorqueX torque force around x axis
     * \param fTorqueY torque force around y axis
     * \param fTorqueZ torque force around z axis
     */
    void setTorqueForces(float fTorqueX, float fTorqueY, float fTorqueZ);

    /**
     * get the channel responsible for the x axis force
     */
    svt_channel* getForceXChannel();
    /**
     * get the channel responsible for the y axis force
     */
    svt_channel* getForceYChannel();
    /**
     * get the channel responsible for the z axis force
     */
    svt_channel* getForceZChannel();

    /**
     * get the channel responsible for the x axis torque force
     */
    svt_channel* getTorqueForceXChannel();
    /**
     * get the channel responsible for the y axis torque force
     */
    svt_channel* getTorqueForceYChannel();
    /**
     * get the channel responsible for the z axis torque force
     */
    svt_channel* getTorqueForceZChannel();

    /**
     * get the temperature of the motor for the translational x axis forces
     * \return temp 0.0f .. 1.0f (1.0f is very hot)
     */
    float getForceXTemp();
    /**
     * get the temperature of the motor for the translational y axis forces
     * \return temp 0.0f .. 1.0f (1.0f is very hot)
     */
    float getForceYTemp();
    /**
     * get the temperature of the motor for the translational z axis forces
     * \return temp 0.0f .. 1.0f (1.0f is very hot)
     */
    float getForceZTemp();

    /**
     * get the temperature of the motor for the rotational x axis forces
     * \return temp 0.0f .. 1.0f (1.0f is very hot)
     */
    float getTorqueXTemp();
    /**
     * get the temperature of the motor for the rotational y axis forces
     * \return temp 0.0f .. 1.0f (1.0f is very hot)
     */
    float getTorqueYTemp();
    /**
     * get the temperature of the motor for the rotational z axis forces
     * \return temp 0.0f .. 1.0f (1.0f is very hot)
     */
    float getTorqueZTemp();

    /**
     * get the current "force updates per second" rate
     * \return the current fus
     */
    int getFUS();

protected:
    /**
     * update the forces (send data to the live device)
     */
    void updateForces();

public:

    /**
     * Lock the device and allow the current thread changes in the force level.
     */
    void lockDevice();
    /**
     * Unlock device so that the main thread can update the device.
     */
    void unlockDevice();

};

#endif
