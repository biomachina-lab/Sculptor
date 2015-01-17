/***************************************************************************
                          sculptor_dlg_atomclustering.h
                          -----------------
    begin                : 04.11.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_ATOMCLUSTERING_H
#define SCULPTOR_DLG_ATOMCLUSTERING_H

// uic-qt4 includes
#include <ui_dlg_atomclustering.h>
// forward declaration
class pdb_document;

/**
 * Simple class that inherits the atomclustering dialog and extracts the parameters for use by the method.
 * \author Francisco Serna
 */

class dlg_atomclustering : public QDialog, private Ui::dlg_atomclustering
{
    Q_OBJECT

public:

    /**
     * Constructor
     * \param pParent pointer to parent Widget
     * \param pPdbDoc pointer to pdb_document to perform method on
     */
    dlg_atomclustering(QWidget* pParent, pdb_document* pPdbDoc);

    /**
     * Destructor
     */
    virtual ~dlg_atomclustering();

    /**
     * Reset the dialog by changing the spinbox value back to zero and updating the pdb_document
     * \param pPdbDoc pointer to new pdb_document to perform method on
     */
    void resetDlg(pdb_document* pPdbDoc);

private slots:

    /**
     * Extract the integer value from the spinBox and send it to the method
     */
    void getParameterValue();  

private:
   
    //store a pointer to the pdb document for calling of methods
    pdb_document* m_pPdbDoc;

};

#endif


