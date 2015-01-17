/***************************************************************************
                          svt_qtMessages -  description
                             -------------------
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

//svt includes
#include <svt_qtMessages.h>
#include <svt_exception.h>
#include <svt_window.h>
#include <svt_time.h>
// qt4 include
#include <QMessageBox>
#include <QInputDialog>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QMainWindow>

extern void updateQt();
extern string QtoStd(QString);

bool g_bException = false;

/**
 * Class Constructor.
 */
svt_qtMessages::svt_qtMessages(QMainWindow* pMainWindow)
    : QObject(),
      svt_messages(),
      m_pMainWindow( pMainWindow )
{
    m_pProgressLabel = new QLabel( pMainWindow->statusBar() );
    m_pProgressLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    pMainWindow->statusBar()->addWidget( m_pProgressLabel, 2 );
    m_pProgress = new QProgressBar( pMainWindow->statusBar() );
    pMainWindow->statusBar()->addWidget( m_pProgress, 2 );

    m_pCancelButton = new QToolButton( pMainWindow->statusBar() );
    m_pCancelButton->setText("Cancel");
    m_pCancelButton->setToolButtonStyle( Qt::ToolButtonTextOnly );
    m_pCancelButton->setSizePolicy( QSizePolicy::Fixed , QSizePolicy::Fixed );

    m_pBusyLabel = new QLabel( pMainWindow->statusBar() );
    m_pBusyLabel->setText("|");
    m_pBusyLabel->setSizePolicy( QSizePolicy::Preferred , QSizePolicy::Preferred );
    m_pBusyLabel->setFixedWidth( 30 );
    m_pBusyLabel->setAlignment( Qt::AlignCenter );
    m_pBusyLabel->setFrameStyle( QFrame::Panel | QFrame::Raised );
    QFont oFont = m_pBusyLabel->font();
    oFont.setBold( TRUE );
    m_pBusyLabel->setFont( oFont );
    m_bCancelHidden = true;

    pMainWindow->statusBar()->addWidget( m_pCancelButton, 1 );
    pMainWindow->statusBar()->addWidget( m_pBusyLabel, 1 );

    m_pCancelButton->hide();
    m_pBusyLabel->hide();
    m_pProgress->hide();
    m_pProgressLabel->hide();
};

/**
 * Displays an info message.
 */
void svt_qtMessages::info(const char* pszMessage)
{
    QMessageBox::information(m_pMainWindow, QString("Information"), QString(pszMessage));
};

/**
 * Displays an error
 */
void svt_qtMessages::error(const char* pszFailure)
{
    QMessageBox::warning(m_pMainWindow, QString("Error"), QString(pszFailure));
};

/**
 * Pops up a Progress Dialog (percentage version).
 * Display a progress bar. The current progress status can be updated
 * with progress(int iPercent).
 * \param pszMessage A progress message that will appear in the dialog.
 * \param iPercent   The initial progress status in percent.
 */
