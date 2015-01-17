/***************************************************************************
                          sculptor_dlg_colmapsecstruct.h
                          -----------------
    begin                : 09.11.2010
    author               : Manuel Wahle
			   Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SCULPTOR_DLG_COLMAPSECSTRUCT_H
#define SCULPTOR_DLG_COLMAPSECSTRUCT_H

// uic-qt4 includes
#include <ui_dlg_colmapsecstruct.h>
//forward declaration
class pdb_document;
class dlg_cartoon;
class svt_pdb;

/**
 * Simple class that inherits the colmapsecstruct dialog and connects the parameters to the appropriate slot methods in pdb_document
 * \author Manuel Wahle
 * \author Francisco Serna
 */

class dlg_colmapsecstruct : public QDialog, private Ui::dlg_colmapsecstruct
{
    Q_OBJECT

public:
   
    /**
     * Constructor
     * \param pParent pointer to parent Widget
     * \param pPdbDocument pointer to pdb document
     */
    dlg_colmapsecstruct(QWidget* pParent, pdb_document* pPdbDocument);

    /**
     * Destructor
     */  
    virtual ~dlg_colmapsecstruct();
   
    /**
     * Used to hide the widget
     */
    void hide();

    /**
     * Update the minimum residues in the gui from the parameters defined in the node (at cartoon) 
     */
    void updateMinRes();

    /**
     * Assign the cartoon prop dialog passed in, to a pointer usable by this document
     * \param pDlgCartoon pointer to the cartoon dialog
     */
    void setCartoonPropDlg(dlg_cartoon* pDlgCartoon);

    /**
     * Set the name of the label
     * \param oName QString with label name
     */ 
    void setLabelName(QString oName);

    /**
     * Add item to combobox to specify color change
     * \param iItem int of item (count)
     * \param oChar QString name to display in combobox
     */
    void addColorNameItem(int iItem, QString oChar);

private slots:

    /**
     * Calls method buttonChangeColor in pdb_document with parameter from gui
     */
    void sExtractComboItem();

    /**
     * Sets the minimum residues per sheet in both svt_pdb and svt_atom_structure_colmap.  Also updates dlg_cartoon if it is not null.
     */
    void setMinResPerSheet();
    
    /**
     * Sets the minimum residues per helix in both svt_pdb and svt_atom_structure_colmap.  Also updates dlg_cartoon if it is not null.
     */
    void setMinResPerHelix();



private:

    pdb_document* m_pPdbDocument;
    dlg_cartoon* m_pDlgCartoon;
    svt_pdb* m_pPdbNode;

};

#endif
