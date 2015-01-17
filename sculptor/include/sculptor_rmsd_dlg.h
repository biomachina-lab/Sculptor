/***************************************************************************
                          sculptor_rmsd_dlg.h  -  description
                             -------------------
    begin                : 14.08.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SENSITUS_RMSD_DLG_H
#define SENSITUS_RMSD_DLG_H

// sculptor includes
#include <sculptor_plot_dlg.h>
#include <pdb_document.h>

#define RMSD_COUNT 500

/**
 * sculptor rmsd plot dialog
 */
class sculptor_rmsd_dlg : public sculptor_plot_dlg
{ 
    Q_OBJECT

protected:
    pdb_document* m_pDoc1;
    pdb_document* m_pDoc2;
    double m_aRmsd[ RMSD_COUNT ];
    int m_iRmsdCnt;

public:
    sculptor_rmsd_dlg( const char* pTitle, pdb_document* pDoc1, pdb_document* pDoc2, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~sculptor_rmsd_dlg();

public slots:

    /**
     * update curves
     */
    void updateCurves();
};

#endif
