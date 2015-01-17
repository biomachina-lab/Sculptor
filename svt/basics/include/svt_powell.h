/***************************************************************************
                          svt_powell
			  ----------
    begin                : 07/22/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_POWELL_H
#define SVT_POWELL_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_stlVector.h>

/**
 * A powell maximizer.
 * Attention, this is only a C++ wrapper around an older C implementation. This is not meant to be thread-safe! Especially,
 * only one maximize function call can run at the same time!!
 */
class DLLEXPORT svt_powell
{
protected:

    Real64 m_fTolerance;
    FILE* m_pFile;
    vector<Real64> m_aVector; // current/initial vector
    Real64 m_fScore;   // current/initial score of the vector
    vector<Real64> m_aDirection; // initial search direction
    
    //maximum number of total iteration - go trough all the elements of the vector
    unsigned int m_iMaxIter;

    // the maximum number of iterations per bracket
    unsigned int m_iMaxIter2Bracket;
   
    // the maximum numner of iterations per minimum search after the breaket was identified
    unsigned int m_iMaxIter2Minimize;

    // the current Line - variable in the vector that is optimized at this step
    int m_iLine;

public:

    /**
     * Constructor
     */
    svt_powell();
   
    /**
     * Destructor  
     */
    virtual ~svt_powell();
   
    /**
     * set start vector (call before maximize())
     * \param aVector stl-vector object, size determines the dimsionality of the search
     */
    inline void setVector(vector<Real64> aVector);
    
    /**
     * set vector
     * \param pVector Real64* object
     */
    void setVector(Real64* pVector);
 
    /**
     * return current vector, call after maximize.
     * \return pointer
     */
    vector<Real64> getVector();
    
    /**
     * set initial search direction
     * \param aDirection stl-vector object
     */
    void setDirection(vector<Real64> aDirection);
   
    /**
     * get dimensionality of the search
     */
    inline unsigned int getDimensions();
  
    /**
     * set maximum number of iteration
     * \param iMaxIter maximum number of iterations
     */
    void setMaxIter(unsigned int iMaxIter);
 
    /**
     * set maximum number of iteration in the bracket search
     * \param iMaxIter2Bracket maximum number of iterations
     */
    void setMaxIter2Bracket(unsigned int iMaxIter2Bracket);

    /**
     * set maximum number of iteration in the minimization - once the bracket was identified
     * \param iMaxIter2Minimize maximum number of iterations
     */
    void setMaxIter2Minimize(unsigned int);

    /**
     * get maximum number of iteration in the bracket search
     * \return iMaxIter2Bracket maximum number of iterations
     */
    inline unsigned int getMaxIter2Bracket();

    /**
     * get maximum number of iteration in the minimization - once the bracket was identified
     * \param iMaxIter2Minimize maximum number of iterations
     */
    inline unsigned int getMaxIter2Minimize();

    /**
     * set tolerance, if change is smaller the algorithm stops
     * \param fTolerance tolerance
     */
    void setTolerance(Real64 fTolerance);

    /**
     * set output file
     * \param pFile pointer FILE
     */
    inline void setOutputFile(FILE* pFile);
   
    /**
     * set current line 
     * \param iLine the line number
     */
    virtual void setLine(int iLine);

    /**
     * maximize the scoring function
     * \return true if it has converged, false if it did not reach tolerance
     */
    bool maximize();

    /**
     * Scoring function, please overload!
     */
    virtual Real64 score(vector<Real64>&);

    /**
     * Return the last time of the score calculation
     * \return the time
     */
    Real64 getScoreTime();
   
    /**
     * Return the number of times the score was computed
     **/
    unsigned int getScoreCount();

    /**
     * Outputs some sort of results; please overload
     */
    virtual void outputResult();
};

#endif
