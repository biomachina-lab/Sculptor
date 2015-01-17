/*-*-c++-*-**************************************************************************
                          situs_document
			  --------------
    begin                : 27.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SITUS_DOCUMENT_H
#define SITUS_DOCUMENT_H

// sculptor includes
#include <sculptor_document.h>
#include <situs_document_dialogs.h>
class dlg_floodfill;
class dlg_mpfloodfill;
class dlg_swapaxis;
class dlg_DPSV_filter;
class dlg_maptable;
class prop_situs;
class sculptor_mapexplorer;
class dlg_crop;

// svt includes
#include <svt_volume.h>
#include <svt_volume_slices.h>
#include <svt_volume_compact.h>
#include <svt_volume_3dtextures.h>
#include <svt_float2color.h>
#include <svt_float2color_bw.h>
#include <svt_float2color_rwb.h>
#include <svt_float2color_ryg.h>
#include <svt_float2color_rgb.h>
#include <svt_float2color_points.h>
#include <svt_float2opacity.h>
#include <svt_float2opacity_constant.h>
#include <svt_float2opacity_linear.h>
#include <svt_float2opacity_points.h>
#include <svt_float2mat_points.h>
#include <svt_stlVector.h>
class svt_palette;
class svt_box;


/**
 * This class stores a single situs document (electron density data)
 * @author Stefan Birmanns
 */
class situs_document : public sculptor_document
{
    Q_OBJECT

protected:

    // the main volume object
    svt_volume<Real64> m_cVolume;

    // the segmentation volume object
    svt_volume<unsigned int> m_cSegVol;

    // the laplace filtered object
    svt_volume<Real64> m_cVolumeLaplace;

    // the rendering objects
    svt_volume_slices<Real64>*     m_pVolumeSlices;
    svt_volume_3dtextures<Real64>* m_pVolume3DText;
    svt_volume_compact<Real64>*    m_pVolumeMC;
    svt_box* m_pVolumeCube;
    svt_box* m_pVolumeSelectionCube;

    // for the haptic rendering we need a precomputed gradient
    svt_volume<Real64> m_cGradientX;
    svt_volume<Real64> m_cGradientY;
    svt_volume<Real64> m_cGradientZ;

    // flag, if situs dataset should be laplace filtered for docking (has to be calculated!!!)
    bool m_bDockToLaplaceData;

    // transfer functions
    svt_float2mat_points m_cF2M_points;
    svt_float2color_bw m_cF2C_bw;
    svt_float2color_rwb m_cF2C_rwb;
    svt_float2color_ryg m_cF2C_ryg;
    svt_float2color_rgb m_cF2C_rgb;
    svt_float2color_points m_cF2C_points;
    svt_float2opacity_constant m_cF2O_constant;
    svt_float2opacity_linear m_cF2O_linear;
    svt_float2opacity_points m_cF2O_points;

    // root node for visualization objects
    svt_node* m_pVolNode;

    // number of samples in the histogram
    unsigned int m_iSizeHistogram;
    double* m_pHisX;
    double* m_pHisY;

    // codebook
    vector< svt_vector3<Real32> > m_aCodebook;

    // selected voxels
    vector< svt_vector4<Real64> > m_aSelectedVoxels;

    // root node for the selected voxels
    svt_container* m_pSelVox;

    // the high contrast palette
    static svt_palette* s_pHighContrastPalette;

    //variable to indicate how many documents were loaded
    static unsigned int s_iLoadedDocuments;

    // properties dialog
    prop_situs* m_pSitusPropDlg;

    // table to display density values like in a situs file
    dlg_maptable* m_pMapTable;

    // map explorer
    sculptor_mapexplorer* m_pMapExplorer;

    // floodfill dialog
    dlg_floodfill* m_pFloodfillDlg;
    dlg_mpfloodfill* m_pMpFloodfillDlg;

    // swapaxis dialog
    dlg_swapaxis* m_pSwapaxisDlg;
    
