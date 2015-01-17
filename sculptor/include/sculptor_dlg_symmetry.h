/*-*-c++-*-*****************************************************************
                          sculptor_dlg_symmetry.cpp
                          -----------------
    begin                : 11.11.2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_SYMMETRY_H
#define SCULPTOR_DLG_SYMMETRY_H

// sculptor includes
class pdb_document;
// svt includes
#include <svt_stlVector.h>
// uic-qt4 includes
#include <ui_dlg_symmetry.h>
// qt4 includes
#include <QDialog>

/*
 * This class inherits from the QT4 Designer dlg_symmetry class and implements its functionality.
 * \author Sayan Ghosh
 */
class dlg_symmetry : public QDialog, private Ui::dlg_symmetry
{
    Q_OBJECT

    // the pdb doc this dialog belongs to
    pdb_document* m_pDoc;

    // all pdb documents in the documentlist
    vector<pdb_document*> m_pPDBDocs;

public:

    /**
     * Constructor
     */
    dlg_symmetry(QWidget* pParent, pdb_document* pDoc);

    /**
     * Destructor
     */
    virtual ~dlg_symmetry();
    
public slots: 
  
    /**
     * init dialog
     */    
    void init();

    /**
     * update contents
     */    
    void updatePoint2(int iIndex);
   
    /**
     * call the pdb_docuemnt's method to apply symmetry
     */        
    void applySymmetry();
    
};    
    
#endif // SCULPTOR_DLG_SYMMETRY_H    
