/***************************************************************************
                          sculptor_mapviewer
			  ------------------
    begin                : 27.07.2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// qt includes
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <math.h>
// svt includes
#include <sculptor_mapviewer.h>

/**
 * sculptor_mapviewer extends QWidget, and overrides the paintEvent(), setPenColor() and sizeHint()
 * functions associated with QWidget.  sculptor_mapviewer takes a 2D-array and renders the arrays contents
 * to produce a black and white map to the screen.  This is done by calling the createDataSet() function.
 * \param *parent is the parent QWidget that sculptor_mapviewer resides in
 * \param *name is some text that can be used to identify the sculptor_mapviewer from other QObjects.
 */ 
sculptor_mapviewer::sculptor_mapviewer(QWidget *parent, const char *name, Qt::WFlags flags) : QWidget(parent, name, flags | Qt::WNoAutoErase)
{
    m_iZoomMin = 2;
    m_iZoomMax = 64;
    m_iZoomScale = 4;
    m_fGammaValue = 1;
    m_iThresholdScale = 5;
    m_pData = NULL;
    m_pDataMask = NULL;
    m_iSegment = 0;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_oCurColor = Qt::black;
    setZoomScale(m_iZoomScale);
    setBackgroundMode (Qt::NoBackground);
}

/**
 * Reimplementation from QWidget, returns the ideal size of the sculptor_mapviewer
 */
QSize sculptor_mapviewer::sizeHint() const
{
    QSize size = QSize(m_iSizeX * m_iZoomScale, m_iSizeY * m_iZoomScale);

    if (m_iZoomScale >= 3)
        size += QSize(1, 1);

    return size;
}

/**
 * Sets the current pen color
 * \param newColor the pen color
 */
void sculptor_mapviewer::setPenColor(const QColor &oNewColor)
{
    m_oCurColor = oNewColor;
}

/**
 * Sets the zoom factor of the viewer
 * \param newZoom the new zoom scale
 */
void sculptor_mapviewer::setZoomScale(int iZoomScale)
{
    m_iZoomScale = iZoomScale;
    update();
    updateGeometry();
}

/**
 * Sets the gamma value for the gamma curve.
 * \param gammaValue the new gamma value
 */
void sculptor_mapviewer::setGammaCurve(int iGammaValue)
{
    int iTemp = iGammaValue;
    m_fGammaValue = (float)(iTemp) / 10.0f;
    update();
    updateGeometry();
}
/**
 * Sets the binary threshold value for binary mode
 * \param newZoom the new zoom scale
 */
void sculptor_mapviewer::setBinaryThreshold(int iThresholdScale)
{
    m_iThresholdScale = iThresholdScale;
    emit updateThresholdLabel(m_iThresholdScale);
    update();
    updateGeometry();
}
/**
 * Reimplementation from QWidget, to force the sculptor_mapviewer to paint itself
 * \param Automatically generated event by the application
 */
void sculptor_mapviewer::paintEvent(QPaintEvent *event)
{
    static QPixmap pixmap;
    QRect rect = event->rect();

    QSize newSize = rect.size().expandedTo(pixmap.size());
    pixmap.resize(newSize);
    pixmap.fill(this, rect.topLeft());

// QT3FIXME 
/*
    QPainter painter(&pixmap, this);
    painter.translate(-rect.x(), -rect.y());

    if (getZoomFactor() >= 3)
    {
        painter.setPen(colorGroup().dark());

        for (int x = 0; x < m_iSizeX; ++x)
            painter.drawLine(getZoomFactor() * x, 0,
                             getZoomFactor() * x, getZoomFactor() * m_iSizeY);
        for (int y = 0; y < m_iSizeY; ++y)
            painter.drawLine(0, getZoomFactor() * y,
                             getZoomFactor() * m_iSizeX, getZoomFactor() * y);
    }
    for (int x = 0; x < m_iSizeX; ++x)
        for (int y = 0; y < m_iSizeY; ++y)
            drawImagePixel(&painter, x, y);

    painter.drawRect( 0, 0, getZoomFactor() * m_iSizeX +1, getZoomFactor() * m_iSizeY +1 );

    bitBlt( this, rect.topLeft(), &pixmap );
*/
}

/**
 * Reimplementation from QWidgetm to force the sculptor_mapviewer to trigger a mouse
 * press event
 * \param Automatically generated event went a mouse button is pressed
 */
void sculptor_mapviewer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        voxelXYCoord(event->pos(), true);
    if(event->button() == Qt::RightButton)
        voxelXYCoord(event->pos(), false);

    //updateDataSetMask(event->pos());
}

/**
 * Reimplementation from QWidget
 */
