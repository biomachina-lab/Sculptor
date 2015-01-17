/***************************************************************************
                          sculptor_dlg_flexing.h
                          -----------------
    begin                : 10.15.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_FLEXING_H
#define SCULPTOR_DLG_FLEXING_H

// uic-qt4 includes
#include <ui_dlg_flexing.h>

class pdb_document;

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_flexing : public QDialog, private Ui::dlg_flexing
{
    Q_OBJECT

    sculptor_document *m_pDockDoc;
    sculptor_document *m_pTargetDoc;

    // the names of the dock and target documents
    char m_pDockDocFName[50];
    char m_pTargetDocFName[50];


public:

    /**
     * Constructor
     */
    dlg_flexing(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_flexing();

    /**
     * show/modify option based on selection
     */ 
    void updateUi();

    /**
     * set documents
     */ 
    void setDocs( sculptor_document * pDockDoc, sculptor_document *pTargetDoc );

    /**
     * checks the validity of the Dock/TargetDocuments
     */
    void checkDocs();
 
    /**
     * get the doc document
     */
    sculptor_document* getDockDoc();

    /**
     * get the target document
     */
    sculptor_document* getTargetDoc();

    /**
     * set state for showdisplacement
     */  
    void setShowDisp( bool bShowDisp );

    /**
     * get state for ShowDisplacement
     */  
    bool getShowDisp();

private slots:

    /**
     * launch the interpolation
     */
    void interpolate();

    /**
     *Shows the advanced option for the Flexing
     */
    void showFlexingAdvanceOption();

    /**
     * enable/disable create conformers count  
     */
    void setEnableConformersCount();

    /**
     * Disable/Enables the IDW inteepolation related options
     */
    void setEnabledWeightingOptions();

    /**
     * enables the selection of the Refmac path
     */
    void setEnabledRefmac();

    /**
     * choose refmac path
     */
    void chooseRefmacPath();

    /**
    * Render Displacement
    */ 
    void renderDisplacement();



};

#endif
