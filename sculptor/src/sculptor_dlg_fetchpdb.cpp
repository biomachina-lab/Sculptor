/***************************************************************************
                          sculptor_dlg_fetchpdb.cpp
                          -----------------
    begin                : 09.10.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_window.h>
#include <sculptor_dlg_fetchpdb.h>

extern sculptor_window* g_pWindow;


/**
 * Constructor
 */
dlg_fetchpdb::dlg_fetchpdb(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);

    // if user hits enter after entering something, check if the string has correct PDB ID format
    connect( m_pPdbID, SIGNAL(returnPressed()), this, SLOT(checkString()) );

    // if user hits OK button, check if the string has correct PDB ID format
    connect( m_pOk, SIGNAL(clicked()), this, SLOT(checkString()) );
}

/**
 * Destructor
 */
dlg_fetchpdb::~dlg_fetchpdb()
{
}

/**
 * Clear/reset to defaults ui elements of fetchPDB
 */
void dlg_fetchpdb::clearSelections()
{
    // clear textfield
    m_pPdbID->clear();
    // make the textfield focused
    m_pPdbID->setFocus();
    // uncheck checkbox
    m_pBiologicalUnitCheckBox->setChecked(false);
    // set spinbox back to 1 (default)
    m_pBiologicalUnitNumber->setValue(1);
}

/**
 * Check PDBID user has input, and if appropriate, pass input to fetchPDB function for retrieval.
 * Otherwise put up warning message.
 */
void dlg_fetchpdb::checkString()
{
    QString oPDB = m_pPdbID->text();

    // check if valid, if not call a warning
    if (oPDB.length() == 4)
    {
        // add pdb extension.  If user wants biological unit, attach unit number
        if (m_pBiologicalUnitCheckBox->isChecked())
        {
            QString oBioUnitNum = m_pBiologicalUnitNumber->text();
            oPDB += ".pdb";
            oPDB += oBioUnitNum;
        }
        else
        {
            oPDB += ".pdb";
        }

        // hide the widget
        hide();
        // call the fetchPDB method in sculptor_window using full name and extension of PDB
        g_pWindow->queryPDB(oPDB);
    }
    else
    {
        // complain to the user
        g_pWindow->guiWarning("PDB Fetch Error", "PDBID must have 4 alphanumeric characters.", "PDBID Fetch error");

    }
}