void sculptor_mapviewer::wheelEvent ( QWheelEvent * pEvent )
{
    if (pEvent->delta() > 0)
        emit increaseCoord();
    else
        emit decreaseCoord();
};

/**
 * Gets the mouse position in the sculptor_mapviewer as a point.
 * \param position, the position of the mouse on the sculptor_mapviewer
 */
void sculptor_mapviewer::voxelXYCoord(const QPoint &position, bool bLeftMB)
{
    int x = position.x() / getZoomFactor();
    int y = position.y() / getZoomFactor();
    int newYPosition;
    newYPosition = (m_iSizeY - 1) - y;   // because of the coordinate system
    //i.e. (0,0) is the top left on a QWidget this makes it bottom left

    if (bLeftMB)
	emit leftClickedVoxel(x, newYPosition);
    else
	emit rightClickedVoxel(x, newYPosition);
}

/**
 * Gets the mouse position in the sculptor_mapviewer as a point.  This point is used to determine if value
 * at that location within the data set mask should be set to a zero or a one.
 * \param position, the position of the mouse on the sculptor_mapviewer
 */
void sculptor_mapviewer::updateDataSetMask(const QPoint &position)
{
    int x = position.x() / getZoomFactor();
    int y = position.y() / getZoomFactor();
    int newYPosition;
    bool value;
    newYPosition = (m_iSizeY - 1) - y;   // because of the coordinate system
    //i.e. (0,0) is the top left on a QWidget this makes it bottom left
    if (m_pDataMask != NULL && x < m_iSizeX && newYPosition < m_iSizeY && m_iSizeX >= 0 && m_iSizeY >= 0)
    {
        if(m_pDataMask[pindex(x,newYPosition)] == 0)
        {
            m_pDataMask[pindex(x,newYPosition)] = 1;
            value = true;

        } else {

            m_pDataMask[pindex(x,newYPosition)] = 0;
            value = false;
        }
        emit clickedUpdateVolumericMapMask(x, newYPosition, value);
        QPainter painter(this);
        drawImagePixel(&painter, x , newYPosition);
    }
}

/**
 * Draws a zoomed pixel using a QPainter and also determines if a pixel needs a border around it
 * to indicate that it was selected.
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapviewer::drawImagePixel(QPainter *painter, int x, int y)
{
    int color = getColorValue(x, y);
    int newYPosition;
    newYPosition = (m_iSizeY - 1) - y;   // because of the coordinate system
                                        // i.e. (0,0) is the top left on a QWidget this makes it bottom left

    if (getZoomFactor() >= 4)
    {
        if( m_pDataMask != NULL && m_iSegment != 0 && m_pDataMask[pindex(x,y)] == m_iSegment )
        {

            painter->fillRect(getZoomFactor() * x + 1, getZoomFactor() * newYPosition + 1,
                              getZoomFactor() - 1, getZoomFactor() - 1, QColor(254,color,color));

        } else {

            painter->fillRect(getZoomFactor() * x + 1, getZoomFactor() * newYPosition + 1,
                              getZoomFactor() - 1, getZoomFactor() - 1, QColor(color,color,color));
        }

    } else {

        if(m_pDataMask != NULL && m_iSegment != 0 && m_pDataMask[pindex(x,y)] == m_iSegment)
        {
            painter->fillRect(getZoomFactor() * x, getZoomFactor() * newYPosition,
                              getZoomFactor(), getZoomFactor(), QColor(254, color, color));
        } else {

            painter->fillRect(getZoomFactor() * x, getZoomFactor() * newYPosition,
                              getZoomFactor(), getZoomFactor(), QColor(color, color, color));
        }
    }
}

/**
 * Gets a value from the data set and scales the value to match the color palette.  If binary mode
 * is selected then density values below the threshold value are 255  and above are 0.
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 * \return a value that will be used in the construction of a QColor
 */
int sculptor_mapviewer::getColorValue(int x, int y)
{
    int colorValue;
    float pixelValue;
    float newPixelValue;
    if (m_bViewingMode)
    {
        double maxDensity = getMaxDensity();
        double threshold = maxDensity * m_iThresholdScale / 10;
        double value = getValue(x, y);
        if (threshold < value)
        {
            colorValue = 255;
            return colorValue;

        } else {

            colorValue = 0;
            return colorValue;
        }

    } else {

        if (m_fColorFactorValue <= 0.0f)
        {
            colorValue = 0;

        } else {

            pixelValue = ((getValue(x, y) - m_fMinDensity) / m_fColorFactorValue);
            newPixelValue = pow(pixelValue, 1 / m_fGammaValue);
            colorValue = static_cast <int> (newPixelValue * 255 + 0.5);

        }

        return colorValue;
    }
}

