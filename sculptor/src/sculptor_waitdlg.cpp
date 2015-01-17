#include <sculptor_waitdlg.h>

/**
 * Constructor
 * \param seconds number of seconds this dialog should stay open
 * \param parent parent widget
 * \param name name of the dialog
 */
sculptor_waitdlg::sculptor_waitdlg( unsigned int iSeconds, QWidget * parent, const char * name, bool modal, Qt::WFlags f ) : WaitDlg( parent, name, modal, f | Qt::WDestructiveClose),
    m_oTimer( this )
{
    connect( &m_oTimer, SIGNAL(timeout()), this, SLOT(accept()) );
    m_oTimer.start( iSeconds * 1000, TRUE );
};

sculptor_waitdlg::~sculptor_waitdlg()
{
};
