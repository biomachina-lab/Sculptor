/*-*-c++-*-*****************************************************************
                          pdb_document.h
                          --------------
    begin                : 27.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef PDB_DOCUMENT_H
#define PDB_DOCUMENT_H

// sculptor includes
#include <sculptor_document.h>
#include <sculptor_svt_pdb.h>
#include <pdb_mode.h>
class situs_document;
//class sculptor_plot_dlg;
class dlg_plot;
class prop_pdb;
class dlg_atomclustering;
class dlg_cartoon;
class dlg_colmap;
class dlg_colmapsecstruct;
class dlg_colorselect;
class dlg_extract;
class dlg_newtube;
class dlg_pdbvdw;
class dlg_atomtable;
class dlg_symmetry;
class dlg_toon;
// svt includes
#include <svt_atom_type_colmap.h>
#include <svt_atom_resname_colmap.h>
#include <svt_atom_chainid_colmap.h>
#include <svt_atom_segid_colmap.h>
#include <svt_atom_solid_colmap.h>
#include <svt_atom_structure_colmap.h>
#include <svt_atom_model_colmap.h>
#include <svt_atom_beta_colmap.h>
#include <svt_matrix4.h>
#include <svt_vector3.h>
#include <svt_node.h>
#include <svt_point_cloud_pdb.h>
#include <svt_array.h>
class svt_palette;
class svt_sphere;
class svt_arrow;
class svt_textnode;
class svt_pdb;
// qt4 includes
class QListWidgetItem;


/** This class stores a single pdb document
 * @author Stefan Birmanns
 */
class pdb_document : public sculptor_document
{
    Q_OBJECT

protected:

    // pointer to the pdb node
    sculptor_svt_pdb* m_pPdbNode;
    // main node
    svt_node m_oMainNode;

    // color mapping objects
    svt_atom_type_colmap m_cTypeColmap;
    svt_atom_resname_colmap m_cResnameColmap;
    svt_atom_chainid_colmap m_cChainIDColmap;
    svt_atom_segid_colmap m_cSegIDColmap;
    svt_atom_solid_colmap m_cSolidColmap;
    svt_atom_structure_colmap m_cStructureColmap;
    svt_atom_model_colmap m_cModelColmap;
    svt_atom_beta_colmap m_cBetaColmap;

    // pointer to the pdb dialog
    prop_pdb* m_pPdbPropDlg;
    // a widget that will hold a table to display the pdb atomic data
    dlg_atomtable* m_pAtomTable;
    // pointer to the colormapping dialog
    dlg_colmap* m_pColmapDlg;
    // pointer to the colormapping dialog for secondary structure
    dlg_colmapsecstruct* m_pColmapDlgSecStruct;
    // dialog that allows to extract chains, models and frames
    dlg_extract* m_pExtractDlg;
    //dialog for selecting coloring
    dlg_colorselect * m_pColorSelectDlg;
    // pointer to the symmetry dialog
    dlg_symmetry* m_pSymmetryDlg;
    // cartoon rendering options dialog
    dlg_cartoon * m_pCartoonDlg;
    dlg_newtube * m_pNewTubeDlg;
    //dlg_toon    * m_pToonDlg;
    dlg_pdbvdw  * m_pVDWDlg;
    // atomic clustering dialog
    dlg_atomclustering* m_pAtomClusteringDlg;

    // the mode stack
    svt_array<pdb_mode> m_aModes;

    // center of the pdb object
    svt_vector4< Real64 > m_oCenter;

    // dynamic or static dataset?
    bool m_bDynamic;

    // point cloud for refinement
    svt_point_cloud_pdb< svt_vector4< Real64 > > m_oRefinementPC;

    // should pdb mode updates automatically be rendered?
    bool m_bAutoApply;

    // the high contrast palette
    static svt_palette* s_pHighContrastPalette;
    // the low contrast  palette
    static svt_palette* s_pLowContrastPalette;
    // which one is selected
    bool m_bHighContrast;

    static unsigned int s_iPdbDocuments;

    // sphere for the center
    svt_sphere* m_pCenterSphere;

    // sphere for the local maximum highlighting
    svt_sphere* m_pCenterSphereHighlight;

    // text for center
    svt_textnode* m_pCenterText;

    // force arrow
    svt_arrow* m_pForceArrow;

    /**
     * init procedures that all constructors have in common
     */
    void init();

public:

