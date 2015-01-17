/*-*-c++-*-*****************************************************************
                          sculptor_dlg_floodfill.h
                          -----------------
    begin                : 11.02.2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_FLOODFILL_H
#define SCULPTOR_DLG_FLOODFILL_H

// sculptor includes
class situs_document;
// uic-qt4 includes
#include <ui_dlg_floodfill.h>
// qt4 includes
#include <QString>
#include <QDialog>


/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Zbigniew Starosolski
 */
class dlg_floodfill : public QDialog, private Ui::dlg_floodfill
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_floodfill(QWidget* pParent, situs_document* pDoc );

    /**
     * Destructor
     */
    virtual ~dlg_floodfill();

    /**
     * set the value in the voxelX lineedit
     */
    void setVoxelX(unsigned int iX);

    /**
     * get the voxelX value
     */
    unsigned int getVoxelX();

    /**
     * set the value in the voxelY lineedit
     */
    void  setVoxelY(unsigned int oString);

    /**
     * get the voxelY value
     */
    unsigned int getVoxelY();

    /**
     * set the value in the voxelZ lineedit
     */
    void setVoxelZ(unsigned int oString);

    /**
     * get the voxelZ value
     */
    unsigned int getVoxelZ();

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
