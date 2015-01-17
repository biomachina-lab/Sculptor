/*-*-c++-*-*****************************************************************
                          sculptor_dlg_mpfloodfill.h
                          -----------------
    begin                : 01.04.2012
    author               : Mirabela Rusu 
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_MPFLOODFILL_H
#define SCULPTOR_DLG_MPFLOODFILL_H

// sculptor includes
class situs_document;
// uic-qt4 includes
#include <ui_dlg_mpfloodfill.h>
// qt4 includes
#include <QString>
#include <QDialog>


/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Zbigniew Starosolski
 */
class dlg_mpfloodfill : public QDialog, private Ui::dlg_mpfloodfill
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_mpfloodfill(QWidget* pParent, situs_document* pDoc );

    /**
     * Destructor
     */
    virtual ~dlg_mpfloodfill();

    /**
     * set the value in the voxelX lineedit
     */
    void setPointCount(unsigned int iX);

    /**
     * get the voxelX value
     */
    unsigned int getPointCount();

    /**
     * set the Threshold in lineedit
     */
    void setThreshold(float fThreshold);

    /**
     * get the Threshold value
     */
    double getThreshold();

    /**
     * set the Sigma in lineedit
     */
    void setSigma(float fSigma);

    /**
     * get the Sigma value
     */
    double getSigma();

};

#endif // SCULPTOR_DLG_FLOODFILL_H
