/***************************************************************************
                          sculptor_dlg_DPSV_filter.h
                          -----------------
    begin                : 07.23.2011
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_DPSV_FILTER_H
#define SCULPTOR_DLG_DPSV_FILTER_H

// uic-qt4 includes
#include <ui_dlg_DPSV_filter.h>
// sculptor includes
class situs_document;
// svt includes
#include <svt_volume.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Zbigniew Starosolski
 */
class dlg_DPSV_filter : public QDialog, private Ui::dlg_DPSV_filter
{
    Q_OBJECT
    
    svt_volume<Real64> m_oVolume;
    situs_document *m_pDoc;
   
    
public:

    /**
     * Constructor
     */
    dlg_DPSV_filter(QWidget* pParent, situs_document* pDoc,  svt_volume<Real64> oVolume);

    /**
     * Destructor
     */
    virtual ~dlg_DPSV_filter();

    /**
     * get which button is pressed Neighborhood model
     */
    int getModelOfNeigborhood();
    
    /**
     * get value of Mask Size
     */
   
    int getMaskSize();
    
    
    /**
     * get value of PathLength
     */
   
    int getPathLength();
    

    /**
     * get value of Beta
     */
   
    double getBeta();
   
public slots:

    /**
     * cancel cropping and turn of cropping cube
     */
    void cancel();

    
};

#endif   // SCULPTOR_DLG_DPSV_FILTER
