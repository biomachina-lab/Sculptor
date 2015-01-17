/***************************************************************************
                          sculptor_dlg_atomclustering.cpp
                          -----------------
    begin                : 04.11.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_atomclustering.h>
#include <pdb_document.h>

/**
 *
 */
dlg_atomclustering::dlg_atomclustering(QWidget* pParent, pdb_document* pPdbDoc)
    : QDialog(pParent),
    m_pPdbDoc(pPdbDoc)
{
    setupUi(this);
    //when user hits ok button, get the integer parameter and send to atom clustering method
    connect(m_pCompute, SIGNAL(clicked()), this, SLOT(getParameterValue()));
}

/**
 *
 */
dlg_atomclustering::~dlg_atomclustering()
{
}

/**
 *
 */
void dlg_atomclustering::resetDlg(pdb_document* pPdbDoc)
{
    m_pSkippedCA->setValue(0);
    m_pPdbDoc = pPdbDoc;
}

/**
 *
 */
void dlg_atomclustering::getParameterValue()
{
    //get the value
    unsigned int iSkipCount = m_pSkippedCA->text().toInt();
    //sent to method
    m_pPdbDoc->atomicQuantization(iSkipCount);
}

