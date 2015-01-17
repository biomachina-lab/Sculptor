/***************************************************************************
			  sculptor_prop_situs.cpp
			  -------------------
    begin                : 09/17/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor include
#include <sculptor_prop_situs.h>
#include <situs_document.h>
// svt include
#include <svt_init.h>
#include <svt_volume.h>
#include <svt_volume_compact.h>
#include <svt_volume_3dtextures.h>
// qt4 includes
#include <QString>
#include <QApplication>
#include <QColorDialog>
// qwt5 includes
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>


prop_situs_HistPlotPicker::prop_situs_HistPlotPicker(QwtPlotCanvas* pCanvas, prop_situs* pPropSitus)
    : QwtPlotPicker( pCanvas ),
      m_pPropSitus( pPropSitus )
{
}

void prop_situs_HistPlotPicker::widgetMousePressEvent(QMouseEvent* e)
{
    m_pPropSitus->sIsoSliderPressed();
    QwtPlotPicker::widgetMousePressEvent(e);
}

void prop_situs_HistPlotPicker::widgetMouseReleaseEvent(QMouseEvent* e)
{
    m_pPropSitus->sIsoSliderReleased();
    QwtPlotPicker::widgetMouseReleaseEvent(e);
}


/*
 * Constructor
 */
prop_situs::prop_situs( situs_document* pDoc, QWidget *parent )
    : QWidget(parent),
      m_pDoc( pDoc ),
      m_bIsoSliderPressed( false ),
      m_iTransferDefaultComponent( sculptor_transfer::TRANS_ACTIVE_OPACITY ),
      m_bSlicesCreated( false ),
      m_b3DTTabInitialized( false )
{
    setupUi(this);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // isosurface tab
    //

    // add a picker to the histogram plot's canvas to move the marker
    m_pHistPicker = new prop_situs_HistPlotPicker( m_pHistPlot->canvas(), this );
    m_pHistPicker->setSelectionFlags( QwtPicker::PointSelection | QwtPicker::DragSelection );
    connect( m_pHistPicker, SIGNAL(selected(const QwtDoublePoint&)), this, SLOT(sHistClicked(const QwtDoublePoint&)) );
    connect( m_pHistPicker, SIGNAL(moved(const QwtDoublePoint&)), this, SLOT(sHistClicked(const QwtDoublePoint&)) );

    // add a grid with nice, dotted lines
    QwtPlotGrid* pHistGrid = new QwtPlotGrid();
    pHistGrid->setPen( QPen(QBrush(QColor(127,127,127), Qt::Dense4Pattern), 1.0f) );
    pHistGrid->attach( m_pHistPlot );

    // add a curve like a bar diagram
    m_pHistCurve = new QwtPlotCurve();
    m_pHistCurve->setPen( QPen(Qt::black) );
    m_pHistCurve->setStyle( QwtPlotCurve::Sticks );
    m_pHistCurve->attach( m_pHistPlot );

    // the marker of the current iso surface threshold
    m_pHistMarker = new QwtPlotMarker();
    m_pHistMarker->setLineStyle( QwtPlotMarker::VLine );
    m_pHistMarker->setLinePen( QPen(Qt::red) );
    m_pHistMarker->setXValue( m_pDoc->getIsoLevel() );
    m_pHistMarker->attach( m_pHistPlot );

    // some general plot settings
    m_pHistPlot->setCanvasBackground( Qt::white );
    m_pHistPlot->enableAxis( QwtPlot::yLeft, false );
    QFont oFont = m_pHistPlot->axisFont( QwtPlot::xBottom );
    oFont.setPointSize(8);
    m_pHistPlot->setAxisFont( QwtPlot::xBottom, oFont );
    m_pHistPlot->replot();

    // update ui elements
    updateTabIso( m_pDoc->getIso()->getVisible() );

    // make connections
    connect( m_pIsoCheckWire,       SIGNAL(toggled(bool)),       m_pDoc, SLOT(sSetIsoWireframe(bool)) );
    connect( m_pIsoButtonColor,     SIGNAL(clicked()),           this,   SLOT(sSetIsoColor()) );
    connect( m_pIsoCheckEnable,     SIGNAL(toggled(bool)),       this,   SLOT(sEnableIso(bool)) );
    connect( m_pIsoSliderTrans,     SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetIsoTransparency(int)) );
    connect( m_pIsoTELevel,         SIGNAL(editingFinished()),   this,   SLOT(sIsoTextInput()) );
    connect( m_pIsoTESteps,         SIGNAL(editingFinished()),   this,   SLOT(sCreateIsoSurface()) );
    connect( m_pIsoTESteps,         SIGNAL(valueChanged(int)),   this,   SLOT(sCreateIsoSurface()) );
    connect( m_pIsoButtonCreate,    SIGNAL(clicked() ),          this,   SLOT(sIsoTextInput()) );
    connect( m_pIsoCheckFlat,       SIGNAL(toggled(bool)),       m_pDoc, SLOT(sRendering_Flat(bool)) );
    connect( m_pIsoSlider,          SIGNAL(sliderPressed()),     this,   SLOT(sIsoSliderPressed()), Qt::DirectConnection );
    connect( m_pIsoSlider,          SIGNAL(sliderReleased()),    this,   SLOT(sIsoSliderReleased()), Qt::DirectConnection );
    connect( m_pIsoSlider,          SIGNAL(valueChanged(int)),   this,   SLOT(sIsoSliderChanged(int)) );


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // slices tab
    //

    m_bSlicesCreated = m_pDoc->getSlices()->getVisible();
    updateTabSlices( (int)m_bSlicesCreated );

    //connect( m_pSliceButtonApply,   SIGNAL(clicked()),           m_pDoc, SLOT(sCreateSlices()) );
    connect( m_pSliceSpinX,         SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetSliceZY(int)) );
    connect( m_pSliceSpinY,         SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetSliceXZ(int)) );
    connect( m_pSliceSpinZ,         SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetSliceXY(int)) );
    connect( m_pSliderZY,           SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetSliceZY(int)) );
    connect( m_pSliderXZ,           SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetSliceXZ(int)) );
    connect( m_pSliderXY,           SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sSetSliceXY(int)) );
    connect( m_pSliceCheckXY,       SIGNAL(toggled(bool)),       m_pDoc, SLOT(sEnableSliceXY(bool)) );
    connect( m_pSliceCheckXZ,       SIGNAL(toggled(bool)),       m_pDoc, SLOT(sEnableSliceXZ(bool)) );
    connect( m_pSliceCheckZY,       SIGNAL(toggled(bool)),       m_pDoc, SLOT(sEnableSliceZY(bool)) );
    connect( m_pSliceCheckEnable,   SIGNAL(toggled(bool)),       this,   SLOT(sEnableSlices(bool)) );
    connect( m_pSliceCheckBox,      SIGNAL(toggled(bool)),       m_pDoc, SLOT(sBoxSlices(bool)) );
    connect( m_pSliceComboColors,   SIGNAL(activated(int)),      m_pDoc, SLOT(sChangeF2C(int)) );

    if (m_bSlicesCreated)
	m_pTabs->setCurrentIndex( 1 );



    ///////////////////////////////////////////////////////////////////////////////////////////////
    // direct volume rendering tab
    //

    m_pTransfer       ->setFloat2Mat( m_pDoc->getFloat2Mat() );
    m_pTransferPalette->setFloat2Mat( m_pDoc->getFloat2Mat() );

    // if shaders are switched off, disable DVR tab completely
    if ( !svt_getAllowShaderPrograms() )
    {
        set3DTTabEnabled(false);
        sEnableIso(true);
    }
    else
    {
        bool b = m_pDoc->get3DText()->getVisible();
        updateTab3DT( (int)b );
        initTab3DT();

        if (b)
            m_pTabs->setCurrentIndex( 2 );
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // information tab
    //

    updateTabInfo();

    connect( m_pInfoOriginX,        SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeOrigin()) );
    connect( m_pInfoOriginY,        SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeOrigin()) );
    connect( m_pInfoOriginZ,        SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeOrigin()) );
    connect( m_pInfoVoxelWidth,     SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeVoxelWidth()) );
    connect( m_pRenderBox,          SIGNAL(toggled(bool)),       m_pDoc, SLOT(sEnableBox(bool)) );
}

