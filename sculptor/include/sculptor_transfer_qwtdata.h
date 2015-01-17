/*-*-c++-*-*****************************************************************
			  sculptor_transfer_qwtdata.h
			  -----------------
    begin                : 09/30/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCULPTOR_TRANSFER_QWTDATA_H
#define __SCULPTOR_TRANSFER_QWTDATA_H


// svt includes
#include <svt_stlVector.h>
// qwt5 includes
#include <qwt_data.h>


/**
 * \brief A class that derives from QwtData and implements flexible curve point management.
 *
 * It points to two stl vectors storing the x and y coordinates of the points of a curve in
 * QwtPlot. Using the stl vectors makes it easy, flexible and safe.
 * \author Manuel Wahle
 */
class sculptor_transfer_qwtdata : public QwtData
{

    // pointers to the two vectors containing the curve points
    vector<double>* m_pX;
    vector<double>* m_pY;

public:

    /**
     * Constructor
     * \int iSize initial size. will be reserved in the vectors
     * \param pointer to vector<double> for the x-coord
     * \param pointer to vector<double> for the y-coord
     */
    sculptor_transfer_qwtdata(int iSize, vector<double>* pX, vector<double>* pY);

    /**
     * Destructor
     */
    ~sculptor_transfer_qwtdata();

    /**
     * QwtData "copy constructor"
     */
    virtual QwtData *copy() const;

    /**
     * returns the number of curve points
     * \return size_t number of curve points
     */
    virtual size_t size() const;

    /**
     * returns i-th x-coord
     * \return double
     */
    virtual double x(size_t i) const;

    /**
     * returns i-th y-coord
     * \return double
     */
    virtual double y(size_t i) const;

    /**
     * returns the current vector's capacity
     * \return size_t
     */
    size_t capacity() const;

    /**
     * reserves space in the vectors
     * \param size_t estimated number of points
     */
    void reserve(size_t i);

    /**
     * clears the vectors
     */
    void clear();

    /**
     * sets point number i to the given coords
     * \param size_t i the point to be changed
     * \param double x
     * \param double y
     */
    void set(size_t i, double x, double y);

    /**
     * sets point's number i x coord
     * \param size_t i the point to be changed
     * \param double x
     */
    void setX(size_t i, double x);

    /**
     * sets point's number i y coord
     * \param size_t i the point to be changed
     * \param double y
     */
    void setY(size_t i, double y);

    /**
     * inserts point at position i
     * \param size_t position at which to insert
     * \param double x
     * \param double y
     */
    void insert(size_t i, double x, double y);

    /**
     * adds point at the end
     * \param double x
     * \param double y
     */
    void push_back(double x, double y);

    /**
     * removes point number i
     * \param size_t which point to remove
     */
    void erase(size_t i);

    /**
     * removes all points from i to j (including)
     * \param size_t i first point to be removed
     * \param size_t j last point to be removed (including)
     */
    void erase(size_t i, size_t j);

    /**
     * removes first point
     */
    void pop_front();

    /**
     * removes last point
     */
    void pop_back();

};

#endif
