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

#ifndef MAPVIEWER_H
#define MAPVIEWER_H

// qt includes
#include <qimage.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpoint.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>
// clib
#include <math.h>
// svt includes
#include <svt_basics.h>

class DLLEXPORT sculptor_mapviewer : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(int getZoomFactor READ getZoomFactor WRITE setZoomScale)

public:

    /**
     * sculptor_mapviewer extends QWidget, and overrides the paintEvent(), setPenColor() and sizeHint()
     * functions associated with QWidget.  sculptor_mapviewer takes a 2D-array and renders the arrays contents
     * to produce a black and white map to the screen.  This is done by calling the createDataSet() function.
     * \param *parent is the parent QWidget that sculptor_mapviewer resides in
     * \param *name is some text that can be used to identify the sculptor_mapviewer from other QObjects.
     */
    sculptor_mapviewer(QWidget *parent = 0, const char *name = 0, Qt::WFlags flags = 0);
    /**
     * Sets the current pen color
     * \param newColor the pen color
     */
    void setPenColor(const QColor &newColor);

    QColor penColor() const
    {
        return m_oCurColor;
    }

    /**
     * Reimplementation from QWidget, returns the ideal size of the sculptor_mapviewer
     */
    QSize sizeHint() const;
    /** Gets a value from the data set and scales the value to match the color palette. If binary
     * mode is selected then density values below the threshold value are 255  and above are 0.
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     * \return a value that will be used in the construction of a QColor
     */
    int getColorValue(int , int );

    /**
     * Installs a new cross-section
     * ATTENTION: This function will delete the old volumeric map
     * when called.
     * \param newVolumericMap is the new volumeric map that is set
     * \param newVolumericMapMask is the new volumeric map mask, used for editing the data set
     * \param sizeX x size of the new volume data
     * \param sizeY y size of the new volume data
     * \param maxDensity maximum density of the new volume
     * \param minDensity minimum density of the new volume
     * \param viewingMode determines the mode
     */
    void setCrossSection(float* , unsigned int *, int , int, float , float , bool);
    /**
     * Get the value at a position inside the data set
     * \param x, x coordinate
     * \param y, y coordinate
     * \return value
     */
    float getValue(int , int );
    /**
     * Get the value at a position inside the data set
     * \param count counter
     * \return value
     */
    float getValue(int count) const;
    /**
     * Calculate the minimum and maximum density
     */
    void calcMinMaxDensity();
    /**
     * Gets the minimum density value in the data set
     * \return the minimum density
     */
    float getMinDensity();
    /**
     * Gets the maximum density value in the data set
     * \return the maximum density
     */
    float getMaxDensity();
    /**
     * Sets the minimum density value in the data set
     * \param the minimum density
     */
    void setMinDensity( float );
    /**
     * Sets the maximum density value in the data set
     * \param the maximum density
     */
    void setMaxDensity( float );

    /**
     * Set the show segment
     */
    void setSegment( unsigned int iSegment );

    /**
     * Gets the index of cross section point in the data set
     * \param x the x coordinate in the data set
     * \param y the y coordinate in the data set
     * \return cross section point
     */
    unsigned long pindex(int , int ) const;
    /**
     * Sets the zoom to default zoom factor
     */
    void applyDefaultZoom();
    /**
     * Sets the zoom to minimum zoom factor
     */
    void applyMinZoom();
    /**
     * Sets the zoom to maximum zoom factor
     */
    void applyMaxZoom();

    int getZoomFactor() const { return m_iZoomScale; }
    /**
     * Returns the maximum zoom factor the viewer is allowed to go to
     * \return Maximum zoom factor
     */
    int getMaxZoomFactor();
    /**
     * Returns the minimum zoom factor the viewer is allowed to go to
     * \return Minimum zoom factor
     */
    int getMinZoomFactor();

    /**
     * Gets the color factor value i.e. color value = (max - min) /255
     * \return the color factor value
     */
    double getColorFactor();

public slots:

    /**
     * Sets the zoom factor of the viewer
     * \param newZoom the new zoom scale
     */
    void setZoomScale(int newZoom);
    /**
     * Sets the gamma value for the gamma curve.
     * \param gammaValue the new gamma value
     */
    void setGammaCurve(int gammaValue);
    /**
     * Sets the binary threshold value for binary mode
     * \param newZoom the new zoom scale
     */
    void setBinaryThreshold(int newThreshold);

signals:

    /**
     * Sends a signal that indicates the mouse position in the sculptor_mapviewer
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     */
    void leftClickedVoxel(int x, int y);
    /**
     * Sends a signal that indicates the mouse position in the sculptor_mapviewer
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     */
    void rightClickedVoxel(int x, int y);

    /**
     * Sends a signal that indicates that the coordinate should get increased
     */
    void increaseCoord();
    /**
     * Sends a signal that indicates that the coordinate should get decreased
     */
    void decreaseCoord();

    /**
     * Sends a signal that indicates the mouse position in the sculptor_mapviewer where the data set mask
     * needs to be updated.
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     * \param value, the mask needs to be set to either a one or a zero.
     */
    void clickedUpdateVolumericMapMask(int x, int y, bool value);

    /**
     * Sends a signal to update the threshold label on the MapEditor
     * \param m_thresholdScale, the value that the label is to be changed to.
     */
    void updateThresholdLabel(int );

protected:

    /**
     * Reimplementation from QWidget, to force the sculptor_mapviewer to paint itself
     * \param Automatically generated event by the application
     */
    void paintEvent(QPaintEvent *event);
    /**
     * Reimplementation from QWidgetm to force the sculptor_mapviewer to trigger a mouse
     * press event
     * \param Automatically generated event went a mouse button is pressed
     */
    void mousePressEvent(QMouseEvent *event);
    /**
     * Reimplementation from QWidget
     */
    void wheelEvent ( QWheelEvent * pEvent );

private:

    /**
     * Draws a zoomed pixel using a QPainter and also determines if a pixel needs a border around it
     * to indicate that it was selected.
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    void drawImagePixel(QPainter *painter, int i, int j);

    /**
     * Gets the mouse position in the sculptor_mapviewer as a point.
     * \param position, the position of the mouse on the sculptor_mapviewer
     */
    void voxelXYCoord(const QPoint &position, bool bLeftMB);

    /**
     * Gets the mouse position in the sculptor_mapviewer as a point.  This point is used to determine if value
     * at that location within the data set mask should be set to a zero or a one.
     * \param position, the position of the mouse on the sculptor_mapviewer
     */
    void updateDataSetMask(const QPoint &position);

private:

    int m_iZoomMax;
    int m_iZoomMin;
    int m_iZoomScale;
    int m_iThresholdScale;

    double m_fColorFactorValue;

    float *m_pData;
    unsigned int *m_pDataMask;
    unsigned int m_iSegment;
    float m_fMaxDensity;
    float m_fMinDensity;
    int m_iSizeX;
    int m_iSizeY;

    float m_fGammaValue;

    bool m_bViewingMode;

    QColor m_oCurColor;
    QPixmap m_pixmap;

};

#endif