/*
 * Destructor
 */
prop_situs::~prop_situs()
{
}


/*
 *
 */
void prop_situs::initTab3DT()
{
    if (m_b3DTTabInitialized)
        return;

    // connect the picker plotChanged signal to this->sApplyTF method
    m_pTransfer->connectPicker( this );

    connect( m_p3DTButtonApply,     SIGNAL(clicked()),           this,   SLOT(sButtonApplyTF()) );
    connect( m_p3DTAutoApply,       SIGNAL(toggled(bool)),       this,   SLOT(sButtonApplyTF(bool)) );
    connect( m_p3DTCheckEnable,     SIGNAL(toggled(bool)),       this,   SLOT(sEnable3DT(bool)) );
    connect( m_pZero,               SIGNAL(toggled(bool)),       m_pDoc, SLOT(sZero(bool)) );
    connect( m_p3DRed,              SIGNAL(clicked()),           this,   SLOT(sChangeTransferComp()) );
    connect( m_p3DGreen,            SIGNAL(clicked()),           this,   SLOT(sChangeTransferComp()) );
    connect( m_p3DBlue,             SIGNAL(clicked()),           this,   SLOT(sChangeTransferComp()) );
    connect( m_p3DOpacity,          SIGNAL(clicked()),           this,   SLOT(sChangeTransferComp()) );
    connect( m_p3DRGB,              SIGNAL(clicked()),           this,   SLOT(sChangeTransferComp()) );
    connect( m_p3DSampling,         SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DSampling,         SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DDiffuse,          SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DDiffuse,          SIGNAL(valueChanged(double)),m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DAmbient,          SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DAmbient,          SIGNAL(valueChanged(double)),m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DShininess,        SIGNAL(editingFinished()),   m_pDoc, SLOT(sChangeSampling()) );
    connect( m_p3DShininess,        SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sChangeSampling()) );

    connect( m_p3DShading,          SIGNAL(toggled(bool)),       m_pDoc, SLOT(sEnable3DTShading(bool)) );
    connect( m_p3DTStart,           SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sStartSlice(int)) );
    connect( m_p3DTEnd,             SIGNAL(valueChanged(int)),   m_pDoc, SLOT(sEndSlice(int)) );
    connect( m_pTFReset,            SIGNAL(clicked()),           this,   SLOT(sResetTF()) );

    m_b3DTTabInitialized = true;
}

