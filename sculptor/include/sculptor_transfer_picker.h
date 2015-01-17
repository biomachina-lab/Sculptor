/*-*-c++-*-*****************************************************************
			  sculptor_transfer_picker
			  ------------------------
    begin                : 03/03/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef __SENSITUS_TRANSFER_PICKER_H
#define __SENSITUS_TRANSFER_PICKER_H

// sculptor includes
class sculptor_qwtdata;
// qwt5 includes
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_item.h>
#include <qwt_double_rect.h>
class QwtPlotCanvas;
class QwtPlotCurve;
// qt4 includes
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QMenu>
class QMouseEvent;


/**
 * A simple class that implements a selection rectangle for a QwtPlot.
 * \author Manuel Wahle
 */
class sculptor_transfer_rectitem : public QwtPlotItem
{
    // pen for the boundary
    QPen m_oPen;
    // brush to fill the rectangle
    QBrush m_oBrush;
    // the rectangle coords (relative to the plot)
    QwtDoubleRect m_oRect;

public:

    /**
     * Constructor
     */
    sculptor_transfer_rectitem();

    /**
     * Destructor
     */
    virtual ~sculptor_transfer_rectitem()
    {};

    /**
     * Set the boundary pen
     * \param QPen boundary pen
     */
    void setPen(const QPen &oPen);

    /**
     * Set the area brush
     * \param QBruch area fill pattern
     */
    void setBrush(const QBrush &oBrush);

    /**
     * Set the rectangle coords and size
     * \param QwtDoubleRect left lower corner and dimensions in plot coords
     */
    void setRect(const QwtDoubleRect& oRect);

    /**
     * Draw the valid rectangle
     */
    virtual void draw(QPainter *oPainter, const QwtScaleMap &oMapX, const QwtScaleMap &oMapY, const QRect& ) const;
};


/**
 * This class derives from QwtPlotPicker and implements adding points and dragging areas on the 3d
 * texture plot.
 *
 * The only thing it has in common with the previous Qt3 version is the file name.
 * \author Manuel Wahle
 */
class sculptor_transfer_picker : public QwtPlotPicker
{
    Q_OBJECT

    // the color component curves ...
    QwtPlotCurve* m_pCurveR;
    QwtPlotCurve* m_pCurveG;
    QwtPlotCurve* m_pCurveB;
    QwtPlotCurve* m_pCurveO;

    // ... and their data objects
    sculptor_qwtdata* m_pDataR;
    sculptor_qwtdata* m_pDataG;
    sculptor_qwtdata* m_pDataB;
    sculptor_qwtdata* m_pDataO;

    // convenience pointer to the currently selected curve and its data
    QwtPlotCurve* m_pSelectedCurve;
    sculptor_qwtdata* m_pSelectedCurveData;

    // enum of the currently selected curve
    int m_eSelectedCurve;

    // the currently selected point on a curve
    int m_iSelectedCurvePoint;

    // min and max density for scaling the x-axis
    double m_fMinDensity;
    double m_fMaxDensity;

    // remember where the button was pressed down
    double m_fMousePressPos_X;

    // need to check on button release if something was dragged
    bool m_bMoved;

    // the little red symbol that highlights a selected curve point
    QwtPlotMarker m_oSelectionMarker;

    // the area that highlights a dragged range
    sculptor_transfer_rectitem m_oRangeMarker;

    // remember the user selected color
    QColor m_oColor;

    // the context menu
    QMenu m_oContextMenu;

    // the action to remove a point
    QAction* m_pActionRemove;

public:

    /**
     * Constructor
     */
    sculptor_transfer_picker(QwtPlotCanvas* pCanvas);

    /*
     * Destructor
     */
    virtual ~sculptor_transfer_picker();

    /**
     * Select a curve:
     * \param int eCurve:
     * - scultpor_transfer::TRANS_ACTIVE_RED
     * - scultpor_transfer::TRANS_ACTIVE_GREEN
     * - scultpor_transfer::TRANS_ACTIVE_BLUE
     * - scultpor_transfer::TRANS_ACTIVE_OPACITY
     * - scultpor_transfer::TRANS_ACTIVE_RGB
     */
    void selectComponent(int eCurve);

    /**
     * \name Init methods
     * Deliver vital data for this picker to work and must be called before it is used.
     */
    //@{
    /**
     * set the min and max density of the current volume
     * \param double fMin
     * \param double fMax
     */
    void setMinMaxDensity(double fMin, double fMax);

    /**
     * set pointers to the curve and corresponding data objects which are held in sculptor_transfer
     */
    void initCurves(QwtPlotCurve* pCurveR, sculptor_qwtdata* pDataR,
		    QwtPlotCurve* pCurveG, sculptor_qwtdata* pDataG,
		    QwtPlotCurve* pCurveB, sculptor_qwtdata* pDataB,
		    QwtPlotCurve* pCurveO, sculptor_qwtdata* pDataO);
    //@}

signals:

    /**
     * emit this signal when plot data changed
     * \param specify a specific curve to be replotted
     */
    void plotChanged(QwtPlotCurve* pCurve = NULL);

protected:

    /**
     * show the context menu at the specified point
     */
    void showContextMenu(QPoint oPoint);

    /**
     * overloaded mouse press event function that adds a point to the currently selected curves or
     * remember the mouse position to determine the dragged area
     * \param QMouseEvent* e
     */
    virtual void widgetMousePressEvent(QMouseEvent* e);

    /**
     * overloaded mouse release event function. determines if dragging occurred and colors it if so.
     * \param QMouseEvent* e
     */
    virtual void widgetMouseReleaseEvent(QMouseEvent* e);

    /**
     * adds a point to a curve
     * \param fX, fY doubles with the x- and y-coords (plot coords)
     * \param QwtPlotCurve* pCurve the desired curve
     * \param sculptor_qwtdata* pData the desired curve's data
     */
    int addPoint(double fX, double fY, QwtPlotCurve* pCurve, sculptor_qwtdata* pData);

    /**
     * asks the user for a color and sets that in the given range. calls clearRange() to remove all
     * existing curve points in that range
     * \param fX, fY doubles with the x- and y-coords
     * \param double fStart start value (plot coord)
     * \param double fEnd end value (plot coord)
     */
    void putRange(double fStart, double fEnd);

    /**
     * removes all existing points in the given range
     * \param fStart, fEnd the range
     * \param sculptor_qwtdata* pData the data objects from which to remove the points
     */
    void clearRange(double fStart, double fEnd, sculptor_qwtdata* pData);

protected slots:

    /**
     * slot that is called when the mouse is dragged
     * either moves a point or highlights an area
     * \param QwtDoublePoint& oPoint the current mouse position in the plot
     */
    void sMoved(const QwtDoublePoint& oPoint);

    /**
     * slot that is called when the selected curve point should be removed
     */
    void sRemovePoint();

    /**
     * slot that is called to reset the red curve
     */
    void sResetCurveR();

    /**
     * slot that is called to reset the red curve
     */
    void sResetCurveG();

    /**
     * slot that is called to reset the blue curve
     */
    void sResetCurveB();

    /**
     * slot that is called to reset the opacity curve
     */
    void sResetCurveO();
};

#endif
