/***************************************************************************
                          svt_qtMessages -  description
                             -------------------
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SVT_QT_MESSAGES_H
#define SVT_QT_MESSAGES_H

// svt includes
#include <svt_basics.h>
#include <svt_messages.h>
// qt4 includes
#include <QObject>
class QMainWindow;
class QProgressBar;
class QLabel;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////
// class svt_qtMessages
///////////////////////////////////////////////////////////////////////////////
 
/**
 * This is an alternative implementation of the svt_messages class, based on the qt library
 * Class was ported to qt4 by Zbigniew Starosolski
 *
  \author Stefan Birmanns
*/
class DLLEXPORT svt_qtMessages : public QObject, public svt_messages
{
    Q_OBJECT

        QMainWindow* m_pMainWindow;
public:

    /**
     * Class Constructor.
     */
    svt_qtMessages(QMainWindow*);

    /**
     * Class Destructor.
     */
    virtual ~svt_qtMessages()
    {
    };

    /**
     * Displays an info message.
     */
    virtual void info(const char* pszMessage);

    /**
     * Displays an error
     */
    virtual void error(const char* pszFailure);
 
    /**
     * Pops up a Progress Dialog (percentage version).
     * Display a progress bar. The current progress status can be updated
     * with progress(int iPercent).
     * \param pszMessage A progress message that will appear in the dialog.
     * \param iPercent   The initial progress status in percent.
     */
    virtual void progressPopup(const char* pszMessage, int iPercent=0);

    /**
     * updates current process state by given value (percentage version).
     * Automatically pops down the dialog if iPercent >= 100, or if the user cancels action.
     * \param iPercent The current progress sate in percent.
     * \throw svt_userInterrupt if user cancels action.
     */
    virtual void progress(int iPercent);

    /**
     * Pops up a Progress Dialog ("m of n" version).
     * Display a progress bar. The current progress status can be updated with
     * progress(int iPercent).
     * \param pszMessage A progress message that will appear in the dialog.
     * \param iDone      The initial progress status (amount of steps already performed, usually 0).
     * \param iToDo      The overall amount of steps to do.
     */
    virtual void progressPopup(const char* pszMessage, int iDone, int iToDo);


    /**
     * Pops down the progress dialog.
     * Automatically performed if iPercent >= 100 or iDone >= iToDo within
     * progress() methods, or if the user cancels action. Therefore,
     * it is usually not necessary to call this method manually.
     */
    virtual void progressPopdown();

    /**
     * updates current process state by given value ("m of n" version).
     * Automatically pops down the dialog if iDone >= iToDo, or if the user cancels action.
     * \param iDone Amount of steps already performed.
     * \param iToDo The overall amount of steps to do.
     * \throw svt_userInterrupt if user cancels action.
     * \note The parameter iToDo should be kept constant for all calls of this method
     * between a progressPopup and progressPopdown.
     */
    virtual void progress(int iDone, int iToDo);

    /** Simple question dialog.
     * To be used to ask the user a simple question, that can be answered with "Yes" or "No".
     * \param pszQuestion:  The question string.
     * \param bAllowCancel: Turn on/off cancel possibility <BR>
     * (in terminal version: <ESC>-key, adds a "Cancel" button in gui versions).
     * \param pszYes:       The string to be used for answer no. 1. <BR>
     * Set this parameter if you want to overwrite the default string "Yes"
     * \param pszNo:        The string to be used for answer no. 2 <BR>
     * Set this parameter if you want to overwrite the default string "No"
     * \param bYesDefault   if true, answer1 is default, 2 otherwise
     * \return true  - if the question was answered with "Yes". <BR>
     * false - if the question was answered with "No".
     * \throw svt_userInterrupt, if the user cancels action.
     */
    virtual bool question(const char* pszQuestion, bool bAllowCancel=false,
                          const char *pszYes="Yes", const char *pszNo="No",
                          bool bYesDefault=true );

    /**
     * text input dialog
     * \param pszMessage A message that will appear before the text input starts
     * \param pszDefault Default answer, if user presses return
     */
    virtual string textInput(const char* pszMessage, const char* pszDefault);

    /// Pops up a busy message.
    virtual void busyPopup(const char* pszMessage);

    /** Let a bar rotate within the busy dialog to indicate that the program
     is still alive. <BR>
     This methos lets a bar rotate one position only, so it needs to be called
     repeatative.
     \param iOmitThisParameter This parameter is for internal usage only and should
     therefore be omitted.
     \throw svt_userInterrupt, if the user cancels action. In this case, the dialog
     pops down automatically.
     */
    virtual void busyRotate(int iOmitThisParameter=-1);

    /// Pops down the busy dialog.
    virtual void busyPopdown();

public slots:

    /**
     * Qt slot that handles the cancel button signal
     */
    void cancelPressed();

protected:

    bool m_bCancelHidden;
    QProgressBar* m_pProgress;
    QLabel* m_pProgressLabel;
    QToolButton* m_pCancelButton;
    QLabel* m_pBusyLabel;
};

#endif
