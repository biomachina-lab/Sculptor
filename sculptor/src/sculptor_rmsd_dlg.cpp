/***************************************************************************
                          sculptor_rmsd_dlg.cpp  -  description
                             -------------------
    begin                : 06.08.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_rmsd_dlg.h>

///////////////////////////////////////////////////////////////////////////////
// rmsd dlg
///////////////////////////////////////////////////////////////////////////////

/**
 * sculptor rmsd dialog
 */
sculptor_rmsd_dlg::sculptor_rmsd_dlg( const char* pTitle, pdb_document* pDoc1, pdb_document* pDoc2, QWidget* pParent,  const char* pName, bool bModal, Qt::WFlags iFlags )
    : sculptor_plot_dlg( pTitle, pParent, pName, bModal, iFlags ), m_iRmsdCnt(0)

{
    m_pDoc1 = pDoc1;
    m_pDoc2 = pDoc2;

    for(int i=0;i < RMSD_COUNT; i++)
        m_aRmsd[i] = 0.0;

    sculptor_curve oCurve( "RMSD", NULL, m_aRmsd, RMSD_COUNT );

    addCurve( oCurve );
    getCurve( 0 )->setColor( 255, 255, 255 );
    getCurve( 0 )->setStyle( QwtPlotCurve::Lines );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
sculptor_rmsd_dlg::~sculptor_rmsd_dlg()
{
}

/**
 * update curve
 */
void sculptor_rmsd_dlg::updateCurves()
{
    // update rmsd calculation
    m_aRmsd[ m_iRmsdCnt ] = m_pDoc1->getRmsd( m_pDoc2 );
    m_iRmsdCnt++;

    if (m_iRmsdCnt % RMSD_COUNT == 0)
    {
        m_iRmsdCnt = 0;

        for(int i=0;i < RMSD_COUNT; i++)
            m_aRmsd[i] = 0.0;
    }

    // replot
    sculptor_plot_dlg::updateCurves();
}
