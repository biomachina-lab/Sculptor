/***************************************************************************
                          sculptor_dlg_colmapsecstruct.cpp
                          -----------------
    begin                : 09.11.2010
    author               : Manuel Wahle
			   Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_colmapsecstruct.h>
#include <sculptor_dlg_cartoon.h>
#include <pdb_document.h>
// svt includes
#include <svt_pdb.h>


/**
 *
 */
dlg_colmapsecstruct::dlg_colmapsecstruct(QWidget* pParent, pdb_document * pPdbDocument)
    : QDialog(pParent),
    m_pPdbDocument(pPdbDocument),
    m_pDlgCartoon(NULL),
    m_pPdbNode((svt_pdb*)pPdbDocument->getNode())

{
    setupUi(this);
    //update spinboxes
    updateMinRes();
    //connect spinbox value changes to changes in cartoon dialog and svt_pdb, svt_atom_structure_colmap
    connect(spinBoxHelixRes, 	  SIGNAL(valueChanged(int)), this, SLOT(setMinResPerHelix()));
    connect(spinBoxSheetRes, 	  SIGNAL(valueChanged(int)), this, SLOT(setMinResPerSheet()));
    //connect change color button to method in pdb document that changes the color
    connect(m_pButtonColorChange, SIGNAL(clicked()),  	     this, SLOT(sExtractComboItem()));


}

/**
 *
 */
dlg_colmapsecstruct::~dlg_colmapsecstruct()
{
}

/**
 *
 */
void dlg_colmapsecstruct::setMinResPerSheet()
{
    // the min residues per sheet parameter has to be set twice:
    // once in svt_pdb, where the geometry is computed, and
    // once in svt_atom_structure_colmap, where the color for the geometry is computed
    m_pPdbNode->setCartoonParam(CARTOON_SHEET_MIN_RESIDUES, (float)spinBoxSheetRes->value());
    m_pPdbDocument->getAtomStructureColmap()->setMinResiduesPerSheet((float)spinBoxSheetRes->value());

    if (m_pDlgCartoon != NULL)
	m_pDlgCartoon->updateMinRes();

}

/**
 *
 */
void dlg_colmapsecstruct::setMinResPerHelix()
{
    // the min residues per helix parameter has to be set twice:
    // once in svt_pdb, where the geometry is computed, and
    // once in svt_atom_structure_colmap, where the color for the geometry is computed
    m_pPdbNode->setCartoonParam(CARTOON_HELIX_MIN_RESIDUES, (float)spinBoxHelixRes->value());
    m_pPdbDocument->getAtomStructureColmap()->setMinResiduesPerHelix((float)spinBoxHelixRes->value());

    if (m_pDlgCartoon != NULL)
	m_pDlgCartoon->updateMinRes();

}

/**
 *
 */
void dlg_colmapsecstruct::hide()
{
    QWidget::hide();
}

/**
 *
 */
void dlg_colmapsecstruct::updateMinRes()
{
    //block signals to prevent any slots unnecessarily being called
    spinBoxSheetRes->blockSignals(true);
    spinBoxSheetRes->setValue((int) m_pPdbNode->getCartoonParam(CARTOON_SHEET_MIN_RESIDUES));
    spinBoxSheetRes->blockSignals(false);

    spinBoxHelixRes->blockSignals(true);
    spinBoxHelixRes->setValue((int) m_pPdbNode->getCartoonParam(CARTOON_HELIX_MIN_RESIDUES));
    spinBoxHelixRes->blockSignals(false);
}

/**
 *
 */
void dlg_colmapsecstruct::setCartoonPropDlg(dlg_cartoon* pDlgCartoon)
{
    m_pDlgCartoon = pDlgCartoon;
}

/**
 *
 */
void dlg_colmapsecstruct::setLabelName(QString oName)
{
    m_pStaticColorName->setText(oName);
}

/**
 *
 */
void dlg_colmapsecstruct::addColorNameItem(int iItem, QString oChar)
{
    m_pComboColorName->insertItem(iItem, oChar);
}

/**
 *
 */
void dlg_colmapsecstruct::sExtractComboItem()
{
    m_pPdbDocument->buttonChangeColor(m_pComboColorName->currentIndex());
}


