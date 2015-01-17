/***************************************************************************
                          sculptor_dlg_preferences.ccp
                          -----------------
    begin                : 12/08/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_preferences.h>
#include <sculptor_dlg_lighting.h>
#include <sculptor_window.h>
#include <sculptor_scene.h>


extern sculptor_window* g_pWindow;

/**
 * Constructor
 */
dlg_preferences::dlg_preferences(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);

    setWindowTitle(QString("Preferences"));


    // fill in current settings
    m_pCheckBoxRestoreWindow->setChecked( g_pWindow->getRestoreWindowStateOnStart() );
    if ( !svt_getAllowShaderPrograms() )
    {
        m_pCheckBoxART->setChecked ( TRUE );
        m_pCheckBoxAO->setChecked ( TRUE );
        m_pCheckBoxAO->setEnabled ( FALSE );
    }
    else if (!svt_getAllowAmbientOcclusion() )
        m_pCheckBoxAO->setChecked ( TRUE );


    // setup the connections
    connect( m_pCheckBoxRestoreWindow,   SIGNAL(toggled(bool)), this, SLOT(sRestoreWindow(bool)) );
    connect( m_pPushButtonRestoreWindow, SIGNAL(clicked()),     this, SLOT(sRestoreWindow()) );
    connect( m_pCheckBoxAO,              SIGNAL(toggled(bool)), this, SLOT(sDisableAO(bool)) );
    connect( m_pCheckBoxART,             SIGNAL(toggled(bool)), this, SLOT(sDisableART(bool)) );
    //connect( m_pCheckBoxAA,              SIGNAL(toggled(bool)), this, SLOT(sDisableAA(bool)) );
    connect( m_pClose,                   SIGNAL(clicked()),     this, SLOT(hide()) );
}

/**
 * Destructor
 */
dlg_preferences::~dlg_preferences()
{
}

/**
 *
 */
void dlg_preferences::sRestoreWindow(bool bRestoreWindow)
{
    g_pWindow->setRestoreWindowStateOnStart(bRestoreWindow);
}

/**
 *
 */
void dlg_preferences::sRestoreWindow()
{
    g_pWindow->restoreDefaultWindowState();
}

/**
 *
 */
void dlg_preferences::sDisableAO(bool bDisabled)
{
    g_pWindow->setAllowAmbientOcclusion( !bDisabled );

    if ( g_pWindow->getLightingDlg() != NULL )
        g_pWindow->getLightingDlg()->setAOGroupBoxEnabled(!bDisabled);
}

/**
 *
 */
void dlg_preferences::sDisableART(bool bDisable)
{
    g_pWindow->setAllowShaderPrograms(!bDisable);

    if (bDisable)
    {
        m_pCheckBoxAO->blockSignals(TRUE);
        m_pCheckBoxAO->setEnabled(FALSE);
        m_pCheckBoxAO->setChecked(TRUE);
        m_pCheckBoxAO->blockSignals(FALSE);
    }
    else
    {
        m_pCheckBoxAO->blockSignals(TRUE);
        m_pCheckBoxAO->setEnabled(TRUE);
        m_pCheckBoxAO->setChecked(!svt_getAllowAmbientOcclusion());
        m_pCheckBoxAO->blockSignals(FALSE);
    }

    svt_redraw();
}

/**
 *
 */
void dlg_preferences::sDisableAA(bool)
{
    printf("dlg_preferences::sDisableAA(bool) to be implemented\n");
}
