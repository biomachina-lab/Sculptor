/*-*-c++-*-*****************************************************************
                          sculptor_qwtplotcurve.h
                          ---------------------
    begin                : 11/04/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_QWTPLOTCURVE_H
#define SCULPTOR_QWTPLOTCURVE_H

// sculptor includes
class sculptor_qwtdata;
class dlg_plot;
// svt includes
#include <svt_core.h>
#include <svt_types.h>
// qwt includes
class QwtPlot;
#include <qwt_plot_curve.h>
// stl include
#include <svt_stlVector.h>

/**
 * \brief A class for curves.
 *
 * It inherits from QwtPlotCurve and additionally provides some convenience methods.
 * \author Manuel Wahle
 */
class sculptor_qwtplotcurve : public QwtPlotCurve
{
    // the name of the curve
    QString m_oName;

    // the curve data
    sculptor_qwtdata* m_pData;

    /**
     * private member to be called from the constructor to set up things
     */
    void init();

public:

    /**
     * create a new curve
     * \param oName QString the name of the curve
     * \param pXData pointer to an array of Real64 with the x data of the curve
     * \param pYData pointer to an array of Real64 with the y data of the curve
     * \param iCount number of values
     */
    sculptor_qwtplotcurve(QString oName, Real64* pXData, Real64* pYData, int iCount);
    
    /**
     * create a new curve
     * \param oName QString the name of the curve
     * \param pXData pointer to vector of Real64 with the x data of the curve
     * \param pYData pointer to vector of Real64 with the y data of the curve
     * \param iCount number of values
     */
    sculptor_qwtplotcurve(QString oName, vector<Real64> oXData, vector<Real64> oYData, int iCount);
            
    /**
     * Destructor
     */
    virtual ~sculptor_qwtplotcurve();

    /**
     * get curve name
     */
    QString getName();

    /**
     * get data value (x-axis)
     * \param i index of data value
     * \return data itself
     */
    Real64 getDataX(int i);

    /**
     * get data value (y-axis)
     * \param i index of data value
     * \return data itself
     */
    Real64 getDataY(int i);

    /**
     * get number of data values
     * \return number of data values of the curve
     */
    int getSize();
    
    /**
     * Add data point at end
     * \param fX
     * \param fY
     */
    void push_back( Real64 fX, Real64 fY);

    /**
     * Insert data point at index i
     * \param fX
     * \param fY
     */
    void insert( unsigned int i, Real64 fX, Real64 fY);

    /**
     * Remove data point at end
     */
    void pop_back();

    /**
     * Remove data point at beginning
     */
    void pop_front();

    /**
     * Set the color of the curve
     */
    void setColor(int iR, int iG, int iB);
};

#endif
