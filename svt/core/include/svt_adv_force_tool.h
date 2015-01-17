/***************************************************************************
                          svt_adv_force_tool.h  -  description
                             -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ADV_FORCE_TOOL_H
#define SVT_ADV_FORCE_TOOL_H

#include <svt_force_tool.h>
#include <svt_adv_force_calc.h>

/**
 * An advanced force tool.
 * Please read first the documentation of svt_force_tool.
 * In svt_force_tool your main programm will update the forces by simply calling the function
 * setForces().
 * svt_adv_force_tool will require an svt_adv_force_calc object. svt_adv_force_tool will start a thread
 * calculating the forces all the time. This calculation is done inside the svt_adv_force_calc object.
 *
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_adv_force_tool : public svt_force_tool
{

protected:

    // Pointer to force calculation routine
    svt_adv_force_calc* m_pForceCalc;

    // Signals for force thread
    bool m_bThreadCreated;
    bool m_bThreadExit;

    // Flag to enable/disable automatic thread start/stop
    bool m_bThreadAutoStart;

public:
    /**
     * Constructor
     * \param pName the name of the force device
     */
    svt_adv_force_tool(const char* pName);
    virtual ~svt_adv_force_tool() { }

    /**
     * set the svt_adv_force_calc object
     * \param pForceCalc pointer to the force calculation object
     */
    void setForceCalc(svt_adv_force_calc* pForceCalc);
    /**
     * get the svt_adv_force_calc object
     * \return pointer to the force calculation object
     */
    svt_adv_force_calc* getForceCalc();

    /** 
     * Button pressed
     */
    void onButtonPressed();
    /**
     * Button released
     */
    void onButtonReleased();

    /**
     * Init thread
     */
    void initThread();
    /**
     * Stop thread
     */
    void exitThread();

    /**
     * Shall I stop thread
     * \return true if thread should be stopped. Internal function, should not be called by application.
     */
    bool shallExitThread();

    /**
     * Thread created
     */
    bool threadCreated();

    /**
     * OK, thread was stopped. Internal function, should not be called by application.
     */
    void setStopped();

    /**
     * Set thread autostarting flag
     * \param bAutostart (true for automatic thread start/stop)
     */
    void setThreadAutoStart(bool bAutoStart);

    /**
     * Get thread autostarting flag
     * \return true for automatic thread start/stop
     */
    bool setThreadAutoStart();

};

#endif
