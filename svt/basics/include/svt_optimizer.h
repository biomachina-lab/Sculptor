/***************************************************************************
                          svt_optimizer
			  ----------
    begin                : 22/03/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_OPTIMIZER_H
#define SVT_OPTIMIZER_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_stlVector.h>

/**
 * A basic optimizer class to be used as parent of optimizers
 */
class DLLEXPORT svt_optimizer
{
protected:

    //Stop Criteria
    Real64 m_fTolerance;

    // current/initial vector
    vector<Real64> m_aVector; 

    // current/initial score of the vector
    Real64 m_fScore;   
  
    // initial search direction
    //vector<Real64> m_aDirection; 
    
    //maximum number of total iteration - go trough all the elements of the vector
    unsigned int m_iMaxIter;

    // the maximum number of iterations per bracket
    unsigned int m_iMaxIter2Bracket;

public:

    /**
     * Constructor
     */
    svt_optimizer();
    
    /** 
     * Destructor 
     */
    virtual ~svt_optimizer();

    /**
     * set start vector (call before maximize())
     * \param aVector stl-vector object, size determines the dimsionality of the search
     */
    inline void setVector(vector<Real64> aVector);

    /**
     * return current vector, call after maximize.
     * \return pointer
     */
    inline vector<Real64> getVector();

    /**
     * set initial search direction
     * \param aDirection stl-vector object
     */
    /* inline void setDirection(vector<Real64> aDirection)
    {
	m_aDirection = aDirection;
    };*/

    /**
     * get dimensionality of the search
     */
    inline unsigned int getDimensions();
   
    /**
     * set maximum number of iteration
     * \param iMaxIter maximum number of iterations
     */
    inline void setMaxIter(unsigned int iMaxIter);

    /**
     * set maximum number of iteration in the bracket search
     * \param iMaxIter2Bracket maximum number of iterations
     */
    inline void setMaxIter2Bracket(unsigned int iMaxIter2Bracket);

    /**
     * get maximum number of iteration in the bracket search
     * \return iMaxIter2Bracket maximum number of iterations
     */
    inline unsigned int getMaxIter2Bracket();

    /**
     * set tolerance, if change is smaller the algorithm stops
     * \param fTolerance tolerance
     */
    inline void setTolerance(Real64 fTolerance);
   
    /**
     * maximize the scoring function
     * \return true if it has converged, false if it did not reach tolerance
     */
    virtual bool maximize()= 0;

    /**
     * Scoring function, please overload!
     */
    virtual Real64 score(vector<Real64>);

    /**
     * Return the last time of the score calculation
     * \return the time
     */
    Real64 getScoreTime();
   
    /**
     * Return the number of times the score was computed
     **/
    unsigned int getScoreCount();
};

#endif