    /**
     * Constructor
     * \param pGraphNode the node in the svt_scenegraph to which pdb_document will add its svt_nodes to
     * \param pFileA pointer to an char array with the filename of the first file
     * \param iTypeA file-type of the first file (e.g. SVT_NONE, SVT_PDB, SVT_PSF, SVT_DCD, ...)
     * \param pFileB pointer to an char array with the filename of the second file
     * \param iTypeB file-type of the second file (e.g. SVT_PDB, SVT_PSF, SVT_DCD, ...)
     */
    pdb_document(svt_node* pGraphNode,
                 string oFile,           int iType,
                 string oFile_SRCB = "", int iType_SRCB = SVT_NONE,
                 svt_point_cloud_pdb< svt_vector4<Real64 > >* rPDB = NULL);

    /**
     * 'Copy' Constructor
     * \param pGraphNode the node in the svt_scenegraph to which pdb_document will add its svt_nodes to
     * \param rOrig reference to the PDB document to be copied
     */
    pdb_document(svt_node* pGraphNode, pdb_document& rOrig);

    /**
     * Destructor
     */
    virtual ~pdb_document();

    /**
     * get the svt_node the document is providing
     * \return pointer to the svt_node object
     */
    svt_node* getNode();

    /**
     * get the mode stack
     * \return reference to the mode stack
     */
    svt_array<pdb_mode>& getModes();

    /**
     * Return a pointer to the svt_atom_structure_colmap
     * \return &cStructureColmap
     */
    svt_atom_structure_colmap * getAtomStructureColmap();

    /**
     * Remove all existing modes and add a new global mode
     * \param iGraphicsMode new global graphics mode
     */
    void setGlobalMode( int iGraphicsMode );

    /**
     * Change the coloring for the first mode in the stack
     * \param iColmapMode new global coloring mode
     */
    void setGlobalColmap( int iColmapMode, unsigned int iColor );

    /**
     * Get the global color mapping mode
     */
    int getGlobalColmapMode();

    /**
     * Get the global color mapping color
     */
    int getGlobalColmapColor();

    /**
     * Show color chooser dialog
     */
    int showColorChooser();

    /**
     * Change the coloring for the first mode in the stack - to solid and ask interactively for the color itself
     */
    void setGlobalColmapSOLIDINTERACTIVE();

    /**
     * set if dynamic point cloud
     */
    void setDynamic(bool bDynamic);

    /**
     * get if dynamic point cloud
     */
    bool getDynamic();

    /**
     * update the timesteps per second in the dlg_prop
     */
    void updatePropDlgDynTPS(int iCurrent);

    /**
     * update the current timestep in the dlg_prop
     */
    void updatePropDlgDynTimestep(int iCurrent, int iMaxTimestep);

    /**
     * update the dynamics loop mode in the dlg_prop
     */
    void updatePropDlgDynLoop(int iLoop);

    /**
     * update the dynamics control buttons dlg_prop
     */
    void updatePropDlgDynButtons(int iPlaying);

    /**
     * set the export movie button checked or not
     * \param bActive button checked or not
     */
    void updatePropDlgDynMovieButton(bool bActive);

    /**
     * Copy PDB document
     */
    pdb_document* copy();

public slots:

    /**
     * the user has changed the modes and want to update the scene now (to make the changes visible).
     * this is a QT slot.
     */
    void sApply();

    /**
     * the user wants to add a graphics mode.
     * this is a QT slot.
     */
    void sAddGraphicsMode();
    /**
     * the user wants to insert a graphics mode.
     * this is a QT slot.
     */
    void sInsGraphicsMode();
    /**
     * the user wants to delete a graphics mode.
     * this is a QT slot.
     */
    void sDelGraphicsMode();
    /**
     * the user wants to adjust the graphics mode
     * this is a QT slot.
     */
    void sAdjGraphicsMode();
    /**
     * the user wants to adjust the color settings
     * this is a QT slot.
     */
    void sAdjColor();
    /**
     * the user has clicked on change color in the colormap dlg
     * this is a QT slot.
     * \param iName unsigned int to name chosen in dialog
     */
    void buttonChangeColor(unsigned int iName);
    /**
     * the user has change the state of the auto apply check box
     */
    void sAutoApply( bool );
    /**
     * Transformation/Docking Tab: Add Transform
     */
    void sTransAdd();
    /**
     * Transformation/Docking Tab: Remove Transform
     */
    void sTransDel();
    /**
     * Transformation/Docking Tab: Transform up in list
     */
    void sTransUp();
    /**
     * Transformation/Docking Tab: Transform down in list
     */
    void sTransDown();
    /**
     * Transformation/Docking Tab: Transform rename in list
     */
    void sTransRename();
    /**
     * Transformation/Docking Tab: Clear transform list
     */
    void sTransClear();

