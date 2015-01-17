/***************************************************************************
                          sculptor_dlg_volselection.cpp
			  -----------------------------
    begin                : 11-03-2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_VOLSELECTION_H
#define SCULPTOR_DLG_VOLSELECTION_H
//ui
#include <ui_dlg_volselection.h>
//sculptor includes
class situs_document;
//svt includes
#include <svt_volume.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Sayan Ghosh 
 */

class dlg_volselection : public QDialog, private Ui::dlg_volselection
{
    Q_OBJECT
    
    svt_volume<Real64> m_oVolume;
    situs_document *m_pDoc;

public:

    /**
     * Constructor
     */
    dlg_volselection(QWidget* pParent, situs_document* pDoc, svt_volume<Real64> oVolume);

    /**
     * Destructor
     */
    virtual ~dlg_volselection();
      
    /**
     * set the init values for the ui elements
     */
    void setValues();  
    
public slots:    

      
    /** 
     *  modify the volselectioncube as the user changes the corrdinates
     *  calls situs_document::setSelection(int, int, int, int, int, int)
     */ 
    void setSelection(); 
    
    
    /**
     * calls situs_document::crop with user specified params
     */
    void crop();

    /**
     * custom slot that erases a volselectioncube before signaling 'reject' when Cancel button is pressed
     */
    void onCancel();

    /**
     * Reimplemented keyPressEvent from QKeyEvent to handle key presses..at present only implemented Esc key press
     */
    void keyPressEvent ( QKeyEvent * e );
     
};

#endif