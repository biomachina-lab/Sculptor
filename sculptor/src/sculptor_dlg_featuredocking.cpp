/***************************************************************************
                          sculptor_dlg_featuredockingdocking.cpp
                          -----------------
    begin                : 09.10.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_featuredocking.h>

/**
 * Constructor
 */
dlg_featuredocking::dlg_featuredocking(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);
};

/**
 * Destructor
 */
dlg_featuredocking::~dlg_featuredocking()
{
};


/**
 * get Lamdba
 */ 
Real64 dlg_featuredocking::getLambda()
{
    return m_pLambda->text().toDouble();
};

/**
 * get Gamma
 */ 
Real64 dlg_featuredocking::getGamma()
{
    return m_pGamma->text().toDouble();
};

/**
 * get MatchingZone
 */ 
int dlg_featuredocking::getMatchingZone()
{
    return m_pMatchingZone->text().toInt();
};

/**
 * get MaxSolutions
 */ 
int dlg_featuredocking::getMaxSolutions()
{
    return m_pMaxSolutions->text().toInt();
};

/**
 * get Wildcards
 */ 
int dlg_featuredocking::getWildcards()
{
    return m_pWildcards->text().toInt();
};

/**
 * get SkipPenalty
 */ 
Real64 dlg_featuredocking::getSkipPenalty()
{
    return m_pSkipPenalty->text().toDouble();
};