    // DPSV_filter dialog
    dlg_DPSV_filter* m_pDPSV_filterDlg;
    //
    
    //crop dialog
    dlg_crop* m_pCropDlg;

public:

    enum {
    	SITUS_DOC_SWAPAXIS_OLD_X_X,
	SITUS_DOC_SWAPAXIS_OLD_X_Y,
    	SITUS_DOC_SWAPAXIS_OLD_X_Z,
    	SITUS_DOC_SWAPAXIS_OLD_Y_X,
    	SITUS_DOC_SWAPAXIS_OLD_Y_Y,
    	SITUS_DOC_SWAPAXIS_OLD_Y_Z,
    	SITUS_DOC_SWAPAXIS_OLD_Z_X,
    	SITUS_DOC_SWAPAXIS_OLD_Z_Y,
    	SITUS_DOC_SWAPAXIS_OLD_Z_Z};

    enum {
    	 SITUS_DOC_NEIGHBOOR4,
    	 SITUS_DOC_NEIGHBOOR8,
    	 SITUS_DOC_NEIGHBOOR6,
    	 SITUS_DOC_NEIGHBOOR26};
    
    /**
     * Constructor
     * \param pNode pointer to the svt_node, where the class can connect its nodes to
     * \param oFilename the filename of the situs file
     * \param pVol pointer to svt_volume object
     */
    situs_document(svt_node* pNode, string oFilename, svt_volume<Real64>* pVol = NULL);

    /**
     * Destructor
     */
    virtual ~situs_document();

    /**
     * get the svt_node the document is providing
     */
    virtual svt_node* getNode();

    /**
     * get the type of the document
     * \return SENSITUS_DOC_SITUS
     */
    virtual int getType();

    /**
     * restores the state of document from xml datastructure
     * \param xml element with stored state of document
     */
    void setState(TiXmlElement* pElem);
    /**
     * stores the state of document in xml datastructure
     * \param xml element to store state of document
     */
    void getState(TiXmlElement* pElem);

    /**
     * save the document
     * \param pFilename pointer to an array of char with the filename
     */
    virtual void save(string oFilename);
    /**
     * is this document saveable?
     * \return true if the document can be saved to disk
     */
    virtual bool getSaveable();


public slots:

    /**
     * show the table with all data values
     */
    void showTable();

    /**
     * SLOT enable the surrounding box
     * \param bEnable if true the surrounding box is rendered
     */
    void sEnableBox(bool bEnable);


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

public:

    /**
     * return the properties dialog for the document. if it does not exist, create it
     * \param pParent pointer to parent widget
     * \return QWidget* pointer to the prop dialog
     */
    virtual QWidget* createPropDlg(QWidget* pParent);
    /**
     * return the prop dialog without creating it
     * \return QWidget* pointer to the prop dialog
     */
    virtual QWidget* getPropDlg();


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * \name Iso Surface
     * Functions dealing with the iso surface
     */
    //@{

public:

    /**
     * create isosurface
     */
    void createIsoSurface(float fLevel, int iSteps);

    /**
     * Enable/disable the isosurface
     * \param bEnable if true the iso surface is enabled
     */
    void setIsoEnabled(bool bDisable);

    /**
     * Get isosurface object
     * \return svt_volume_mc_cline pointer
     */
    svt_volume_compact<Real64>* getIso();

    /**
     * if the data has changed the isosurface is updated
     */
    virtual void dataChanged();

    /**
     *
     */
    Real64 getIsoLevel();

    /**
     * SLOT set the color for the isosurface
     */
    void setColor(svt_color oCol);

    /**
     * Get a representative color for the document
     * \return svt_color object
     */
    virtual svt_color getColor();

public slots:

    /**
     * SLOT set wireframe mode for the isosurface
     * \param bWireframe true if the iso surface should be drawn in wireframe mode
     */
    void sSetIsoWireframe(bool bWireframe);

    /**
     * SLOT setting transparency
     * \param iTrans 100 means full opacity
     */
    void sSetIsoTransparency(int iTrans);

