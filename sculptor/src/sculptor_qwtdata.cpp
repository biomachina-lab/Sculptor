/***************************************************************************
			  sculptor_transfer_qwtdata.h
			  -----------------
    begin                : 09/30/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_qwtdata.h>


/**
 * Constructor
 */
sculptor_qwtdata::sculptor_qwtdata()
    : QwtData(),
      m_pX( new vector<double> ),
      m_pY( new vector<double> )
{
}

/**
 * Constructor
 * \int iSize initial size. will be reserved in the vectors. this is only for efficiency reasons if
 * data size is known beforehand
 */
sculptor_qwtdata::sculptor_qwtdata(int iSize)
    : QwtData(),
      m_pX( new vector<double> ),
      m_pY( new vector<double> )
{
    // reserve requested space
    m_pX->reserve( iSize );
    m_pY->reserve( iSize );
}

/**
 * Constructor
 * \int iSize initial size. will be reserved in the vectors
 * \param vector<double> for the x-coord
 * \param vector<double> for the y-coord
 */
sculptor_qwtdata::sculptor_qwtdata(int iSize, vector<double>& oX, vector<double>& oY)
    : QwtData(),
      m_pX( new vector<double> ),
      m_pY( new vector<double> )
{
    // reserve requested space
    m_pX->reserve( iSize );
    m_pY->reserve( iSize );

    // copy over the values
    for (int i=0; i<iSize; ++i)
    {
        m_pX->push_back(oX[i]);
        m_pY->push_back(oY[i]);
    }
}

/**
 * Constructor
 * \int iSize initial size. will be reserved in the vectors
 * \param pX pointer to an array with Real64s, that will be copied into the internal vector for
 * y-data storage
 * \param pYpointer to an array with Real64s, that will be copied into the internal vector for
 * x-data storage
 */
sculptor_qwtdata::sculptor_qwtdata(int iSize, Real64* pX, Real64* pY)
    : QwtData(),
      m_pX( new vector<double> ),
      m_pY( new vector<double> )
{
    // reserve requested space
    m_pX->reserve( iSize );
    m_pY->reserve( iSize );

    // copy over the values
    for (int i=0; i<iSize; ++i)
    {
        m_pX->push_back(pX[i]);
        m_pY->push_back(pY[i]);
    }
}

/**
 * Destructor
 */
sculptor_qwtdata::~sculptor_qwtdata()
{
    delete m_pX;
    delete m_pY;
}

/**
 * QwtData "copy constructor"
 */
QwtData* sculptor_qwtdata::copy() const
{
    return (QwtData*) this;
}

/**
 * returns the number of curve points
 * \return size_t number of curve points
 */
size_t sculptor_qwtdata::size() const
{
    return m_pX->size();
}

/**
 * returns i-th x-coord
 * \return double
 */
double sculptor_qwtdata::x(size_t i) const
{
    return (*m_pX)[i];
}

/**
 * returns i-th y-coord
 * \return double
 */
double sculptor_qwtdata::y(size_t i) const
{
    return (*m_pY)[i];
}

/**
 * returns the current vector's capacity
 * \return size_t
 */
size_t sculptor_qwtdata::capacity() const
{
    return m_pX->capacity();
}

/**
 * reserves space in the vectors
 * \param size_t estimated number of points
 */
void sculptor_qwtdata::reserve(size_t i)
{
    m_pX->reserve(i);
    m_pY->reserve(i);
}

/**
 * clears the vectors
 */
void sculptor_qwtdata::clear()
{
    m_pX->clear();
    m_pY->clear();
}

/**
 * sets point number i to the given coords
 * \param size_t i the point to be changed
 * \param double x
 * \param double y
 */
void sculptor_qwtdata::set(size_t i, double x, double y)
{
    (*m_pX)[i] = x;
    (*m_pY)[i] = y;
}

/**
 * sets point's number i x coord
 * \param size_t i the point to be changed
 * \param double x
 */
void sculptor_qwtdata::setX(size_t i, double x)
{
    (*m_pX)[i] = x;
}

/**
 * sets point's number i y coord
 * \param size_t i the point to be changed
 * \param double y
 */
void sculptor_qwtdata::setY(size_t i, double y)
{
    (*m_pY)[i] = y;
}

/**
 * inserts point at position i
 * \param size_t position at which to insert
 * \param double x
 * \param double y
 */
void sculptor_qwtdata::insert(size_t i, double x, double y)
{
    vector<double>::iterator itr = m_pX->begin();
    itr += i;
    m_pX->insert(itr, x);

    itr = m_pY->begin();
    itr += i;
    m_pY->insert(itr, y);
}

/**
 * adds point at the end
 * \param double x
 * \param double y
 */
void sculptor_qwtdata::push_back(double x, double y)
{
    m_pX->push_back(x);
    m_pY->push_back(y);
}

/**
 * removes point number i
 * \param size_t which point to remove
 */
void sculptor_qwtdata::erase(size_t i)
{
    vector<double>::iterator itr = m_pX->begin();
    itr += i;
    m_pX->erase(itr);

    itr = m_pY->begin();
    itr += i;
    m_pY->erase(itr);
}

/**
 * removes all points from i to j (including)
 * \param size_t i first point to be removed
 * \param size_t j last point to be removed (including)
 */
void sculptor_qwtdata::erase(size_t i, size_t j)
{
    vector<double>::iterator start = m_pX->begin();
    vector<double>::iterator end = start;
    start += i;
    end += (++j);
    m_pX->erase(start, end);

    start = m_pY->begin();
    end = start;
    start += i;
    end += (++j);
    m_pY->erase(start, end);
}

/**
 * removes first point
 */
void sculptor_qwtdata::pop_front()
{
    m_pX->erase( m_pX->begin() );
    m_pY->erase( m_pY->begin() );
}

/**
 * removes last point
 */
void sculptor_qwtdata::pop_back()
{
    m_pX->pop_back();
    m_pY->pop_back();
}
