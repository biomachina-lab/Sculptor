/***************************************************************************
                          svt_force_tool.cpp  -  description
                             -------------------
    begin                : 21.06.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_force_tool.h>
#include <svt_init.h>
#include <svt_time.h>

/**
 * Constructor
 * \param pName the name of the force device
 */
svt_force_tool::svt_force_tool(const char* name) : svt_move_tool(name)
{
    char tmp[256];

    //printf("Force tool:\n\n");

    if (strcmp("mouse",m_pDevice->getName())!=0)
    {
        //printf("Device is a haptic device.\n");

	m_pLiveDevice        = m_pDevice;
	m_pLiveMatrixChannel = m_pMatrixChannel;
	m_pLiveButtonChannel = m_pButtonChannel;

	m_pMouseDevice        = new svt_device("mouse");
	m_pMouseMatrixChannel = m_pMouseDevice->getChannel(1);
	m_pMouseButtonChannel = m_pMouseDevice->getChannel(2);
	
    } else {
        //printf("Device is NOT a haptic device.\n");

	m_pLiveDevice        = NULL;
	m_pLiveMatrixChannel = NULL;
	m_pLiveButtonChannel = NULL;

	m_pMouseDevice        = m_pDevice;
	m_pMouseMatrixChannel = m_pMatrixChannel;
	m_pMouseButtonChannel = m_pButtonChannel;
    }

    sprintf(tmp,"%sForceXChannel", name);
    m_pForceXChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sForceYChannel", name);
    m_pForceYChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sForceZChannel", name);
    m_pForceZChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));

    sprintf(tmp,"%sTorqueForceXChannel", name);
    m_pTorqueXChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sTorqueForceYChannel", name);
    m_pTorqueYChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sTorqueForceZChannel", name);
    m_pTorqueZChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));

    sprintf(tmp,"%sForceXTempChannel", name);
    m_pForceXTempChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sForceYTempChannel", name);
    m_pForceYTempChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sForceZTempChannel", name);
    m_pForceZTempChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));

    sprintf(tmp,"%sTorqueXTempChannel", name);
    m_pTorqueXTempChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sTorqueYTempChannel", name);
    m_pTorqueYTempChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));
    sprintf(tmp,"%sTorqueZTempChannel", name);
    m_pTorqueZTempChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)0));

    m_iOldSec = 0;
    m_iFUS = 0;
    m_iFUSCounter = 0;
}

/**
 * Switch to the mouse for input
 */
void svt_force_tool::switchToMouse()
{
    //printf("Switching to Mouse\n");

    m_pDevice = m_pMouseDevice;
    m_pMatrixChannel = m_pMouseMatrixChannel;
    m_pButtonChannel = m_pMouseButtonChannel;

    setJitterSensitivity(0.001);

    //svt_tool::recalc();
}

/**
 * Switch to the LIVE device for input
 */
void svt_force_tool::switchToLIVE()
{
    if (m_pLiveDevice == NULL) return;

    //printf("Switching to LIVE\n");

    m_pDevice = m_pLiveDevice;
    m_pMatrixChannel = m_pLiveMatrixChannel;
    m_pButtonChannel = m_pLiveButtonChannel;

    setJitterSensitivity(0.01);

    svt_tool::recalc();
}

/**
 * set the x,y,z forces
 * \param fForceX force in x axis direction
 * \param fForceY force in y axis direction
 * \param fForceZ force in z axis direction
 */
void svt_force_tool::setForces(float fx, float fy, float fz)
{
    m_oForceVec.x( fx );
    m_oForceVec.y( fy );
    m_oForceVec.z( fz );

    // Get rid of translations in global view matrix since
    // they should not affect the forces.

    svt_matrix4f oTempMat(*getGlobalMatrix());
    oTempMat.setTranslationX(0.0);
    oTempMat.setTranslationY(0.0);
    oTempMat.setTranslationZ(0.0);

    // Apply only the rotations to the force vector

    m_oForceVec.postMult( oTempMat );

    updateForces();
    m_pDevice->update();

    // force updates per second counter
    int curr = svt_getElapsedTime() / 1000;
    if (curr != 0 && curr > m_iOldSec )
    {
        m_iFUS = m_iFUSCounter;
        m_iFUSCounter = 0;
        m_iOldSec = curr;
    }
    m_iFUSCounter++;
}

/**
 * set the torque forces
 * \param fTorqueX torque force around x axis
 * \param fTorqueY torque force around y axis
 * \param fTorqueZ torque force around z axis
 */
