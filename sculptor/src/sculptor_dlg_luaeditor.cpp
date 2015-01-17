/***************************************************************************
                          sculptor_dlg_luaeditor.cpp
			  -------------------
    begin                : Nov 11, 2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_luaeditor.h>
#include <sculptor_window.h>

/*
 * Constructor
 */
dlg_luaeditor::dlg_luaeditor(QWidget* pParent)
    : QWidget( pParent )
{
    setupUi(this);
    m_pEditor->setTabStopWidth(17);
    m_pEditor->setMinimumSize(QSize(100,40));
}

/*
 * Destructor
 */
dlg_luaeditor::~dlg_luaeditor()
{
}

/*
 *
 */
void dlg_luaeditor::initConnections(sculptor_window* pWindow)
{
    connect( m_pRunButton,     SIGNAL(clicked()),                  pWindow, SLOT(scriptRun()) );
    connect( m_pStopButton,    SIGNAL(clicked()),                  pWindow, SLOT(scriptStop()) );
    connect( m_pNewScript,     SIGNAL(clicked()),                  pWindow, SLOT(scriptAdded()) );
    connect( m_pScriptList,    SIGNAL(activated(int)),             pWindow, SLOT(scriptSelected(int)) );
    connect( m_pScriptName,    SIGNAL(textEdited(const QString&)), pWindow, SLOT(scriptChangeName()), Qt::DirectConnection );
    connect( m_pScriptName,    SIGNAL(editingFinished()),          pWindow, SLOT(scriptChangeName()), Qt::DirectConnection );
    connect( m_pRemoveScript,  SIGNAL(clicked()),                  pWindow, SLOT(scriptRemoved()) );

    installEventFilter( pWindow );
}

/*
 *
 */
void dlg_luaeditor::addScriptName(QString oScriptName)
{
    int iIndex = m_pScriptList->count();
    m_pScriptList->addItem( oScriptName, QVariant(iIndex) );
}

/*
 *
 */
void dlg_luaeditor::removeScriptName(int iIndex)
{
    m_pScriptList->removeItem(iIndex);
}

/*
 *
 */
void dlg_luaeditor::setCurrentScript(int iIndex, QString oScript, QString oName)
{
    if (oName.isEmpty())
    {
        m_pScriptName->setText(m_pScriptList->itemText(iIndex));
    }
    else
    {
        m_pScriptName->setText(oName);
        m_pScriptList->setItemText(iIndex, oName);
    }

    m_pScriptList->setCurrentIndex(iIndex);
    m_pEditor->setText(oScript);
}

/*
 *
 */
void dlg_luaeditor::updateScriptListName(int iIndex)
{
    m_pScriptList->setItemText(iIndex, m_pScriptName->text());
}

/*
 *
 */
void dlg_luaeditor::showScriptRunning(bool bRunning)
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
QString dlg_luaeditor::getScriptText()
{
    return m_pEditor->toPlainText();
}

/*
 *
 */
void dlg_luaeditor::setScriptName(int iIndex, QString oName)
{
    m_pScriptName->setText(oName);
    updateScriptListName(iIndex);
}

/*
 *
 */
QString dlg_luaeditor::getScriptName()
{
    return m_pScriptName->text();
}

/*
 *
 */
QTextEdit* dlg_luaeditor::getEditor()
{
    return m_pEditor;
}

/**
 *
 */
void dlg_luaeditor::scriptDown(int iIndex, QString oScript)
{
    QString oStr = m_pScriptList->itemText( iIndex+1 );
    m_pScriptList->blockSignals(TRUE);
    m_pScriptList->setItemText( iIndex+1, m_pScriptList->itemText(iIndex) );
    m_pScriptList->setItemText( iIndex, oStr );
    m_pScriptList->setCurrentIndex(iIndex+1);
    m_pScriptList->blockSignals(FALSE);

    m_pEditor->setText(oScript);

}

/**
 *
 */
void dlg_luaeditor::scriptUp(int iIndex, QString oScript)
{
    QString oStr = m_pScriptList->itemText(iIndex-1);

    m_pScriptList->setItemText( iIndex-1, m_pScriptList->itemText(iIndex) );
    m_pScriptList->setItemText( iIndex, oStr );
    m_pScriptList->setCurrentIndex(iIndex-1);

    m_pEditor->setText(oScript);
}