void svt_qtMessages::progressPopup(const char* pszMessage, int iPercent)
{
    m_bCancelHidden = false;
    m_pCancelButton->show();
    m_pProgress->show();
    m_pProgressLabel->show();

    m_pProgressLabel->setText(pszMessage);
    m_pProgress->setValue( iPercent );
    m_pProgress->setRange( 0, 100 );
    m_pProgress->setTextVisible( TRUE );

    connect( m_pCancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    svt_forceUpdateGUIWindows();
};

/**
 * Qt slot that handles the cancel button signal
 */
void svt_qtMessages::cancelPressed()
{
    m_bCancelHidden = true;
    m_pCancelButton->hide();
    m_pProgress->hide();
    m_pProgressLabel->hide();
    m_pBusyLabel->hide();

    disconnect( m_pCancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    // we cannot throw an exception here anymore, as this function might be called from a different thread (gui thread). So, now we only set a variable and check
    // again in a progress or busyrotate function that is definitely called from the correct thread...
    //throw svt_userInterrupt();
    g_bException = true;
}

/**
 * updates current process state by given value (percentage version).
 * Automatically pops down the dialog if iPercent >= 100, or if the user cancels action.
 * \param iPercent The current progress sate in percent.
 * \throw svt_userInterrupt if user cancels action.
 */
void svt_qtMessages::progress(int iPercent)
{
    // did an exception happen in the meantime?
    if (g_bException)
    {
        g_bException = false;
        throw svt_userInterrupt();
    }

    svt_forceUpdateGUIWindows();
    m_pProgress->setValue( iPercent );
    svt_forceUpdateGUIWindows();
};

/**
 * Pops up a Progress Dialog ("m of n" version).
 * Display a progress bar. The current progress status can be updated with
 * progress(int iPercent).
 * \param pszMessage A progress message that will appear in the dialog.
 * \param iDone      The initial progress status (amount of steps already performed, usually 0).
 * \param iToDo      The overall amount of steps to do.
 */
void svt_qtMessages::progressPopup(const char* pszMessage, int iDone, int iToDo)
{
    m_bCancelHidden = false;
    m_pCancelButton->show();
    m_pProgress->show();
    m_pProgressLabel->show();

    m_pProgressLabel->setText(pszMessage);
    m_pProgress->setValue(iDone);
    m_pProgress->setRange(0, iToDo );
    m_pProgress->setTextVisible( TRUE );

    connect( m_pCancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    svt_forceUpdateGUIWindows();
};

/**
 * Pops down the progress dialog.
 * Automatically performed if iPercent >= 100 or iDone >= iToDo within
 * progress() methods, or if the user cancels action. Therefore,
 * it is usually not necessary to call this method manually.
 */
void svt_qtMessages::progressPopdown()
{
    m_bCancelHidden = true;
    m_pCancelButton->hide();
    m_pProgress->hide();
    m_pProgressLabel->hide();
    m_pBusyLabel->hide();
    svt_forceUpdateGUIWindows();

};

/**
 * updates current process state by given value ("m of n" version).
 * Automatically pops down the dialog if iDone >= iToDo, or if the user cancels action.
 * \param iDone Amount of steps already performed.
 * \param iToDo The overall amount of steps to do.
 * \throw svt_userInterrupt if user cancels action.
 * \note The parameter iToDo should be kept constant for all calls of this method
 * between a progressPopup and progressPopdown.
 */
void svt_qtMessages::progress(int iDone, int)
{
    // did an exception happen in the meantime?
    if (g_bException)
    {
        g_bException = false;
        throw svt_userInterrupt();
    }

    svt_forceUpdateGUIWindows();
    m_pProgress->setValue(iDone);
    svt_forceUpdateGUIWindows();
};

/**
 * Ask user a simple question, to be answered with "Yes" (->true) or "No"(->false)
 *
 * Parameters:Was
 * \param pszQuestion the question as string
 * \param bAllowCancel turn on/off cancel possibility (in text version: ESC) if cancelled, svt_userInterrupt is thrown - NOT used in qt version
 * \param pszYes string for answer no. 1 (for which true is returned)
 * \param pszNo string for answer no. 2 (for which false is returned)
 * \param bYesDefault if true, answer1 is default, 2 otherwise - NOT used in qt version
 * \return "Yes" (->true) or "No"(->false)
 */
bool svt_qtMessages::question(const char* pszQuestion, bool,
                              const char */*pszYes*/, const char */*pszNo*/,
                              bool)
{
    if ( QMessageBox::question(m_pMainWindow, QString("Question"), QString(pszQuestion), QMessageBox::Yes, QMessageBox::No) 
                             == QMessageBox::Yes )
        return true;
    else
        return false;
}

/**
 * text input dialog
 * \param pszMessage A message that will appear before the text input starts
 * \param pszDefault Default answer, if user presses return
 */
string svt_qtMessages::textInput(const char* pszMessage, const char* pszDefault)
{
    string oAns;
    bool bOk = false;

    QString text = QInputDialog::getText(m_pMainWindow, "Question", pszMessage, QLineEdit::Normal, QString::null, &bOk );
    if ( bOk && !text.isEmpty() ) {

        oAns = QtoStd(text);

    } else {
        oAns = string(pszDefault);
    }

    return oAns;
};

/// Pops up a busy message.
void svt_qtMessages::busyPopup(const char* pszMessage)
{
    m_pBusyLabel->show();
    if (m_bCancelHidden)
    {
    	m_pCancelButton->show();
        connect( m_pCancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));
        m_pProgressLabel->show();
        m_pProgressLabel->setText(pszMessage);
        m_pProgress->show();
        m_pProgress->setValue( 0 );
        m_pProgress->setMaximum( 100 );
        m_pProgress->setTextVisible( FALSE );
    }

    svt_forceUpdateGUIWindows();
};

/** Let a bar rotate within the busy dialog to indicate that the program
 is still alive. <BR>
 This methos lets a bar rotate one position only, so it needs to be called
 repeatative.
 \param iOmitThisParameter This parameter is for internal usage only and should
 therefore be omitted.
 \throw svt_userInterrupt, if the user cancels action. In this case, the dialog
 pops down automatically.
 */
void svt_qtMessages::busyRotate(int)
{
    static int iDone = 1;
    static long iTime = svt_getToD();

    // did an exception happen in the meantime?
    if (g_bException)
    {
        g_bException = false;
        throw svt_userInterrupt();
    }

    if (svt_getToD() - iTime > 1000)
    {
        iTime = svt_getToD();

        if (iDone == 1)
            m_pBusyLabel->setText("|");
        if (iDone == 2)
            m_pBusyLabel->setText("/");
        if (iDone == 3)
            m_pBusyLabel->setText("-");
        if (iDone == 4)
            m_pBusyLabel->setText("\\");

        iDone++;
        if (iDone > 4)
            iDone = 1;

        svt_forceUpdateGUIWindows();
    }
};

/// Pops down the busy dialog.
void svt_qtMessages::busyPopdown()
{
    // only hide the busy label, if the rest of the progress widgets are not shown. Otherwise, the busy label will be hidden in the progressPopdown function...
    if (m_bCancelHidden)
    {
        m_pBusyLabel->hide();
        m_pCancelButton->hide();
        m_pProgressLabel->hide();
        m_pProgress->hide();
        m_pProgress->setTextVisible( TRUE );
    }

    svt_forceUpdateGUIWindows();
};
