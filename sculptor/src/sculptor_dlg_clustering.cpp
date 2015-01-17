/***************************************************************************
                          sculptor_dlg_clustering.cpp
                          -----------------
    begin                : 29.10.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

//sculptor includes
#include <sculptor_dlg_clustering.h>
#include <sculptor_document.h>
#include <situs_document.h>

/**
 *
 */
dlg_clustering::dlg_clustering(QWidget* pParent, sculptor_document* pSculptorDoc)
    : QDialog( pParent),
    m_pSculptorDoc(pSculptorDoc),
    m_pCV(NULL),
    m_iMethod(NULL)
{
    setupUi(this);
    
    //when user hits ok, obtain all values and call appropriate method
    connect(m_pVQ_Calculate, SIGNAL(clicked()), this, SLOT(extractParameters()));
}

/**
 *
 */
dlg_clustering::~dlg_clustering()
{
}

/**
 *
 */
void dlg_clustering::hideResolutionVoxelOptions()
{
    m_pResolution->hide();
    m_pResolution_Label->hide();
    m_pVoxelWidth->hide();
    m_pVoxelWidth_Label->hide();

}

/**
 *
 */
void dlg_clustering::setupLapLaceOptions()
{
    hideTElements();
    hideCutoffs();
    adjustSizeDlg();
}

/**
 *
 */
void dlg_clustering::setCVStartPos(svt_point_cloud_pdb< svt_vector4<Real64> >* pCV)
{
    setupLapLaceOptions();
    m_pVQ_Conn->hide();
    
    //Start from position indicated by pCV, update codebookSize, keep local copy of pointer to pass to method
    m_pVQ_CodebookSize->setValue( pCV->size() );
    m_pVQ_CodebookSize->setEnabled(false);
    m_pCV = pCV;
}

/**
 *
 */
void dlg_clustering::hideTElements()
{
    m_pVQ_Ti->hide();
    m_pVQ_Tf->hide();
    m_pVQ_Ti_Label->hide();
    m_pVQ_Tf_Label->hide();
}

/**
 *
 */
void dlg_clustering::hideCutoffs()
{
    m_pVQ_Cutoff->hide();
    m_pVQ_Cutoff_Label->hide();
}

/**
 *
 */
void dlg_clustering::adjustSizeDlg()
{
    //adjust size because of extra hidden and shown conditions (dependent on pdb or situs document usage)
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    adjustSize();
}

/**
 *
 */
void dlg_clustering::assignMethod(int iMethod)
{
    m_iMethod = iMethod;
}



/**
 *
 */
void dlg_clustering::resetDlg(sculptor_document* pSculptorDoc)
{
    //show everything back
    m_pResolution->show();
    m_pResolution_Label->show();
    m_pVoxelWidth->show();
    m_pVoxelWidth_Label->show();

    m_pVQ_Ti->show();
    m_pVQ_Tf->show();
    m_pVQ_Ti_Label->show();
    m_pVQ_Tf_Label->show();
    m_pVQ_Cutoff->show();
    m_pVQ_Cutoff_Label->show();

    m_pVQ_Conn->show();
    m_pVQ_CodebookSize->setEnabled(true);

    //dialog will be called on different documents, so this must be updated to the currently selected document
    m_pSculptorDoc = pSculptorDoc;
    //set method and codebook pdb back to null, and delete the pdb that was created to store the codebook vectors
    m_iMethod = NULL;
    delete m_pCV;
    m_pCV = NULL;


}

/**
 *
 */
void dlg_clustering::extractParameters()
{
    //obtain all the values regardless of method
    unsigned int iCV = m_pVQ_CodebookSize->value();
    int iMaxStep = m_pVQ_Maxsteps->value();
    Real64 fEi = m_pVQ_Epsiloni->text().toFloat();
    Real64 fEf = m_pVQ_Epsilonf->text().toFloat();
    Real64 fLi = m_pVQ_Lambdai->text().toFloat();
    Real64 fLf = m_pVQ_Lambdaf->text().toFloat();
    bool bTopologyDetermination = m_pVQ_Conn->isChecked();
    
    // if there is no codebook vector pdb, assign a cutoff for the search space of codebook vector generation.
    if (m_pCV == NULL)
    {
        if (m_pSculptorDoc->getType() == SENSITUS_DOC_SITUS)
        {
            Real64 fCutoff = m_pVQ_Cutoff->text().toFloat();
            ((situs_document*)m_pSculptorDoc)->setCutoff(fCutoff);
        }
	
    }
    

    //if the method is for laplaceQuantization, call laplaceQuantization method. fResolution and fWidth are not needed in the case of situs documents.
    if (m_iMethod == CLUSTER_METHOD_LAPLACE)
    {
	Real64 fResolution = 0;
	Real64 fWidth = 0;
	if (m_pSculptorDoc->getType() == SENSITUS_DOC_PDB)
        {
	    fResolution = m_pResolution->text().toDouble();
	    fWidth = m_pVoxelWidth->text().toDouble();
  	}
        
        m_pSculptorDoc->laplaceQuantization(fLi, fLf, fEi, fEf, iMaxStep, iCV, bTopologyDetermination, fResolution, fWidth);
    }
    //if the method is clustering, call clustering method.  fTi and fTf are not needed if the codebook vector PDB is provided.
    if (m_iMethod == CLUSTER_METHOD_CLUSTERING)
    {
	Real64 fTi = 0;
	Real64 fTf = 0;
	if (m_pCV != NULL)
	{
	   fTi = m_pVQ_Ti->text().toFloat();
	   fTf = m_pVQ_Tf->text().toFloat();
	}

	m_pSculptorDoc->clustering(fLi, fLf, fEi, fEf, iMaxStep, iCV, fTi, fTf, bTopologyDetermination, m_pCV);
    }
}

