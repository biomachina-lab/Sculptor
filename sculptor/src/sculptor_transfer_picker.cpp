/***************************************************************************
			  sculptor_transfer_picker
			  ------------------------
    begin                : 03/03/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////


// sculptor includes
#include <sculptor_transfer.h>
#include <sculptor_transfer_picker.h>
#include <sculptor_qwtdata.h>
// svt includes
#include <svt_iostream.h>
#include <svt_cmath.h>
// qt4 includes
#include <QMouseEvent>
#include <QColorDialog>
#include <QPainter>
// qwt5 includes
#include <qwt_painter.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
// forward declarations
class sculptor_window;

extern sculptor_window* g_pWindow;


///////////////////////////////////////////////////////////////////////////////
// First the little rectangle class for highlighting an area
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
sculptor_transfer_rectitem::sculptor_transfer_rectitem()
    : QwtPlotItem()
{
}

/**
 *
 */
void sculptor_transfer_rectitem::setPen(const QPen &oPen)
{
    m_oPen = oPen;
    itemChanged();
}

/**
 *
 */
void sculptor_transfer_rectitem::setBrush(const QBrush &oBrush)
{
    m_oBrush = oBrush;
    itemChanged();
}

/**
 *
 */
void sculptor_transfer_rectitem::setRect(const QwtDoubleRect& oRect)
{
    if ( m_oRect != oRect )
    {
	m_oRect = oRect;
	itemChanged();
    }
}

/**
 *
 */
void sculptor_transfer_rectitem::draw(QPainter* pPainter, const QwtScaleMap& oMapX,
				      const QwtScaleMap& oMapY, const QRect& ) const
{
    if ( m_oRect.isValid() )
    {
	const QRect oRect = transform(oMapX, oMapY, m_oRect);
	pPainter->setPen(m_oPen);
	pPainter->setBrush(m_oBrush);
	QwtPainter::drawRect(pPainter, oRect);
    }
}


///////////////////////////////////////////////////////////////////////////////
// The transfer picker
///////////////////////////////////////////////////////////////////////////////


/*
 * Constructor
 */
sculptor_transfer_picker::sculptor_transfer_picker(QwtPlotCanvas* pCanvas)
    : QwtPlotPicker( pCanvas ),
      m_pCurveR( NULL ),
      m_pCurveG( NULL ),
      m_pCurveB( NULL ),
      m_pCurveO( NULL ),
      m_pDataR( NULL ),
      m_pDataG( NULL ),
      m_pDataB( NULL ),
      m_pDataO( NULL ),
      m_pSelectedCurve( NULL ),
      m_pSelectedCurveData( NULL ),
      m_eSelectedCurve( -1 ),
      m_iSelectedCurvePoint( -1 ),
      m_fMinDensity( 0.0 ),
      m_fMaxDensity( 0.0 ),
      m_fMousePressPos_X( 0.0 ),
      m_bMoved( false )
{
    // set up the marker for a selected point
    QwtSymbol oSym;
    oSym.setBrush( QBrush(QColor(Qt::red)) );
    oSym.setSize( 9 );
    oSym.setStyle( QwtSymbol::Diamond );
    m_oSelectionMarker.setSymbol( oSym);
    m_oSelectionMarker.setYAxis( QwtPlot::yRight );

    // set up the markers that mark start and end of a range
    m_oRangeMarker.setPen( QPen( Qt::NoPen) );
    m_oRangeMarker.setBrush( QBrush(QColor(Qt::gray), Qt::Dense5Pattern) );
    m_oRangeMarker.setYAxis( QwtPlot::yRight );

    // we want points to be determined, and allow for drag selection (i.e., moving mouse with button pressed)
    setSelectionFlags( QwtPicker::PointSelection | QwtPicker::DragSelection );

    // set the axis where the QwtDoublePoint in the moved signal gets its values from
    setAxis(QwtPlot::xBottom, QwtPlot::yRight);

    // and connect the signal to the respective slot
    connect( this, SIGNAL(moved(const QwtDoublePoint&)), this, SLOT(sMoved(const QwtDoublePoint&)) );

    // the context menu
    m_pActionRemove = m_oContextMenu.addAction( QString("Remove Point"),  this, SLOT(sRemovePoint()) );//, QKeySequence(Qt::Key_Delete) );
    m_oContextMenu.addSeparator();
    m_oContextMenu.addAction( QString("Reset Red"),     this, SLOT(sResetCurveR()) );
    m_oContextMenu.addAction( QString("Reset Green"),   this, SLOT(sResetCurveG()) );
    m_oContextMenu.addAction( QString("Reset Blue"),    this, SLOT(sResetCurveB()) );
    m_oContextMenu.addAction( QString("Reset Opacity"), this, SLOT(sResetCurveO()) );
    //m_oContextMenu.setWindowOpacity(0.5);
}

