/***************************************************************************
                          svt_tool.h  -  description
                             -------------------
    begin                : 26.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TOOL_H
#define SVT_TOOL_H

#include <svt_core.h>
#include <svt_node.h>
#include <svt_device.h>
#include <svt_channel.h>
#include <svt_matrix4f.h>

/** A universal tool. The tool is a node, which is tracked by a live device. The class will read the concrete live device driver name and the channel for the matrix and the button from the application configuration file. Therefor you must specify a name for the tool (e.g. "Pointer").
 * The class will search for PointerDevice, PointerMatrixChannel, PointerButtonChannel in the config file.
 *
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_tool : public svt_node
{
protected:
    svt_device* m_pDevice;
    svt_channel* m_pMatrixChannel;
    svt_channel* m_pButtonChannel;
    bool m_bButtonPressed;
    bool m_bButtonLogic;
    svt_matrix4f* m_pTransformation;
    svt_matrix4f* m_pTmpMat;

    svt_matrix4<Real32> m_oOldMat;

    bool m_bActive;

    Real64 m_fJitterSensitivity;

public:
    /// 1st param: the name of the pointer device
    svt_tool(const char* name);
    virtual ~svt_tool(){};

    /**
     * Set the tool into an active/inactive state.
     * \param bActive if true, the tool is active
     */
    void setActive( bool bActive );

    /**
     * Get the tool active/inactive state.
     * \return if true, the tool is active
     */
    bool getActive( );

    /**
     * Set the jitter sensitivity
     * \param fJitterSensitivity
     */
    void setJitterSensitivity( Real64 fJitterSensitivity );

    /**
     * Get the jitter sensitivity
     * \param fJitterSensitivity
     */
    Real64 getJitterSensitivity();

    /// the function is called if the user presses the button
    virtual void onButtonPressed();
    /// the function is called if the user releases the button
    virtual void onButtonReleased();

    /// returns the current state of the button
    bool isButtonPressed();

    /// returns the matrix of the device
    svt_matrix4f* getDeviceMatrix();
    /// returns the device
    svt_device* getDevice();

    /**
     * switch the logic of the button
     * \param bLogic false - normal logic, true - reverse logic
     */
    void setButtonLogic(bool bLogic);
    /**
     * get the logic of the button
     * \return false - normal logic, true - reverse logic
     */
    bool getButtonLogic();

    virtual bool recalc();
};

#endif
