/***************************************************************************
                          sculptor_transfer
                          -----------------
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
#include <svt_vector3.h>
// qwt5 includes
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_data.h>


/**
 * Constructor
 */
sculptor_transfer::sculptor_transfer(QWidget *pParent)
    : QwtPlot( pParent ),
      m_pF2M( NULL )
{
    // construct the histogram curve object
    m_pHistCurve = new QwtPlotCurve();
    m_pHistCurve->setStyle( QwtPlotCurve::Sticks );
    m_pHistCurve->setPen( QPen(Qt::gray) );
    m_pHistCurve->attach(this);

    // construct the component curve and their data objects
    m_pCompR = new QwtPlotCurve("Red Component");
    m_pCompR->setPen( QPen(Qt::red) );
    m_pCompR->setYAxis( QwtPlot::yRight );
    m_pCompR->attach(this);
    m_pDataR = new sculptor_qwtdata(10);
    m_pCompR->setData( *m_pDataR );

    m_pCompG = new QwtPlotCurve("Green Component");
    m_pCompG->setPen( QPen(Qt::green) );
    m_pCompG->setYAxis( QwtPlot::yRight );
    m_pCompG->attach(this);
    m_pDataG = new sculptor_qwtdata(10);
    m_pCompG->setData( *m_pDataG );

    m_pCompB = new QwtPlotCurve("Blue Component");
    m_pCompB->setPen( QPen(Qt::blue) );
    m_pCompB->setYAxis( QwtPlot::yRight );
    m_pCompB->attach(this);
    m_pDataB = new sculptor_qwtdata(10);
    m_pCompB->setData( *m_pDataB );

    m_pCompO = new QwtPlotCurve("Opacity");
    m_pCompO->setPen( QPen(Qt::darkGray) );
    m_pCompO->setYAxis( QwtPlot::yRight );
    m_pCompO->attach(this);
    m_pDataO = new sculptor_qwtdata(10);
    m_pCompO->setData( *m_pDataO );

    // add a grid with dotted lines
    QwtPlotGrid* pHistGrid = new QwtPlotGrid();
    pHistGrid->setPen( QPen(QBrush(QColor(127,127,127), Qt::Dense4Pattern), 1.0f) );
    pHistGrid->attach( this );

    // some settings for the plot canvas
    enableAxis( QwtPlot::yLeft, false );
    setAxisScale( QwtPlot::yRight, 0.0, 255.0 );
    setCanvasBackground( Qt::white );
    QFont oFont = axisFont( QwtPlot::xBottom );
    oFont.setPointSize(8);
    setAxisFont( QwtPlot::xBottom, oFont );

    // create the curve picker
    m_pPicker = new sculptor_transfer_picker( canvas() );
    m_pPicker->initCurves( m_pCompR, m_pDataR,
                           m_pCompG, m_pDataG,
                           m_pCompB, m_pDataB,
                           m_pCompO, m_pDataO );

    // set a default components
    setActiveComp( sculptor_transfer::TRANS_ACTIVE_OPACITY );

    // deactivate auto-delete, since in the destructor we delete everything
    setAutoDelete(false);
}

/**
 * Destructor
 */
sculptor_transfer::~sculptor_transfer()
{
/*
    delete m_pHistCurve;
    delete m_pCompR;
    delete m_pCompG;
    delete m_pCompB;
    delete m_pCompO;
    delete m_pPicker;
*/
}

/**
 *
 */
void sculptor_transfer::setVolume(svt_volume<Real64>* pVolume, double* pXHist, double* pYHist, unsigned int iSizeHistogram)
{
    m_pVolume = pVolume;

    setAxisScale( QwtPlot::xBottom, m_pVolume->getMinDensity(), m_pVolume->getMaxDensity() );

    m_pPicker->setMinMaxDensity( m_pVolume->getMinDensity(), m_pVolume->getMaxDensity() );

    double fMax = 0.0;
    for( unsigned int i=0; i<iSizeHistogram; ++i )
        if (pYHist[i] > fMax)
            fMax = pYHist[i];

    setAxisScale( QwtPlot::yLeft, 0.0, fMax );

    m_pHistCurve->setData(pXHist, pYHist, iSizeHistogram);

    replot();
}

/**
 *
 */
void sculptor_transfer::setFloat2Mat( svt_float2mat_points* pF2M )
{
    m_pF2M = pF2M;
}

/**
 *
 */
svt_float2mat_points* sculptor_transfer::getFloat2Mat()
{
    return m_pF2M;
}

/**
 *
 */