/*
 * Destructor
 */
sculptor_transfer_picker::~sculptor_transfer_picker()
{
}

/**
 *
 */
void sculptor_transfer_picker::showContextMenu(QPoint oPoint)
{
    // only if a point is selected, activate the menu item to delete a point
    if (m_iSelectedCurvePoint < 0)
        m_pActionRemove->setEnabled( FALSE );
    else
        m_pActionRemove->setEnabled( TRUE );

    // show the contect menu
    m_oContextMenu.popup(oPoint);
}

/**
 *
 */
void sculptor_transfer_picker::sRemovePoint()
{
    // no point selected -- nothing to remove
    if ( m_iSelectedCurvePoint == -1 )
        return;

    // if first or last point to be removed, just set to boundary values
    if ( m_iSelectedCurvePoint == 0 )
        m_pSelectedCurveData->set( m_iSelectedCurvePoint, m_fMinDensity, 0.0 );
    else if ( m_iSelectedCurvePoint == (int) m_pSelectedCurveData->size()-1 )
        m_pSelectedCurveData->set( m_iSelectedCurvePoint, m_fMaxDensity, 255.0 );
    else
        m_pSelectedCurveData->erase( m_iSelectedCurvePoint );

    // deselect the removed point
    m_iSelectedCurvePoint = -1;
    m_oSelectionMarker.detach();
    emit plotChanged();
}

/**
 *
 */
void sculptor_transfer_picker::sResetCurveR()
{
    // clear the curve data and insert start and end point
    m_pDataR->clear();
    m_pDataR->push_back( m_fMinDensity, 0.0 );
    m_pDataR->push_back( m_fMaxDensity, 255.0 );

    // if a point on the cleared curve was selected, unselect it now
    if ( m_eSelectedCurve == sculptor_transfer::TRANS_ACTIVE_RED && m_iSelectedCurvePoint != -1 )
    {
        m_oSelectionMarker.detach();
        m_iSelectedCurvePoint = -1;
    }
    emit plotChanged();
}

/**
 *
 */
void sculptor_transfer_picker::sResetCurveG()
{
    m_pDataG->clear();
    m_pDataG->push_back( m_fMinDensity, 0.0 );
    m_pDataG->push_back( m_fMaxDensity, 255.0 );

    if ( m_eSelectedCurve == sculptor_transfer::TRANS_ACTIVE_GREEN && m_iSelectedCurvePoint != -1 )
    {
        m_oSelectionMarker.detach();
        m_iSelectedCurvePoint = -1;
    }
    emit plotChanged();
}

/**
 *
 */
void sculptor_transfer_picker::sResetCurveB()
{
    m_pDataB->clear();
    m_pDataB->push_back( m_fMinDensity, 0.0 );
    m_pDataB->push_back( m_fMaxDensity, 255.0 );

    if ( m_eSelectedCurve == sculptor_transfer::TRANS_ACTIVE_BLUE && m_iSelectedCurvePoint != -1 )
    {
        m_oSelectionMarker.detach();
        m_iSelectedCurvePoint = -1;
    }
    emit plotChanged();
}

/**
 *
 */
void sculptor_transfer_picker::sResetCurveO()
{
    m_pDataO->clear();
    m_pDataO->push_back( m_fMinDensity, 0.0 );
    m_pDataO->push_back( m_fMaxDensity, 255.0 );

    if ( m_eSelectedCurve == sculptor_transfer::TRANS_ACTIVE_OPACITY && m_iSelectedCurvePoint != -1 )
    {
        m_oSelectionMarker.detach();
        m_iSelectedCurvePoint = -1;
    }
    emit plotChanged();
}

/**
 *
 */
void sculptor_transfer_picker::selectComponent(int eCurve)
{
    // remember the selected curve and also set a pointer to it
    m_eSelectedCurve = eCurve;

    switch (m_eSelectedCurve)
    {
	case sculptor_transfer::TRANS_ACTIVE_RED:
	    m_pSelectedCurve = m_pCurveR;
	    m_pSelectedCurveData = m_pDataR;
	    break;

	case sculptor_transfer::TRANS_ACTIVE_GREEN:
	    m_pSelectedCurve = m_pCurveG;
	    m_pSelectedCurveData = m_pDataG;
	    break;

	case sculptor_transfer::TRANS_ACTIVE_BLUE:
	    m_pSelectedCurve = m_pCurveB;
	    m_pSelectedCurveData = m_pDataB;
	    break;

	case sculptor_transfer::TRANS_ACTIVE_OPACITY:
	    m_pSelectedCurve = m_pCurveO;
	    m_pSelectedCurveData = m_pDataO;
	    break;

	default:
	    m_pSelectedCurve = NULL;
	    m_pSelectedCurveData = NULL;
	    break;
    }

    m_iSelectedCurvePoint = -1;
    m_oSelectionMarker.detach();
}

