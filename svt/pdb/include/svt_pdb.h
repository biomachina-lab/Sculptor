/***************************************************************************
                          svt_pdb
			  -------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PDB_H
#define SVT_PDB_H

#include <svt_fstream.h>
#include <svt_core.h>
#include <svt_node.h>
#include <svt_node_noautodl.h>
#include <svt_atom.h>
#include <svt_atom_colmap.h>
#include <svt_bond.h>
#include <svt_container.h>
#include <svt_cr_spline.h>
#include <svt_kb_spline.h>
#include <svt_volume.h>
#include <svt_vector4.h>
#include <svt_point_cloud_pdb.h>
#include <svt_container_cartoon.h>
#include <svt_container_toon.h>
#include <svt_container_surf.h>

enum {
    SVT_NONE,
    SVT_PDB,
    SVT_FETCHPDB,
    SVT_PSF,
    SVT_DCD
};

enum {
    SVT_DYNAMICS_ONCE,
    SVT_DYNAMICS_LOOP,
    SVT_DYNAMICS_ROCK,
    SVT_DYNAMICS_FORWARD,
    SVT_DYNAMICS_STOPPED,
    SVT_DYNAMICS_BACKWARD
};

enum {
    SVT_PDB_OFF,
    SVT_PDB_LINE,
    SVT_PDB_CPK,
    SVT_PDB_POINT,
    SVT_PDB_VDW,
    SVT_PDB_DOTTED,
    SVT_PDB_LICORIZE,
    SVT_PDB_TUBE,
    SVT_PDB_CARTOON,
    SVT_PDB_NEWTUBE,
    SVT_PDB_TOON,
    SVT_PDB_SURF
};

/**
 * A pdb file node. This node can load a pdb file (molecule) and display it in various drawing modes. The default mode is a line drawing mode, which is very fast but not very good looking. You can switch to other modes or even mix modes. That requires the use of the enumeration functions which search for atoms with certain properties (name, residue, etc). You can specify the drawing mode for every atom!
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_pdb : public svt_node, public svt_point_cloud_pdb<svt_vector4<Real64> >
{

protected:

    // another atom array, but this time with rendering objects
    vector< svt_atom > m_aRenAtoms;

    // another bond array, but this time with rendering objects
    vector< svt_bond > m_aRenBonds;

    // color mapping
    svt_atom_colmap* m_pMapping;

    // is at least one atom flagged to be drawn with a label?
    bool m_bShowAtomLabel;

    // notify the user only once about incomplete residues (i.e., without O atoms in the peptide plane)
    bool m_bNotifiedUser;

    // tube mode
    float m_fTubeDiameter;
    int m_iTubeSegs;

    // newtube and cartoon mode
    bool m_bCartoonUseShader; // used for both NewTube and Cartoon
    svt_container_cartoon * m_pCartoonCont;
    svt_container_cartoon_dynamic * m_pCartoonCont_Dynamic;
    svt_container_cartoon_static * m_pCartoonCont_Static;
    //
    unsigned int m_iNewTubeProfileCorners;
    unsigned int m_iNewTubeSegs;
    float        m_fNewTubeDiameter;
    //
    unsigned int m_iMinResiduesPerHelix;
    unsigned int m_iMinResiduesPerSheet;
    unsigned int m_iCartoonSheetHelixProfileCorners;
    unsigned int m_iCartoonSegments;
    Real32       m_fCartoonHelixWidth;
    Real32       m_fCartoonHelixHeight;
    Real32       m_fCartoonSheetWidth;
    Real32       m_fCartoonSheetHeight;
    Real32       m_fCartoonTubeDiameter;
    bool         m_bCartoonHelixArrowhead;
    bool         m_bCartoonHelixCylinder;
    bool         m_bCartoonPeptidePlanes;

    // member variables for the molecular dynamic visualization
    int m_iTimestep;
    int m_iMaxStep;
    int m_iSpeedTime;
    float m_fSpeed;
    float m_fSpeedTS;
    unsigned int m_iLoop;
    bool m_bDynamic;

    // basic container
    svt_container* m_pCont;

    // toon mode
    svt_container_toon * m_pToonCont;
    int m_iVDWRenderingMode;

    // splines
    svt_cr_spline m_oCRSpline;
    svt_kb_spline m_oKBSpline;

    // surf mode
    svt_container_surf* m_pContainer_Surf;

public:


    /**
     * Default Constructor
     */
    svt_pdb();
    /**
     * Constructor
     * \param pFileA pointer to a char array with the filename of the first file
     * \param iTypeA file-type of the first file (e.g. SVT_NONE, SVT_PDB, SVT_PSF, SVT_DCD, ...)
     * \param pFileB pointer to a char array with the filename of the second file
     * \param iTypeB file-type of the second file (e.g. SVT_PDB, SVT_PSF, SVT_DCD, ...)
     * \param pPos pointer to svt_pos object for the position
     * \param pProp pointer to svt_properties object
     */
    svt_pdb(char* pFileA, int iTypeA =SVT_PDB, char* pFileB =NULL, int iTypeB =SVT_NONE, svt_pos* pPos =NULL, svt_properties* pProp =NULL);
    /**
     * Constructor
     * Copy an existing svt_point_cloud_pdb object
     * \param rPDB reference to svt_point_cloud_pdb object
     */
    svt_pdb( svt_point_cloud_pdb< svt_vector4<Real64 > >& rPDB );
    /**
     * Copy constructor
     */
    svt_pdb( svt_pdb& );
    /**
     * Destructor
     */
    virtual ~svt_pdb();

    /**
     * Return a rendering atom
     */
    svt_atom* getRenAtom( int i );
    /**
     * Return a rendering bond
     */
    svt_bond* getRenBond( int i );

    /**
     * add atom
     */
    unsigned int addAtom( svt_point_cloud_atom& rAtom, svt_vector4<Real64>& rVec);
    /**
     * add a bond
     * \param iAtomA index of atom A
     * \param iAtomB index of atom B
     */
    void addBond( unsigned int iAtomA, unsigned int iAtomB );
    /**
     * remove bond between two atoms?
     * \param iAtomA index of atom A
     * \param iAtomB index of atom B
     */
    void delBond( unsigned int iAtomA, unsigned int iAtomB );
    /**
     * Append an existing svt_point_cloud_pdb object
     * \param rPDB reference to svt_point_cloud_pdb object
     */
    void addPointCloud( svt_point_cloud_pdb< svt_vector4<Real64 > >& rPDB );
    /**
     * delete all atoms
     */
    void deleteAllAtoms();
    /**
     * delete all bonds
     */
    void deleteAllBonds();

    /**
     * \name Drawing mode functions
     * With this functions you can change the way the pdb file is displayed.
     */
    //@{
    /**
     * set the complete pdb to a certain display mode
     * \param iDisplayMode the new display mode (e.g. SVT_PDB_LINE)
     */
    void setDisplayMode(Int16 iDisplayMode);
    /**
     * set the complete pdb to a certain text label mode
     * \param iLabelMode the new text label mode (e.g. SVT_PDB_LABEL_INDEX)
     */
    void setLabelMode(Int16 iLabelMode);
    /**
     * set all atoms and bonds to a certain display mode
     * \param iAtomMode display mode for the atoms
     * \param iBondMode display mode for the bonds
     */
    void setAllAtomBondDisplayMode(Int16 iAtomMode, Int16 iBondMode);
    /**
     * set the scale for the cpk sphere radius for all atoms
     * \param fRadius the new sphere scale
     */
    void setCPKRadiusScaling(float fRadius);
    /**
     * set the scale for the vdw sphere radius for all atoms
     * \param fRadius the new sphere scale
     */
    void setVDWRadiusScaling(float fRadius);
    /**
     * set the svt_atom_colmap object (responsible for the atom->color mapping)
     * \param pMapping pointer to an svt_atom_colmap object
     */
    void setAtomColmap(svt_atom_colmap*);
    /**
     * get the svt_atom_colmap object (responsible for the atom->color mapping)
     * \return pointer to the svt_atom_colmap object
     */
    svt_atom_colmap* getAtomColmap();
    //@}


    /**
     * \name Tube Mode
     * Functions dealing with the old tube mode
     */
    //@{
    /**
     * set the tube cylinder diameter scaling (default 2.0f)
     * \param fTubeDiameter tube diameter scaling value
     */
    void setTubeDiameter(float fTubeDiameter);
    /**
     * get the tube cylinder diameter scaling (default 2.0f)
     * \return tube diameter scaling value
     */
    float getTubeDiameter();
    /**
     * set the number of tube segments between two CA atoms (default: 5)
     * \param iTubeSegs number of tube segments between two atoms
     */
    void setTubeSegs(int iTubeSegs);
    /**
     * get the number of tube segments between two CA atoms (default: 5)
     * \return number of tube segments between two atoms
     */
    int getTubeSegs();
    //@}


    /**
     * \name Cartoon Mode
     * Functions dealing with the cartoon mode
     */
    //@{
    /**
     * set a parameter for the cartoon mode
     * \param eParameter can be one of
     * - CARTOON_USE_SHADER (1 or 0)
     * - CARTOON_PER_PIXEL_LIGHTING (1 or 0)
     * - CARTOON_PEPTIDE_PLANES (1 or 0)
     * - CARTOON_SHEET_MIN_RESIDUES (default 2)
     * - CARTOON_SHEET_WIDTH
     * - CARTOON_SHEET_HEIGHT
     * - CARTOON_HELIX_MIN_RESIDUES (default 2)
     * - CARTOON_HELIX_WIDTH
     * - CARTOON_HELIX_HEIGHT
     * - CARTOON_TUBE_DIAMETER
     * - CARTOON_HELIX_CYLINDER (1 or 0)
     * - CARTOON_HELIX_ARROWHEADS (1 or 0)
     * - CARTOON_SPLINE_SEGMENTS (default 6)
     * \param fValue will be used as float or converted to boolean or int depending on the parameter that is to be set (determined by eParameter)
     */
    void setCartoonParam(EnumCartoonParam eParameter, float fValue);
    /**
     * get a parameter for the cartoon mode
     * \param eParameter determines the parameter value to be returned (see setCartoonParam)
     */
    float getCartoonParam(EnumCartoonParam eParameter);
    /**
     * Check if use of shaders for the Cartoon mode is possible (needs at least vertex profile ARBVP1).
     */
    bool getShaderSupported();
    /**
     * 
     */
    int getBackboneO(int i);
    //@}


    /**
     * \name NewTube Mode
     * Functions dealing with the new tube mode
     */
    //@{
    /**
     * set the number of profile corners of the new Tube (default: 4)
     * \param iNumCorners number of profile corners
     */
    void setNewTubeProfileCorners( int iNumCorners );
    /**
     * return the number of profile corners of the new tube
     */
    int getNewTubeProfileCorners();
    /**
     * set the tube cylinder diameter scaling (default 2.0f)
     * \param fTubeDiameter tube diameter scaling value
     */
    void setNewTubeDiameter(float fNewTubeDiameter);
    /**
     * get the tube cylinder diameter scaling (default 2.0f)
     * \return tube diameter scaling value
     */
    float getNewTubeDiameter();
    /**
     * set the number of tube segments between two CA atoms (default: 6)
     * \param iTubeSegs number of tube segments between two atoms
     */
    void setNewTubeSegs(int iTubeSegs);
    /**
     * get the number of tube segments between two CA atoms (default: 6)
     * \return number of tube segments between two atoms
     */
    int getNewTubeSegs();
    //@}


    /**
     * \name Toon Mode
     * Functions dealing with the toon mode. The toon mode is also used for the van-der-Wasls
     * spheres
     */
    //@{
    /**
     * set the rendering mode for the toons. This can be either
     * - SVT_PDB_VDW_NO_SHADER: geometry will be added to svt_container
     * - SVT_PDB_VDW_GLOSSY: atoms will be added to svt_container_toon and per pixel shaded
     */
    void setVDWRenderingMode(int iMode);
    /**
     * get the rendering mode for the toons
     * \return int with the vdw rendering mode
     */
    int getVDWRenderingMode();
    /**
     * check if toons are supported at all
     * \return true if toons are supported, false otherwise
     */
    bool getToonSupported();
    //@}


    /**
     * Prepare the OpenGL output
     */
    virtual void outputGL();

    /**
     * draw the pdb data
     */
    virtual void drawGL();

    /**
     * rebuild display list
     */
    virtual void rebuildDL();

    /**
     * prepare container
     * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
     */
    virtual void prepareContainer();

    /**
     * set loop
     * \param iLoop SVT_DYNAMICS_ONCE, SVT_DYNAMICS_LOOP, SVT_DYNAMICS_ROCK
     */
    void setLoop( unsigned int iLoop );

    /**
     * get loop
     * \return SVT_DYNAMICS_ONCE, SVT_DYNAMICS_LOOP, SVT_DYNAMICS_ROCK
     */
    unsigned int getLoop();

    /**
     * set speed
     * \param fSpeed timesteps per second
     */
    void setSpeed(float fSpeed);

    /**
     * get speed
     * \return timesteps per second
     */
    float getSpeed();

    /**
     * print name of scenegraph node to stdout
     */
    virtual void printName();

    /**
     * update internal state according to the global SVT variable if usage of customized shader
     * programs is allowed. Here, it should also be checked if shaders were not initialized yet and
     * do the appropriate initializations, if necessary
     */
    void setAllowShaderPrograms();

