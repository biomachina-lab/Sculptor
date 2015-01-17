/***************************************************************************
                          sculptor_dlg_transform
                          ------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


// sculptor include
#include <sculptor_dlg_transform.h>


dlg_transform::dlg_transform(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect( m_pDialTransY,    SIGNAL(valueChanged(int)), m_pSpinBoxTransY, SLOT(setValue(int)) );
    connect( m_pDialTransX,    SIGNAL(valueChanged(int)), m_pSpinBoxTransX, SLOT(setValue(int)) );
    connect( m_pDialTransZ,    SIGNAL(valueChanged(int)), m_pSpinBoxTransZ, SLOT(setValue(int)) );
    connect( m_pDialRotX,      SIGNAL(valueChanged(int)), m_pSpinBoxRotX,   SLOT(setValue(int)) );
    connect( m_pDialRotY,      SIGNAL(valueChanged(int)), m_pSpinBoxRotY,   SLOT(setValue(int)) );
    connect( m_pDialRotZ,      SIGNAL(valueChanged(int)), m_pSpinBoxRotZ,   SLOT(setValue(int)) );
    connect( m_pSpinBoxTransX, SIGNAL(valueChanged(int)), m_pDialTransX,    SLOT(setValue(int)) );
    connect( m_pSpinBoxTransY, SIGNAL(valueChanged(int)), m_pDialTransY,    SLOT(setValue(int)) );
    connect( m_pSpinBoxTransZ, SIGNAL(valueChanged(int)), m_pDialTransZ,    SLOT(setValue(int)) );
    connect( m_pSpinBoxRotX,   SIGNAL(valueChanged(int)), m_pDialRotX,      SLOT(setValue(int)) );
    connect( m_pSpinBoxRotY,   SIGNAL(valueChanged(int)), m_pDialRotY,      SLOT(setValue(int)) );
    connect( m_pSpinBoxRotZ,   SIGNAL(valueChanged(int)), m_pDialRotZ,      SLOT(setValue(int)) );
    // connect( m_pSpinBoxTransX, SIGNAL(editingFinished()), m_pDialTransX,    SLOT(setValue(int)) );
    // connect( m_pSpinBoxTransY, SIGNAL(editingFinished()), m_pDialTransY,    SLOT(setValue(int)) );
    // connect( m_pSpinBoxTransZ, SIGNAL(editingFinished()), m_pDialTransZ,    SLOT(setValue(int)) );
    // connect( m_pSpinBoxRotX,   SIGNAL(editingFinished()), m_pDialRotX,      SLOT(setValue(int)) );
    // connect( m_pSpinBoxRotY,   SIGNAL(editingFinished()), m_pDialRotY,      SLOT(setValue(int)) );
    // connect( m_pSpinBoxRotZ,   SIGNAL(editingFinished()), m_pDialRotZ,      SLOT(setValue(int)) );
    connect( m_pButtonClose,   SIGNAL(clicked()),         this,              SLOT(accept()) );
}
