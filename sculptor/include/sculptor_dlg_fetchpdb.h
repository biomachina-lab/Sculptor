/***************************************************************************
                          sculptor_dlg_fetchpdb.h
                          -----------------
    begin                : 09.10.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_FETCHPDB_H
#define SCULPTOR_DLG_FETCHPDB_H

// uic-qt4 includes
#include <ui_dlg_fetchpdb.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Francisco Serna
 */
class dlg_fetchpdb : public QDialog, private Ui::dlg_fetchpdb
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_fetchpdb(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_fetchpdb();

    /**
     * Clear/reset to defaults ui elements of fetchPDB
     */
    void clearSelections();

private slots:

    /**
     * Check PDBID user has input, and if appropriate, pass input to fetchPDB function for
     * retrieval.  Otherwise put up warning message.
     */
    void checkString();

};

#endif
