/***************************************************************************
                          sculptor_gamultifit.h
                          -----------------
    begin                : 12.18.2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_GAMULTIFIT_H
#define SCULPTOR_GAMULTIFIT_H

//svt includes
#include <pdb_document.h>
#include <svt_basics.h>
#include <svt_gamultifit.h>

/** This class stores a svt_gamultif object
 * @author Mirabela Rusu
 */
template<class T> class sculptor_gamultifit : public svt_gamultifit<T>
{
protected:
    //pointers to the documents used to create the ga object
    vector< pdb_document *> m_oDocs;

    //
    sculptor_document * m_pTarget;
public:
    
    /**
     * constructor
     */
    sculptor_gamultifit( unsigned int iGenes );
    
    /**
     * set documents
     * \param a vector with the pointers to the documents used
     */
    void setDocs( vector< pdb_document*> &oDocs );

    /**
     *  get documents
     */ 
    vector<pdb_document* >& getDocs();
	
    /**
     * 	set the target
     * 	\param the target a pointer to a map
     */
     void setTarget( sculptor_document *pTarget );

    /**
     * get targe
     * \return the target a pointer to a map
     */
    sculptor_document* getTarget();

};

/**
 * Constructor
 */
template<class T> 
sculptor_gamultifit<T>::sculptor_gamultifit(unsigned int iGenes) : svt_gamultifit<T>(iGenes),
	m_pTarget(NULL)
{
    for (unsigned int iUnit=0; iUnit<iGenes/4; iUnit++)
		m_oDocs.push_back( NULL );
};
 
/**
 * set documents
 * \param a vector with the pointers to the documents used
 */
template<class T> 
void sculptor_gamultifit<T>::setDocs( vector< pdb_document*> &oDocs ) 
{ 
    m_oDocs = oDocs;
};

/**
 *  get documents
 */ 
template<class T> 
vector<pdb_document* >& sculptor_gamultifit<T>::getDocs() 
{ 
    return m_oDocs; 
};
/**
 * 	set the target
 * 	\param the target a pointer to a map
 */
template<class T> 
void sculptor_gamultifit<T>::setTarget( sculptor_document *pTarget )
{
    m_pTarget = pTarget;
};

/**
 * get targe
 * \return the target a pointer to a map
 */
template<class T>
sculptor_document* sculptor_gamultifit<T>::getTarget()
{
    return m_pTarget;
};



#endif