void sculptor_transfer::setActiveComp(int iActiveComp)
{
    if (m_pPicker == NULL)
        return;

    QPen oPen;
    QwtSymbol oSym;
    oSym.setStyle( QwtSymbol::NoSymbol );

    // visually deselect old curve
    switch ( m_iActiveComp )
    {
        case TRANS_ACTIVE_RED:
            oPen.setColor( Qt::red );
            m_pCompR->setSymbol( oSym );
            m_pCompR->setPen( oPen );
            break;

        case TRANS_ACTIVE_GREEN:
            oPen.setColor( Qt::green );
            m_pCompG->setSymbol( oSym );
            m_pCompG->setPen( oPen );
            break;

        case TRANS_ACTIVE_BLUE:
            oPen.setColor( Qt::blue );
            m_pCompB->setSymbol( oSym );
            m_pCompB->setPen( oPen );
            break;

        case TRANS_ACTIVE_OPACITY:
            oPen.setColor( Qt::darkGray );
            m_pCompO->setSymbol( oSym );
            m_pCompO->setPen( oPen );
            break;

        case TRANS_ACTIVE_RGB:
            oPen.setColor( Qt::red );
            m_pCompR->setPen( oPen );
            oPen.setColor( Qt::green );
            m_pCompG->setPen( oPen );
            oPen.setColor( Qt::blue );
            m_pCompB->setPen( oPen );
            break;

        default:
            break;
    }

    // remember active component
    m_iActiveComp = iActiveComp;

    // set pen and symbol style for selected curve
    oPen.setWidth( 2 );
    oSym.setSize( 7 );
    oSym.setStyle( QwtSymbol::Diamond );


    // now visually select desired curve and set it active in the picker
    switch( iActiveComp )
    {
        case TRANS_ACTIVE_RED:
            oPen.setColor( Qt::red );
            m_pCompR->setPen( oPen );
            m_pCompR->setSymbol( oSym );
            m_pPicker->selectComponent( TRANS_ACTIVE_RED );
            break;

        case TRANS_ACTIVE_GREEN:
            oPen.setColor( Qt::green );
            m_pCompG->setPen( oPen );
            m_pCompG->setSymbol( oSym );
            m_pPicker->selectComponent( TRANS_ACTIVE_GREEN );
            break;

        case TRANS_ACTIVE_BLUE:
            oPen.setColor( Qt::blue );
            m_pCompB->setPen( oPen );
            m_pCompB->setSymbol( oSym );
            m_pPicker->selectComponent( TRANS_ACTIVE_BLUE );
            break;

        case TRANS_ACTIVE_OPACITY:
            oPen.setColor( Qt::darkGray );
            m_pCompO->setPen( oPen );
            m_pCompO->setSymbol( oSym );
            m_pPicker->selectComponent( TRANS_ACTIVE_OPACITY );
            break;

        case TRANS_ACTIVE_RGB:
            oPen.setColor( Qt::red );
            m_pCompR->setPen( oPen );
            oPen.setColor( Qt::green );
            m_pCompG->setPen( oPen );
            oPen.setColor( Qt::blue );
            m_pCompB->setPen( oPen );
            m_pPicker->selectComponent( TRANS_ACTIVE_RGB );
            break;

        default:
            break;
    }

    replot();
};

/**
 *
 */
void sculptor_transfer::setPickerEnabled(bool bEnabled)
{
    m_pPicker->setEnabled( bEnabled );
}

/**
 *
 */
void sculptor_transfer::connectPicker(prop_situs* pDlg)
{
    connect( (QObject*)m_pPicker, SIGNAL(plotChanged(QwtPlotCurve*)), (QObject*)pDlg, SLOT(sApplyTF(QwtPlotCurve*)), Qt::DirectConnection );
}

/**
 *
 */
void sculptor_transfer::transToPlot()
{
    unsigned int iSize, i;

    if ( m_pF2M == NULL )
        return;


    // update the R color curve's data points (with comments):

    // clear the old points
    m_pDataR->clear();

    // check for the number of points in the plot
    vector<svt_vector3<double> >& rPoints = m_pF2M->getPointsR();
    iSize = rPoints.size();

    // reserve new space in the data object if necessary
    if ( iSize > m_pDataR->capacity() )
        m_pDataR->reserve(iSize);

    // copy over the data from the plot
    for ( i=0; i<iSize; ++i )
        m_pDataR->push_back( rPoints[i].x(), rPoints[i].y() );


    //
    // identical procedure for the other components G, B, O
    //

    m_pDataG->clear();
    rPoints = m_pF2M->getPointsG();
    iSize = rPoints.size();
    if ( iSize > m_pDataG->capacity() )
        m_pDataG->reserve(iSize);
    for ( i=0; i<iSize; ++i )
        m_pDataG->push_back( rPoints[i].x(), rPoints[i].y() );

    m_pDataB->clear();
    rPoints = m_pF2M->getPointsB();
    iSize = rPoints.size();
    if ( iSize > m_pDataB->capacity() )
        m_pDataB->reserve(iSize);
    for ( i=0; i<iSize; ++i )
        m_pDataB->push_back( rPoints[i].x(), rPoints[i].y() );

    m_pDataO->clear();
    rPoints = m_pF2M->getPointsO();
    iSize = rPoints.size();
    if ( iSize > m_pDataO->capacity() )
        m_pDataO->reserve(iSize);
    for ( i=0; i<iSize; ++i )
        m_pDataO->push_back( rPoints[i].x(), rPoints[i].y() );
}

/**
 *
 */
void sculptor_transfer::plotToTrans()
{
    if ( m_pF2M == NULL )
        return;

    int iSize, i;
    m_pF2M->deletePoints();

    iSize = m_pDataR->size();
    for( i=0; i<iSize; ++i )
        m_pF2M->addPointR( svt_vector3<double>( m_pDataR->x(i), m_pDataR->y(i), 0.0) );

    iSize = m_pDataG->size();
    for( i=0; i<iSize; ++i )
        m_pF2M->addPointG( svt_vector3<double>( m_pDataG->x(i), m_pDataG->y(i), 0.0) );

    iSize = m_pDataB->size();
    for( i=0; i<iSize; ++i )
        m_pF2M->addPointB( svt_vector3<double>( m_pDataB->x(i), m_pDataB->y(i), 0.0) );

    iSize = m_pDataO->size();
    for( i=0; i<iSize; ++i )
        m_pF2M->addPointO( svt_vector3<double>( m_pDataO->x(i), m_pDataO->y(i), 0.0) );
}