    /**
     * SLOT rendering of the triangular mesh was set to smooth/flat shading
     */
    void sRendering_Flat( bool bToggle );

    /**
     * SLOT rendering of the triangular mesh was set to single/two-sided
     */
    void sRendering_TwoSide( bool bToggle );

    /**
     * SLOT rendering of the triangular mesh was set to culling on/off
     */
    void sRendering_Culling( bool bToggled );

    //@}

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * \name Slices
     * Functions dealing with the slices
     */
    //@{

public:

    /**
     * create the slices
     */
    void createSlices();

public slots:

    /**
     * SLOT for setting the current XY slice
     * \param iXY number of XY slice
     */
    void sSetSliceXY(int iXY);
    /**
     * SLOT for setting the current XZ slice
     * \param iXZ number of XZ slice
     */
    void sSetSliceXZ(int iXZ);
    /**
     * SLOT for setting the current ZY slice
     * \param iZY number of ZY slice
     */
    void sSetSliceZY(int iZY);
    /**
     * SLOT enable drawing slice XY
     * \param bEnable if true the XY slice will be displayed
     */
    void sEnableSliceXY(bool bEnable);
    /**
     * SLOT enable drawing slice XZ
     * \param bEnable if true the XZ slice will be displayed
     */
    void sEnableSliceXZ(bool bEnable);
    /**
     * SLOT enable drawing slice ZY
     * \param bEnable if true the ZY slice will be displayed
     */
    void sEnableSliceZY(bool bEnable);
    /**
     * enable slices
     * \param bEnable if true the slices are enabled
     */
    void setSlicesEnabled(bool bEnabled);
    /**
     * SLOT enable box around crosssections
     * \param bEnable if true a box around the crosssections is shown
     */
    void sBoxSlices(bool bEnable);

    /**
     * SLOT start slice slider was changed
     */
    void sStartSlice(int iStartSlice);

    /**
     * SLOT start slice slider was changed
     */
    void sEndSlice(int iEndSlice);
    //@}


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * \name 3d textures
     * Functions dealing with the 3d textures
     */
    //@{

public:

    /**
     * disable/enable the 3d textures
     * also switches off the isosurface if necessary
     */
    void set3DTextEnabled(bool bEnabled);

    /**
     * create the 3d textures
     */
    void create3DT(bool bForce = false);

    /**
     * update rendering after transfer function has changed
     */
    void applyTF();

    /**
     * reset tf
     */
    void resetTF();

public slots:

    /**
     * disable/enable shading
     */
    void sEnable3DTShading(bool bEnabled);

    /**
     * SLOT for changing the float2color mapping
     * \param iF color transfer function (0 means bw, 1 rwb, 2 ryg, 3 rgb)
     */
    void sChangeF2C(int iF);

    /**
     * SLOT enable volume mode
     * \param bEnable if true the volume mode is enabled
     */
    void sEnableVolumeMode(bool bEnable);

    /**
     * SLOT disable/enable the zero treatment in the transfer function
     */
    void sZero(bool tf);

    /**
     * SLOT for changing the sampling rate
     */
    void sChangeSampling();

    /**
     * SLOT load a transfer function from disk
     */
    void sLoadTF();

    /**
     * SLOT save a transfer function to disk
     */
    void sSaveTF();
    //@}

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * \name Histogram
     * Functions dealing with the volume histogram (on the isosurface tab)
     */
    //@{

public:

    /**
     * calculate the histogram
     */
    void calcHistogram();

    /**
     * get histogram x
     * \return pointer to an array with double values with the histogram values
     */
    double* getHistogramX();

    /**
     * get histogram y
     * \return pointer to an array with double values with the histogram values
     */
    double* getHistogramY();

    /**
     * get histogram size
     * \return int size of the histogram
     */
    unsigned int getHistogramSize();

public slots:

    /**
     * show the volume data histogram dialog
     */
    void showHistogramDlg();

    //@}


public:

