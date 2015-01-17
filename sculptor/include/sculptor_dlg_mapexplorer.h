/*-*-c++-*-*****************************************************************
                          sculptor_dlg_mapexplorer.h
			  -------------------
    begin                : 10/21/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_MAPEXPLORER_H
#define SCULPTOR_DLG_MAPEXPLORER_H

// sculptor includes
class situs_document;
class dlg_mapexplorer;
class sculptor_mapexplorer;
// svt includes
#include <svt_volume.h>
// uic-qt4 includes
#include <ui_dlg_mapexplorer.h>
// qt4 includes
#include <QLabel>
class QPaintEvent;
class QResizeEvent;
class QMenu;


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief This class derives from QLabel to show the density map.
 *
 * It draws the map from the normalized and gamma'd or thresholded volume density data that it gets
 * from sculptor_mapexplorer.
 *
 * \author Manuel Wahle
 */
class sculptor_mapexplorer_label : public QLabel
{
    // height and width of the pixmap
    int m_iWidth;
    int m_iHeight;
    // size of one voxel on the screen, including border (i.e., grid width)
    int m_iTileSize;

    // pointer to sculptor_mapexplorer, which manages the conversion of 3d voxel to 2d image data
    sculptor_mapexplorer* m_pExplorer;

    // context menu
    QMenu* m_pContextMenu;

public:

    /**
     * Constructor
     */
    sculptor_mapexplorer_label(QWidget* pParent, sculptor_mapexplorer* pExplorer);

    /**
     *  Calculate the size of the map, taking in to account data size, zoom, and grid. Will also
     *  redraw the map
     */
    void updateSize();

    /**
     * just redraw the map without changing any state
     */
    void updateImage();

protected:

    /**
     * Return the size in pixels of the map. Important for the containing QScrollArea
     */
    virtual QSize sizeHint() const;

    /**
     * Paint the map
     */
    virtual void paintEvent(QPaintEvent* pPaintEvent);

    /**
     * Get mouse move events to display voxel info
     */
    virtual void mouseMoveEvent(QMouseEvent* pEvent);

    /**
     * Catch mouse press events to show a context menu
     */
    virtual void mousePressEvent(QMouseEvent* pEvent);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Mousefilter for the mapexplorer.
 *
 * This is a QObject that implements an eventFilter method. It is the mouse button filter for
 * the map_explorer. It should be installed in the map and in the containing QScrollArea
 * \author Manuel Wahle
 */
class dlg_mapexplorer_mousefilter : public QObject
{
    Q_OBJECT

    // this is the map_explorer to call its methods
    dlg_mapexplorer* m_pDlg;
    // this is the viewport whose events should be filtered
    QObject *m_pTarget;

public:

    /**
     * Constructor
     */
    dlg_mapexplorer_mousefilter(dlg_mapexplorer* pME, QWidget* pTableWidget);

    /**
     * this is the virtual method from QObject where the event filter must be implemented
     */
    bool eventFilter(QObject *pObject, QEvent *pEvent);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Class to implement functionality for the map explorer user interface.
 *
 * It inherits the ui created with the designer.
 * \author Manuel Wahle
 */
class dlg_mapexplorer : public QWidget, private Ui::dlg_mapexplorer
{
    Q_OBJECT

    // the document's volume
    svt_volume<Real64>* m_pVolume;
    // the map explorer
    sculptor_mapexplorer* m_pExplorer;
    // grid visible?
    bool m_bGrid;
    // zoom factor (scale a voxel onto this number of pixels
    int m_iZoom;
    // the widget displaying the map
    sculptor_mapexplorer_label* m_pLabel;
    // gamma value
    float m_fGamma;
    // threshold value
    float m_fThreshold;
    // the displayed slice of the volume 
    int m_iSlice;
    // holds the current mapping method
    int m_iMapping;
    // holds the current display mode (gamma/threshold)
    int m_iDisplayMode;
    // the pixmap that will display the map image
    QPixmap* m_pPixmap;

public:

    /*
     * Constructor
     */
    dlg_mapexplorer(sculptor_mapexplorer* pExplorer, svt_volume<Real64>* pVolume);

    /*
     * Destructor
     */
    virtual ~dlg_mapexplorer();

    /*
     * display current voxel info at pixel coordinates iX,iY
     */
    void showVoxelInfo(int iX, int iY);

    /**
     * clear the voxel info fields
     */
    void clearVoxelInfo();

    /**
     * Causes its sculptor_mapexplorer_label to redraw
     */
    void updateImage();

    /**
     * This method is supposed to be called when a relevant mouseWheelDown event occurs
     */
    void mouseWheelDown();

    /**
     * This method is supposed to be called when a relevant mouseWheelUp event occurs
     */
    void mouseWheelUp();

    /**
     * This method should be called when the underlying data changed, i.e., the svt_volume. it will
     * update the ui elements and redraw the currently displayed image
     */
    void dataChanged();

    /*
     * get the voxel values displayed in the lineedits (i.e., the ones that were lastly under the
     * mouse)
     * \return bool true if success, false if no voxel values found
     * \param pX pointer to unsigned int where the x coord will be written to
     * \param pY pointer to unsigned int where the y coord will be written to
     * \param pZ pointer to unsigned int where the z coord will be written to
     * \param pDensity pointer to float where the density will be written to
     */
    bool getCurrentVoxel(unsigned int* pX, unsigned int* pY, unsigned int* pZ, float* pDensity);

    /*
     * get the label where the image is displayed
     */
    sculptor_mapexplorer_label* getLabel();

private slots:

    /*
     * slot to be called when the desired crosssection plane changes
     */
    void sSetMapping(int iMapping);

    /*
     * slot to set the display mode (gamma/thresholded)
     */
    void sSetDisplayMode(int iMode);

    /*
     * slot to set zoom factor
     */
    void sSetZoom(int iZoom);

    /*
     * set the grid visible/invisible
     */
    void sSetGridVisible(bool bGrid);

    /*
     * slot to update the slider when the spinbox changes
     */
    void sUpdateSliderSlice(int iValue = -1);
    /*
     * slot to update the spinbox when the slider changes
     */
    void sUpdateSpinBoxSlice(int iValue);
    /*
     * slot to be called when the crosssection selection changes
     */
    void sSetSlice(int iValue);


    /**
     * slot to update the gamma slider from the gamma spinbox
     */
    void sUpdateSliderGamma(double fValue = -1.0);
    /**
     * slot to update the gamma spinbox from the gamma slider
     */
    void sUpdateSpinBoxGamma(int iValue);
    /*
     * slot to set gamma factor
     */
    void sSetGamma(int iValue);


    /**
     * slot to update the threshold slider from the threshold spinbox
     */
    void sUpdateSliderThreshold(double fValue = -1.0);
    /**
     * slot to update the threshold spinbox from the threshold slider
     */
    void sUpdateSpinBoxThreshold(int iValue);
    /**
     * slot to set the threshold in the delegate
     */
    void sSetThreshold(int iValue);
};

#endif // SCULPTOR_DLG_MAPEXPLORER_H
