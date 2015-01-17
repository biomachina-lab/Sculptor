/***************************************************************************
                          sculptor_dlg_mapexplorer.cpp
                          -------------------
    begin                : 10/21/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_mapexplorer.h>
#include <sculptor_dlg_mapexplorer.h>
#include <situs_document.h>
// svt includes
#include <svt_cmath.h>
// qt4 includes
#include <QTableView>
#include <QPainter>
#include <QMenu>
#include <QWheelEvent>


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Constructor
 */
sculptor_mapexplorer_label::sculptor_mapexplorer_label(QWidget* pParent, sculptor_mapexplorer* pExplorer)
    : QLabel( pParent ),
      m_pExplorer( pExplorer )
{
    // need to have a fixes size so that the containing QScrollArea behaves correctly
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    // build the context menu
    m_pContextMenu = new QMenu(pParent);
    m_pContextMenu->addAction( tr("Start Floodfill From Here"), pExplorer, SLOT(sShowFloodfillDialog()) );
    m_pContextMenu->addSeparator();
    m_pContextMenu->addAction( tr("Print Image"),               pExplorer, SLOT(sPrint()) );
    m_pContextMenu->addAction( tr("Save Image"),                pExplorer, SLOT(sSave()) );

    // calculate the size of the map
    updateSize();
}

/*
 *
 */
QSize sculptor_mapexplorer_label::sizeHint() const
{
    // the containing QScrollArea calls this to know about the map's size
    return QSize( m_iWidth, m_iHeight );
}

/**
 *
 */
void sculptor_mapexplorer_label::mouseMoveEvent(QMouseEvent* pEvent)
{
    m_pExplorer->getDialog()->showVoxelInfo( pEvent->pos().x(), pEvent->pos().y() );
}

/**
 *
 */
void sculptor_mapexplorer_label::mousePressEvent(QMouseEvent* pEvent)
{
    m_pContextMenu->popup( mapToGlobal(pEvent->pos()) );
}

/*
 *
 */
void sculptor_mapexplorer_label::paintEvent(QPaintEvent* pPaintEvent)
{
    QPainter pPainter(this);
    int iRectSize = m_iTileSize;
    int c;

    // compute the area that has to be redrawn, extending it so that it fully covers the partially
    // shown tiles at the borders
    int iLeft   =  max(0,         pPaintEvent->rect().left()-1);
    int iRight  =  min(m_iWidth,  pPaintEvent->rect().right()+m_iTileSize);
    int iTop    =  max(0,         pPaintEvent->rect().top()-1);
    int iBottom =  min(m_iHeight, pPaintEvent->rect().bottom()+m_iTileSize);

    // compute the indices of the tiles, which correspond to the volume coordinates. using these, we
    // will query the voxel density values
    int iXFirstTile  = iLeft  / m_iTileSize;
    int iXLastTile   = iRight / m_iTileSize;
    int iYFirstTile  = iTop    / m_iTileSize;
    int iYLastTile   = iBottom / m_iTileSize;

    // from the tile indices, compute the pixel positions. these are used to acutally draw the
    // rectangles that represent the density values
    int iXFirstPixel = iXFirstTile  * m_iTileSize;
    int iXLastPixel  = iXLastTile   * m_iTileSize;
    int iYFirstPixel = iYFirstTile  * m_iTileSize;
    int iYLastPixel  = iYLastTile   * m_iTileSize;


/*  printf("\npaintEvent  %i,%i - %i,%i\n",
           pPaintEvent->rect().left(),  pPaintEvent->rect().top(), 
           pPaintEvent->rect().right(), pPaintEvent->rect().bottom() );
    printf("extended    %i,%i - %i,%i\n", iLeft,        iTop,         iRight,      iBottom);
    printf("pixels      %i,%i - %i,%i\n", iXFirstPixel, iYFirstPixel, iXLastPixel, iYLastPixel);
    printf("tile s      %i,%i - %i,%i\n", iXFirstTile , iYFirstTile , iXLastTile , iYLastTile );
*/    

    // fill the background black. this way, we can skip drawing voxels that have zero density and
    // would be drawn black anyway
    pPainter.fillRect(iLeft,  iTop, iRight - iLeft, iBottom - iTop,
                      QColor(Qt::black) );

    // check if we should draw a grid
    if ( m_pExplorer->getGridVisible() )
    {
        pPainter.setPen(QColor(Qt::lightGray));

        for (int x=iXFirstPixel; x<=iXLastPixel; x+= m_iTileSize)
            pPainter.drawLine(x, iYFirstPixel, x, iYLastPixel);

        for (int y=iYFirstPixel; y<=iYLastPixel; y+= m_iTileSize)
            pPainter.drawLine(iXFirstPixel, y, iXLastPixel, y);

        // if there's a grid, the rectangles will be one pixel smaller than the tiles including the
        // grid line
        --iRectSize;
    }

    // now walk over the draw area, and paint the rectangles representing voxel densities greater
    // than zero
    for (int x=iXFirstPixel+1, vx=iXFirstTile ; vx<iXLastTile ; x+= m_iTileSize, ++vx)
    {
        for (int y=iYFirstPixel+1, vy=iYFirstTile ; vy<iYLastTile ; y+= m_iTileSize, ++vy)
        {
            // if the density is not zero, draw the rectangle
            c = m_pExplorer->getDataValue(vx, vy);

            if ( c != 0 )
                pPainter.fillRect(x, y, iRectSize, iRectSize, m_pExplorer->getColor(c));
        }
    }
}