    // General

    /**
     * subtract two maps
     */
    void subtractMap();

    /**
     * get the voxel-width of the situs data
     * \return voxel width in angstroem
     */
    float getVoxelWidth();
    /**
     * get the number of voxels in x of the situs data
     * \return extx
     */
    int getExtX();
    /**
     * get the number of voxels in y of the situs data
     * \return exty
     */
    int getExtY();
    /**
     * get the number of voxels in z of the situs data
     * \return extz
     */
    int getExtZ();

    /**
     * get a voxel value
     * \param iX x index
     * \param iY y index
     * \param iZ z index
     * \return voxel value
     */
    float getAt(int iX, int iY, int iZ);
    /**
     * set a voxel value
     * \param iX x index
     * \param iY y index
     * \param iZ z index
     * \param fVal new value
     */
    void setAt(int iX, int iY, int iZ, float fVal);

    // Laplacian

    /**
     * switch docking data to laplace data
     */
    void setDockToLaplace(bool b);

    /**
     * get state of docking: laplace or spatial
     */
    bool getDockToLaplace();

    // Gradient

    /**
     * Calculate gradient map
     */
    void calcGradientMap();
    /**
     * get gradient value for x axis at a certain position
     * \param iX x coord
     * \param iY y coord
     * \param iZ z coord
     */

    Real64 getGradientX(int iX, int iY, int iZ);
    /**
     * get gradient value for y axis at a certain position
     * \param iX x coord
     * \param iY y coord
     * \param iZ z coord
     */
    Real64 getGradientY(int iX, int iY, int iZ);
    /**
     * get gradient value for z axis at a certain position
     * \param iX x coord
     * \param iY y coord
     * \param iZ z coord
     */
    Real64 getGradientZ(int iX, int iY, int iZ);

    /**
     * Was the Gradient already calculated?
     * \return true if the calcGradient() routine was already called once.
     */
    bool gradientCalculated();

public slots:

    /**
     * normalize
     */
    void normalize();

    /**
     * scale volume with a scaling factor
     */
    void scale();

    /**
     * threshold
     */
    void threshold();

    /**
     * crop
     */
    void crop();

    /**
     * turn off vol selection cube if that exists
     */
    void turnOffVolSelectCube();
    
    /**
     * crop a subregion
     * \param iMinX minimum x coordinate/index
     * \param iMaxX maximum x coordinate/index
     * \param iMinY minimum y coordinate/index
     * \param iMaxY maximum y coordinate/index
     * \param iMinZ minimum z coordinate/index
     * \param iMaxZ maximum z coordinate/index
     */
    void crop( unsigned int iMinX, unsigned int iMaxX, unsigned int iMinY, unsigned int iMaxY, unsigned int iMinZ, unsigned int iMaxZ );

    /**
     * Calculate number of occupied voxels
     */
    void calcOccupied();

    /**
     * change the origin of the volume data
     */
    void sChangeOrigin();

    /**
     * change the origin of the volume data
     */
    void changeOrigin();

    /**
     * swap axis
     */
    void swapaxis();

    /**
     * method called from the swapaxis dialog to start the swapaxis dialog
     */
    void swapaxis_DOIT();
    
       
    /**
     * DPSV_filter
     */
    void DPSV_filter();

    /**
      * method called from the DPSV_filter dialog to start the DPSV_filter dialog
      */
    
    void DPSV_filter_DOIT();
    
    
    /**
     * set the voxel-width of the situs data
     */
    void changeVoxelWidth();

    /**
     * set the voxel-width of the situs data
     */
    void sChangeVoxelWidth();

    /**
     * Apply a gaussian kernel filter to volume
     */
    void gaussian();
    /**
     * Apply a laplacian kernel filter to volume
     */
    void laplacian();

    /**
     * Apply a local normalization
     */
    void localNorm();

    /**
     * Apply a bilateral Filter to volume
     */
    void bilateralFilter();


