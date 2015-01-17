/***************************************************************************
                          sculptor_dlg_wait.cpp
                          -----------------
    begin                : 03.11.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
/////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_wait.h>

/**
 * 
 */
dlg_wait::dlg_wait(unsigned int iSeconds, QWidget * parent)
    : QDialog(parent),
    m_oTimer(this),
    m_iSeconds(iSeconds)
{
    setupUi(this);
    //have this occur only once.  indicating false would mean to keep recurring every interval number of seconds.
    m_oTimer.setSingleShot(true);
    connect(&m_oTimer, SIGNAL(timeout()), this, SLOT(hide()));
    startTimer();
};

/**
 *
 */
dlg_wait::~dlg_wait()
{
};

/**
 *
 */
void dlg_wait::setTimer(unsigned int iSeconds)
{
    m_iSeconds = iSeconds;
    startTimer();
}

void dlg_wait::startTimer()
{
    m_oTimer.start( m_iSeconds * 1000);
}