/**
 * Creates a new data set.
 * ATTENTION: This function will delete the old volumeric map when called.
 * \param newVolumericMap is the new volumeric map that is set
 * \param newVolumericMapMask is the new volumeric map mask, used for editing the data set
 * \param sizeX x size of the new volume data
 * \param sizeY y size of the new volume data
 * \param maxDensity maximum density of the new volume
 * \param minDensity minimum density of the new volume
 * \param viewingMode determines the mode 
 */
void sculptor_mapviewer::setCrossSection(float* pData, unsigned int *pDataMask, int sizeX, int sizeY, float fMaxDensity, float fMinDensity, bool bViewingMode)
{
    m_pData = pData;
    m_pDataMask = pDataMask;
    m_iSizeX = sizeX;
    m_iSizeY = sizeY;
    m_fMaxDensity = fMaxDensity;
    m_fMinDensity = fMinDensity;
    m_fColorFactorValue = (m_fMaxDensity - m_fMinDensity);
    m_bViewingMode = bViewingMode;
    update();
    updateGeometry();
}

/**
 * Get the value at a position inside the data set
 * \param x, x coordinate
 * \param y, y coordinate
 * \return value
*/
float sculptor_mapviewer::getValue(int x, int y)
{
    if (m_pData !=NULL && x < m_iSizeX && y < m_iSizeY && m_iSizeX >= 0 && m_iSizeY >= 0)
        return m_pData[pindex(x,y)];
    else
        return 0.0f;
}

/**
 * Get the value at a position inside the data set
 * \param count counter
 * \return value
 */
float sculptor_mapviewer::getValue(int iCount) const
{
    if (m_pData != NULL )
        return m_pData[iCount];
    else
        return 0.0f;
}

/**
 * Calculate the minimum and maximum density
 */
void sculptor_mapviewer::calcMinMaxDensity()
{
    long int iCount;
    long int iNvox =  m_iSizeX * m_iSizeY;

    /* compute range of density and set cutoff */
    m_fMaxDensity = getValue(0,0);
    m_fMinDensity = getValue(0,0);

    for ( iCount=0; iCount<iNvox; iCount++)
    {
        if (m_fMaxDensity < getValue(iCount)) m_fMaxDensity = getValue(iCount);
        if (m_fMinDensity > getValue(iCount)) m_fMinDensity = getValue(iCount);
    }
}

/**
 * Gets the minimum density value in the data set
 * \return the minimum density
 */
float sculptor_mapviewer::getMinDensity()
{
    return m_fMinDensity;
}

/**
 * Gets the maximum density value in the data set
 * \return the maximum density
 */
float sculptor_mapviewer::getMaxDensity()
{
    return m_fMaxDensity;
}

/**
 * Sets the minimum density value in the data set
 * \param the minimum density
 */
void sculptor_mapviewer::setMinDensity(float fMinDensity)
{
    m_fMinDensity = fMinDensity;
}
/**
 * Sets the maximum density value in the data set
 * \param the maximum density
 */
void sculptor_mapviewer::setMaxDensity(float fMaxDensity)
{
    m_fMaxDensity = fMaxDensity;
}

/**
 * Gets the color factor value i.e. color value = (max - min) /255
 * \return the color factor value 
 */
double sculptor_mapviewer::getColorFactor()
{
    return m_fColorFactorValue;
}
/**
 * Gets the index of cross section point in the data set
 * \param x the x coordinate in the data set
 * \param y the y coordinate in the data set
 * \return cross section point
 */
unsigned long sculptor_mapviewer::pindex(int x, int y) const
{
    return  y * m_iSizeX + x;
}
/**
 * Sets the zoom to default zoom factor
 */
void sculptor_mapviewer::applyDefaultZoom()
{
    m_iZoomScale = 6;
    setZoomScale(m_iZoomScale);
}

/**
 * Sets the zoom to minimum zoom factor
 */
void sculptor_mapviewer::applyMinZoom()
{
    m_iZoomScale = 1;
    setZoomScale(m_iZoomScale);
}

/**
 * Sets the zoom to maximum zoom factor
 */
void sculptor_mapviewer::applyMaxZoom()
{
    m_iZoomScale = 64;
    setZoomScale(m_iZoomScale);
}
/**
 * Returns the maximum zoom factor the viewer is allowed to go to
 * \return Maximum zoom factor
 */
int sculptor_mapviewer::getMaxZoomFactor()
{
    return m_iZoomMax;
}

/**
 * Returns the minimum zoom factor the viewer is allowed to go to
 * \return Minimum zoom factor
 */
int sculptor_mapviewer::getMinZoomFactor()
{
    return m_iZoomMin;
}

/**
 * Set the show segment
 */
void sculptor_mapviewer::setSegment( unsigned int iSegment )
{
    m_iSegment = iSegment;
};
