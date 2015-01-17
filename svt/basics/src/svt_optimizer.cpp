#include <svt_optimizer.h>

/**
 * Constructor
 */
svt_optimizer::svt_optimizer():
    m_fTolerance(1.0E-6),
    m_iMaxIter (100),
    m_iMaxIter2Bracket(20)
{
};

svt_optimizer::~svt_optimizer()
{
};

/**
 * set start vector (call before maximize())
 * \param aVector stl-vector object, size determines the dimsionality of the search
 */
void svt_optimizer::setVector(vector<Real64> aVector)
{
    m_aVector = aVector;
};

/**
 * return current vector, call after maximize.
 * \return pointer
 */
vector<Real64> svt_optimizer::getVector()
{
    return m_aVector;
};

/**
 * set initial search direction
 * \param aDirection stl-vector object
 */
/*    inline void setDirection(vector<Real64> aDirection)
{
    m_aDirection = aDirection;
};*/

/**
 * get dimensionality of the search
 */
unsigned int svt_optimizer::getDimensions()
{
    return m_aVector.size();
};

/**
 * set maximum number of iteration
 * \param iMaxIter maximum number of iterations
 */
void svt_optimizer::setMaxIter(unsigned int iMaxIter)
{
    m_iMaxIter = iMaxIter;
};

/**
 * set maximum number of iteration in the bracket search
 * \param iMaxIter2Bracket maximum number of iterations
 */
void svt_optimizer::setMaxIter2Bracket(unsigned int iMaxIter2Bracket)
{
    m_iMaxIter2Bracket = iMaxIter2Bracket;
};

/**
 * get maximum number of iteration in the bracket search
 * \return iMaxIter2Bracket maximum number of iterations
 */
unsigned int svt_optimizer::getMaxIter2Bracket()
{
    return m_iMaxIter2Bracket;
};

/**
 * set tolerance, if change is smaller the algorithm stops
 * \param fTolerance tolerance
 */
void svt_optimizer::setTolerance(Real64 fTolerance)
{
    m_fTolerance = fTolerance;
};

/**
 * Scoring function, please overload!
 */
Real64 svt_optimizer::score(vector<Real64>)
{
    return 0.0;
}

/**
 * Return the last time of the score calculation
 * \return the time
 */
Real64 svt_optimizer::getScoreTime()
{
    return 0.0;
};

/**
 * Return the number of times the score was computed
 **/
unsigned int svt_optimizer::getScoreCount()
{
    return 0;
};