protected:


    /**
     * \name Tube Mode
     * Functions dealing with the old tube mode
     */
    //@{
    /**
     * calculate a tube
     * \param rAtoms reference to svt_array<int> with the indizes of the atoms
     */
    void calcTube( vector<int>& rAtoms );
    /**
     * create a tube by extruding a circle
     * \param aPath array with the path information
     * \param fRadius radius of the circle
     * \param iPoints number of points representing the circle
     */
    void extrudeCircle( vector<svt_vec4real32> aPath, Real32 fRadius, int iPoints);
    /**
     * local function to correct the length of the cylinders
     */
    void correctCylinder( svt_vector4<Real32>& rPrev, svt_vector4<Real32>& rStart, svt_vector4<Real32>& rEnd, svt_vector4<Real32> rNext);
    //@}


    /**
     * \name Cartoon Mode
     * Functions dealing with the cartoon mode
     */
    //@{
    /**
     * this function adds the cartoon container and initializes member variables
     */
    void initCartoon();
    /**
     * Calculate the Cartoon
     * \param aCA reference to vector< int > with indices of all C_alpha atoms
     * \param aO reference to vector< int > with indices of all O atoms in the backbone
     * \param aSS reference to vector< char > with the secondary structure (H, S, C) of the C_alpha atoms
     */
    void calcCartoon( vector< int >* aCA, vector< char >* aSS  );
    /**
     * Calculate the NewTube
     * \param aCA reference to vector< int > with indices of all C_alpha atoms
     */
    void calcNewTube( vector< int > * aCA );
    //@}


    /**
     * \name Toon Mode
     * Functions dealing with the toon mode
     */
    //@{
    /**
     * this function adds the toon container and initializes member variables
     */
    void initToon();
    //@}

public:

    /**
     * \name Surface Mode
     * Functions dealing with the solvent accessible surfaces mode
     */
    //@{
    /**
     * Set the radius of the probe sphere for the solvent accessible surfaces (default: 1.4A)
     * \param fProbeRadius radius of probe sphere
     */
    void setProbeRadius( Real64 fProbeRadius );
    /**
     * Get the radius of the probe sphere for the solvent accessible surfaces (default: 1.4A)
     * \return radius of probe sphere
     */
    Real64 getProbeRadius( );
    //@}

};

#endif // SVT_PDB_H
