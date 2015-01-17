
/***************************************************************************
                          svt_adv_force_tool.cpp  -  description
                             -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_adv_force_tool.h>
#include <svt_threads.h>
#include <svt_time.h>
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_init.h>

extern void __svt_idle();

svt_semaphore g_oThreadSema;

/**
 * thread
 */
void* calcForceThread(void* pData)
{
    if (!pData)
        return NULL;

    svt_adv_force_tool* pForceTool = (svt_adv_force_tool*)pData;
    svt_adv_force_calc* pForceCalc = pForceTool->getForceCalc();

    unsigned long iTime1 = svt_getToD();
    unsigned long iTime2 = 0;

    bool bMouseMode = false;

    if (strcmp(pForceTool->getDevice()->getName(),"mouse") == 0)
        bMouseMode = true;

    g_oThreadSema.P();

    while(!pForceTool->shallExitThread())
    {
        pForceTool->lockDevice();

        if (pForceCalc)
        {
            if (!bMouseMode) pForceTool->recalc();

            pForceCalc->calculateForces();
            pForceTool->setForces(pForceCalc->getForceX(),
		                  pForceCalc->getForceY(),
				  pForceCalc->getForceZ());
	    pForceTool->setTorqueForces(pForceCalc->getTorqueForceX(),
		                        pForceCalc->getTorqueForceY(),
					pForceCalc->getTorqueForceZ());
        }

        pForceTool->unlockDevice();

	iTime2 = svt_getToD();

	// Sleep if using mouse since we only need 30Hz refresh
	if (bMouseMode && (iTime2 - iTime1 < 30))
	{
	    svt_sleep( 30 - (iTime2-iTime1) );
	    iTime1 = svt_getToD();
	} else
	    iTime1 = iTime2;

	// If we are using a haptic device, but the button isn't pressed,
	// sleep for a while (no need to run the thread at full speed).
	if (!bMouseMode && !pForceTool->isButtonPressed())
	{
            pForceTool->setForces(0.0, 0.0, 0.0);
	    pForceTool->setTorqueForces(0.0, 0.0, 0.0);
	    svt_sleep(30);
	}

    }

    pForceTool->setStopped();

    g_oThreadSema.V();

    return NULL;
};

/**
 * Constructor
 * \param pName the name of the force device
 */
svt_adv_force_tool::svt_adv_force_tool(const char* pName) : svt_force_tool(pName),
    m_pForceCalc( NULL ),
    m_bThreadCreated( false ),
    m_bThreadExit( false ),
    m_bThreadAutoStart( false )
{
    char tmp[256];
    sprintf(tmp,"%sDevice", pName);

    svtout << "Force feedback device created: " << svt_getConfig()->getValue(tmp, "mouse") << endl;

    if (strcmp(this->getDevice()->getName(),"mouse") == 0)
    {
	svtout << "Steric clashing and force calculations will be " << endl;
	svtout << "performed automatically" << endl;
	setThreadAutoStart(true);
    } else {
	svtout << "Steric clashing and force calculations will only be " << endl;
	svtout << "performed when forces are turned on" << endl;
	setThreadAutoStart(false);
    }
}

/** 
 * Button pressed
 */
void svt_adv_force_tool::onButtonPressed()
{
    // Start the thread
    if (m_bThreadAutoStart) initThread();

    // Propagate event to parent classes
    svt_force_tool::onButtonPressed();
}

/**
 * Button released
 */
void svt_adv_force_tool::onButtonReleased()
{
    // Stop the thread
    if (m_bThreadAutoStart) exitThread();

    // Propagate event to parent classes
    svt_force_tool::onButtonReleased();
}

/**
 * Init thread
 */
void svt_adv_force_tool::initThread()
{
    if (!threadCreated())
    {
	g_oThreadSema.P();
        m_bThreadExit = false;
        svt_createThread(&calcForceThread, (void*)this, SVT_THREAD_PRIORITY_NORMAL);
        m_bThreadCreated = true;
	g_oThreadSema.V();
    }
};

/**
 * Stop thread
 */
void svt_adv_force_tool::exitThread()
{
    if (threadCreated() && !m_bThreadExit)
    {
        m_bThreadExit = true;
        g_oThreadSema.P();
        g_oThreadSema.V();

	// Spinlock to make sure the thread is dead
	while (m_bThreadExit || m_bThreadCreated) ; // Wait...
    }
};

/**
 * Shall I stop thread
 * \return true if thread should be stopped...
 */
bool svt_adv_force_tool::shallExitThread()
{
    return m_bThreadExit;
}

/**
 * OK, thread was stopped
 */
void svt_adv_force_tool::setStopped()
{
    m_bThreadExit = false;
    m_bThreadCreated = false;
};

/**
 * Thread created
 */
bool svt_adv_force_tool::threadCreated()
{
    bool bCreated = m_bThreadCreated;

    return bCreated;
};

/**
 * set the svt_adc_force_calc object
 * \param pForceCalc pointer to the force calculation object
 */
void svt_adv_force_tool::setForceCalc(svt_adv_force_calc* pForceCalc)
{
    lockDevice();

    m_pForceCalc = pForceCalc;

    unlockDevice();
};
/**
 * get the svt_adc_force_calc object
 * \return pointer to the force calculation object
 */
svt_adv_force_calc* svt_adv_force_tool::getForceCalc()
{
    return m_pForceCalc;
};

/**
 * Set thread autostarting flag
 * \param bAutostart (true for automatic thread start/stop)
 */
void svt_adv_force_tool::setThreadAutoStart(bool bAutoStart)
{
    m_bThreadAutoStart = bAutoStart;
}

/**
 * Get thread autostarting flag
 * \return true for automatic thread start/stop
 */
bool svt_adv_force_tool::setThreadAutoStart()
{
    return m_bThreadAutoStart;
}
