/***************************************************************************
                          sculptor_dlg_colmap.h
                          -----------------
    begin                : 05.11.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_COLMAP_H
#define SCULPTOR_DLG_COLMAP_H

// uic-qt4 includes
#include <ui_dlg_colmap.h>
//forward declaration
class pdb_document;

/**
 * Simple class that inherits the colmap dialog and connects the parameters to the proper methods.
 * \author Francisco Serna
 */
class dlg_colmap : public QDialog, private Ui::dlg_colmap
{
    Q_OBJECT

public:

    /**
     * Constructor
     * \param pParent pointer to parent Widget
     * \param pPdbDoc pointer to pdb document
     */
    dlg_colmap(QWidget* pParent, pdb_document* pPdbDoc);

    /**
     * Destructor
     */ 
    virtual ~dlg_colmap();

    /**
     * Method to reset dialog to defaults clearing name label and combo box
     */
    void resetDlg();

    /**
     * Set name of label in dialog
     * \param oLabelName QString with name
     */
    void setLabelName(QString oLabelName);

    /**
     * Populate the combo box with names
     * \param iIndex int corresponding to item number
     * \param oName QString corresponding to name of item
     */
    void addColorNameItem(int iIndex, QString oName);

private slots:

    /**
     * Extract the parameters from the dlg and call the method in pdb_document
     */ 
    void sExtractColmapSetting();

private:

    pdb_document* m_pPdbDoc;

};

#endif