/**
 *
 */
void prop_situs::updateTabIso(int iEnabled)
{
    if ( m_pDoc == NULL )
	return;

    QString oStr;
    float fR, fG, fB;
    svt_volume<Real64>*         pVol = m_pDoc->getVolume();
    svt_volume_compact<Real64>* pMC  = m_pDoc->getIso();

    m_pIsoCheckWire->setChecked   (  pMC->getCont()->getProperties()->getWireframe());
    m_pIsoCheckFlat->setChecked   ( !pMC->getCont()->getProperties()->getSmooth());
    m_pIsoCheckEnable->setChecked( pMC->getVisible());
    m_pIsoSliderTrans->setValue   ((int)( pMC->getCont()->getProperties()->getColor()->getTransparency() * 100.0f) );
    fR = 255.0f*(pMC->getCont()->getProperties()->getColor()->getR());
    fG = 255.0f*(pMC->getCont()->getProperties()->getColor()->getG());
    fB = 255.0f*(pMC->getCont()->getProperties()->getColor()->getB());

    //m_pIsoButtonColor->setAutoFillBackground( TRUE );
    //oPalette.setColor(m_pIsoButtonColor->backgroundRole(), oColor);
    //m_pIsoButtonColor->setPalette(oPalette);
    //
    // some style engines override the palette. so force color via stylesheet
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", (int)fR, (int)fG, (int)fB);
    m_pIsoButtonColor->setStyleSheet(oStr);

    m_pIsoTESteps->setValue( pMC->getStep() );
    m_pIsoTELevel->setText( oStr.number(pMC->getIsolevel()) );

    double fMax = pVol->getMaxDensity();
    double fMin = pVol->getMinDensity();
    int iSlider = (int)(((pMC->getIsolevel() - fMin) / (fMax - fMin))*999.0);
    updateIsoSlider(iSlider);

    // update the iso histogram plot
    updatePlots(true, false);

    // only enabled/disable this tab if a specific enable/disable option is given
    if (iEnabled != -1)
        sEnableIsoControls( (bool)iEnabled );
}

