/***************************************************************************
                          sculptor_dlg_DPSV_filter.ccp
                          -----------------
    begin                : 07.23.2011
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <situs_document.h>
#include <sculptor_dlg_DPSV_filter.h>


/**
 * Constructor
 */
dlg_DPSV_filter::dlg_DPSV_filter(QWidget* pParent, situs_document* pDoc,  svt_volume<Real64> oVolume)
    : QDialog( pParent ), 
    m_oVolume ( oVolume ),  
    m_pDoc ( pDoc )
    
{
    setupUi(this);

    connect( m_pOKButton, SIGNAL(clicked()), pDoc, SLOT(DPSV_filter_DOIT()) );
    connect( m_pCancel, SIGNAL(clicked()), this, SLOT(cancel()) );
    
    m_pButtonGroup->setId(m_pNeighb4, situs_document::SITUS_DOC_NEIGHBOOR4);
    m_pButtonGroup->setId(m_pNeighb8, situs_document::SITUS_DOC_NEIGHBOOR8);
    m_pButtonGroup->setId(m_pNeighb6, situs_document::SITUS_DOC_NEIGHBOOR6);
    m_pButtonGroup->setId(m_pNeighb26, situs_document::SITUS_DOC_NEIGHBOOR26);
      
}

/**
 * Destructor
 */
dlg_DPSV_filter::~dlg_DPSV_filter()
{
}

/**
 * get which button is pressed Neighborhood model
 */
     
int dlg_DPSV_filter::getModelOfNeigborhood()
{
	return m_pButtonGroup->checkedId();
}
    
/**
 * get value of Mask Size
 */
   
int dlg_DPSV_filter::getMaskSize()
{
 return m_pMaskSize->value();
 }

/**
 * get value of PathLength
 */
   
int dlg_DPSV_filter::getPathLength()
{
 return   m_pPathLength->value();
}
 
/**
 * get value of Beta
 */
   
double dlg_DPSV_filter::getBeta()
{
  double fBeta;
  QString pBeta = m_pBeta->text();
  if (!pBeta.isEmpty())
  {
    fBeta = pBeta.toDouble();
    return fBeta;
  }
  else
   {
    svt_exception::ui()->info("Please select a value for the parameter Beta!");
    return 0;
   }
  	
}
/**
 *
 */
void dlg_DPSV_filter::cancel()
{
    //hide
    hide();
}

 
 
 