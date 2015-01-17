/***************************************************************************
                          sculptor_qwtplotcurve.cpp
			  ---------------------
    begin                : 11/04/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_qwtplotcurve.h>
#include <sculptor_dlg_plot.h>
#include <sculptor_qwtdata.h>
// qwt includes
#include <qwt_plot.h>


/**
 *
 */
sculptor_qwtplotcurve::sculptor_qwtplotcurve(QString oName, Real64* pXData, Real64* pYData, int iCount)
    : QwtPlotCurve(),
      m_oName( QString(oName) ),
      m_pData( new sculptor_qwtdata(iCount, pXData, pYData) )
{
    init();
}

/**
 *
 */
sculptor_qwtplotcurve::sculptor_qwtplotcurve(QString oName, vector<Real64 > oXData, vector<Real64 > oYData, int iCount)
    : QwtPlotCurve( oName ),
      m_oName( QString(oName) ),
      m_pData( new sculptor_qwtdata(iCount, oXData, oYData) )
{
    init();
}

/**
 * Destructor
 */
sculptor_qwtplotcurve::~sculptor_qwtplotcurve()
{
}

/**
 *
 */
void sculptor_qwtplotcurve::init()
{
    QPen oPen;
    setPen(oPen);
    setData(*m_pData);
    setYAxis( QwtPlot::yLeft );
}

/**
 *
 */
QString sculptor_qwtplotcurve::getName()
{
    return m_oName;
}


/**
 *
 */
Real64 sculptor_qwtplotcurve::getDataX(int i)
{
    return (Real64)(m_pData->x(i));
}

/**
 *
 */
Real64 sculptor_qwtplotcurve::getDataY(int i)
{
    return (Real64)(m_pData->y(i));
}

/**
 *
 */
int sculptor_qwtplotcurve::getSize()
{
    return (int)(m_pData->size());
}
    
/**
 *
 */
void sculptor_qwtplotcurve::push_back( Real64 fX, Real64 fY)
{
    m_pData->push_back( (double)fX, (double)fY );
}

/**
 *
 */
void sculptor_qwtplotcurve::insert( unsigned int i, Real64 fX, Real64 fY)
{
    m_pData->insert( i, (double)fX, (double)fY );
}

/**
 *
 */
void sculptor_qwtplotcurve::pop_back()
{
    m_pData->pop_back();
}

/**
 *
 */
void sculptor_qwtplotcurve::pop_front()
{
    m_pData->pop_front();
}

/**
 *
 */
void sculptor_qwtplotcurve::setColor(int iR, int iG, int iB)
{
    setPen( QPen(QColor(iR, iG, iB)) );
}
