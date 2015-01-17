/***************************************************************************
                          sculptor_dlg_colorselect.h
                          -----------------
    begin                : 10.27.2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_COLORSELECT_H
#define SCULPTOR_DLG_COLORSELECT_H

// uic-qt4 includes
#include <ui_dlg_colorselect.h>
//svt includes
class svt_palette;


/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Sayan Ghosh
 */
class dlg_colorselect : public QDialog, private Ui::dlg_colorselect
{
    Q_OBJECT

    // the palette whose color should appear in the list
    svt_palette *m_pPalette;

    /**
     * adds a color item to the listwidget
     */
    void addItemFromPalette(int iIndex);

private slots:

    /**
     * adds a user selected color to the palette
     */
    void addItemToPalette();

public:

    /**
     * Constructor
     */
    dlg_colorselect(QWidget* pParent, svt_palette *pPalette);

    /**
     * Destructor
     */
    virtual ~dlg_colorselect();

    /**
     * get selected colors
     */
    int getCurrent();

    /**
     * set selected color
     */
    void setCurrent( int iIndex );
};

#endif