/**
 *
 */
void prop_situs::updateIsoSlider(int iValue)
{
    m_pIsoSlider->blockSignals( TRUE );
    m_pIsoSlider->setValue( iValue );
    m_pIsoSlider->blockSignals( FALSE );
}

/**
 *
 */
void prop_situs::updateTabSlices(int iEnabled)
{
    QString oStr;
    svt_volume<Real64>*        pVol       = m_pDoc->getVolume();
    svt_volume_slices<Real64>* pVolSlices = m_pDoc->getSlices();

    if ( m_pSliceComboColors->count() == 0 )
    {
	m_pSliceComboColors->insertItem(BW,  "BW");
	m_pSliceComboColors->insertItem(RWB, "RWB");
	m_pSliceComboColors->insertItem(RYG, "RYG");
	m_pSliceComboColors->insertItem(RGB, "RGB");
    }
    m_pSliceSpinX->setMaximum(pVol->getSizeX());
    m_pSliceSpinY->setMaximum(pVol->getSizeY());
    m_pSliceSpinZ->setMaximum(pVol->getSizeZ());
    m_pSliderZY->setMaximum(pVol->getSizeX());
    m_pSliderXZ->setMaximum(pVol->getSizeY());
    m_pSliderXY->setMaximum(pVol->getSizeZ());

    m_pSliceTECutoff      ->setText(oStr.number(pVolSlices->getCutoff()));
    m_pSliceSpinX         ->setValue(pVolSlices->getCurrentSliceZY());
    m_pSliceSpinY         ->setValue(pVolSlices->getCurrentSliceXZ());
    m_pSliceSpinZ         ->setValue(pVolSlices->getCurrentSliceXY());
    m_pSliderZY           ->setValue(pVolSlices->getCurrentSliceZY());
    m_pSliderXZ           ->setValue(pVolSlices->getCurrentSliceXZ());
    m_pSliderXY           ->setValue(pVolSlices->getCurrentSliceXY());
    m_pSliceCheckBox      ->setChecked(pVolSlices->getBox());
    m_pSliceCheckXY       ->setChecked(pVolSlices->getDrawXY());
    m_pSliceCheckXZ       ->setChecked(pVolSlices->getDrawXZ());
    m_pSliceCheckZY       ->setChecked(pVolSlices->getDrawZY());

    if (pVolSlices->getFloat2Color())
    {
        m_pSliceComboColors->blockSignals( TRUE );
	m_pSliceComboColors->setCurrentIndex(pVolSlices->getFloat2Color()->getFloat2ColorName());
        m_pSliceComboColors->blockSignals( FALSE );
    }

    // only enabled/disable this tab if a specific enable/disable option is given
    if (iEnabled != -1)
        sEnableSlicesControls( (bool)iEnabled );
}

void prop_situs::updateTab3DT(int iEnabled)
{
    QString oStr;
    svt_volume_3dtextures<Real64>* p3DText = m_pDoc->get3DText();

    m_p3DTStart->setValue( (int)(p3DText->getStartSlice() * 99.0f) );
    m_p3DTEnd  ->setValue( (int)(p3DText->getEndSlice()   * 99.0f) );

    m_p3DShading->setChecked( p3DText->getShading() );
    m_p3DDiffuse  ->setValue( p3DText->getDiffuse() );
    m_p3DAmbient  ->setValue( p3DText->getAmbient() );
    m_p3DShininess->setValue( (int)p3DText->getShininess() );
    m_p3DSampling ->setValue( (int)p3DText->getSampling() );

    oStr.sprintf("%lf", m_pDoc->getVoxelWidth() );
    m_pInfoVoxelWidth->setText( oStr);

    m_pZero->setChecked( m_pTransfer->getFloat2Mat()->getZero() );

    // update the transfer plot
    updatePlots(false, true);

    // only enabled/disable this tab if a specific enable/disable option is given
    if (iEnabled != -1)
        sEnable3DTControls( (bool)iEnabled );
}

