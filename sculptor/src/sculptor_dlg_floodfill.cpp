/***************************************************************************
                          sculptor_dlg_floodfill.ccp
                          -----------------
    begin                : 11.02.2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <situs_document.h>
#include <sculptor_dlg_floodfill.h>


/**
 * Constructor
 */
dlg_floodfill::dlg_floodfill(QWidget* pParent, situs_document* pDoc)
    : QDialog( pParent )
{
    setupUi(this);

    connect( m_pOKButton, SIGNAL(clicked()), pDoc, SLOT(floodfill_DOIT()) );
}

/**
 * Destructor
 */
dlg_floodfill::~dlg_floodfill()
{
}


/**
 * set the value in the voxelX lineedit
 */
void dlg_floodfill::setVoxelX(unsigned int iX)
{
    m_pVoxelX->setText( QString().setNum(iX) );
}


/**
 * get the voxelX value
 */
unsigned int dlg_floodfill::getVoxelX()
{
    return m_pVoxelX->text().toUInt();
}


/**
 * set the value in the voxelY lineedit
 */
void dlg_floodfill::setVoxelY(unsigned int iY)
{
    m_pVoxelY->setText( QString().setNum(iY) );
}

/**
 * get the voxelY value
 */
unsigned int dlg_floodfill::getVoxelY()
{
    return m_pVoxelY->text().toUInt();
}

/**
 * set the value in the voxelZ lineedit
 */
void dlg_floodfill::setVoxelZ(unsigned int iZ)
{
    m_pVoxelZ->setText( QString().setNum(iZ) );
}


/**
 * get the voxelZ value
 */
unsigned int dlg_floodfill::getVoxelZ()
{
    return m_pVoxelZ->text().toUInt();
}


/**
 * set the Threshold in lineedit
 */
void dlg_floodfill::setThreshold(float fThreshold)
{
    m_pThreshold->setText( QString().setNum(fThreshold) );
}

/**
 * get the Threshold value
 */

double dlg_floodfill::getThreshold()
{
    return m_pThreshold->text().toDouble();
}

/**
 * set the Sigma in lineedit
 */
void dlg_floodfill::setSigma(float fSigma)
{
    m_pSigma->setText( QString().setNum(fSigma) );
}


/**
 * get the Sigma value
 */
double dlg_floodfill::getSigma()
{
    return m_pSigma->text().toDouble();
}
