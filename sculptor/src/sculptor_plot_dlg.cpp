/***************************************************************************
                          sculptor_plot_dlg.cpp
			  ---------------------
    begin                : 06.08.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_plot_dlg.h>
// qt includes
#include <qvariant.h>
#include <q3frame.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qtimer.h>
#include <qprinter.h>
#include <qcheckbox.h>
#include <q3filedialog.h>
#include <qlabel.h>
// qwt includes
#include <qwt_plot.h>
//Added by qt3to4:
#include <Q3GridLayout>
// svt includes
#include <svt_iostream.h>

///////////////////////////////////////////////////////////////////////////////
// curve
///////////////////////////////////////////////////////////////////////////////

/**
 * create a new curve
 * \param pTitle pointer to array of char with the name of the curve
 * \param pXData pointer to an array of Real64 with the x data of the curve
 * \param pYData pointer to an array of Real64 with the y data of the curve
 * \param iCount number of values
 */
sculptor_curve::sculptor_curve(const char* pTitle, Real64* pXData, Real64* pYData, int iCount)
    : m_iCurve(0),
      m_pQwtPlot(NULL),
      m_oXData(NULL),
      m_oYData(NULL),
      m_pCheckBox(NULL),
      m_iStyle(QwtPlotCurve::Lines),
      m_iR(0),
      m_iG(0),
      m_iB(0)
{
    strcpy(m_pTitle, pTitle);
    m_iCount = iCount;
    m_pXData = pXData;
    m_pYData = pYData;

    if (m_pXData == NULL)
    {
        m_oXData = new Real64[m_iCount];
        for(int i=0; i<m_iCount; i++)
            m_oXData[i] = i;
        m_pXData = m_oXData;
    }
}

/**
 * create a new curve
 * \param pTitle pointer to array of char with the name of the curve
 * \param pXData vector of Real64 with the x data of the curve
 * \param pYData vector Real64 with the y data of the curve
 * \param iCount number of values
 */
sculptor_curve::sculptor_curve(const char* pTitle, vector<Real64 > pXData, vector<Real64 > pYData, int iCount) :
    m_iCurve(0),
    m_pQwtPlot(NULL),
    m_oXData(NULL),
    m_oYData(NULL),
    m_pCheckBox(NULL),
    m_iStyle(QwtPlotCurve::Lines),
    m_iR(0),
    m_iG(0),
    m_iB(0)
{
    strcpy(m_pTitle, pTitle);
    m_iCount = iCount;
    
    m_oXData = new Real64[m_iCount];
    for(int i=0; i<m_iCount; i++)
        m_oXData[i] = pXData[i];
    m_pXData = m_oXData;
    
    m_oYData = new Real64[m_iCount];
    for(int i=0; i<m_iCount; i++)
        m_oYData[i] = pYData[i];
    m_pYData = m_oYData;
}

/**
 * copy constructor
 * \param rCurve reference to other sculptor_curve
 */
sculptor_curve::sculptor_curve(const sculptor_curve &rCurve)
{
    strcpy(m_pTitle, rCurve.m_pTitle);
    m_pXData = rCurve.m_pXData;
    m_pYData = rCurve.m_pYData;
    m_iCount = rCurve.m_iCount;
    m_iCurve = rCurve.m_iCurve;
    m_pQwtPlot = rCurve.m_pQwtPlot;
    m_iR = rCurve.m_iR;
    m_iG = rCurve.m_iG;
    m_iB = rCurve.m_iB;
    m_pCheckBox = rCurve.m_pCheckBox;
    m_iStyle = rCurve.m_iStyle;

    if (rCurve.m_oXData != NULL)
    {
	m_oXData = new Real64[m_iCount];
	memcpy(m_oXData, rCurve.m_oXData, m_iCount*sizeof(Real64));
        m_pXData = m_oXData;
        
    } else
        m_oXData = NULL;
        
    if (rCurve.m_oYData != NULL)
    {
	m_oYData = new Real64[m_iCount];
	memcpy(m_oYData, rCurve.m_oYData, m_iCount*sizeof(Real64));
        m_pYData = m_oYData;
        
    } else
        m_oYData = NULL;
}

sculptor_curve::~sculptor_curve()
{
    if (m_oXData)
        delete[] m_oXData;
    
    if (m_oYData)
        delete[] m_oYData;
}

/**
 * add the curve to a qwt plot widget
 * \param pQwtPlot pointer to the widget
 */