/*
 * update the information tab
 */
void prop_situs::updateTabInfo()
{
    if ( m_pDoc == NULL )
	return;

    QString oStr;
    svt_volume<Real64>* pVol = m_pDoc->getVolume();

    setWindowTitle( QString(m_pDoc->getFileName().c_str()) );

    m_pRenderBox->blockSignals( TRUE );
    m_pRenderBox->setChecked( m_pDoc->getBoxEnabled() );
    m_pRenderBox->blockSignals( FALSE );

    oStr.sprintf( "%i", pVol->getSizeX() );
    info_size_x->setText(oStr);
    oStr.sprintf( "%i", pVol->getSizeY() );
    info_size_y->setText(oStr);
    oStr.sprintf( "%i", pVol->getSizeZ() );
    info_size_z->setText(oStr);
    oStr.sprintf( "%f", pVol->getMaxDensity() );
    info_max_density->setText(oStr);
    oStr.sprintf( "%f", pVol->getMinDensity() );
    info_min_density->setText(oStr);
    oStr.sprintf( "%f", pVol->getGridX() );
    m_pInfoOriginX->setText(oStr);
    oStr.sprintf( "%f", pVol->getGridY() );
    m_pInfoOriginY->setText(oStr);
    oStr.sprintf( "%f", pVol->getGridZ() );
    m_pInfoOriginZ->setText(oStr);
}

/**
 *
 */
void prop_situs::updatePlots(bool bHistPlot, bool bTransferPlot)
{
    double fMax = 0.0;
    double* pHistogramY = m_pDoc->getHistogramY();
    int iHistogramSize = m_pDoc->getHistogramSize();

    if (bHistPlot)
    {
	// set histogram y-axis maximum
	for( int i=0; i<iHistogramSize; ++i )
	    if (pHistogramY[i] > fMax)
		fMax = pHistogramY[i];
	m_pHistPlot->setAxisScale( QwtPlot::yLeft, 0.0, fMax );

	// set x-axis min and max
	m_pHistPlot->setAxisScale( QwtPlot::xBottom, m_pDoc->getMinDensity(), m_pDoc->getMaxDensity() );

	// update the histogram curve data
	m_pHistCurve->setData( m_pDoc->getHistogramX(), pHistogramY, iHistogramSize );

	// check if the histogram marker is out of range
	if ( m_pHistMarker->xValue() > m_pDoc->getMaxDensity() )
	    m_pHistMarker->setXValue( m_pDoc->getMaxDensity() );
	else if ( m_pHistMarker->yValue() > m_pDoc->getMaxDensity() )
	    m_pHistMarker->setYValue( m_pDoc->getMaxDensity() );

	// and replot the histogram
	m_pHistPlot->replot();
    }

    if (bTransferPlot && m_pTabs->isTabEnabled(2) )
    {
	m_pTransfer->transToPlot();
	m_pTransfer->setVolume( m_pDoc->getVolume(), m_pDoc->getHistogramX(), pHistogramY, iHistogramSize );
	sChangeTransferComp();

	m_pTransferPalette->repaint();
    }
}

void prop_situs::updateIsoColorButton(QColor oCol)
{
    //m_pIsoButtonColor->setAutoFillBackground( TRUE );
    //oPalette.setColor(m_pIsoButtonColor->backgroundRole(), oColor);
    //m_pIsoButtonColor->setPalette(oPalette);
    //
    // some style engines override the palette. so force color via stylesheet
    QString oStr;
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", oCol.red(), oCol.green(), oCol.blue() );
    m_pIsoButtonColor->setStyleSheet(oStr);
}

