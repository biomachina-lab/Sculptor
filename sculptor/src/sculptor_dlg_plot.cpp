/***************************************************************************
                          sculptor_dlg_plot.cpp
			  ---------------------
    begin                : 28/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_plot.h>
#include <sculptor_qwtplotcurve.h>
#include <svt_messages.h>
#include <svt_exception.h>
// svt includes
#include <svt_iostream.h>
// qwt includes
#include <qwt_plot.h>
#include <qwt_legend.h>
// qt4 includes
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QLabel>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

extern string QtoStd(QString);

/**
 *
 */
dlg_plot_checkbox::dlg_plot_checkbox(sculptor_qwtplotcurve* pCurve)
    : QCheckBox(),
      m_pCurve( pCurve)
{
}

/**
 *
 */
sculptor_qwtplotcurve* dlg_plot_checkbox::getCurve()
{
    return m_pCurve;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
dlg_plot::dlg_plot( QWidget* pParent,  QString oTitle)
    : QDialog( pParent ),
      m_pTimer( NULL )
{
    setWindowTitle(oTitle);
    m_oTitle = oTitle;

    resize( 640, 480 );
    m_pPlotDlgLayout = new QVBoxLayout( this ); 

    // qwt plot widget
    m_pQwtPlot = new QwtPlot(this);
    //m_pQwtPlot->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding) );
    m_pQwtPlot->insertLegend(new QwtLegend);
    m_pQwtPlot->setCanvasBackground( Qt::white );


    m_pPlotDlgLayout->addWidget( m_pQwtPlot );

    // comment
    m_pComment = new QLabel( this );
    m_pComment->setText( oTitle );
    m_pComment->setAlignment( Qt::AlignCenter );
    m_pPlotDlgLayout->addWidget( m_pComment );

    // row of curve check boxes
    m_pCheckBoxesLayout = new QHBoxLayout();
    m_pPlotDlgLayout->addLayout( m_pCheckBoxesLayout, 0 );

    // space, close, save, print buttons, spacer
    m_pButtonsLayout = new QHBoxLayout();
    QSpacerItem* pSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred );
    m_pButtonsLayout->addSpacerItem(pSpacer);
    //
    m_pButtonClose = new QPushButton( QString("Close"), this );
    m_pButtonsLayout->addWidget(m_pButtonClose);
    connect( m_pButtonClose, SIGNAL(clicked()), this, SLOT(sHide()) );
    //
    m_pButtonSave = new QPushButton( QString("Save"), this );
    m_pButtonsLayout->addWidget(m_pButtonSave);
    connect( m_pButtonSave, SIGNAL(clicked()), this, SLOT(sSave()) );
    //
    m_pButtonPrint = new QPushButton( QString("Print"), this );
    m_pButtonsLayout->addWidget(m_pButtonPrint);
    connect( m_pButtonPrint, SIGNAL(clicked()), this, SLOT(sPrint()) );
    //
    pSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred );
    m_pButtonsLayout->addSpacerItem(pSpacer);
    //
    m_pPlotDlgLayout->addLayout( m_pButtonsLayout );
}

/*  
 *
 */
dlg_plot::~dlg_plot()
{
    if (m_pTimer)
    {
	m_pTimer->stop();
	delete m_pTimer;
    }
}

/**
 *
 */
void dlg_plot::attachCurve(sculptor_qwtplotcurve* pCurve)
{
    // associate the pCurve with its visibility checkbox and put the checkbox into the window
    dlg_plot_checkbox* pCheckBox = new dlg_plot_checkbox(pCurve);
    pCheckBox->setChecked(true);
    pCheckBox->setText( pCurve->getName() );
    m_pCheckBoxesLayout->addWidget( pCheckBox );

    // set the connection betewen curve and checkbox
    connect(pCheckBox, SIGNAL(toggled(bool)), this, SLOT(sSetVisible(bool)) );

    // attach the curve to the plot
    pCurve->attach(getQwtPlot());

    // and replot the plot
    getQwtPlot()->replot();
}

/**
 * get the qwt plot widget
 * \return pointer to QwtPlot widget
 */
QwtPlot* dlg_plot::getQwtPlot()
{
    return m_pQwtPlot;
}

/**
 * set the x axis title
 * \param pTitle pointer to array of char
 */
void dlg_plot::setXAxisTitle(QString oTitle)
{
    m_pQwtPlot->setAxisTitle(QwtPlot::xBottom, oTitle);
}

/**
 * set the y axis title
 * \param pTitle pointer to array of char
 */
void dlg_plot::setYAxisTitle(QString oTitle)
{
    m_pQwtPlot->setAxisTitle(QwtPlot::yLeft, oTitle);
}

/**
 * start auto update
 * \param iMSec frequency of the update in milliseconds (if 0 autoupdate is turned off)
 */
void dlg_plot::autoUpdate(int iMSec)
{
    if (m_pTimer)
        delete m_pTimer;

    if (iMSec != 0)
    {
	m_pTimer = new QTimer( this );
	connect( m_pTimer, SIGNAL(timeout()), SLOT(sReplot()) );

        m_pTimer->setSingleShot( FALSE );
	m_pTimer->start( iMSec );
    }
}

