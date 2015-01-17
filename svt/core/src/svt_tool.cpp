/***************************************************************************
                           svt_tool.cpp
                           -------------------
    begin                : 26.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_tool.h>
#include <svt_init.h>

svt_tool::svt_tool(const char* name) : svt_node()
{
    //printf("SVT Tool Name: %s\n",name);
    char tmp[256];
    sprintf(tmp,"%sDevice", name);
    if (strcmp("Tracker",svt_getConfig()->getValue(tmp, "mouse"))==0)
        m_pDevice = svt_getTrackerDevice();
    else
        m_pDevice = new svt_device(svt_getConfig()->getValue(tmp, "mouse"));

    sprintf(tmp,"%sMatrixChannel", name);
    m_pMatrixChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)1));
    sprintf(tmp,"%sButtonChannel", name);
    m_pButtonChannel = m_pDevice->getChannel(svt_getConfig()->getValue(tmp, (int)2));

    m_bActive = true;

    m_bButtonPressed = false;
    m_bButtonLogic = false;

    m_fJitterSensitivity = 0.001;
}

bool svt_tool::recalc()
{
    // Check if tool is active
    if (m_bActive == false) return false;

    m_pDevice->update();

    // if the device position has changed, please update scenegraph rendering
    svt_matrix4<Real32> oMat(*(m_pMatrixChannel->getMatrix()));
    // but only if it has moved enough, as a lot of device (tracker, haptic device, etc)
    // out slightly fluctuating values
    svt_vector4<Real32> oVecMat(1.0f,1.0f,1.0f); oVecMat = oMat * oVecMat;
    svt_vector4<Real32> oVecOld(1.0f,1.0f,1.0f); oVecOld = m_oOldMat * oVecOld;

    // only if the device moves enough to move an object by 0.01 (typically a mm),
    // a redrawing is triggered
    if (oVecMat.distance(oVecOld) > m_fJitterSensitivity)
    {
	svt_redraw();
    }
    m_oOldMat = oMat;

    setTransformation(m_pMatrixChannel->getMatrix());

    if (m_pButtonChannel->getInt() && !m_bButtonPressed)
    {
        onButtonPressed();
        m_bButtonPressed = !m_bButtonPressed;
    }

    if (!m_pButtonChannel->getInt() && m_bButtonPressed)
    {
        onButtonReleased();
        m_bButtonPressed = !m_bButtonPressed;
    }

    return true;
}

/**
 * Set the jitter sensitivity
 * \param fJitterSensitivity
 */
void svt_tool::setJitterSensitivity( Real64 fJitterSensitivity )
{
    m_fJitterSensitivity = fJitterSensitivity;
}

/**
 * Get the jitter sensitivity
 * \param fJitterSensitivity
 */
Real64 svt_tool::getJitterSensitivity()
{
    return m_fJitterSensitivity;
}

/**
 * Set the tool into an active/inactive state.
 * \param bActive if true, the tool is active
 */
void svt_tool::setActive( bool bActive )
{
    m_bActive = bActive;
}

/**
 * Get the tool active/inactive state.
 * \return if true, the tool is active
 */
bool svt_tool::getActive( )
{
    return m_bActive;
}

void svt_tool::onButtonPressed()
{
}

void svt_tool::onButtonReleased()
{
}

bool svt_tool::isButtonPressed()
{
    if (m_bButtonLogic)
        return !m_bButtonPressed;
    else
        return m_bButtonPressed;
};

svt_matrix4f* svt_tool::getDeviceMatrix()
{
    return m_pMatrixChannel->getMatrix();
}

svt_device* svt_tool::getDevice()
{
    return m_pDevice;
}

/**
 * switch the logic of the button
 * \param bLogic false - normal logic, true - reverse logic
 */
void svt_tool::setButtonLogic(bool bLogic)
{
    m_bButtonLogic = bLogic;
}

/**
 * get the logic of the button
 * \return false - normal logic, true - reverse logic
 */
bool svt_tool::getButtonLogic()
{
    return m_bButtonLogic;
}