/**
 * SLOT for setting the color of the isosurface (displays a color chooser dlg)
 */
void prop_situs::sSetIsoColor()
{
    // get the current color
    svt_color oDocCol = m_pDoc->getColor();

    int iR = (int) ( 255.0f * oDocCol.getR() );
    int iG = (int) ( 255.0f * oDocCol.getG() );
    int iB = (int) ( 255.0f * oDocCol.getB() );
    QColor oCol = QColorDialog::getColor(QColor(iR, iG, iB));

    if (!oCol.isValid())
	return;

    // ok, what did the user want...
    oDocCol.setR( (float)oCol.red()   / 255.0f );
    oDocCol.setG( (float)oCol.green() / 255.0f );
    oDocCol.setB( (float)oCol.blue()  / 255.0f );

    m_pDoc->setColor(oDocCol);

    updateIsoColorButton(oCol);
}

void prop_situs::sHistClicked(const QwtDoublePoint &pos)
{
    double fMax = m_pDoc->getMaxDensity();
    double fMin = m_pDoc->getMinDensity();

    // clamp to between min and max
    double fLevel = min( fMax, max(fMin, pos.x()) );

    // update the value in the iso level text field
    QString oStr;
    oStr.sprintf( "%f", fLevel );
    m_pIsoTELevel->setText(oStr);

    // update the iso slider
    int iSlider = (int)(((fLevel - fMin) / (fMax - fMin)) * 999.0);
    updateIsoSlider(iSlider);

    // update the iso marker position in the plot
    m_pHistMarker->setXValue( fLevel );
    m_pHistPlot->replot();

    sCreateIsoSurface();
}

void prop_situs::sCreateIsoSurface()
{
    int iStep = m_pIsoTESteps->text().toInt();
    float fLevel = m_pIsoTELevel->text().toFloat();

    if (m_bIsoSliderPressed && (iStep == 1))
	m_pDoc->createIsoSurface(fLevel, 2);
    else
	m_pDoc->createIsoSurface(fLevel, iStep);
}

void prop_situs::sIsoTextInput()
{
    QString oStr;
    double fLevel = (double)m_pIsoTELevel->text().toFloat();

    double fMax = m_pDoc->getMaxDensity();
    double fMin = m_pDoc->getMinDensity();

    // clamp to between min and max and update input field if necessary
    if (fLevel<fMin)
    {
	fLevel = fMin;
	oStr.sprintf( "%f", fLevel );
	m_pIsoTELevel->setText(oStr);
    }
    else if (fLevel>fMax)
    {
	fLevel = fMax;
	oStr.sprintf( "%f", fLevel );
	m_pIsoTELevel->setText(oStr);
    }

    // update slider
    int iSlider = (int)(((fLevel - fMin) / (fMax - fMin)) * 999.0);
    updateIsoSlider(iSlider);

    // update the iso marker position in the plot
    m_pHistMarker->setXValue( fLevel );
    m_pHistPlot->replot();

    sCreateIsoSurface();
}

/**
 * The user has clicked on the isosurface slider
 */
void prop_situs::sIsoSliderPressed()
{
    m_bIsoSliderPressed = true;
}

/**
 * The user has released the isosurface slider
 */
void prop_situs::sIsoSliderReleased()
{
    m_bIsoSliderPressed = false;
    sIsoSliderChanged( m_pIsoSlider->value() );
}

/**
 * The isosurface slider has changed
 */
void prop_situs::sIsoSliderChanged(int iValue)
{
    float fLevel;

    float fMax = m_pDoc->getMaxDensity();
    float fMin = m_pDoc->getMinDensity();
    // slider value goes from 0 to 999
    float fSlider = ((float)iValue)/999.0f;

    fLevel = ((fMax - fMin) * fSlider) + fMin;

    // clamp to between min and max
    fLevel = min( fMax, max(fMin, fLevel) );


    // update the iso marker position in the plot
    m_pHistMarker->setXValue( fLevel );

    // update the value in the iso level text field
    QString oStr;
    oStr.sprintf( "%f", fLevel );
    m_pIsoTELevel->setText(oStr);

    m_pHistPlot->replot();

    sCreateIsoSurface();
}

