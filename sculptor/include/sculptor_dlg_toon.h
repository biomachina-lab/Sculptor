/***************************************************************************
                          sculptor_dlg_toon.h
                          -----------------
    begin                : 12.11.2010
    author               : Manuel Wahle
			 : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_TOON_H
#define SCULPTOR_DLG_TOON_H

// uic-qt4 includes
#include <ui_dlg_toon.h>
//forward declarations
class svt_pdb;

/**
 * Simple class that inherits toon dialog and supplies parameters for methods in pdb node
 * \author Manuel Wahle
 * \author Francisco Serna
 */
class dlg_toon : public QDialog, private Ui::dlg_toon
{
    Q_OBJECT

public:

    /**
     * Constructor
     * \param pParent pointer to parent widget
     * \param pPdbNode pointer to svt pdb node
     */
    dlg_toon(QWidget* pParent, svt_pdb * pPdbNode);

    /**
     * Destructor
     */ 
    virtual ~dlg_toon();

private:

    svt_pdb * m_pPdbNode;

};


#endif