void sculptor_curve::addCurve(QwtPlot* pQwtPlot)
{
    m_pQwtPlot = pQwtPlot;
    // QT3FIXME m_iCurve = m_pQwtPlot->insertCurve( m_pTitle );    

    // QT3FIXME m_pQwtPlot->setCurveData(m_iCurve, m_pXData, m_pYData, m_iCount);
    // QT3FIXME m_pQwtPlot->setCurveStyle(m_iCurve, m_iStyle );
    
    QPen oPen( QColor( m_iR, m_iG, m_iB ) , 2, Qt::SolidLine );
    // QT3FIXME m_pQwtPlot->setCurvePen(m_iCurve, oPen );
    
    if (m_oXData != NULL)
        m_pQwtPlot->setAxisScale(QwtPlot::xBottom, 0, m_iCount);

    m_pQwtPlot->replot();
}

/**
 * set the checkbox that controls the drawing state (visible/invisible) of the curve
 * \param pCheckBox pointer to QCheckBox object
 */
void sculptor_curve::setCheckBox(QCheckBox* pCheckBox)
{
    m_pCheckBox = pCheckBox;
}
/**
 * get the checkbox that controls the drawing state (visible/invisible) of the curve
 * \return pointer to QCheckBox object
 */
QCheckBox* sculptor_curve::getCheckBox()
{
    return m_pCheckBox;
}

/**
 * get name
 */
char* sculptor_curve::getName()
{
    return m_pTitle;
}

/**
 * set curve style
 */
void sculptor_curve::setStyle(enum QwtPlotCurve::CurveStyle iStyle)
{
    m_iStyle = iStyle;
    // QT3FIXME if (m_pQwtPlot)
	// QT3FIXME m_pQwtPlot->setCurveStyle(m_iCurve, iStyle );
}

/**
 * set Pen style
 */
void sculptor_curve::setPenStyle(enum Qt::PenStyle iStyle)
{
    if (m_pQwtPlot)
    {
        QPen oPen;
        oPen.setColor( QColor( m_iR, m_iG, m_iB ) );
        oPen.setWidth( 2 );
        oPen.setStyle( iStyle );
        // QT3FIXME m_pQwtPlot->setCurvePen(m_iCurve, oPen );
        
        // QT3FIXME m_pQwtPlot->replot();
    }
}

/**
 * set Pen style
 */
void sculptor_curve::setPenStyle(unsigned int iStyle)
{
    switch (iStyle)
    {
        case 0:
            setPenStyle( Qt::NoPen);
            break;
        case 1:
            setPenStyle( Qt::SolidLine );
            break;
        case 2:
            setPenStyle( Qt::DashLine );
            break;
        case 3:
            setPenStyle( Qt::DotLine );
            break;    
        case 4:
            setPenStyle( Qt::DashDotLine );
            break;
        case 5:
            setPenStyle( Qt::DashDotDotLine );
            break;
        default:
            setPenStyle( Qt::SolidLine );
    }
            
}

/**
 * update the curve data
 */
void sculptor_curve::updateData()
{
    // QT3FIXME m_pQwtPlot->setCurveRawData(m_iCurve, m_pXData, m_pYData, m_iCount);
}

/**
 * set curve color
 * \param iR red component
 * \param iG green component
 * \param iB blue component
 */
void sculptor_curve::setColor(int iR, int iG, int iB)
{
    m_iR = iR;
    m_iG = iG;
    m_iB = iB;
}

/**
 * enable
 * \param bEnable if true the curves is drawn
 */
void sculptor_curve::setEnable(bool bEnable)
{
// QT3FIXME
/*    if (bEnable == false)
	m_pQwtPlot->setCurveStyle(m_iCurve, QwtCurve::NoCurve);
    else
	m_pQwtPlot->setCurveStyle(m_iCurve, m_iStyle );

    m_pQwtPlot->replot();
*/
}

/**
 * get data value (x-axis)
 * \param i index of data value
 * \return data itself
 */
Real64 sculptor_curve::getDataX(int i)
{
    if (i < m_iCount)
	return m_pXData[i];
    else
        return 0.0;
}
/**
 * get data value (y-axis)
 * \param i index of data value
 * \return data itself
 */
Real64 sculptor_curve::getDataY(int i)
{
    if (i < m_iCount)
	return m_pYData[i];
    else
        return 0.0;
}

/**
 * get number of data values
 * \return number of data values of the curve
 */
int sculptor_curve::getSize()
{
    return m_iCount;
}

/**
 * Add data point
 */
void sculptor_curve::addDataPoint( Real64 fX, Real64 fY)
{
    m_iCount++;
    
    m_oXData = new Real64[m_iCount];
    for(int i=0; i<m_iCount-1; i++)
        m_oXData[i] = m_pXData[i];
    m_oXData[m_iCount-1] = fX;
    m_pXData = m_oXData;
    
    m_oYData = new Real64[m_iCount];
    for(int i=0; i<m_iCount-1; i++)
        m_oYData[i] = m_pYData[i];
    m_oYData[m_iCount-1] = fY;
    m_pYData = m_oYData;
    
}

/**
 * Remove data point
 */
