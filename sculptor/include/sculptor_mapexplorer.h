/*-*-c++-*-*****************************************************************
                          sculptor_mapexplorer.h
			  -------------------
    begin                : 10/29/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_MAPEXPLORER_H
#define SCULPTOR_MAPEXPLORER_H

// sculptor includes
class dlg_mapexplorer;
class situs_document;
#include <svt_volume.h>
// qt4 includes
#include <QObject>
#include <QColor>
// svt includes
#include <svt_stlVector.h>

class sculptor_mapexplorer : public QObject
{
    Q_OBJECT

    // the map explorer gui
    dlg_mapexplorer* m_pExplorerDlg;
    // the situs document holding the volume
    situs_document* m_pDoc;
    // the document's volume
    svt_volume<Real64>* m_pVolume;
    // a vector containing precomputed gamma or threshodled color values
    vector<QColor> m_aColors;
    // zoom factor (scale a voxel onto this number of pixels
    int m_iZoom;
    // gamma value
    float m_fGamma;
    // threshold value
    float m_fThreshold;
    // should a grid be drawn
    bool m_bGrid;
    // the displayed slice of the volume 
    unsigned int m_iSlice;
    // the type of the volume-to-image mapping
    int m_iMapping;
    // display mode (gamma/thresholded)
    int m_iDisplayMode;
    // minimum and maximum density
    float m_fMinDensity;
    float m_fDensityRange;

    int** m_pAlongX;
    int** m_pAlongY;
    int** m_pAlongZ;


   /**
    * build a two dimensional array that will hold the projected and normalized voxel data, so that
    * it has to be constructed only once
    */
    int** generateProjection();

public:

    enum {
        SCULPTOR_MAPEXPLORER_CROSSSECTION_X,
        SCULPTOR_MAPEXPLORER_CROSSSECTION_Y,
        SCULPTOR_MAPEXPLORER_CROSSSECTION_Z,
        SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X,
        SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y,
        SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Z,
        SCULPTOR_MAPEXPLORER_GAMMA,
        SCULPTOR_MAPEXPLORER_THRESHOLD
    };
        
    /**
     * Constructor
     */
    sculptor_mapexplorer(situs_document* pDoc);

    /**
     * Destructor
     */
    virtual ~sculptor_mapexplorer();

    /**
     * show the dialog, creating it if it does not yet exist
     */
    void showDlg(QString oTitle);

    /**
     * return a pointer to the dialog
     */
    dlg_mapexplorer* getDialog();

    /**
     * this method should be called when data changes
     */
    void dataChanged();

    /**
     * set the mapping method, either per-slice or a projection:
     * SCULPTOR_MAPEXPLORER_CROSSSECTION_X
     * SCULPTOR_MAPEXPLORER_CROSSSECTION_Y
     * SCULPTOR_MAPEXPLORER_CROSSSECTION_Z
     * SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X
     * SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y
     * SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Z
     */
    void setMapping(int iMapping);

    /**
     * get the current mapping method, see setMapping()
     */
    int getMapping();

    /**
     * set the current slice
     */
    void setSlice(int iSlice);

    /**
     * get the current slice
     */
    int getSlice();

    /**
     * set the diplaymode, either gamma'd or thresholded:
     * SCULPTOR_MAPEXPLORER_GAMMA
     * SCULPTOR_MAPEXPLORER_THRESHOLD
     */
    void setDisplayMode(int iDisplayMode);

    /**
     * get the current displaymode, see setDisplayMode()
     */
    int getDisplayMode();

    /**
     * set a threshold value in percent
     */
    void setThreshold(float fThreshold);

    /**
     * get the current threshold value
     */
    float getThreshold();

    /**
     * set a gamma value
     */
    void setGamma(float fGamma);

    /**
     * get the current gamma value
     */
    float getGamma();

    /**
     * set the number of pixels per voxel
     */
    void setZoom(int iZoom);

    /**
     * get the number of pixels per voxel
     */
    int getZoom();

    /*
     * slot to set grid visibility
     */
    void setGridVisible(bool bGrid);

    /**
     * get grid visibility
     */
    bool getGridVisible();

    /**
     * get the number of voxels of the current slice/projection, vertically
     */
    unsigned int getDataHeight() const;

    /**
     * get the number of voxels of the current slice/projection, horizontally
     */
    unsigned int getDataWidth() const;

    /**
     * get the voxel density in the current slice at the given coordinates
     */
    int getDataValue(unsigned int iX, unsigned int iY);

    /**
     * for a density value normalized to 0..255, return the gamma'd or thresholded color
     */
    QColor getColor(int iIndex);

public slots:

    /**
     * show floodfill dialog and fill in given values
     */
    void sShowFloodfillDialog();

    /**
     * show print dialog to print the currently displayed image
     */
    void sPrint();

    /**
     * save the currently displayed map to disc
     */
    void sSave();
};

#endif
