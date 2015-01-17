/***************************************************************************
                          sculptor_dlg_swapaxis.h
                          -----------------
    begin                : 11.04.2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_SWAPAXIS_H
#define SCULPTOR_DLG_SWAPAXIS_H

// sculptor includes
class situs_document;
// uic-qt4 includes
#include <ui_dlg_swapaxis.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Zbigniew Starosolski
 */


class dlg_swapaxis : public QDialog, private Ui::dlg_swapaxis
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_swapaxis(QWidget* pParent, situs_document* pDoc );

    /**
     * Destructor
     */
    virtual ~dlg_swapaxis();

    /**
     * get which button is pressed for X axis
     */
    int getOldX();

    /**
     * get which button is pressed for Y axis
     */
    int getOldY();

    /**
     * get which button is pressed for Z axis
     */
    int getOldZ();

    /**
     * check if axis X button is check in dialog window
     */
    bool getAxisXcheck();

    /**
     * check if axis Y button is check in dialog window
     */
    bool getAxisYcheck();

    /**
     * check if axis Z button is check in dialog window
     */
    bool getAxisZcheck();


};

#endif   // SCULPTOR_DLG_SWAPAXIS_H