/**
 * update canvas
 */
void dlg_plot::sReplot()
{
    getQwtPlot()->replot();
}

/**
 * print canvas
 */
void dlg_plot::sPrint()
{
    // by default, print in landscape - with the legend this is usually the best option
    QPrinter oPrinter;
    //oPrinter.setOrientation(QPrinter::Landscape); -- causes problems on Fedora12, so disabled FIXME

    QPrintDialog *oDialog = new QPrintDialog(&oPrinter, this);
    oDialog->setWindowTitle(tr("Plot Printing"));

    if (oDialog->exec() != QDialog::Accepted)
        return;

    m_pQwtPlot->print(oPrinter);
}

/**
 * close window
 */
void dlg_plot::sHide()
{  
    if (m_pTimer != NULL)
        m_pTimer->stop();
    hide();
}

/**
 * save data to file in CSV format
 */
void dlg_plot::sSave()
{
    unsigned int i, j, iMaxDataSize, iNumCuves;

    // first get a list of all curves attached (i.e., visible curves)
    vector<sculptor_qwtplotcurve*> aCurves;
    QwtPlotItemList aQwtList = getQwtPlot()->itemList();
    for (int i=0; i<aQwtList.size(); ++i)
        if (aQwtList[i]->rtti() == QwtPlotItem::Rtti_PlotCurve)
            aCurves.push_back((sculptor_qwtplotcurve*)aQwtList[i]);

    // return if there are no visible curves
    iNumCuves = aCurves.size();
    if (iNumCuves == 0)
        return;

    // get a filename
    QFileDialog oFD(this);
    QString oFilterCSV( QString("Comma Separated Values (*.csv)") );
    QStringList oFilterList;
    oFilterList << oFilterCSV;
    oFD.setNameFilters( oFilterList );
    oFD.selectNameFilter( oFilterCSV );
    oFD.setFileMode( QFileDialog::AnyFile );
    oFD.setAcceptMode( QFileDialog::AcceptSave );

    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oFiles = oFD.selectedFiles();
        if (oFiles.isEmpty())
            return;

        QString oFn = oFiles[0];

        // does the new filename have an extension?
        if (oFn.endsWith( QString(".csv"), Qt::CaseInsensitive) == false)
            oFn.append( ".csv" );

        // does the file exist already?
        QFileInfo oFileInf( oFn );
        if (oFileInf.exists() )
            if ( !svt_exception::ui()->question("Attention: File exists already - Do you want to overwrite the file?") )
                return;

	FILE* pFile = fopen(QtoStd(oFn).c_str(), "w");

	if (pFile != NULL)
	{
            // write a line with the headers ( replace , by _ )
            QString oName = aCurves[0]->getName().replace(QString(","), QString("_"));

            // first columns
            fprintf( pFile, "%s-X,%s-Y", QtoStd(oName).c_str(), QtoStd(oName).c_str() );

            // all consecutive coumns
	    for (i=1; i<iNumCuves; i++)
            {
                oName = aCurves[i]->getName().replace(QString(","), QString("_"));
                fprintf( pFile, ",%s-X,%s-Y", QtoStd(oName).c_str(), QtoStd(oName).c_str() );
            }
            fprintf(pFile, "\n");


            // find the maximum data size of all curves (so we can nicely handle curves of different data sizes
            iMaxDataSize = 0;
            for (i=0; i<iNumCuves; ++i)
                if ( (unsigned int)(aCurves[i]->dataSize()) > iMaxDataSize)
                    iMaxDataSize = aCurves[i]->dataSize();


            // now walk over all data points and all curves and write out the values (or empty fields)
            for (i=0; i<iMaxDataSize; ++i)
            {
                // first columns
                fprintf( pFile, "%f,%f", aCurves[0]->getDataX(i), aCurves[0]->getDataY(i) );

                // all consecutive coumns
                for (j=1; j<iNumCuves; ++j)
                {
                    if ( i < (unsigned int)aCurves[j]->dataSize() )
                        fprintf(pFile, ",%f,%f", aCurves[j]->getDataX(i), aCurves[j]->getDataY(i) );
                    else
                        fprintf(pFile, ",,");
                }
                fprintf(pFile, "\n");
            }
            fclose(pFile);
	}
    }
}

/**
 *
 */
void dlg_plot::setComment(QString oComment)
{
    m_pComment->setText(oComment);
}

/**
 *
 */
vector<sculptor_qwtplotcurve*> dlg_plot::getCurves()
{
    vector<sculptor_qwtplotcurve*> oVec;

    QwtPlotItemList pList = getQwtPlot()->itemList();

    for (QwtPlotItemIterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        if ( (*itr)->rtti() == QwtPlotItem::Rtti_PlotCurve )
            oVec.push_back( (sculptor_qwtplotcurve*)*itr );
    }

    return oVec;
}

/**
 *
 */
void dlg_plot::sSetVisible(bool bVisible)
{
    ((dlg_plot_checkbox*)sender())->getCurve()->setVisible(bVisible);
    getQwtPlot()->replot();
}
