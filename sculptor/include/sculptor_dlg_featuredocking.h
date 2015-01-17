/***************************************************************************
                          sculptor_dlg_featuredocking.h
                          -----------------
    begin                : 10.05.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_FEATUREDOCKING_H
#define SCULPTOR_DLG_FEATUREDOCKING_H

// uic-qt4 includes
#include <ui_dlg_featuredocking.h>
#include <svt_types.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_featuredocking : public QDialog, private Ui::dlg_featuredocking
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_featuredocking(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_featuredocking();

    /**
     * get Lamdba
     */ 
    Real64 getLambda();

    /**
     * get Gamma
     */ 
    Real64 getGamma();

    /**
     * get MatchingZone
     */ 
    int getMatchingZone();

    /**
     * get MaxSolutions
     */ 
    int getMaxSolutions();

    /**
     * get Wildcards
     */ 
    int getWildcards();

    /**
     * get SkipPenalty
     */ 
    Real64 getSkipPenalty();
};

#endif