void sculptor_curve::popDataPoint()
{
    m_iCount--;
    
    m_oXData = new Real64[m_iCount];
    for(int i=0; i<m_iCount; i++)
        m_oXData[i] = m_pXData[i];
    m_pXData = m_oXData;
    
    m_oYData = new Real64[m_iCount];
    for(int i=0; i<m_iCount; i++)
        m_oYData[i] = m_pYData[i];
    m_pYData = m_oYData; 
}
///////////////////////////////////////////////////////////////////////////////
// plot dlg
///////////////////////////////////////////////////////////////////////////////

/**
 * universal sculptor data plot dialog
 */
sculptor_plot_dlg::sculptor_plot_dlg( const char* pTitle, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    strcpy(m_pTitle, pTitle);

    if ( !name )
	setName( "PlotDlg" );
    resize( 640, 480 );
    setProperty( "caption", tr( "Plot" ) );
    m_pPlotDlgLayout = new Q3GridLayout( this ); 
    m_pPlotDlgLayout->setSpacing( 6 );
    m_pPlotDlgLayout->setMargin( 11 );

    // qwt plot widget

    QwtText oText(m_pTitle);
    m_pQwtPlot = new QwtPlot(QString(m_pTitle));// QT3FIXME , this);
/*
// QT3FIXME 
    m_pQwtPlot->setAutoLegend(true);
    m_pQwtPlot->setLegendPos(Qwt::Right);
    m_pQwtPlot->setCanvasBackground(QColor(255,255,255));
    m_pQwtPlot->enableGridX(false);
    m_pQwtPlot->enableGridY(false);
    m_pPlotDlgLayout->addMultiCellWidget( m_pQwtPlot, 0, 0, 0, 4 );
*/
    // comment
    m_pComment = new QLabel( this );
    m_pComment->setText( "" );
    m_pComment->setAlignment( Qt::AlignCenter );
    m_pPlotDlgLayout->addMultiCellWidget( m_pComment, 1, 1, 0, 4 );

    // row of check boxes for the curves
    /*
    m_pCurvesLayout = new Q3GridLayout( );
    m_pPlotDlgLayout->addMultiCellLayout( m_pCurvesLayout, 2,2,0,4 );
    m_pCurvesLayout->setSpacing( 6 );
    m_pCurvesLayout->setMargin( 11 );
    */
    // close button
    m_pButtonClose = new QPushButton( this, "m_pButtonClose" );
    m_pButtonClose->setProperty( "text", tr( "Close" ) );
    m_pPlotDlgLayout->addWidget( m_pButtonClose, 3, 1 );
    // save button
    m_pButtonSave = new QPushButton( this, "m_pButtonSave" );
    m_pButtonSave->setProperty( "text", tr( "Save Data" ) );
    m_pPlotDlgLayout->addWidget( m_pButtonSave, 3, 2 );
    // print button
    m_pButtonPrint = new QPushButton( this, "m_pButtonPrint" );
    m_pButtonPrint->setProperty( "text", tr( "Print" ) );
    m_pPlotDlgLayout->addWidget( m_pButtonPrint, 3, 3 );
    // spacer left and right
    QSpacerItem* pSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    m_pPlotDlgLayout->addItem( pSpacer, 3, 0 );
    QSpacerItem* pSpacer2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    m_pPlotDlgLayout->addItem( pSpacer2, 3, 4 );

    // signals and slots connections
    connect( m_pButtonClose, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( m_pButtonSave, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect( m_pButtonPrint, SIGNAL( clicked() ), this, SLOT( print() ) );

    // init vars
    m_pTimer = NULL;
    m_bClosedByUser = false;
}

/*  
 *  Destroys the object and frees any allocated resources
 */
sculptor_plot_dlg::~sculptor_plot_dlg()
{
    if (m_pTimer)
    {
	m_pTimer->stop();
	delete m_pTimer; m_bClosedByUser; // FIXME
    }
}

/**
 * get the qwt plot widget to insert data and adjust the plot
 * \return pointer to QwtPlot widget
 */
QwtPlot* sculptor_plot_dlg::getQwtPlot()
{
    return m_pQwtPlot;
}

/**
 * set the state of the CloseByUser variable
 * \param bClosedByUser whether the user closed or opened the window
 */
void sculptor_plot_dlg::setClosedByUser(bool bClosedByUser )
{
    m_bClosedByUser = bClosedByUser;
}

/**
 * \return whether the window was closed by the user
 */
bool sculptor_plot_dlg::wasClosedByUser()
{
    return m_bClosedByUser;
}

/**
 * set the x axis title
 * \param pTitle pointer to array of char
 */
void sculptor_plot_dlg::setXAxisTitle(const char* pTitle)
{
    m_pQwtPlot->setAxisTitle(QwtPlot::xBottom, pTitle);
}
/**
 * set the y axis title
 * \param pTitle pointer to array of char
 */
void sculptor_plot_dlg::setYAxisTitle(const char* pTitle)
{
    m_pQwtPlot->setAxisTitle(QwtPlot::yLeft, pTitle);
}

/**
 * add a curve
 */
void sculptor_plot_dlg::addCurve(sculptor_curve& rCurve)
{
    return; // QT3FIXME
    QCheckBox* pCheckBox = new QCheckBox( tr(rCurve.getName()), this, "pCheckBox" );
    pCheckBox->setChecked(true);
    m_pCurvesLayout->addWidget(pCheckBox, 1, m_aCurves.size());
    connect( pCheckBox, SIGNAL( clicked() ), this, SLOT( enableCurve() ) );

    rCurve.addCurve( m_pQwtPlot );
    rCurve.setCheckBox( pCheckBox );

    m_aCurves.push_back( rCurve );
}
/**
 * get curve
 */
sculptor_curve* sculptor_plot_dlg::getCurve(int i)
{
    return &m_aCurves[i];
};

/**
 * enable/disable curve
 */
void sculptor_plot_dlg::enableCurve()
{
    for(unsigned int i=0; i<m_aCurves.size(); i++)
	m_aCurves[i].setEnable(m_aCurves[i].getCheckBox()->isChecked());
}

/**
 * update curve
 */
void sculptor_plot_dlg::updateCurves()
{
    int iCnt = m_aCurves.size();

    unsigned int iMaxCount=0;
    for(int i=0; i < iCnt; i++)
    {   
        m_aCurves[i].updateData();
        if (m_aCurves[i].getSize() > iMaxCount)
            iMaxCount = m_aCurves[i].getSize();
    }
    
    m_pQwtPlot->setAxisScale(QwtPlot::xBottom, 0, iMaxCount);
    m_pQwtPlot->replot();
}

/**
 * Get curve number
 * \return the number of curves added to the plot
 */
unsigned int sculptor_plot_dlg::getCurveCount()
{
    return m_aCurves.size();
}

/**
 * start auto update
 * \param iMSec frequency of the update in milliseconds (if 0 autoupdate is turned off)
 */
void sculptor_plot_dlg::autoUpdate(int iMSec)
{
    if (m_pTimer)
        delete m_pTimer;

    if (iMSec != 0)
    {
	m_pTimer = new QTimer( this );
	connect( m_pTimer, SIGNAL(timeout()), SLOT(updateCurves()) );
	m_pTimer->start( iMSec, FALSE );
    }
}

/**
 * print canvas
 */
void sculptor_plot_dlg::print()
{
    QPrinter oPrinter;

    if ( oPrinter.setup( this ) )
	m_pQwtPlot->print( oPrinter );
}

/**
 * close window
 */
void sculptor_plot_dlg::close()
{  
    m_bClosedByUser = true; 
    accept();
}

/**
 * save data to file
 */
void sculptor_plot_dlg::save()
{
    unsigned int i,j;
    bool bNotEnd = true;
    Q3FileDialog oFD(this);
    oFD.setMode( Q3FileDialog::AnyFile );

    if ( oFD.exec() == QDialog::Accepted )
    {
	QString oFn = oFD.selectedFile();
	FILE* pFile = fopen(oFn, "w");

	if (pFile != NULL)
	{
	    for(i=0; i<m_aCurves.size(); i++)
                if (m_aCurves[i].getCheckBox()->isChecked())
		    fprintf(pFile, "%s-X %s-Y ", m_aCurves[i].getName(),m_aCurves[i].getName());
	    fprintf(pFile, "\n");

            j=0;
	    while(bNotEnd)
	    {
                // are we already at the end of _all_ curves?
                bNotEnd = false;
		for(i=0; i<m_aCurves.size(); i++)
		{
		    if (m_aCurves[i].getCheckBox()->isChecked())
		    {
			if (j < (unsigned int)(m_aCurves[i].getSize()))
			    bNotEnd = true;
		    }
		}
                // no?
		if (bNotEnd)
		{
		    for(i=0; i<m_aCurves.size(); i++)
		    {
			if (m_aCurves[i].getCheckBox()->isChecked())
			{
			    if (j < (unsigned int)(m_aCurves[i].getSize()))
				fprintf(pFile, "%f %f ", m_aCurves[i].getDataX(j), m_aCurves[i].getDataY(j));
			    else
				fprintf(pFile, "0.0 0.0 ");
			}
		    }
		    fprintf(pFile, "\n");
		    j++;
		}
	    }

            fclose(pFile);
	} else {
            // warning - couldn't save that file
	}
    }
}

/**
 * set the comment
 * \param pText the new commentary text
 */
void sculptor_plot_dlg::setComment(const char* pComment)
{
    m_pComment->setText(pComment);
}