/**
 *
 */
void sculptor_transfer_picker::setMinMaxDensity(double fMin, double fMax)
{
    m_fMaxDensity = fMax;
    m_fMinDensity = fMin;
}

/**
 *
 */
void sculptor_transfer_picker::initCurves(QwtPlotCurve* pCurveR, sculptor_qwtdata* pDataR,
                                          QwtPlotCurve* pCurveG, sculptor_qwtdata* pDataG,
                                          QwtPlotCurve* pCurveB, sculptor_qwtdata* pDataB,
                                          QwtPlotCurve* pCurveO, sculptor_qwtdata* pDataO)
{
    m_pCurveR = pCurveR; m_pDataR = pDataR;
    m_pCurveG = pCurveG; m_pDataG = pDataG;
    m_pCurveB = pCurveB; m_pDataB = pDataB;
    m_pCurveO = pCurveO; m_pDataO = pDataO;
}

/**
 *
 */
void sculptor_transfer_picker::widgetMousePressEvent(QMouseEvent* e)
{
    double fDist;

    if ( e->button() == Qt::RightButton )
    {
        // check if right click occured close to a point in the selected curve,
        // and if so, select that point
        if ( m_eSelectedCurve != sculptor_transfer::TRANS_ACTIVE_RGB )
        {
            // determine the distance from the click position to the nearest point on the curve
            if ( m_pSelectedCurve != NULL )
                m_iSelectedCurvePoint = m_pSelectedCurve->closestPoint( e->pos(), &fDist );
            else
                m_iSelectedCurvePoint = -1;

            // clicked close enough to the closest point, so select and highlight it
            if ( m_iSelectedCurvePoint != -1 && fDist <= 10 )
            {
                // position the marker
                m_oSelectionMarker.setValue( m_pSelectedCurve->x(m_iSelectedCurvePoint),
                                             m_pSelectedCurve->y(m_iSelectedCurvePoint) );

                // attach marker
                m_oSelectionMarker.attach( m_pSelectedCurve->plot() );
                emit plotChanged();
            }
        }

        // now show the context menu
        showContextMenu( e->globalPos() );
    }
    else
    {
        double fX, fY;

        // moving starts on mouse press, so so far no movement
        m_bMoved = false;


        // if R, G, B, or O curve is selected, a point may be selected or added
        if ( m_eSelectedCurve != sculptor_transfer::TRANS_ACTIVE_RGB )
        {

            // determine the distance from the click position to the nearest point on the curve
            if ( m_pSelectedCurve != NULL )
                m_iSelectedCurvePoint = m_pSelectedCurve->closestPoint( e->pos(), &fDist );
            else
                m_iSelectedCurvePoint = -1;


            // clicked close enough to the closest point, so select and highlight it
            if ( m_iSelectedCurvePoint != -1 && fDist <= 10 )
            {
                // position the marker
                m_oSelectionMarker.setValue( m_pSelectedCurve->x(m_iSelectedCurvePoint),
                                             m_pSelectedCurve->y(m_iSelectedCurvePoint) );

                // attach marker
                m_oSelectionMarker.attach( m_pSelectedCurve->plot() );

            }
            else // clicked far away from the point
            {

                // get current position
                fY = plot()->invTransform( QwtPlot::yRight, e->pos().y() );
                svt_clamp( fY, 0.0, 255.0 );

                fX = plot()->invTransform( QwtPlot::xBottom, e->pos().x() );
                svt_clamp( fX, m_fMinDensity, m_fMaxDensity );

                // add point and select it
                m_iSelectedCurvePoint = addPoint( fX, fY, m_pSelectedCurve, m_pSelectedCurveData );

                // attach marker
                m_oSelectionMarker.attach( m_pSelectedCurve->plot() );
                m_oSelectionMarker.setValue( fX, fY );
            }

            emit plotChanged();
        }

        // user may start dragging, so remember x-coordinate
        m_fMousePressPos_X = plot()->invTransform( QwtPlot::xBottom, e->pos().x() );
        svt_clamp( m_fMousePressPos_X, m_fMinDensity, m_fMaxDensity );
    }


    // let the widget do its mouse press procedure
    // (to start emitting the moved signal, and who knows what)
    QwtPlotPicker::widgetMousePressEvent(e);
}

