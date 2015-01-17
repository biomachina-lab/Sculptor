/*-*-c++-*-*****************************************************************
                          sculptor_dlg_crop.h
                          -----------------------------
    begin                : 11-03-2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_CROP_H
#define SCULPTOR_DLG_CROP_H

// uic-qt4 includes
#include <ui_dlg_crop.h>
// sculptor includes
class situs_document;
// svt includes
#include <svt_volume.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Sayan Ghosh
 */
class dlg_crop : public QDialog, private Ui::dlg_crop
{
    Q_OBJECT

    svt_volume<Real64> m_oVolume;
    situs_document *m_pDoc;

public:

    /**
     * Constructor
     */
    dlg_crop(QWidget* pParent, situs_document* pDoc, svt_volume<Real64> oVolume);

    /**
     * Destructor
     */
    virtual ~dlg_crop();

    /**
     * init values for the ui elements
     */
    void setValues();

public slots:

    /**
     *  modify the cropcube as the user changes the cordinates
     *  calls situs_document::setSelection(int, int, int, int, int, int)
     */
    void setSelection();

    /**
     * calls situs_document::crop with user specified params
     */
    void crop();

    /**
     * cancel cropping and turn of cropping cube
     */
    void cancel();

protected:

    /**
     * reimplement closeEvent to hide the cube on close
     */
    virtual void closeEvent(QCloseEvent* pEvent);

    /**
     * reimplement keyPressEvent to hide cropping cube on Escape key
     */
    virtual void keyPressEvent(QKeyEvent* pEvent);

};

#endif