    /**
     * return a pointer to the map explorer. create it if it does not exist
     */
    sculptor_mapexplorer* createMapExplorer();

    /**
     * return a pointer to the map explorer, or NULL if it does not exist
     */
    sculptor_mapexplorer* getMapExplorer();

    /**
     * Select a voxel
     */
    void selectVoxel( int iX, int iY, int iZ );
    /**
     * UnSelect all voxels
     */
    void unselectAllVoxel( int iX, int iY, int iZ );

    /**
     * Floodfill
     */
    void floodfill();

    /**
     * Floodfill
     */
    void multiPointFloodfill();

    /**
     * method called from the floodfill dialog to start the floodfill dialog
     */
    void floodfill_DOIT();

   /**
     * method called from the floodfill dialog to start the floodfill dialog
     */
    void multiPointFloodfill_DOIT();

    /**
     * create floodfill dialog and return pointer to it
     * \return pointer to floodfill dialog
     */
    dlg_floodfill* createFloodfillDlg();

public:

    /**
     * Return a pointer to the transfer function
     */
    svt_float2mat_points* getFloat2Mat();

    /**
     * get maximum density
     * \return maximum
     */
    float getMaxDensity();
    /**
     * get minimum density
     * \return minimum
     */
    float getMinDensity();

    /**
     * get the position of the sculptor document in real space (e.g. for pdb or situs files)
     * \return svt_vector4f with the position
     */
    virtual svt_vector4f getRealPos();

    /**
     * Set the cutoff for the clustering
     * \param fCutoff only voxels above this value are considered.
     */
    void setCutoff( Real64 fCutoff );

    /**
     * Return the special svt_sampled object for clustering
     */
    virtual svt_sampled< svt_vector4<Real64> >& getSampled();

    /**
     * Return a pointer to the volume data
     */
    svt_volume<Real64>* getVolume();

    /**
     * Return a pointer to the 3d texture data
     */
    svt_volume_3dtextures<Real64>* get3DText();

    /**
     * Return a pointer to the volume slices data
     */
    svt_volume_slices<Real64>* getSlices();

    /**
     * Return a pointer to the laplace-filtered volume data
     */
    svt_volume<Real64>* getLaplaceVol();

    /**
     * set the subregion that is selected
     * \param iMinX minimum x coordinate/index
     * \param iMaxX maximum x coordinate/index
     * \param iMinY minimum y coordinate/index
     * \param iMaxY maximum y coordinate/index
     * \param iMinZ minimum z coordinate/index
     * \param iMaxZ maximum z coordinate/index
     */
    void setSelection( unsigned int iMinX, unsigned int iMaxX, unsigned int iMinY, unsigned int iMaxY, unsigned int iMinZ, unsigned int iMaxZ );

public slots:

    /**
     * Load a new segmentation file
     */
    void loadSegmentation();

    /**
     * Save a new segmentation file
     */
    void saveSegmentation();

    /**
     * Set segment
     */
    void setSegment();

    /**
     * Show the segmentation map in the mapexplorer
     */
    void showSegmentation();

public:

    /**
     * Is the box enabled?
     */
    bool getBoxEnabled();
    /**
     * Get the size of the document
     * \return svt_vector4 with the three dimensions of the size of the document
     */
    virtual svt_vector4<Real32> getBoundingBox();

    /**
     * Some map parameters have changed, update and redraw everything
     * /param bScaleRebuildDLs If true, scale objects and rebuild all DLs
     */
    void updateRedraw(bool bScaleRebuildDLs = false);

    /**
     * enable/disable the use of shaders
     */
    void setAllowShaderPrograms();

private:

    /**
     * read situs file
     * \param pFilename pointer to array of char with the filename
     */
    void loadSitus(const char* pFilename);

    /**
     * read mrc file
     * \param pFilename pointer to array of char with the filename
     */
    void loadMRC(const char* pFilename);

    /**
     * read Spider file
     * \param pFilename pointer to array of char with the filename
     */
    void loadSpider(const char* pFilename);
};

#endif