    /**
     * Transformation/Docking Tab: Apply current transform
     */
    void sTransApply(QListWidgetItem*);
    /**
     * Show transformation list
     */
    void showTransList();
    /**
     * Update transformation list
     */
    void updateTransList();
    /**
     * adds the PDF of the current pdb document to the
     *\param sculptor_plot_dlg dialog
     *\return status = was curve added 1 or user cancel calculation 0
     */
    int addPdf2Plot(dlg_plot* pDlg, svt_color pColor);

public:

    
    bool getAutoApply();

    /**
     * get the type of the document
     * \return SENSITUS_DOC_PDB
     */
    virtual int getType();

    /**
     * get the position of the pdb document in real space (e.g. for pdb or situs files)
     * \return svt_vector4f with the position
     */
    virtual svt_vector4f getRealPos();

    /**
     * get the position of the atom iIndexAtom in real space
     * \param iIndexAtom the index of the atom
     * \param oOrigin the systems origin
     * \return iIndexAtom atom's real space coordinates
     */
    svt_vector4f getRealPos(unsigned int iIndexAtom, svt_vector4f oOrigin);

    /**
     * get the point cloud in real space
     * \return pointer svt_point_cloud_pdb with the coordinates in real space
     */
    svt_point_cloud_pdb< svt_vector4< Real64 > >* getPDB();

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

    /**
     * Get a representative color for the document
     * \return svt_color object
     */
    virtual svt_color getColor();

    /**
     * Get high contrast palette
     *\return svt_palette the high constrast palette;
     */
    svt_palette* getHighContrastPalette();

    /**
     * Get Low contrast palette
     *\return svt_palette the low constrast palette;
     */
    svt_palette* getLowContrastPalette();

    /**
     * Get the number of atoms of the molecule
     * \return number of atoms
     */
    unsigned int size();

public slots:
    /**
     * calculate the spericity
     */
    void showSphericity();
    /**
     * show table with all values
     */
    void showTable();

    /**
     * calculate the rmsd between two pdb documents
     * \param pDoc pointer to the second pdb document
     * \return rmsd
     */
    double getRmsd( pdb_document* pDoc );

    /**
     * subtract this crystal structure from a volumetric map
     */
    //void subtractFromMap();

    /**
     * blur atomic structure by convolving with gaussian kernel
     */
    situs_document* blur(bool bAddDoc = true, Real64 fWidth = 0.0, Real64 fResolution = 0.0);

    /**
     * this routine actually changes from high to low contrast palette
     */
    void setHighContrast( bool bHighContrast );

    /**
     * show atomic quantization dialog
     */
    void showAtomicQuantizationDlg(); 

    /**
     * atomic quantization method
     * \param iToSkipCACount unsigned int number of alpha carbons to skip
     */
    void atomicQuantization(unsigned int iToSkipCACount);

    /**
     * atomic quantization (no dialog, every C alpha)
     */
    void atomicQuantizationEveryCA(svt_vector4f & oOrigin);

    /**
     * generates steric clashing codebook based on CAs
     */
    void generateStericClashingCodebook();

    /**
     * Show the Extract from PDB dialog
     */
    void showExtractDlg();

    /**
     * Show the symmetry dialog
     */
    void showSymmetryDlg();

    /**
     * create multiple copies of the pdb using symmetry
     */
    void applySymmetry(svt_vector3<Real64> oPoint1, svt_vector3<Real64> oPoint2, int iOrder);

private:

    /**
     * set an atom and all connected bonds to a graphics mode
     */
    void setAtomGM(svt_atom* atom, pdb_mode m);

    /**
     * apply changes to the graphics mode
     */
    void applyPdbMode( pdb_mode& );

public:

    /**
     * rebuild the complete mode list, according to the current internal mode array
     */
    void rebuildModeList();

    /**
     * calculate the mode string
     */
    QString calcModeString(int i);

    /**
     * get the currently selected pdb_mode
     * \return pointer to the currently selected pdb_mode or NULL if nothing is selected
     */
    pdb_mode* getPdbMode(int iIndex = -1);