/**
 * 
 */
void prop_situs::sEnableIso(bool bEnable)
{
    m_pDoc->setIsoEnabled( bEnable );
    sEnableIsoControls( bEnable );

    // if iso surface is to be enabled, disable the 3d textures and slices
    if ( bEnable )
    {
	if ( m_p3DTCheckEnable->isChecked() == true )
	{
	    sEnable3DTControls( false );
	    m_pDoc->set3DTextEnabled( false );
	}
	if ( m_pSliceCheckEnable->isChecked() == true )
	{
	    sEnable3DTControls( false );
	    m_pDoc->setSlicesEnabled( false );
	}
    }
}

/**
 * 
 */
void prop_situs::sEnableIsoControls(bool bEnable)
{
    // (un)check the chekcboc
    m_pIsoCheckEnable->blockSignals( TRUE );
    m_pIsoCheckEnable->setChecked( bEnable );
    m_pIsoCheckEnable->blockSignals( FALSE );
    // (de)activate the iso controls
    m_pIsoControls->setEnabled( bEnable );
    // (de)activate the iso plot picker
    m_pHistPicker->setEnabled( bEnable );

    if (bEnable)
	m_pHistPlot->setCanvasBackground( Qt::white );
    else
	m_pHistPlot->setCanvasBackground( Qt::lightGray );
    
    m_pHistPlot->replot();
}

/**
 * SLOT enable slices
 * \param bEnable if true the slices are enabled
 */
void prop_situs::sEnableSlices(bool bEnable)
{
    m_pDoc->setSlicesEnabled( bEnable );
    m_pSliceControls->setEnabled( bEnable );

    // if slices are to be enabled, disable the 3d textures and iso surface
    if ( bEnable )
    {
	if ( m_p3DTCheckEnable->isChecked() == true )
	{
	    sEnable3DTControls( false );
	    m_pDoc->set3DTextEnabled( false );
	}
	if ( m_pIsoCheckEnable->isChecked() == true )
	{
            sEnableIsoControls( false );
	    m_pDoc->setIsoEnabled( false );
	}

        if ( !m_bSlicesCreated )
        {
            m_pDoc->createSlices();
            m_bSlicesCreated = true;
        }
    }
}

/**
 * 
 */
void prop_situs::sEnableSlicesControls(bool bEnable)
{
    // (un)check the checkbox
    m_pSliceCheckEnable->blockSignals( TRUE );
    m_pSliceCheckEnable->setChecked( bEnable );
    m_pSliceCheckEnable->blockSignals( FALSE );

    // (de)activate the iso controls
    m_pSliceControls->setEnabled( bEnable );
}

/**
 * 
 */
void prop_situs::sEnable3DT(bool bEnable)
{
    if (bEnable)
    {
        // disable the iso surface and slices
	if ( m_pIsoCheckEnable->isChecked() == true )
	{
            sEnableIsoControls( false  );
	    m_pDoc->setIsoEnabled( false );
	}
	if ( m_pSliceCheckEnable->isChecked() == true )
	{
            sEnableSlicesControls( false );
	    m_pDoc->setSlicesEnabled( false );
	}

	// need to clear the event queue here, so that Qt can update the progress bar right now
	svt_forceUpdateGUIWindows();
        m_pDoc->create3DT();
	m_pDoc->set3DTextEnabled( true );

        // make sure all is done before enabling the transfer plot
	svt_forceUpdateGUIWindows();
	sEnable3DTControls( true );
    }
    else
    {
        sEnable3DTControls( false );
        m_pDoc->set3DTextEnabled( false );
    }
}

/**
 * 
 */
