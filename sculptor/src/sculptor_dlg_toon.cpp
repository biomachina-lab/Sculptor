/***************************************************************************
                          sculptor_dlg_toon.cpp
                          -----------------
    begin                : 12.11.2010
    author               : Manuel Wahle
                         : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_toon.h>
// svt includes
#include <svt_pdb.h>

/**
 *
 */
dlg_toon::dlg_toon(QWidget* pParent,svt_pdb * pPdbNode)
    : QDialog(pParent),
      m_pPdbNode(pPdbNode)
{
    setupUi(this);

    // this dialog not used right now. it will make sense when the toon mode is extended

    // signals and slots connections
    connect( m_pButtonClose,             SIGNAL(clicked()),               this, SLOT(hide()) );
}

/**
 *
 */
dlg_toon::~dlg_toon()
{
}
