/***************************************************************************
                          sculptor_dlg_lualist.cpp
			  -------------------
    begin                : Nov 11, 2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_lualist.h>
#include <sculptor_window.h>
// qt4 includes
#include <QListWidgetItem>
// xpm includes
#include <raise.xpm>
#include <lower.xpm>

extern sculptor_window* g_pWindow;

/*
 * Constructor
 */
dlg_lualist::dlg_lualist(QWidget* pParent)
    : QWidget( pParent )
{
    setupUi(this);

    m_pScriptUp  ->setIcon( QIcon(QPixmap(raiseIcon)) );
    m_pScriptDown->setIcon( QIcon(QPixmap(lowerIcon)) );
}

/*
 * Destructor
 */
dlg_lualist::~dlg_lualist()
{
}

/*
 *
 */
void dlg_lualist::addScriptName(QString oScriptName)
{
    m_pScriptList->addItem(oScriptName);
}

/*
 *
 */
void dlg_lualist::setScriptName(int iIndex, QString oName)
{
    m_pScriptList->item(iIndex)->setText(oName);
}

/*
 *
 */
void dlg_lualist::initConnections(sculptor_window* pWindow)
{
    connect( m_pRunButton,   SIGNAL(clicked()),                     pWindow, SLOT(scriptRun()) );
    connect( m_pStopButton,  SIGNAL(clicked()),                     pWindow, SLOT(scriptStop()) );
    connect( m_pRemoveButton,SIGNAL(clicked()),                     pWindow, SLOT(scriptRemoved()) );
    connect( m_pScriptList,  SIGNAL(itemClicked(QListWidgetItem*)), this,    SLOT(sItemClicked(QListWidgetItem*)) );
    connect( m_pScriptUp,      SIGNAL(clicked()),                  pWindow, SLOT(scriptUp()) );
    connect( m_pScriptDown,    SIGNAL(clicked()),                  pWindow, SLOT(scriptDown()) );
}

/*
 *
 */
void dlg_lualist::removeScriptName(int iIndex)
{
    delete (m_pScriptList->takeItem(iIndex));
}

/*
 *
 */
void dlg_lualist::updateScriptListName(int iIndex, QString oName)
{
    m_pScriptList->blockSignals(TRUE);
    m_pScriptList->item(iIndex)->setText(oName);
    m_pScriptList->blockSignals(FALSE);
}

/*
 *
 */
void dlg_lualist::setCurrentScript(int iIndex)
{
    m_pScriptList->setCurrentRow(iIndex);
}

/*
 *
 */
int dlg_lualist::getCurrentScript()
{
    return m_pScriptList->currentRow();
}

/*
 *
 */
void dlg_lualist::showScriptRunning(bool bRunning)
{
    if (bRunning)
    {
        m_pStopButton->setEnabled( TRUE );
        m_pRunButton ->setEnabled( FALSE );
    }
    else
    {
        m_pStopButton->setEnabled( FALSE );
        m_pRunButton ->setEnabled( TRUE );
    }
}

/*
 *
 */
void dlg_lualist::sItemClicked(QListWidgetItem* pItem)
{
    if (m_pScriptList->row(pItem) > m_pScriptList->count())
        return;

    g_pWindow->scriptSelected(m_pScriptList->row(pItem));
}

/**
 *
 */
void dlg_lualist::scriptDown(int iIndex)
{
    QListWidgetItem* pItem = m_pScriptList->takeItem(iIndex);
    m_pScriptList->insertItem(iIndex+1, pItem);
    setCurrentScript(iIndex+1);
}

/**
 *
 */
void dlg_lualist::scriptUp(int iIndex)
{
    QListWidgetItem* pItem = m_pScriptList->takeItem(iIndex);
    m_pScriptList->insertItem(iIndex-1, pItem);
    setCurrentScript(iIndex-1);
}
