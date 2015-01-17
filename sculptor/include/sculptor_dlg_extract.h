/*-*-c++-*-*****************************************************************
			  sculptor_dlg_extract.h
			  -------------------
    begin                : 08/2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_EXTRACT_H
#define SCULPTOR_DLG_EXTRACT_H

// svt includes
class svt_pdb;
// sculptor includes
class sculptor_window;
class pdb_document;
// uic-qt4 includes
#include <ui_dlg_extract.h>
// qt4 includes
#include <QString>
#include <QDialog>

/*
 * This class inherits from the QT4 Designer dlg_extract class and implements its functionality.
 * \author Sayan Ghosh
 */
class dlg_extract : public QDialog, private Ui::dlg_extract
{
    Q_OBJECT

    QString m_oShortName;
    sculptor_window * m_pSculptorWindow;
    pdb_document * m_pPdbDocument;
    svt_pdb * m_pPdbNode;

public:

    /**
     * Constructor
     * \param pPdbNode A pdb file node. This node can load a pdb file (molecule) and display it in
     * various drawing modes. The default mode is a line drawing mode, which is very fast but not
     * very good looking. You can switch to other modes or even mix modes. That requires the use of
     * the enumeration functions which search for atoms with certain properties (name, residue,
     * etc). You can specify the drawing mode for every atom!
     * \param pPdbDocument A pdb document type
     * \param oFileName Name of the PDB document without extension and file path
     * \param pSculptorWindow Pointer to sculptor window
     */
    dlg_extract( QWidget* pParent,
		 svt_pdb * pPdbNode,
		 pdb_document * pPdbDocument,
		 QString oFileName,
		 sculptor_window * pSculptorWindow );

    /**
     * Destructor
     */
    virtual ~dlg_extract();

public slots:

    /**
     * Change the content of the extract Dlg based on the selected method
     * e.g. if selected extract model, fill the id with the model number
     */
    void updateExtractDlg();

    /**
     * Extract chains, frames, models, backbones and carbon alpha from a PDB document
     */
    void extract();
};

#endif // SCULPTOR_DLG_EXTRACT_H