    /**
     * the user has changed something on the combo boxes - now the mode-array and the mode list must be updated.
     */
    void updatePdbMode();

    /**
     * create a svt_point_cloud object out of the current pdb object
     */
    svt_point_cloud_pdb< svt_vector4<Real64> >& getPointCloud();
    /**
     * get special point cloud for refinement
     */
    svt_point_cloud_pdb< svt_vector4<Real64> >& getRefinementPC();


public:

    /**
     * Return the special svt_sampled object for clustering
     */
    virtual svt_sampled< svt_vector4<Real64> >& getSampled();

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
     * Returns the center of the structure
     */
    void setCenter( svt_vector4<Real64> oCenter);


    /**
     * Returns the center of the structure
     */
    svt_matrix4< Real64 > getCenter();

public slots:

    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderMode( int );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeOFF( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModePOINT( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeLINE( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeVDW( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeCPK( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeDOTTED( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeLICORICE( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeTUBE( );
    /**
    * Select a global graphics mode (for pdb documents only)
    */
    void changeRenderModeCARTOON( );
    /**
     * Select a global graphics mode (for pdb documents only)
     */
    void changeRenderModeSURF( );
//     /**
//     * Select a global graphics mode (for pdb documents only)
//     */
//     void changeRenderModeTOON( );

    /**
     * Select a global coloring mode (for pdb documents only)
     * \param iMode the mode
     */
    void changeColmapMode( int iMode );

    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeSOLID();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeTYPE();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeRESNAME();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeCHAINID();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeSEGID();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeSTRUCTURE();
    /**
     * Select a global coloring mode (for pdb documents only)
     */
    void changeColmapModeMODEL();

public:

    /**
     * Set center sphere visibility
     * \param bVisible of true the sphere is visible
     */
    void setCenterSphereVisible( bool bVisible );

    /**
     * Set the radius/scaling of the center sphere
     * \param fRadius the new radius (1.0 is default)
     */
    void setCenterSphereRadius( float fRadius );

    /**
     * Set the color of the center sphere
     * \param fR red component
     * \param fG red component
     * \param fB red component
     */
    void setCenterSphereColor( float fR, float fG, float fB );

    /**
     * Set center sphere highlight visibility
     * \param bVisible true if highlight is visible
     */
    void setCenterSphereHighlightVisible( bool bVisible );

    /**
     * Set center sphere highlight color
     * \param fR red component
     * \param fG red component
     * \param fB red component
     */
    void setCenterSphereHighlightColor( float fR, float fG, float fB );

    /**
     * Set center text visibility
     * \param bVisible of true the text is visible
     */
    void setCenterTextVisible( bool bVisible );

    /**
     * Get center text visibility
     * \return true if the text is visible
     */
    bool getCenterTextVisible();

    /**
     * Set the text shown above the center of the structure
     * \param char* pText
     */
    void setCenterText( char* pText);

    /**
     * Get force arrow visibility
     * \return true the force arrow is visible
     */
    bool getForceArrowVisible();

    /**
     * Set force arrow visibility
     * \param bVisible true if the force arrow is visible
     */
    void setForceArrowVisible( bool bVisible );

    /**
     * Get force arrow transformation matrix
     * \return matrix4f* pTransformation
     */
    svt_matrix4f* getForceArrowTransformation();

    /**
     * Set force arrow transformation matrix
     * \param matrix4f* pTransformation
     */
    void setForceArrowTransformation(svt_matrix4f* pTransformation);

    /**
     * Get a pointer to the force arrow
     * \return svt_arrow pointer
     */
    svt_arrow* getForceArrow();

    /**
     * Get the size of the document
     * \return svt_vector4 with the three dimensions of the size of the document
     */
    virtual svt_vector4<Real32> getBoundingBox();

    /**
     * get the sphere scaling factor
     */
    Real32 getSphereScale();

    /**
     * set the sphere scaling factor
     * \param fSphereScale the sphere scaling factor
     */
    void setSphereScale(Real32 fSphereScale);

    /**
     * virtual method to create the property dialog. should be implemented in the documents that inherit from sculptor_document
     * \param pParent pointer to parent widget
     */
    virtual QWidget* createPropDlg(QWidget* pParent);

    /**
     * return the prop dialog without creating it
     * \return QWidget* pointer to the prop dialog
     */
    virtual QWidget* getPropDlg();

    /**
     * enable/disable the use of shaders
     */
    void setAllowShaderPrograms();
};

#endif