/*
 *
 */
void sculptor_mapexplorer_label::updateSize()
{
    // get the size of a tile, which includes space for the grid if it is enabled
    m_iTileSize = m_pExplorer->getZoom() + (int)(m_pExplorer->getGridVisible());

    // compute the space that is needed for all tiles
    m_iWidth  = m_pExplorer->getDataWidth()  * m_iTileSize;
    m_iHeight = m_pExplorer->getDataHeight() * m_iTileSize;

    // add one pixel for the last gridline
    if ( m_pExplorer->getGridVisible() )
    {
        ++m_iWidth;
        ++m_iHeight;
    }

    // generate a resize event
    resize( QSize(m_iWidth, m_iHeight) );
    // and redraw the image
    updateImage();
}

/*
 *
 */
void sculptor_mapexplorer_label::updateImage()
{
    // generate a repaint event spanning the whole visible region
    repaint(visibleRegion());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
dlg_mapexplorer_mousefilter::dlg_mapexplorer_mousefilter(dlg_mapexplorer* pDlg, QWidget* pMapWidget)
    : QObject( pMapWidget ),
      m_pDlg( pDlg ),
      m_pTarget( pMapWidget )
{
}

/**
 *
 */
bool dlg_mapexplorer_mousefilter::eventFilter(QObject* pObject, QEvent* pEvent)
{
    // correct event receiver?
    if ( pObject == m_pTarget )
    {
        // is it a wheel event?
	if ( pEvent->type() == QEvent::Wheel )
	{
            // srolled up or down?
            if ( ((QWheelEvent*)pEvent)->delta() > 0 )
                m_pDlg->mouseWheelUp();
            else
                m_pDlg->mouseWheelDown();

            return true;
	}
    }
    return QObject::eventFilter(pObject, pEvent);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Constructor
 */
dlg_mapexplorer::dlg_mapexplorer(sculptor_mapexplorer* pExplorer, svt_volume<Real64>* pVolume)
    : QWidget( NULL ),
      m_pVolume( pVolume ),
      m_pExplorer   ( pExplorer ),
      m_bGrid       ( pExplorer->getGridVisible() ),
      m_iZoom       ( pExplorer->getZoom() ),
      m_fGamma      ( pExplorer->getGamma() ),
      m_fThreshold  ( pExplorer->getThreshold() ),
      m_iSlice      ( pExplorer->getSlice() ),
      m_iMapping    ( pExplorer->getMapping() ),
      m_iDisplayMode( pExplorer->getDisplayMode() )
{
    setupUi(this);

    //setWindowIcon( QIcon(QPixmap(mapExplorerTool)) );

    // update values in the fields
    m_pCheckBoxGrid->setChecked( m_bGrid );
    m_pSliderZoom->setValue( m_iZoom );
    m_pSliderThreshold ->setValue((int)(m_fThreshold*1000.0f));
    m_pSpinBoxThreshold->setValue(m_fThreshold*100.0f);
    m_pSliderGamma ->setValue((int)(m_fGamma*100.0f));
    m_pSpinBoxGamma->setValue(m_fGamma);
    m_pSliderSlice ->setValue(m_iSlice);
    m_pSpinBoxSlice->setValue(m_iSlice);
    m_pSliderSlice ->setMaximum(m_pVolume->getSizeX()-1);
    m_pSpinBoxSlice->setMaximum(m_pVolume->getSizeX()-1);
    clearVoxelInfo();

    // create the widget that will display the map image
    m_pLabel = new sculptor_mapexplorer_label(m_pScrollAreaMap, m_pExplorer);
    // enable its mouse tracking
    m_pLabel->setMouseTracking(TRUE);
    // and set it as the scrollarea's widget
    m_pScrollAreaMap->setWidget(m_pLabel);


    //////////////////////////////////////////////////////////////////////////////////////////
    // this eventfilter filters wheel events and sends them to the dialog
    // 
    dlg_mapexplorer_mousefilter* pMousefilter;

    // put one filter into the label displaying the image, so that we catch that the mouse wheel is
    // used *on* the image
    pMousefilter = new dlg_mapexplorer_mousefilter( this, m_pLabel );
    m_pLabel->installEventFilter(pMousefilter);

    // and put one filter into the scrollarea, to catch usage of mouse wheel *next* to the image
    pMousefilter = new dlg_mapexplorer_mousefilter( this, m_pScrollAreaMap->viewport() );
    m_pScrollAreaMap->viewport()->installEventFilter(pMousefilter);



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  cross section selection and projection along axis

    m_pButtonGroupMapping->setId(m_pRadioButtonCrossSectionX, sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_X);
    m_pButtonGroupMapping->setId(m_pRadioButtonCrossSectionY, sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Y);
    m_pButtonGroupMapping->setId(m_pRadioButtonCrossSectionZ, sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Z);
    m_pButtonGroupMapping->setId(m_pRadioButtonProjectionX,   sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X);
    m_pButtonGroupMapping->setId(m_pRadioButtonProjectionY,   sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y);
    m_pButtonGroupMapping->setId(m_pRadioButtonProjectionZ,   sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Z);

    // cross sections to through x are default
    sSetMapping( sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_X );

    connect( m_pButtonGroupMapping,  SIGNAL(buttonClicked(int)), this, SLOT(sSetMapping(int)) );

    connect( m_pSliderSlice,  SIGNAL(valueChanged(int)),  this, SLOT(sSetSlice(int)) );
    connect( m_pSliderSlice,  SIGNAL(valueChanged(int)),  this, SLOT(sUpdateSpinBoxSlice(int)) );
    connect( m_pSpinBoxSlice, SIGNAL(valueChanged(int)),  this, SLOT(sUpdateSliderSlice(int)) );
    connect( m_pSpinBoxSlice, SIGNAL(editingFinished()),  this, SLOT(sUpdateSliderSlice()) );


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  display normal with gamma adjustment or binary with threshold

    m_pButtonGroupDisplay->setId(m_pRadioButtonDisplayThreshold, sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_THRESHOLD);
    m_pButtonGroupDisplay->setId(m_pRadioButtonDisplayGamma,     sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_GAMMA);

    // gamma is default
    sSetDisplayMode( sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_GAMMA );

    connect( m_pButtonGroupDisplay, SIGNAL(buttonClicked(int)),   this, SLOT(sSetDisplayMode(int)) );

    connect( m_pSliderGamma,        SIGNAL(valueChanged(int)),    this, SLOT(sSetGamma(int)) );
    connect( m_pSliderGamma,        SIGNAL(valueChanged(int)),    this, SLOT(sUpdateSpinBoxGamma(int)) );
    connect( m_pSpinBoxGamma,       SIGNAL(valueChanged(double)), this, SLOT(sUpdateSliderGamma(double)) );
    connect( m_pSpinBoxGamma,       SIGNAL(editingFinished()),    this, SLOT(sUpdateSliderGamma()) );

    connect( m_pSliderThreshold,    SIGNAL(valueChanged(int)),    this, SLOT(sSetThreshold(int)) );
    connect( m_pSliderThreshold,    SIGNAL(valueChanged(int)),    this, SLOT(sUpdateSpinBoxThreshold(int)) );
    connect( m_pSpinBoxThreshold,   SIGNAL(valueChanged(double)), this, SLOT(sUpdateSliderThreshold(double)) );
    connect( m_pSpinBoxThreshold,   SIGNAL(editingFinished()),    this, SLOT(sUpdateSliderThreshold()) );


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  grid

    connect( m_pCheckBoxGrid, SIGNAL(toggled(bool)), this, SLOT(sSetGridVisible(bool)) );


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  zoom

    connect( m_pSliderZoom, SIGNAL(valueChanged(int)), this, SLOT(sSetZoom(int)) );
}

/*
 * Destructor
 */
dlg_mapexplorer::~dlg_mapexplorer()
{
}

/*
 *
 */
sculptor_mapexplorer_label* dlg_mapexplorer::getLabel()
{
    return m_pLabel;
}

/*
 *
 */
bool dlg_mapexplorer::getCurrentVoxel(unsigned int* pX, unsigned int* pY, unsigned int* pZ, float* pDensity)
{
    bool bConversionSuccess = false;
    QString oStr;

    // get the voxel coordinates from the textfields. return if any one's conversion fails
    oStr = m_pX->text().remove(0, 2);
    *pX = oStr.toUInt(&bConversionSuccess);
    if (!bConversionSuccess)
        return false;

    oStr = m_pY->text().remove(0, 2);
    *pY = oStr.toUInt(&bConversionSuccess);
    if (!bConversionSuccess)
        return false;

    oStr = m_pZ->text().remove(0, 2);
    *pZ = oStr.toUInt(&bConversionSuccess);
    if (!bConversionSuccess)
        return false;

    // get the voxel density value
    *pDensity = m_pDensity->text().toFloat(&bConversionSuccess);
    if (!bConversionSuccess)
        return false;

    return true;
}

/*
 *
 */
void dlg_mapexplorer::sSetMapping(int iMapping)
{
    int iMax;

    m_iMapping = iMapping;

    m_pSliderSlice->blockSignals( TRUE );
    m_pSpinBoxSlice->blockSignals( TRUE );

    // set the maximum values for the slider and the spinbox according to volume size
    switch (iMapping)
    {
        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_X:
            iMax = m_pVolume->getSizeX() - 1;
            m_pSpinBoxSlice->setMaximum( iMax );
            m_pSliderSlice ->setMaximum( iMax );
            if ( m_iSlice > iMax )
                sSetSlice( iMax );
            m_pSpinBoxSlice->setEnabled( TRUE );
            m_pSliderSlice ->setEnabled( TRUE );
            break;

        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Y:
            iMax = m_pVolume->getSizeY() - 1;
            m_pSpinBoxSlice->setMaximum( iMax );
            m_pSliderSlice ->setMaximum( iMax );
            if ( m_iSlice > iMax )
                sSetSlice( iMax );
            m_pSpinBoxSlice->setEnabled( TRUE );
            m_pSliderSlice ->setEnabled( TRUE );
            break;

        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Z:
            iMax = m_pVolume->getSizeZ() - 1;
            m_pSpinBoxSlice->setMaximum( iMax );
            m_pSliderSlice ->setMaximum( iMax );
            if ( m_iSlice > iMax )
                sSetSlice( iMax );
            m_pSpinBoxSlice->setEnabled( TRUE );
            m_pSliderSlice ->setEnabled( TRUE );
            break;

        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X:
        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y:
        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Z:
            m_pSpinBoxSlice->setEnabled( FALSE );
            m_pSliderSlice ->setEnabled( FALSE );
            break;
    }

    m_pSliderSlice->blockSignals( FALSE );
    m_pSpinBoxSlice->blockSignals( FALSE );

    // set the plane in the model/view framework and repaint it
    m_pExplorer->setMapping(iMapping);

    // update the image
    m_pLabel->updateSize();

    // clear info fields
    clearVoxelInfo();
}

/*
 *
 */
void dlg_mapexplorer::sSetDisplayMode(int iMode)
{
    switch (iMode)    {
        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_GAMMA:
            m_pSpinBoxGamma->setEnabled( TRUE );
            m_pSliderGamma ->setEnabled( TRUE );
            m_pSpinBoxThreshold->setEnabled( FALSE );
            m_pSliderThreshold ->setEnabled( FALSE );
            break;

        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_THRESHOLD:
            m_pSpinBoxGamma->setEnabled( FALSE );
            m_pSliderGamma ->setEnabled( FALSE );
            m_pSpinBoxThreshold->setEnabled( TRUE );
            m_pSliderThreshold ->setEnabled( TRUE );
            break;
    }
    m_pExplorer->setDisplayMode(iMode);
    m_pLabel->updateImage();
}

/*
 *
 */
void dlg_mapexplorer::sUpdateSliderSlice(int iValue)
{
    if ( iValue == -1 )
        iValue = m_pSpinBoxSlice->value();

    m_pSliderSlice->setValue(iValue);
}

/*
 *
 */
void dlg_mapexplorer::sUpdateSpinBoxSlice(int iValue)
{
    m_pSpinBoxSlice->blockSignals( TRUE );
    m_pSpinBoxSlice->setValue( iValue );
    m_pSpinBoxSlice->blockSignals( FALSE );
}

/*
 *
 */
void dlg_mapexplorer::sSetSlice(int iValue)
{
    m_iSlice = iValue;
    m_pExplorer->setSlice(m_iSlice);
    m_pLabel->updateSize();
}

/*
 *
 */
void dlg_mapexplorer::sUpdateSliderGamma(double fValue)
{
    if ( fValue == -1.0 )
        fValue = m_pSpinBoxGamma->value();

    m_pSliderGamma->setValue( (int)(fValue*100.0) );
}

/*
 *
 */
void dlg_mapexplorer::sUpdateSpinBoxGamma(int iValue)
{
    m_pSpinBoxGamma->blockSignals( TRUE );
    m_pSpinBoxGamma->setValue( ((double)iValue)/100.0 );
    m_pSpinBoxGamma->blockSignals( FALSE );
}

/*
 *
 */
void dlg_mapexplorer::sSetGamma(int iValue)
{
    m_fGamma = ((double)iValue) / 100.0;
    m_pExplorer->setGamma(m_fGamma);
    m_pLabel->updateImage();
}

/*
 *
 */
void dlg_mapexplorer::sUpdateSliderThreshold(double fValue)
{
    if ( fValue == -1.0 )
        fValue = m_pSpinBoxGamma->value();

    m_pSliderThreshold->setValue( (int)(fValue*10.0) );
}

/*
 *
 */
void dlg_mapexplorer::sUpdateSpinBoxThreshold(int iValue)
{
    m_pSpinBoxThreshold->blockSignals( TRUE );
    m_pSpinBoxThreshold->setValue( ((double)iValue) / 10.0 );
    m_pSpinBoxThreshold->blockSignals( FALSE );
}

/*
 *
 */
void dlg_mapexplorer::sSetThreshold(int iValue)
{
    m_fThreshold = ((double)iValue) / 1000.0;
    m_pExplorer->setThreshold(m_fThreshold);
    m_pLabel->updateImage();
}

/*
 *
 */
void dlg_mapexplorer::sSetGridVisible(bool bGrid)
{
    m_bGrid = bGrid;
    m_pExplorer->setGridVisible(bGrid);
    m_pLabel->updateSize();
}

/*
 *
 */
void dlg_mapexplorer::sSetZoom(int iZoom)
{
    m_iZoom = iZoom;
    m_pExplorer->setZoom(m_iZoom);
    m_pLabel->updateSize();
}

/*
 *
 */
void dlg_mapexplorer::showVoxelInfo(int iX, int iY)
{
    QString oStr;

    // convert the screen pixel coordinates into voxel coordinates
    int iTileSize = m_iZoom + (int)m_bGrid;
    iX /= iTileSize;
    iY /= iTileSize;

    if ( iX<0 || iX >= (int)m_pExplorer->getDataWidth() || iY<0 || iY >= (int)m_pExplorer->getDataHeight() )
    {
        clearVoxelInfo();
        return;
    }

    switch ( m_iMapping )
    {
        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_X:
            oStr = QString("x=%1").arg(m_iSlice); m_pX->setText( oStr );
            oStr = QString("y=%1").arg(iY      ); m_pY->setText( oStr );
            oStr = QString("z=%1").arg(iX      ); m_pZ->setText( oStr );
            //
            oStr.setNum( m_pVolume->at(m_iSlice, iY, iX) );
            m_pDensity->setText(oStr);
            break;

        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Y:
            oStr = QString("x=%1").arg(iX       ); m_pX->setText( oStr );
            oStr = QString("y=%1").arg(m_iSlice ); m_pY->setText( oStr );
            oStr = QString("z=%1").arg(iY       ); m_pZ->setText( oStr );
            //
            oStr.setNum( m_pVolume->at(iX, m_iSlice, iY) );
            m_pDensity->setText(oStr);
            break;

        case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Z:
            oStr = QString("x=%1").arg(iX       ); m_pX->setText( oStr );
            oStr = QString("y=%1").arg(iY       ); m_pY->setText( oStr );
            oStr = QString("z=%1").arg(m_iSlice ); m_pZ->setText( oStr );
            //
            oStr.setNum( m_pVolume->at(iX, iY, m_iSlice) );
            m_pDensity->setText(oStr);
            break;

        default:
            m_pX->setText(QString("-"));
            m_pY->setText(QString("-"));
            m_pZ->setText(QString("-"));
            m_pDensity->setText(QString("-"));
            break;
    }
}

/**
 *
 */
void dlg_mapexplorer::clearVoxelInfo()
{
    m_pX->setText(QString("-"));
    m_pY->setText(QString("-"));
    m_pZ->setText(QString("-"));
    m_pDensity->setText(QString("-"));
}

/**
 *
 */
void dlg_mapexplorer::updateImage()
{
    m_pLabel->update();
}

/**
 *
 */
void dlg_mapexplorer::dataChanged()
{
    clearVoxelInfo();
    updateImage();
}

/**
 *
 */
void dlg_mapexplorer::mouseWheelUp()
{
    if ( QApplication::keyboardModifiers() == Qt::ControlModifier )
    {
        if ( m_iZoom == m_pSliderZoom->maximum() )
            return;
        sSetZoom( m_iZoom+1 );
        m_pSliderZoom->blockSignals(TRUE);
        m_pSliderZoom->setValue(m_iZoom);
        m_pSliderZoom->blockSignals(FALSE);
    }
    else
    {
        switch (m_iMapping)
        {
            case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_X:
                if ( m_iSlice >= (int)(m_pVolume->getSizeX() - 1) )
                    return;
                break;

            case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Y:
                if ( m_iSlice >= (int)(m_pVolume->getSizeY() - 1) )
                    return;
                break;

            case sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Z:
                if ( m_iSlice >= (int)(m_pVolume->getSizeZ() - 1) )
                    return;
                break;

            default:
                // do not change the slice if the mapping is not slice
                return;
        }

        // the slider has a valueChanged signal connection to actually set the slice
        sUpdateSliderSlice(m_iSlice+1);
        // the spinbox does not
        sUpdateSpinBoxSlice(m_iSlice);
    }

    QPoint oPos = m_pLabel->mapFromGlobal( QCursor::pos() );
    showVoxelInfo(oPos.x(), oPos.y());
}

/**
 *
 */
void dlg_mapexplorer::mouseWheelDown()
{
    if ( QApplication::keyboardModifiers() == Qt::ControlModifier )
    {
        if ( m_iZoom == 1 )
            return;
        sSetZoom( m_iZoom-1 );
        m_pSliderZoom->blockSignals(TRUE);
        m_pSliderZoom->setValue(m_iZoom);
        m_pSliderZoom->blockSignals(FALSE);
    }
    else
    {
        // return if mapping is not per slice, or we are already at slice zero
        if ( ( m_iMapping != sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_X &&
               m_iMapping != sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Y &&
               m_iMapping != sculptor_mapexplorer::SCULPTOR_MAPEXPLORER_CROSSSECTION_Z    ) ||
             m_iSlice == 0 )
        {
            return;
        }

        sUpdateSliderSlice(m_iSlice-1);
        sUpdateSpinBoxSlice(m_iSlice);
    }

    QPoint oPos = m_pLabel->mapFromGlobal( QCursor::pos() );
    showVoxelInfo(oPos.x(), oPos.y());
}
