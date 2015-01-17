/*-*-c++-*-*****************************************************************
                          sculptor_dlg_preferences.h
                          -----------------
    begin                : 12/08/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_PREFERENCES_H
#define SCULPTOR_DLG_PREFERENCES_H

// uic-qt4 includes
#include <ui_dlg_preferences.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Manuel Wahle
 */
class dlg_preferences : public QDialog, private Ui::dlg_preferences
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_preferences(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_preferences();

public slots:

    /**
     *
     */
    void sRestoreWindow(bool);

    /**
     *
     */
    void sRestoreWindow();

    /**
     *
     */
    void sDisableAO(bool);

    /**
     *
     */
    void sDisableART(bool);

    /**
     *
     */
    void sDisableAA(bool);
};

#endif
