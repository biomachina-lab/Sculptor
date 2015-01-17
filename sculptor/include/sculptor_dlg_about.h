/***************************************************************************
                          sculptor_dlg_about.h
                          -----------------
    begin                : 11.02.2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_ABOUT_H
#define SCULPTOR_DLG_ABOUT_H

// uic-qt4 includes
#include <ui_dlg_about.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Zbigniew Starosolski
 */
class dlg_about : public QDialog, private Ui::dlg_about
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_about(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_about();



};

#endif
