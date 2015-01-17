/***************************************************************************
                          sculptor_dlg_mpfloodfill.ccp
                          -----------------
    begin                : 01.04.2012
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <situs_document.h>
#include <sculptor_dlg_mpfloodfill.h>


/**
 * Constructor
 */
dlg_mpfloodfill::dlg_mpfloodfill(QWidget* pParent, situs_document* pDoc)
    : QDialog( pParent )
{
    setupUi(this);

    connect( m_pOKButton, SIGNAL(clicked()), pDoc, SLOT(multiPointFloodfill_DOIT()) );
}

/**
 * Destructor
 */
dlg_mpfloodfill::~dlg_mpfloodfill()
{
}


/**
 * set the value in the voxelX lineedit
 */
void dlg_mpfloodfill::setPointCount(unsigned int iX)
{
    m_pPointCount->setText( QString().setNum(iX) );
}


/**
 * get the voxelX value
 */
unsigned int dlg_mpfloodfill::getPointCount()
{
    return m_pPointCount->text().toUInt();
}

/**
 * set the Threshold in lineedit
 */
void dlg_mpfloodfill::setThreshold(float fThreshold)
{
    m_pThreshold->setText( QString().setNum(fThreshold) );
}

/**
 * get the Threshold value
 */

double dlg_mpfloodfill::getThreshold()
{
    return m_pThreshold->text().toDouble();
}

/**
 * set the Sigma in lineedit
 */
void dlg_mpfloodfill::setSigma(float fSigma)
{
    m_pSigma->setText( QString().setNum(fSigma) );
}


/**
 * get the Sigma value
 */
double dlg_mpfloodfill::getSigma()
{
    return m_pSigma->text().toDouble();
}