/**
 *
 */
void sculptor_transfer_picker::widgetMouseReleaseEvent(QMouseEvent* e)
{
    int iBack;
    bool bPlotChanged = false;


    // get the plot x-coordinate from the mouse release event and clamp it
    double fX = plot()->invTransform( QwtPlot::xBottom, e->pos().x() );
    svt_clamp( fX, m_fMinDensity, m_fMaxDensity );


    // mouse was moved with button down
    if ( m_bMoved )
    {

	// RGB was selected, so user wants to color a range
	if ( m_eSelectedCurve == sculptor_transfer::TRANS_ACTIVE_RGB )
	{

	    // color a range
	    if ( fX > m_fMousePressPos_X )
	    {
		putRange( m_fMousePressPos_X, fX );
		m_oRangeMarker.detach();
		bPlotChanged = true;
	    }
	    else if ( fX < m_fMousePressPos_X )
	    {
		putRange( fX, m_fMousePressPos_X );
		m_oRangeMarker.detach();
		bPlotChanged = true;
	    }
	    else // start and end x-position the same
	    {
		return;
	    }

	}

	// if a selected point was moved, check if duplicate points on the y-axes exist ...
	else if ( m_iSelectedCurvePoint != -1 )
	{

	    // ... on left y-axis
	    while ( m_pSelectedCurveData->x(0) >= m_pSelectedCurveData->x(1) ) // == instead of >= should be sufficient
	    {
		m_pSelectedCurveData->pop_front();
		bPlotChanged = true;
	    }

	    // ... on right y-axis
	    iBack = m_pSelectedCurveData->size() - 1;

	    while ( m_pSelectedCurveData->x(iBack-1) >= m_pSelectedCurveData->x(iBack) ) // == instead of >= should be sufficient
	    {
		m_pSelectedCurveData->pop_back();
		bPlotChanged = true;
		--iBack;
	    }
	}

    }
    else // mouse was not moved: simple click
    {

	// if RGB component selected, ask for color and then adjust R, G, and B curve
	if ( m_eSelectedCurve == sculptor_transfer::TRANS_ACTIVE_RGB )
	{
	    // get color and call addPoint three times...

	    QColor oCol = QColorDialog::getColor( m_oColor, (QWidget*)g_pWindow );
	    if ( oCol.isValid() )
	    {
		addPoint(fX, oCol.red(),   m_pCurveR, m_pDataR);
		addPoint(fX, oCol.green(), m_pCurveR, m_pDataG);
		addPoint(fX, oCol.blue(),  m_pCurveR, m_pDataB);
	    }

	    // remember user color, to make it default next time
	    m_oColor = oCol;

	    bPlotChanged = true;
	}

	// clicked on selected point
	if ( m_iSelectedCurvePoint != -1 )
	{
	    // pop up context menu on right button?
	    // actually, this should be done one mouse button press...
	}
    }

    if (bPlotChanged)
	emit plotChanged();

    // let the widget do its mouse release procedure
    // (to stop emitting the moved signal, and who knows what)
    QwtPlotPicker::widgetMouseReleaseEvent(e);
}

/**
 *
 */
int sculptor_transfer_picker::addPoint(double fX, double fY, QwtPlotCurve* pCurve, sculptor_qwtdata* pData)
{
    int i, iSize = pData->size();

    // down sample to single precision to cut off rounding errors etc

    // if at the beginning, overwrite first point (y-coord is enough)
    if ( (float)fX <= (float)m_fMinDensity )
    {
	pData->setY( 0, fY );
	i = 0;
    }

    // if at the end, overwrite last point (y-coord is enough)
    else if ( (float)fX >= (float)m_fMaxDensity )
    {
	pData->setY( iSize-1, fY );
	i = iSize - 1;
    }

    // otherwise, actually insert new data point somewhere in between
    else
    {
	// find that somewhere
	for ( i=0; i<iSize; ++i )
	    if ( pData->x(i) >= fX )
		break;

	pData->insert( i, fX, fY );
    }

    // emit plotChanged signal
    emit plotChanged( pCurve );

    return i;
}

/**
 *
 */
