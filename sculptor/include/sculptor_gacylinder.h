/***************************************************************************
                          sculptor_gacylinder.h
                          -----------------
    begin                : 07.28.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_GACYLINDER_H
#define SCULPTOR_GACYLINDER_H

// svt includes
#include <svt_basics.h>
#include <svt_gacylinder.h>
// sculptor includes
#include <pdb_document.h>


/**
 *  trigger the ga timer
 */
void triggerGaCylTimer();


/**
 * This class stores an svt_gacylinder object
 * \author Mirabela Rusu
 */
template<class T> class sculptor_gacylinder : public svt_gacylinder<T>
{

protected:

    // the document that holds all the results
    pdb_document* m_pResults;

    // the document that holds only the top scoring results  (the number is given in the dialog)
    pdb_document* m_pBestResults;

public:
    
    /**
     * Constructor
     */
    sculptor_gacylinder( unsigned int iGenes )
	: svt_gacylinder<T>( iGenes ),
	  m_pResults( NULL ),
          m_pBestResults( NULL )
    {
    };

    /**
     * Destructor
     */
    virtual ~sculptor_gacylinder( )
    {
    };

    /**
     * update result
     */
    virtual void updateResults( unsigned int iNoOfTubes = 0, int iNum = 0 )
    {
        iNoOfTubes = 0;
        iNum = 0; 
	    //triggerGaCylTimer();
	this->setNewResultsFound (true);
    };

    /**
     * set the results document
     */
    void setResultsDoc( pdb_document* pResults )
    {
        m_pResults = pResults;
    };

    /**
     * set the results document
     */
    pdb_document* getResultsDoc()
    {
	return m_pResults;
    };
  
    /**
     * set the results document
     */
    void setBestResultsDoc( pdb_document* pResults )
    {
	m_pBestResults = pResults;
    };

    /**
     * set the results document
     */
    pdb_document* getBestResultsDoc()
    {
	return m_pBestResults;
    };


};

#endif

