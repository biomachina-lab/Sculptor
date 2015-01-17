/***************************************************************************
                          sculptor_dlg_wait.h
                          -----------------
    begin                : 03.11.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SCULPTOR_DLG_WAIT_H
#define SCULPTOR_DLG_WAIT_H

// uic-qt4 includes
#include <ui_dlg_wait.h>
// qt includes
#include <qtimer.h>

/**
 * This is a vey simple information dialog that shows a warning message that the current operation might take a long time to finish. The dialog will hide automatically after the specified time has expired.
 */
class dlg_wait : public QDialog, private Ui::dlg_wait
{
    Q_OBJECT

protected:
    //qtimer used to keep track of time
    QTimer m_oTimer;
    //number of seconds
    unsigned int m_iSeconds;

public:

    /**
     * Constructor
     * \param iSeconds unsigned int number of seconds this dialog should stay open
     * \param parent pointer to parent widget
     */
    dlg_wait(unsigned int iSeconds, QWidget * parent = 0);

    /**
     * Destructor
     */ 
    virtual ~dlg_wait();

    /**
     * Resets the amount of time
     * \param iSeconds unsigned int number of seconds
     */
    void setTimer(unsigned int iSeconds); 

    /**
     * Starts the timer 
     */
    void startTimer();
};

#endif
