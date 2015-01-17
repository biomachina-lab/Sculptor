#ifndef __SCULPTOR_WAIT_DLG
#define __SCULPTOR_WAIT_DLG

#include <waitdlg.h>
#include <qtimer.h>

/**
 * This is a vey simple information dialog that shows a warning message that the current operation might take a long time to finish. The dialog will close automatically
 * after 20 seconds.
 */
class sculptor_waitdlg : public WaitDlg
{
    Q_OBJECT

protected:

    QTimer m_oTimer;

public:

    /**
     * Constructor
     * \param seconds number of seconds this dialog should stay open
     * \param parent parent widget
     * \param name name of the dialog
     */
    sculptor_waitdlg( unsigned int iSeconds, QWidget * parent = 0, const char * name = 0, bool modal = FALSE, Qt::WFlags f = 0 );
    virtual ~sculptor_waitdlg();
};

#endif