void prop_situs::sEnable3DTControls(bool bEnable)
{
    // (un)check the checkbox
    m_p3DTCheckEnable->blockSignals( TRUE);
    m_p3DTCheckEnable->setChecked( bEnable );
    m_p3DTCheckEnable->blockSignals( FALSE );

    if ( bEnable )
	m_pTransfer->setCanvasBackground( Qt::white );
    else
	m_pTransfer->setCanvasBackground( Qt::lightGray );

    m_pTransfer->replot();

    // (de)activate the widget holding all the controls
    m_p3DTControls->setEnabled( bEnable );

    // (de)activate the transfer plot picker
    m_pTransfer->setPickerEnabled( bEnable );
}


/**
 *
 */
void prop_situs::set3DTTabEnabled(bool bEnabled)
{
    if (!bEnabled)
        m_pTabs->setCurrentIndex(0);
    else
    {
        updateTab3DT();
        initTab3DT();
    }

    m_pTabs->setTabEnabled( 2, bEnabled );
    m_pDVRTab->setEnabled( bEnabled );
}

/**
 * SLOT for changing the currently edited transfer function curve
 */
void prop_situs::sChangeTransferComp()
{
    if ( m_p3DRed->isChecked() )
	m_pTransfer->setActiveComp( sculptor_transfer::TRANS_ACTIVE_RED );
    else if ( m_p3DGreen->isChecked() )
	m_pTransfer->setActiveComp( sculptor_transfer::TRANS_ACTIVE_GREEN );
    else if ( m_p3DBlue->isChecked() )
	m_pTransfer->setActiveComp( sculptor_transfer::TRANS_ACTIVE_BLUE );
    else if ( m_p3DOpacity->isChecked() )
	m_pTransfer->setActiveComp( sculptor_transfer::TRANS_ACTIVE_OPACITY );
    else if ( m_p3DRGB->isChecked() )
	m_pTransfer->setActiveComp( sculptor_transfer::TRANS_ACTIVE_RGB );
}

void prop_situs::sResetTF()
{
    // reset the transfer function in the document
    m_pDoc->resetTF();

    // apply the transfer function to the plot
    m_pTransfer->transToPlot();

    // reset the plot to default (disable marker, highlight curves)
    sChangeTransferComp();

    // redraw the transfer plot and the palette
    m_pTransfer->replot();
    m_pTransferPalette->repaint();
}

/**
 *
 */
void prop_situs::sButtonApplyTF(bool bEnablingAuto)
{
    m_pTransfer->plotToTrans();
    m_pTransfer->replot();
    m_pTransferPalette->repaint();

    if (bEnablingAuto)
	m_pDoc->create3DT();
}

/**
 *
 */
void prop_situs::sApplyTF(QwtPlotCurve*)
{
    m_pTransfer->plotToTrans();
    m_pTransfer->replot();
    m_pTransferPalette->repaint();

    if ( m_p3DTAutoApply->isChecked() )
	m_pDoc->create3DT();

    // could optimize here by trying to replot only that curve that was changed...
}

/**
 *
 */
float prop_situs::getSliceTECutoff()
{
    return m_pSliceTECutoff->text().toFloat();
}

/**
 *
 */
double prop_situs::get3DSamplingRate()
{
    return m_p3DSampling->text().toDouble();
}

/**
 *
 */
svt_vector3<float> prop_situs::getLightingParams()
{
    svt_vector3<float> oV;
    oV.x( m_p3DDiffuse->text().toFloat() );
    oV.y( m_p3DAmbient->text().toFloat() );
    oV.z( m_p3DShininess->text().toFloat() );

    return oV;
}

/**
 *
 */
svt_vector3<double> prop_situs::getOrigin()
{
    svt_vector3<double> oV;
    oV.x( m_pInfoOriginX->text().toDouble() );
    oV.y( m_pInfoOriginY->text().toDouble() );
    oV.z( m_pInfoOriginZ->text().toDouble() );

    return oV;
}

/**
 *
 */
Real64 prop_situs::getVoxelWidth()
{
    return m_pInfoVoxelWidth->text().toDouble();
}

/**
 *
 */
float prop_situs::getIsoTELevel()
{
    return m_pIsoTELevel->text().toFloat();
}

/**
 *
 */
int prop_situs::getIsoTESteps()
{
    return m_pIsoTESteps->text().toInt();
}
