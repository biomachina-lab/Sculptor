/***************************************************************************
                          sculptor_dlg_rmsd.h
                          -----------------
    begin                : 10.05.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_RMSD_H
#define SCULPTOR_DLG_RMSD_H

// uic-qt4 includes
#include <ui_dlg_rmsd.h>

class pdb_document;

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_rmsd : public QDialog, private Ui::dlg_rmsd
{
    Q_OBJECT

    pdb_document *m_pFirst;
    pdb_document *m_pSecond;

public:

    /**
     * Constructor
     */
    dlg_rmsd(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_rmsd();

    /**
     * get selected documents
     */ 
    void getDocs();

    /**
     * show/modify option based on selection
     */ 
    void updateUi();

private slots:

    /**
     * computes RMSD
     */
    void computeRMSD();
 
};

#endif