void svt_force_tool::setTorqueForces(float fx, float fy, float fz)
{
    m_oTorqueVec.x( fx );
    m_oTorqueVec.y( fy );
    m_oTorqueVec.z( fz );
    m_oTorqueVec.postMult( *getGlobalMatrix() );

    updateForces();
    m_pDevice->update();
}

/**
 * get the channel responsible for the x axis force
 */
svt_channel* svt_force_tool::getForceXChannel()
{
    return m_pForceXChannel;
}
/**
 * get the channel responsible for the y axis force
 */
svt_channel* svt_force_tool::getForceYChannel()
{
    return m_pForceYChannel;
}
/**
 * get the channel responsible for the z axis force
 */
svt_channel* svt_force_tool::getForceZChannel()
{
    return m_pForceZChannel;
}

/**
 * get the channel responsible for the x axis torque force
 */
svt_channel* svt_force_tool::getTorqueForceXChannel()
{
    return m_pTorqueXChannel;
}
/**
 * get the channel responsible for the x axis torque force
 */
svt_channel* svt_force_tool::getTorqueForceYChannel()
{
    return m_pTorqueYChannel;
}
/**
 * get the channel responsible for the x axis torque force
 */
svt_channel* svt_force_tool::getTorqueForceZChannel()
{
    return m_pTorqueZChannel;
}

/**
 * get the temperature of the motor for the translational x axis forces
 * \return temp 0.0f .. 1.0f (1.0f is very hot)
 */
float svt_force_tool::getForceXTemp()
{
    if (m_pForceXTempChannel)
        return m_pForceXTempChannel->getFloat();
    else
        return 0.0f;
}

/**
 * get the temperature of the motor for the translational y axis forces
 * \return temp 0.0f .. 1.0f (1.0f is very hot)
 */
float svt_force_tool::getForceYTemp()
{
    if (m_pForceYTempChannel)
        return m_pForceYTempChannel->getFloat();
    else
        return 0.0f;
}

/**
 * get the temperature of the motor for the translational z axis forces
 * \return temp 0.0f .. 1.0f (1.0f is very hot)
 */
float svt_force_tool::getForceZTemp()
{
    if (m_pForceZTempChannel)
        return m_pForceZTempChannel->getFloat();
    else
        return 0.0f;
}

/**
 * get the temperature of the motor for the rotational x axis forces
 * \return temp 0.0f .. 1.0f (1.0f is very hot)
 */
float svt_force_tool::getTorqueXTemp()
{
    if (m_pTorqueXTempChannel)
        return m_pTorqueXTempChannel->getFloat();
    else
        return 0.0f;
}

/**
 * get the temperature of the motor for the rotational y axis forces
 * \return temp 0.0f .. 1.0f (1.0f is very hot)
 */
float svt_force_tool::getTorqueYTemp()
{
    if (m_pTorqueYTempChannel)
        return m_pTorqueYTempChannel->getFloat();
    else
        return 0.0f;
}

/**
 * get the temperature of the motor for the rotational z axis forces
 * \return temp 0.0f .. 1.0f (1.0f is very hot)
 */
float svt_force_tool::getTorqueZTemp()
{
    if (m_pTorqueZTempChannel)
        return m_pTorqueZTempChannel->getFloat();
    else
        return 0.0f;
}

/**
 * get the current "force updates per second" rate
 * \return the current fus
 */
int svt_force_tool::getFUS()
{
    return m_iFUS;
}

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

/**
 * update the forces (send data to the live device)
 */
void svt_force_tool::updateForces()
{
    if (m_pForceXChannel)
        m_pForceXChannel->setFloat(m_oForceVec.x());
    if (m_pForceYChannel)
        m_pForceYChannel->setFloat(m_oForceVec.y());
    if (m_pForceZChannel)
        m_pForceZChannel->setFloat(m_oForceVec.z());

    if (m_pTorqueXChannel)
        m_pTorqueXChannel->setFloat(m_oTorqueVec.x());
    if (m_pTorqueYChannel)
        m_pTorqueYChannel->setFloat(m_oTorqueVec.y());
    if (m_pTorqueZChannel)
        m_pTorqueZChannel->setFloat(m_oTorqueVec.z());
}

/**
 * Lock the device and allow the current thread changes in the force level.
 */
void svt_force_tool::lockDevice()
{
    m_oSema.P();
}

/**
 * Unlock device so that the main thread can update the device.
 */
void svt_force_tool::unlockDevice()
{
    m_oSema.V();
}

