/***************************************************************************
                          sculptor_dlg_colmap.cpp
                          -----------------
    begin                : 05.11.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////


// sculptor includes
#include <sculptor_dlg_colmap.h>
#include <pdb_document.h>

/**
 *
 */
dlg_colmap::dlg_colmap(QWidget* pParent, pdb_document* pPdbDoc)
    : QDialog(pParent),
    m_pPdbDoc(pPdbDoc)
{
    setupUi(this);
    //connect Change Color button to method to change color in pdb document
    connect(m_pButtonColorChange, SIGNAL(clicked()), this, SLOT(sExtractColmapSetting()));
}

/**
 *
 */
dlg_colmap::~dlg_colmap()
{
}

/**
 *
 */
void dlg_colmap::resetDlg()
{
    //clear label and combo box
    m_pStaticColorName->clear();
    m_pComboColorName->clear();
}

/**
 *
 */
void dlg_colmap::setLabelName(QString oLabelName)
{
    m_pStaticColorName->setText(oLabelName);
}

/**
 *
 */
void dlg_colmap::addColorNameItem(int iIndex, QString oName)
{
    m_pComboColorName->insertItem(iIndex, oName);
}

/**
 *
 */
void dlg_colmap::sExtractColmapSetting()
{
    //call buttonChangeColor method with the combobox item selected
    m_pPdbDoc->buttonChangeColor(m_pComboColorName->currentIndex());
}
