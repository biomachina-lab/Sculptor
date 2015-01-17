/***************************************************************************
                          sculptor_doc_eraser.h  -  description
                             -------------------
    begin                : 30.07.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DOC_ERASER_H
#define SCULPTOR_DOC_ERASER_H


// sculptor includes
#include <sculptor_document.h>
class situs_document;
class prop_eraser;
// svt includes
#include <svt_volume_compact.h>


/** This class stores a single eraser tool document
 * @author Francisco Serna
 */
class sculptor_doc_eraser : public sculptor_document
{
    Q_OBJECT

protected:

    // eraser node from the scene
    svt_node* m_pEraserNode;
    // eraser property dialog
    prop_eraser* m_pEraserPropDlg;
    // eraser target document
    situs_document* m_pEraserTarget;
    // target volume to erase
    svt_volume<Real64>* m_pTargetVolume;
    // eraser volume
    svt_volume<Real64>* m_pEraserVolume;
    // marching cube for display of eraser volume
    svt_volume_compact<Real64>* m_pEraserVolumeMC;
    // opacity value for reducing voxel values rather than full erasure
    Real64 m_fOpacity;
    // invert boolean to store whether we are erasing voxels within the volume or outside the volume
    bool m_bInvert;
    // stores which shape we are using for the eraser (currently implented: sphere, cube)
    int m_iEraserShape;
    // stores which method to use when erasing - with target volume or with copy of target volume
    // int m_iEraserMethod;
    // eraser volume size
    int m_iEraserSize;
   

public:

    /**
     * Constructor
     * \param pNode pointer to svt_node to use as eraser node
     * \param pTargetDoc pointer to situs_document with target map
     */
    sculptor_doc_eraser(svt_node* pNode, situs_document* pTargetDoc);
    
    /**
     * Destructor
     */
    virtual ~sculptor_doc_eraser();

    // enum to select the eraser tool
    enum
    {
	SCULPTOR_ERASER_TOOL_CUBE,
	SCULPTOR_ERASER_TOOL_SPHERE,
//	SCULPTOR_ERASER_TOOL_METHOD_MODIFY,
//	SCULPTOR_ERASER_TOOL_METHOD_COPYMODIFY
    };

    /**
     * Get a pointer to the svt_node the document is providing
     * \return m_pEraserNode
     */
    svt_node* getNode();

    /**
     * Creates the eraser property dialog
     * \param QWidget* pointer to parent
     * \return m_pEraserPropDlg
     */
    virtual QWidget* createPropDlg(QWidget*);

    /**
     * Returns the eraser property dialog
     * \return m_pEraserPropDlg
     */
    virtual QWidget* getPropDlg();

    /**
     * get the type of the document
     * \return SENSITUS_DOC_ERASER
     */
    virtual int getType();

    /**
     * method called whenever changes are made to the volume from the gui - ie, different shape,
     * different size, etc.  The method then calls updateVolumeDisplay so those changes are
     * visualized.
     */
    void updateEraserTool();

    /**
     * get eraser size
     * \return m_iEraserSize
     */
    int getEraserSize(); 

    /**
     * get eraser inversion (boolean)
     * \return m_bInvert
     */
    bool getInvertSetting();
    
    /**
     * get eraser opacity GUI - used for slider/spinbox connection.  Slider does not accept double,
     * so switching to int by multiplying by 100.
     * \return (int)m_fOpacity
     */
    int getEraserOpacityGUI();
    
    /**
     * Creates a displayable cube volume in sculptor
     * \param oVol the volume to update with values
     * \param fVoxelValue the value to store in each voxel DEFAULT = 1.0
     */
    void setupCube(svt_volume<Real64>* oVol, Real32 fVoxelValue = 1.0);
    
    /**
     * Creates a displayable sphere volume in sculptor
     * \param oVol the volume to update with values
     * \param fVoxelValue the value to store in each voxels DEFAULT = 1.0
     */
    void setupSphere(svt_volume<Real64>* oVol, Real32 fVoxelValue = 1.0);

    /**
     * Displays visual changes to eraser volume.  It resets isolevels, scales, and recalcs the mesh.
     */
    void updateVolumeDisplay();

    /**
     * get voxel width of target volume
     * \return Real64 target volume voxel width
     */
    Real64 getTargetWidth();
  
    /**
     * set eraser volume voxel width based on target volume
     */
    void setEraserWidth();

    /**
     * get target volume grid X
     * \return Real32 target grid X
     */   
    Real32 getTargetGridX();

    /**
     * get target volume grid Y
     * \return Real32 target grid Y
     */   
    Real32 getTargetGridY();

    /**
     * get target volume grid Z
     * \return Real32 target grid Z
     */   
    Real32 getTargetGridZ();

    /**
     * get eraser volume size of X
     * \return unsigned int size of X
     */
    unsigned int getEraserVolumeSizeX();

    /**
     * get eraser volume size of Y
     * \return unsigned int size of Y
     */
    unsigned int getEraserVolumeSizeY();

    /**
     * get eraser volume size of Z
     * \return unsigned int size of Z
     */
    unsigned int getEraserVolumeSizeZ();

    /**
     * get current shape of eraser
     * \return int ( see setEraserShape() )
     */
    int getEraserShape();

public slots:
   
    /**
     * sets whether the target volume is modified, or a copy of the target volume is made then modified
     */  
    
 //   void sSetEraserMethod(int iMethod);
    /**
     * set shape of eraser, calls updateEraserTool to update volume/display changes.
     * \param iShapeId:
     * - SCULPTOR_ERASER_TOOL_CUBE
     * - SCULPTOR_ERASER_TOOL_SPHERE
     */
    void sSetEraserShape(int iShapeId);

    /**
     * set size of eraser tool and call updateEraserTool to display visual changes.
     * \param iSize is the size (int)
     */
    void sSetEraserSize(int iSize);

    /**
     * erases the eraser volume from the target volume based on location using mask method in
     * svt_volume.  If inverted is checked, changes how target volume is erased (erases everything
     * outside eraser volume instead).
     */
    void sEraseRegion();

    /**
     * sets the opacity of the eraser volume - ie, scale how much to erase. Takes in values from
     * 1-100, with 100 being a full erase, 50 being a half erase (multiply voxels by 0.5), etc.
     * \param iFactor an integer value from 1-100
     */
    void sSetOpacity(int iFactor);

    /**
     * sets whether the erase option is inverted or normal
     * \param bisChecked a boolean value
     */   
    void sSetInverted(bool bisChecked);

};

#endif