void sculptor_transfer_picker::putRange(double fStart, double fEnd)
{
    // ask user to choose a color
    QColor oCol = QColorDialog::getColor( m_oColor, (QWidget*)g_pWindow );

    if ( oCol.isValid() )
    {
	// a range overwrites all points within. so need to delete all existing points between start
	// and end for all three curves

	clearRange( fStart, fEnd, m_pDataR );
	addPoint( fStart, (double)(oCol.red()),   m_pCurveR, m_pDataR );
	addPoint( fEnd,   (double)(oCol.red()),   m_pCurveR, m_pDataR );

	clearRange( fStart, fEnd, m_pDataG );
	addPoint( fStart, (double)(oCol.green()), m_pCurveG, m_pDataG );
	addPoint( fEnd,   (double)(oCol.green()), m_pCurveG, m_pDataG );

	clearRange( fStart, fEnd, m_pDataB );
	addPoint( fStart, (double)(oCol.blue()),  m_pCurveB, m_pDataB );
	addPoint( fEnd,   (double)(oCol.blue()),  m_pCurveB, m_pDataB );
    }

    // remember user color, to make it default next time
    m_oColor = oCol;
}

/**
 *
 */
void sculptor_transfer_picker::clearRange(double fStart, double fEnd, sculptor_qwtdata* pData)
{
    int iBack = pData->size() - 1;
    int iStartErase = -1;
    int i = 1;


    // advance as long as curve point's x-coord smaller than range start
    while ( i < iBack && fStart > pData->x(i) )
	++i;

    // found nothing, so return
    if ( i == iBack )
	return;

    // now, i must be first point within the range
    iStartErase = i;

    ++i;

    // next point last one in curve?
    if ( i == iBack )
    {
	// then delete the only point in the range and return
	pData->erase(iStartErase);
	return;
    }

    // next point already beyond range end?
    if ( fEnd < pData->x(i) )
    {
	// then delete the only point in the range and return
	pData->erase(i);
	return;
    }

    // advance as long as point's x-coord smaller than range end
    while ( i < iBack && pData->x(i) < fEnd )
	++i;

    // delete all points within the range
    pData->erase(iStartErase, i-1);
}

/*
 *
 */
void sculptor_transfer_picker::sMoved(const QwtDoublePoint& oPoint)
{
    int iBack;
    double fW, fX = oPoint.x(), fY;

    // remember that this slot was called
    m_bMoved = true;

    // no point selected, so highlight the dragged area
    if ( m_iSelectedCurvePoint == -1 )
    {
	// highlight dragged area
	svt_clamp(fX, m_fMinDensity, m_fMaxDensity);

	if ( fX > m_fMousePressPos_X )
	{
	    fW = fX - m_fMousePressPos_X;
	    m_oRangeMarker.setRect( QwtDoubleRect(m_fMousePressPos_X, 0.0, fW, 255.0) );
	}
	else
	{
	    fW = m_fMousePressPos_X - fX;
	    m_oRangeMarker.setRect( QwtDoubleRect(fX, 0.0, fW, 255.0) );
	}

	// show the range marker
	m_oRangeMarker.attach(m_pCurveR->plot() );

    }
    else // a point is selected
    {
	fY = oPoint.y();
	svt_clamp(fY, 0.0, 255.0);

	iBack = m_pSelectedCurveData->size() - 1;

	// first curve point
	if ( m_iSelectedCurvePoint == 0 )
	{
	    if ( fX <= m_fMinDensity )
	    {
		// move it vertically...
		m_pSelectedCurveData->setY( 0, fY );
		m_oSelectionMarker.setYValue(fY);
	    }
	    else
	    {
		// ... or insert a new point to the right
		m_pSelectedCurveData->insert( 1, fX, fY );
		m_oSelectionMarker.setValue( fX, fY );
		++m_iSelectedCurvePoint;
	    }
	}

	// last curve point
	else if ( m_iSelectedCurvePoint == iBack )
	{
	    if ( fX >= m_fMaxDensity )
	    {
		// move it vertically...
		m_pSelectedCurveData->setY( iBack, fY );
		m_oSelectionMarker.setYValue(fY);
	    }
	    else
	    {
		// ... or insert a new point to the left
		m_pSelectedCurveData->insert( iBack, fX, fY );
		m_oSelectionMarker.setValue( fX, fY );
	    }
	}

	else // some point in between
	{
	    // for now, don't move neighboring points, instead use them as barriers
	    svt_clamp( fX, m_pSelectedCurveData->x(m_iSelectedCurvePoint-1), m_pSelectedCurveData->x(m_iSelectedCurvePoint+1) );

	    m_pSelectedCurveData->set( m_iSelectedCurvePoint, fX, fY );
	    m_oSelectionMarker.setValue( fX, fY );
	}
    }

    // plot manipulated, send signal
    emit plotChanged(m_pSelectedCurve);
}
