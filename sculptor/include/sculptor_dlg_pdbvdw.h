 /*-*-c++-*-*****************************************************************
                          sculptor_dlg_pdbvdw.h
                          -----------------
    begin                : 12.11.2010
    author               : Manuel Wahle
                         : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_PDBVDW_H
#define SCULPTOR_DLG_PDBVDW_H

// uic-qt4 includes
#include <ui_dlg_pdbvdw.h>
// svt includes
class svt_pdb;
// sculptor includes
class pdb_document;

/**
 * Simple class that inherits the vdw dialog and connects the ui to the methods of svt_pdb and pdb_document classes.
 * \author Manuel Wahle
 * \author Francisco Serna
 */
class dlg_pdbvdw : public QDialog, private Ui::dlg_pdbvdw
{
    Q_OBJECT

    // pointer to the corresponding pdb_document
    pdb_document * m_pPdbDocument;
    // pointer to the corresponding svt_pdb
    svt_pdb* m_pPdbNode;

public:

    /**
     *  Constructor
     *  \param pParent pointer to parent Widget
     *  \param pPdbNode pointer to svt_pdb node
     *  \param pPdbDocument pointer to pdb_document
     */
    dlg_pdbvdw(QWidget* pParent, svt_pdb * pPdbNode, pdb_document * pPdbDocument);

    /**
     * Destructor
     */
    virtual ~dlg_pdbvdw();

    /**
     * Update the sphere scaling line edit from value stored in pdb document
     */
    void updateSphereScaling();

    /**
     * disable any GUI elements that have to do with shaders
     */
    void updateUseShader();

private slots:

    /**
     * Set the sphere scale according to the line edit
     */
    void sSetSphereScale();

    /**
     * Sets the rendering mode of vdw
     * \param iMode integer enum of SVT_PDB_VDW
     */
    virtual void sComboboxRenderingMode(int iIndex);
};

#endif
