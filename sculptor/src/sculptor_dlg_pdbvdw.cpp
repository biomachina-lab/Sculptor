/***************************************************************************
                          sculptor_dlg_pdbvdw.cpp
                          -----------------
    begin                : 12.11.2010
    author               : Manuel Wahle
                         : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_pdbvdw.h>
#include <pdb_document.h>
// svt includes
#include <svt_pdb.h>

/**
 *
 */
dlg_pdbvdw::dlg_pdbvdw(QWidget* pParent, svt_pdb * pPdbNode, pdb_document * pPdbDocument)
    : QDialog(pParent),
      m_pPdbDocument(pPdbDocument),
      m_pPdbNode(pPdbNode)
{
    setupUi(this);

    m_pComboBoxRenderingMode->addItem("Standard",     QVariant(SVT_PDB_VDW_NO_SHADER));
    m_pComboBoxRenderingMode->addItem("High Quality", QVariant(SVT_PDB_VDW_GLOSSY));

    updateUseShader();
    updateSphereScaling();

    connect( m_pComboBoxRenderingMode,   SIGNAL(activated(int)),         this, SLOT(sComboboxRenderingMode(int)) );
    connect( m_pSphereScale,             SIGNAL(editingFinished()),      this, SLOT(sSetSphereScale()) );
    connect( m_pSphereScale,             SIGNAL(valueChanged(double)),   this, SLOT(sSetSphereScale()) );
    connect( m_pOK_Button,               SIGNAL(clicked()),              this, SLOT(hide()) );
}

/**
 *
 */
dlg_pdbvdw::~dlg_pdbvdw()
{
}

/**
 *
 */
void dlg_pdbvdw::updateSphereScaling()
{
    m_pSphereScale->blockSignals(true);
    m_pSphereScale->setValue( (double)(m_pPdbDocument->getSphereScale()) );
    m_pSphereScale->blockSignals(false);
}

/**
 *
 */
void dlg_pdbvdw::sComboboxRenderingMode(int iIndex)
{
    m_pPdbNode->setVDWRenderingMode( m_pComboBoxRenderingMode->itemData(iIndex).toInt() );
}

/**
 *
 */
void dlg_pdbvdw::sSetSphereScale()
{
    m_pPdbDocument->setSphereScale( (Real32)m_pSphereScale->value() );
    m_pPdbNode->rebuildDL();
}

/**
 *
 */
void dlg_pdbvdw::updateUseShader()
{
    m_pComboBoxRenderingMode->blockSignals(TRUE);

    if ( svt_getAllowShaderPrograms() )
    {
        // enable combobox
        m_pComboBoxRenderingMode->setEnabled(TRUE);
        // select appropriate mode
        if (m_pPdbNode->getVDWRenderingMode() == SVT_PDB_VDW_NO_SHADER)
            m_pComboBoxRenderingMode->setCurrentIndex(0);
        else
            m_pComboBoxRenderingMode->setCurrentIndex(1);
    }
    else
    {
        // is shader progs not allowed, select standard mode and diable combobox
        m_pComboBoxRenderingMode->setCurrentIndex(0);
        m_pComboBoxRenderingMode->setEnabled(FALSE);
    }
    m_pComboBoxRenderingMode->blockSignals(FALSE);
}
