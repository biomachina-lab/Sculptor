/***************************************************************************
                          svt_pdb.cpp
			  -----------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_pdb.h>
#include <svt_atom_colmap.h>
#include <svt_atom_type_colmap.h>
#include <svt_bond.h>
#include <svt_color.h>
#include <svt_window.h>
#include <svt_system.h>
#include <svt_swap.h>
#include <svt_time.h>
#include <svt_file_utils.h>
#include <svt_simple_text.h>
// clib includes
#include <stdio.h>
#include <string.h>

#ifdef WIN32
extern "C" DLLEXPORT BOOL WINAPI DllMain( HANDLE hModule, DWORD dwFunction, LPVOID lpNot )
{
    svt_registerCout( &std::cout );
    return true;
};
#endif

///////////////////////////////////////////////////////////////////////////////
// member functions
///////////////////////////////////////////////////////////////////////////////


/**
 * Default Constructor
 */
svt_pdb::svt_pdb()
    : m_pMapping( new svt_atom_type_colmap() ),
      m_bShowAtomLabel( true ),
      m_iTimestep( 0 ),
      m_iMaxStep( 0 ),
      m_fSpeed( 0.0f ),
      m_fSpeedTS( 0.0f ),
      m_iLoop( SVT_DYNAMICS_ONCE ),
      m_bDynamic( false )
{
    m_bIgnoreWater = true;
    m_bSecStructAvailable = false;
    m_eSecStructSource = SSE_NOTAVAILABLE;

    calcAllLists();
    resetAtomEnum();
    resetBondEnum();
    setTubeDiameter(2.0f);
    m_iTubeSegs = 5;

    setAutoDL(true);

    // add containers for graphics
    m_pCont = new svt_container;
    m_pCont->setName("(svt_pdb)m_pCont");
    add(m_pCont);

    // add container for the surf mode
    m_pContainer_Surf = new svt_container_surf();
    m_pContainer_Surf->setName("(svt_pdb)m_pContainer_Surf");
    add( m_pContainer_Surf );

    initCartoon();
    initToon();
}


/**
 * Constructor
 * \param pFileA pointer to an char array with the filename of the first file
 * \param iTypeA file-type of the first file (e.g. SVT_NONE, SVT_PDB, SVT_PSF, SVT_DCD, ...)
 * \param pFileB pointer to an char array with the filename of the second file
 * \param iTypeB file-type of the second file (e.g. SVT_PDB, SVT_PSF, SVT_DCD, ...)
 * \param pPos pointer to svt_pos object for the position
 * \param pProp pointer to svt_properties object
 */
svt_pdb::svt_pdb(char* pFileA, int iTypeA, char* pFileB, int iTypeB, svt_pos* pPos, svt_properties* pProp)
    : svt_node(pPos, pProp),
      svt_point_cloud_pdb< svt_vector4<Real64> >(),
      m_pMapping( new svt_atom_type_colmap() ),
      m_bShowAtomLabel( true ),
      m_iTimestep( 0 ),
      m_iMaxStep( 0 ),
      m_fSpeed( 0.0f ),
      m_fSpeedTS( 0.0f ),
      m_iLoop( SVT_DYNAMICS_ONCE ),
      m_bDynamic( false )
{
    m_bSecStructAvailable = false;
    m_eSecStructSource = SSE_NOTAVAILABLE;

    setName( "svt_pdb" );

    // load file A
    switch(iTypeA)
    {
        case SVT_PDB:
            loadPDB( pFileA );
            if (iTypeB == SVT_NONE)
                calcBonds();
            break;

        case SVT_FETCHPDB:
            fetchPDB( pFileA );
            if (iTypeB == SVT_NONE)
                calcBonds();
            break;

        case SVT_PSF:
            if (iTypeB == SVT_PDB)
            {
                loadPDB( pFileB );
                loadPSF( pFileA );
                break;
            }

            loadPSF( pFileA );

        case SVT_NONE:
        default:
            break;
    }

    // load file B
    switch(iTypeB)
    {
        case SVT_PSF:
            loadPSF( pFileB );
            break;

        case SVT_DCD:
            loadDCD( pFileB );
            break;

        case SVT_NONE:

        default:
            break;
    }

    unsigned int i;

    if (getMinXCoord()<=1000 && getMaxXCoord()<=1000 &&   // this is a real pdb - then calc secondary structure  
        getMinYCoord()<=1000 && getMaxYCoord()<=1000 &&   // otherwise this my a results in a tomograpm
        getMinZCoord()<=1000 && getMaxZCoord()<=1000   )
    calcSecStruct();

    for(i=0; i<m_aAtoms.size(); i++)
	m_aRenAtoms.push_back( svt_atom( i, *this, m_aAtoms[i], m_pMapping ) );

    for(i=0; i<m_aBonds.size(); i++)
        if (m_aBonds[i].getIndexA() != -1 && m_aBonds[i].getIndexB() != -1)
	    m_aRenBonds.push_back( svt_bond( (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexA()],
                                             (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexB()], m_pMapping ) );

    calcAllLists();

    resetAtomEnum();
    resetBondEnum();

    setTubeDiameter(2.0f);
    m_iTubeSegs = 5;

    setAutoDL(true);

    // add containers for graphics
    m_pCont = new svt_container;
    m_pCont->setName("(svt_pdb)m_pCont");
    add(m_pCont);

    // add container for the surf mode
    m_pContainer_Surf = new svt_container_surf();
    m_pContainer_Surf->setName("(svt_pdb)m_pContainer_Surf");
    add( m_pContainer_Surf );

    initCartoon();
    initToon();
}


/**
 * Constructor
 * Copy an existing svt_point_cloud_pdb object
 * \param rPDB reference to svt_point_cloud_pdb object
 */
svt_pdb::svt_pdb( svt_point_cloud_pdb< svt_vector4<Real64 > >& rPDB )
    : svt_node(),
      svt_point_cloud_pdb< svt_vector4<Real64> >( rPDB ),
      m_pMapping( new svt_atom_type_colmap() ),
      m_bShowAtomLabel( true ),
      m_iTimestep( 0 ),
      m_iMaxStep( 0 ),
      m_fSpeed( 0.0f ),
      m_fSpeedTS( 0.0f ),
      m_iLoop( SVT_DYNAMICS_ONCE ),
      m_bDynamic( false )
{
    m_bSecStructAvailable = rPDB.getSecStructAvailable();
    m_eSecStructSource = (svt_secStructSource)rPDB.getSecStructSource();

    if (rPDB.getBondsNum()==0)
        calcBonds(false);
    
    if (!rPDB.getSecStructAvailable())
        calcSecStruct();

    unsigned int i;

    for(i=0; i<m_aAtoms.size(); i++)
	m_aRenAtoms.push_back( svt_atom( i, *this, m_aAtoms[i], m_pMapping ) );

    for(i=0; i<m_aBonds.size(); i++)
	if (m_aBonds[i].getIndexA() != -1 && m_aBonds[i].getIndexB() != -1)
	    m_aRenBonds.push_back( svt_bond( (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexA()],
                                             (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexB()], m_pMapping ) );

    calcAllLists();
    resetAtomEnum();
    resetBondEnum();
    setTubeDiameter(2.0f);
    m_iTubeSegs = 5;


    setAutoDL(true);

    // add containers for graphics
    m_pCont = new svt_container;
    m_pCont->setName("(svt_pdb)m_pCont");
    add(m_pCont);

    // add container for the surf mode
    m_pContainer_Surf = new svt_container_surf();
    m_pContainer_Surf->setName("(svt_pdb)m_pContainer_Surf");
    add( m_pContainer_Surf );

    initCartoon();
    initToon();
}

/**
 * Copy constructor
 */
svt_pdb::svt_pdb( svt_pdb& rOther )
    : svt_node(),
      svt_point_cloud_pdb< svt_vector4<Real64> >( rOther )
{
    m_aRenAtoms                  = rOther.m_aRenAtoms;
    m_aRenBonds                  = rOther.m_aRenBonds;

    m_pMapping = new svt_atom_type_colmap();

    m_bShowAtomLabel             = rOther.m_bShowAtomLabel;

    m_fTraceCutoff               = rOther.m_fTraceCutoff;
    m_fTubeDiameter              = rOther.m_fTubeDiameter;
    m_iTubeSegs                  = rOther.m_iTubeSegs;

    m_pCont = new svt_container;
    m_pCont->setName("(svt_pdb)m_pCont");
    add(m_pCont);

    // add container for the surf mode
    m_pContainer_Surf = new svt_container_surf();
    m_pContainer_Surf->setName("(svt_pdb)m_pContainer_Surf");
    add( m_pContainer_Surf );

    // copy toon settings. compare initToon()
    m_pToonCont = new svt_container_toon();
    m_pToonCont->setName("(svt_pdb)m_pToonCont");
    add(m_pToonCont);
    //
    m_iVDWRenderingMode          = rOther.m_iVDWRenderingMode;


    // copy cartoon settings. compare initCartoon() (in svt_pdb_cartoon.cpp)
    m_iNewTubeSegs               = rOther.m_iNewTubeSegs;
    m_iNewTubeProfileCorners     = rOther.m_iNewTubeProfileCorners;
    m_fNewTubeDiameter           = rOther.m_fNewTubeDiameter;
    //
    m_bCartoonPeptidePlanes      = rOther.m_bCartoonPeptidePlanes;
    m_bCartoonHelixArrowhead     = rOther.m_bCartoonHelixArrowhead;
    m_bCartoonHelixCylinder      = rOther.m_bCartoonHelixCylinder;
    m_fCartoonHelixWidth         = rOther.m_fCartoonHelixWidth;
    m_fCartoonHelixHeight        = rOther.m_fCartoonHelixHeight;
    m_fCartoonSheetWidth         = rOther.m_fCartoonSheetWidth;
    m_fCartoonSheetHeight        = rOther.m_fCartoonSheetHeight;
    m_fCartoonTubeDiameter       = rOther.m_fCartoonTubeDiameter;
    m_iCartoonSegments           = rOther.m_iCartoonSegments;
    m_iMinResiduesPerSheet       = rOther.m_iMinResiduesPerSheet;
    m_iMinResiduesPerHelix       = rOther.m_iMinResiduesPerHelix;
    m_iCartoonSheetHelixProfileCorners = rOther.m_iCartoonSheetHelixProfileCorners;
    //
    m_pCartoonCont_Dynamic = new svt_container_cartoon_dynamic(this);
    m_pCartoonCont_Dynamic->setName("(svt_pdb)m_pCartoonCont_Dynamic");
    add(m_pCartoonCont_Dynamic);
    m_pCartoonCont_Static = new svt_container_cartoon_static(this);
    m_pCartoonCont_Static->setName("(svt_pdb)m_pCartoonCont_Static");
    add(m_pCartoonCont_Static);
    m_bCartoonUseShader          = rOther.m_bCartoonUseShader;
    if (m_bCartoonUseShader)
    {
	m_pCartoonCont =  m_pCartoonCont_Dynamic;
	m_pCartoonCont_Static->setVisible(false);
    }
    else
    {
	m_pCartoonCont =  m_pCartoonCont_Static;
	m_pCartoonCont_Dynamic->setVisible(false);
    }


    m_iTimestep                  = rOther.m_iTimestep;
    m_iMaxStep                   = rOther.m_iMaxStep;
    m_iSpeedTime                 = rOther.m_iSpeedTime;
    m_fSpeed                     = rOther.m_fSpeed;
    m_fSpeedTS                   = rOther.m_fSpeedTS;
    m_iLoop                      = rOther.m_iLoop;
    m_bDynamic                   = rOther.m_bDynamic;

    //m_oLastEndpoint              = rOther.m_oLastEndpoint;
    //m_oNextStartpoint            = rOther.m_oNextStartpoint;
    m_oCRSpline                  = rOther.m_oCRSpline;
    m_oKBSpline                  = rOther.m_oKBSpline;
}


/**
 * add atom
 */
unsigned int svt_pdb::addAtom( svt_point_cloud_atom& rAtom, svt_vector4<Real64>& rVec)
{
    svt_point_cloud_pdb<svt_vector4<Real64> >::addAtom( rAtom, rVec );
    unsigned int i = m_aAtoms.size() - 1;
    m_aRenAtoms.push_back( svt_atom( i, *this, m_aAtoms[i], m_pMapping ) );
    return i;
};

/**
 * add a bond
 * \param iAtomA index of atom A
 * \param iAtomB index of atom B
 */
void svt_pdb::addBond( unsigned int iAtomA, unsigned int iAtomB )
{
    svt_point_cloud_pdb<svt_vector4<Real64> >::addBond( iAtomA, iAtomB );
    unsigned int i = m_aBonds.size() - 1;
    m_aRenBonds.push_back( svt_bond( (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexA()], (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexB()], m_pMapping ) );
};
/**
 * remove bond between two atoms?
 * \param iAtomA index of atom A
 * \param iAtomB index of atom B
 */
void svt_pdb::delBond( unsigned int iAtomA, unsigned int iAtomB )
{
    vector< unsigned int > aList = this->m_aAtoms[iAtomA].getBondList();

    if (aList.size() == 0)
    {
        svtout << "DEBUG: delBond was called, but there are no bonds there?!" << endl;
        return;
    }

    for(unsigned int i=0; i<aList.size(); i++)
    {
        if (this->m_aBonds[aList[i]].getIndexB() == (int)(iAtomB) || this->m_aBonds[aList[i]].getIndexA() == (int)(iAtomB))
        {
            this->m_aAtoms[iAtomA].delFromBondList( aList[i] );

            // now correct the other bonds
            for(unsigned int j=0;j<m_aAtoms.size(); j++)
                this->m_aAtoms[j].adjustBondList(aList[i]);

            this->m_aBonds.erase( m_aBonds.begin() + aList[i]);
            this->m_aRenBonds.erase( m_aRenBonds.begin() + aList[i]);

            return;
        }
    }

    svtout << "DEBUG: delBond was called, but I cannot find that bond?!" << endl;
    return;
};
/**
 * delete all atoms
 */
void svt_pdb::deleteAllAtoms()
{
    svt_point_cloud_pdb<svt_vector4<Real64> >::deleteAllAtoms( );
    m_aRenAtoms.clear();
    m_aRenBonds.clear();
};
/**
 * delete all bonds
 */
void svt_pdb::deleteAllBonds()
{
    svt_point_cloud_pdb<svt_vector4<Real64> >::deleteAllBonds( );
    m_aRenBonds.clear();
};

/**
 * Append an existing svt_point_cloud_pdb object
 * \param rPDB reference to svt_point_cloud_pdb object
 */
void svt_pdb::addPointCloud( svt_point_cloud_pdb< svt_vector4<Real64 > >& rPDB )
{
    unsigned int i;
    unsigned int iAtomStart = m_aAtoms.size();
    unsigned int iBondStart = m_aBonds.size();

    (*this).append( rPDB );

    for(i=iAtomStart; i<m_aAtoms.size(); i++)
	m_aRenAtoms.push_back( svt_atom( i, *this, m_aAtoms[i], m_pMapping ) );

    for(i=iBondStart; i<m_aBonds.size(); i++)
    {
        if (m_aBonds[i].getIndexA() != -1 && m_aBonds[i].getIndexB() != -1)
            m_aRenBonds.push_back( svt_bond( (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexA()], (svt_atom*)&m_aRenAtoms[ m_aBonds[i].getIndexB()], m_pMapping ) );
    }

    calcAllLists();
}

/**
 * Destructor
 */
svt_pdb::~svt_pdb()
{
    delete m_pMapping;

    m_aRenAtoms.clear();
    m_aRenBonds.clear();

    deleteAllAtoms();
    deleteAllBonds();
}


/**
 * this function adds the toon container and initializes member variables
 */
void svt_pdb::initToon()
{
    m_pToonCont = new svt_container_toon();
    m_pToonCont->setName("(svt_pdb)m_pToonCont");
    add(m_pToonCont);

    if ( m_pToonCont->getToonSupported() )
	m_iVDWRenderingMode = SVT_PDB_VDW_GLOSSY;
    else
	m_iVDWRenderingMode = SVT_PDB_VDW_NO_SHADER;
}


/**
 * Return a rendering atom
 */
svt_atom* svt_pdb::getRenAtom( int i )
{
    if (i >= 0 && i < (int)(m_aRenAtoms.size()) )
	return &m_aRenAtoms[(unsigned int)i];
    else
        return NULL;
}
/**
 * Return a rendering bond
 */
svt_bond* svt_pdb::getRenBond( int i )
{
    if (i >= 0 && i < (int)(m_aRenBonds.size()) )
	return &m_aRenBonds[(unsigned int)i];
    else
        return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Drawing mode functions
///////////////////////////////////////////////////////////////////////////////

/**
 * set the complete pdb to a certain display mode
 * \param iDisplayMode the new display mode (e.g. SVT_PDB_LINE)
 */
void svt_pdb::setDisplayMode(Int16 iDisplayMode)
{
    switch(iDisplayMode)
    {
    case SVT_PDB_OFF:
        setAllAtomBondDisplayMode(SVT_ATOM_OFF, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_DOTTED:
        setAllAtomBondDisplayMode(SVT_ATOM_DOTTED, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_VDW:
        setAllAtomBondDisplayMode(SVT_ATOM_VDW, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_CPK:
        setAllAtomBondDisplayMode(SVT_ATOM_CPK, SVT_BOND_CYLINDER);
        rebuildDL();
        break;
    case SVT_PDB_LINE:
        setAllAtomBondDisplayMode(SVT_ATOM_POINT, SVT_BOND_LINE);
        rebuildDL();
        break;
    case SVT_PDB_LICORIZE:
        setAllAtomBondDisplayMode(SVT_ATOM_LICORIZE, SVT_BOND_LICORIZE);
        rebuildDL();
        break;
    case SVT_PDB_POINT:
        setAllAtomBondDisplayMode(SVT_ATOM_POINT, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_TUBE:
        setAllAtomBondDisplayMode(SVT_ATOM_TUBE, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_CARTOON:
        setAllAtomBondDisplayMode(SVT_ATOM_CARTOON, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_NEWTUBE:
        setAllAtomBondDisplayMode(SVT_ATOM_NEWTUBE, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_TOON:
        setAllAtomBondDisplayMode(SVT_ATOM_TOON, SVT_BOND_OFF);
        rebuildDL();
        break;
    case SVT_PDB_SURF:
        setAllAtomBondDisplayMode(SVT_ATOM_SURF, SVT_BOND_OFF);
        rebuildDL();
        break;

    default:
        break;
    }
}

/**
 * set the complete pdb to a certain text label mode
 * \param iLabelMode the new text label mode (e.g. SVT_PDB_LABEL_INDEX)
 */
void svt_pdb::setLabelMode(Int16 iLabelMode)
{
    unsigned int i;

    for ( i=0; i< m_aRenAtoms.size();i++ )
        m_aRenAtoms[i].setLabelMode( iLabelMode );

    if (iLabelMode != SVT_ATOM_LABEL_OFF)
	m_bShowAtomLabel = true;

};

/**
 * set all atoms and bonds to a certain display mode
 * \param iAtomMode display mode for the atoms
 * \param iBondMode display mode for the bonds
 */
void svt_pdb::setAllAtomBondDisplayMode(Int16 iAtomMode, Int16 iBondMode)
{
    unsigned int i;

    for ( i=0; i< m_aRenAtoms.size();i++ )
        m_aRenAtoms[i].setDisplayMode( iAtomMode );
    for ( i=0; i< m_aRenBonds.size();i++ )
        m_aRenBonds[i].setDisplayMode( iBondMode );
}

/**
 * set the scale for the cpk sphere radius for all atoms
 * \param fRadius the new sphere scale
 */
void svt_pdb::setCPKRadiusScaling(float fRadius)
{
    unsigned int i;
    for (i=0; i< m_aRenAtoms.size();i++)
        m_aRenAtoms[i].setCPKRadiusScaling(fRadius);
    for (i=0; i< m_aRenBonds.size();i++)
        m_aRenBonds[i].setCylinderRad(fRadius / 0.2);
}
/**
 * set the scale for the vdw sphere radius for all atoms
 * \param fRadius the new sphere scale
 */
void svt_pdb::setVDWRadiusScaling(float fRadius)
{
    unsigned int i;
    for (i=0; i< m_aRenAtoms.size();i++)
        m_aRenAtoms[i].setVDWRadiusScaling(fRadius);
}

/**
 * set the rendering mode for the toons. This can be either
 * - SVT_PDB_VDW_NO_SHADER: geometry will be added to svt_container
 * - SVT_PDB_VDW_GLOSSY: atoms will be added to svt_container_toon and per pixel shaded
 */
void svt_pdb::setVDWRenderingMode(int iMode)
{
    m_iVDWRenderingMode = iMode;
    if (iMode == SVT_PDB_VDW_GLOSSY)
    {
        m_pToonCont->checkShaderSupport();
        m_pToonCont->setVisible(true);
    }

    rebuildDL();
}

/**
 * get the rendering mode for the toons
 * \return int with the vdw rendering mode
 */
int svt_pdb::getVDWRenderingMode()
{
    return m_iVDWRenderingMode;
}


/**
 * set the svt_atom_colmap object (responsible for the atom->color mapping)
 * \param pMapping pointer to an svt_atom_colmap object
 */
void svt_pdb::setAtomColmap(svt_atom_colmap* pMapping)
{
    unsigned int i;
    for(i=0;i<m_aRenAtoms.size();i++)
        m_aRenAtoms[i].setAtomColmap(pMapping);
    for(i=0;i<m_aRenBonds.size();i++)
        m_aRenBonds[i].setAtomColmap(pMapping);
}

/**
 * get the svt_atom_colmap object (responsible for the atom->color mapping)
 * \return pointer to the svt_atom_colmap object
 */
svt_atom_colmap* svt_pdb::getAtomColmap()
{
    return m_pMapping;
}


///////////////////////////////////////////////////////////////////////////////
// drawing functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Prepare the OpenGL output
 */
void svt_pdb::outputGL()
{
    svt_node::outputGL();

    // check if there atoms that should be drawn with labels
    // rebuildDL sets this bool to true

    if (m_bShowAtomLabel)
    {
	if (getVisible())
	{
	    glPushMatrix();

	    // apply all attributes to the object
	    applyGL();


	    unsigned int iNumAtoms = m_aRenAtoms.size();
	    char pStr[256];
	    int iNumAtomLabels = 0;
	    int iMode;

	    for(unsigned int i=0; i<iNumAtoms; ++i)
	    {
		iMode = m_aRenAtoms[i].getDisplayMode();

		if (iMode == SVT_ATOM_OFF)
		    continue;

		switch(m_aRenAtoms[i].getLabelMode())
		{
		    case SVT_ATOM_LABEL_INDEX:

			if ( (iMode != SVT_ATOM_TUBE && iMode != SVT_ATOM_NEWTUBE && iMode != SVT_ATOM_CARTOON) || ((iMode == SVT_ATOM_TUBE || iMode == SVT_ATOM_NEWTUBE || iMode == SVT_ATOM_CARTOON) && m_aAtoms[i].isCA()) )
			{
			    sprintf( pStr, "%i", m_aAtoms[i].getPDBIndex() );
			    svt_drawText( (((*this)[i].x()) * 1.0E-2), (((*this)[i].y()) * 1.0E-2), (((*this)[i].z()) * 1.0E-2), 1.0f, 1.0f, 1.0f, pStr );
			}
			++iNumAtomLabels;
			break;

		    case SVT_ATOM_LABEL_TYPE:

			if ( (iMode != SVT_ATOM_TUBE && iMode != SVT_ATOM_NEWTUBE && iMode != SVT_ATOM_CARTOON) || ((iMode == SVT_ATOM_TUBE || iMode == SVT_ATOM_NEWTUBE || iMode == SVT_ATOM_CARTOON) && m_aAtoms[i].isCA()) )
			{
			    sprintf( pStr, "%s", m_aAtoms[i].getName() );
			    svt_drawText( (((*this)[i].x()) * 1.0E-2), (((*this)[i].y()) * 1.0E-2), (((*this)[i].z()) * 1.0E-2), 1.0f, 1.0f, 1.0f, pStr );
			}
			++iNumAtomLabels;
			break;

		    case SVT_ATOM_LABEL_RESNAME:

			if (m_aAtoms[i].isCA())
			{
			    sprintf( pStr, "%s", m_aAtoms[i].getResname() );
			    svt_drawText( (((*this)[i].x()) * 1.0E-2), (((*this)[i].y()) * 1.0E-2), (((*this)[i].z()) * 1.0E-2), 1.0f, 1.0f, 1.0f, pStr );
			}
			++iNumAtomLabels;
			break;

		    case SVT_ATOM_LABEL_RESID:

			if (m_aAtoms[i].isCA())
			{
			    sprintf( pStr, "%i", m_aAtoms[i].getResidueSeq() );
			    svt_drawText( (((*this)[i].x()) * 1.0E-2), (((*this)[i].y()) * 1.0E-2), (((*this)[i].z()) * 1.0E-2), 1.0f, 1.0f, 1.0f, pStr );
			}
			++iNumAtomLabels;
			break;

		    case SVT_ATOM_LABEL_MODEL:

			if (m_aAtoms[i].isCA())
			{
			    sprintf( pStr, "%i", m_aAtoms[i].getModel() );
			    svt_drawText( (((*this)[i].x()) * 1.0E-2), (((*this)[i].y()) * 1.0E-2), (((*this)[i].z()) * 1.0E-2), 1.0f, 1.0f, 1.0f, pStr );
			}
			++iNumAtomLabels;
			break;

		    case SVT_ATOM_LABEL_CHAINID:

			if (m_aAtoms[i].isCA())
			{
			    sprintf( pStr, "%c", m_aAtoms[i].getChainID() );
			    svt_drawText( (((*this)[i].x()) * 1.0E-2), (((*this)[i].y()) * 1.0E-2), (((*this)[i].z()) * 1.0E-2), 1.0f, 1.0f, 1.0f, pStr );
			}
			++iNumAtomLabels;
			break;
		}

		if (iNumAtomLabels == 0) // if no labels were found, do not check for labels the next time when in outputGL(). Check only after rebuildDL() was called
		    m_bShowAtomLabel = false;
	    }
	    glPopMatrix();
	}
    }
};

/**
 * rebuild display list
 */
void svt_pdb::rebuildDL()
{
    //svtout << "DEBUG: rebuildDL called in "; printName();

    svt_node::rebuildDL();
    //m_bShowAtomLabel = true;
};

/**
 * draw the pdb data
 */
void svt_pdb::drawGL()
{
    prepareContainer();

    if (m_bDynamic)
    {
        float fElapsed = svt_getElapsedTime() - m_iSpeedTime;
        m_iSpeedTime = svt_getElapsedTime();
        m_fSpeedTS += fElapsed * (m_fSpeed / 1000);
        if (fabs(m_fSpeedTS) > 1)
        {
            setTimestep( m_iTimestep + (int)(m_fSpeedTS) );
            m_fSpeedTS = 0.0f;
        }
    }
}


/**
 * prepare container
 * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
 */
void svt_pdb::prepareContainer()
{
    //unsigned long iTime = svt_getToD();

    unsigned int i;
    toon_v4f_t4f_c4f_n3f_64byte oToonAtom;
    svt_vector4<Real64> oVecA;
    svt_vector4<Real64> oVecB;

    unsigned int iNumAtoms = m_aRenAtoms.size();
    bool bToonSupported = ( m_pToonCont->getToonSupported() && svt_getAllowShaderPrograms() );

    static vector<int>  aTubeAtoms;
    static vector<int>  aNewTubeCA;
    static vector<int>  aNewTubeNA;
    static vector<int>  aCartoonCA;
    static vector<char> aCartoonSS;

    aTubeAtoms.clear();
    aCartoonCA.clear();
    aCartoonSS.clear();
    aNewTubeCA.clear();
    aNewTubeNA.clear();

    m_pCont->delAll();
    m_pCartoonCont->delAll();
    m_pToonCont->delAll();
    m_pContainer_Surf->delAll();

    for(i=0; i<iNumAtoms; ++i)
    {
	switch(m_aRenAtoms[i].getDisplayMode())
	{
	    case SVT_ATOM_TUBE:
	    {	
		if (m_aRenAtoms[i].isCA())
		{
		    // distance & chain check - only if we already have some atoms in the list...
		    if (aTubeAtoms.size() > 0)
		    {
			// distance & chain check. tube creation, if distance too large or new chain begins
			oVecA = svt_vector4<Real64>( (*this)[aTubeAtoms.back()] );
			oVecB = svt_vector4<Real64>( (*this)[i] );

			if (oVecA.distance( oVecB ) > m_fTraceCutoff   ||   m_aRenAtoms[i].getChainID() != m_aRenAtoms[aTubeAtoms.back()].getChainID())
			{
			    // add the last atom twice
			    aTubeAtoms.push_back( aTubeAtoms.back() );
			    calcTube( aTubeAtoms );
			    aTubeAtoms.clear();
			}
		    }

		    aTubeAtoms.push_back( i );
		    // first atom has to be added twice
		    if (aTubeAtoms.size() == 1)
			aTubeAtoms.push_back( i );
		}
		break;
	    }
	    case SVT_ATOM_NEWTUBE:
	    {	    

		// for proteins, collect all C-alphas
		if (m_aRenAtoms[i].isCA())
		{
		    aNewTubeCA.push_back(i);
		    break;
		}


		// if this is a nucleic acid...
		//
		if (m_aRenAtoms[i].isNucleotide() &&
		    m_aRenAtoms[i].isBackbone() )
		{

		    // consider only the P, C3, and C5 atoms as backbone, so break for any other atom
		    if (!((m_aRenAtoms[i].getName()[0] == 'C' && m_aRenAtoms[i].getName()[1] == '3') ||
			  (m_aRenAtoms[i].getName()[0] == 'C' && m_aRenAtoms[i].getName()[1] == '5') ||
			  (m_aRenAtoms[i].getName()[0] == 'P' )))
			break;

		    aNewTubeNA.push_back( i );

		}
		break;
	    }
	    case SVT_ATOM_CARTOON:
	    {

		// proteins
		//
		if (m_aRenAtoms[i].isCA())
		{
		    aCartoonCA.push_back(i);

		    switch (m_aRenAtoms[i].getSecStruct())
		    {
			case 'H': case 'G': case 'I':
                        {
                            aCartoonSS.push_back('H');
                            break;
                        }
			case 'E': case 'B': case 'b':
                        {
                            // sheet geometry depends on the O atom in the peptide plane.
                            // if there is none available, render as coil

                            if (getBackboneO(i) == -1)
                                aCartoonSS.push_back('C');
                            else
                                aCartoonSS.push_back('S');

                            break;
                        }
			default:
                        {
                            aCartoonSS.push_back('C');
                            break;
                        }
		    }
		    break;
		}


		// if this is a nucleic acid...
		//
		if (m_aRenAtoms[i].isNucleotide() &&
		    m_aRenAtoms[i].isBackbone() )
		{

		    // consider only the P, C3, and C5 atoms as backbone, so break for any other atom
		    if (!((m_aRenAtoms[i].getName()[0] == 'C' && m_aRenAtoms[i].getName()[1] == '3') ||
			  (m_aRenAtoms[i].getName()[0] == 'C' && m_aRenAtoms[i].getName()[1] == '5') ||
			  (m_aRenAtoms[i].getName()[0] == 'P' )))
			break;

		    aNewTubeNA.push_back( i );

		}
		break;
	    }
            case SVT_ATOM_SURF:
            {
                m_pContainer_Surf->setPalette( m_aRenAtoms[i].getAtomColmap()->getPalette() );
                m_pContainer_Surf->addAtom( m_aRenAtoms[i].getAtomPos().x(),
                                            m_aRenAtoms[i].getAtomPos().y(),
                                            m_aRenAtoms[i].getAtomPos().z(),
                                            m_aRenAtoms[i].getVDWRadius(),
                                            m_aRenAtoms[i].getAtomColmap()->getColorIndex( &m_aRenAtoms[i] ) );
                break;
            }
	    case SVT_ATOM_VDW:
	    {
		if (m_iVDWRenderingMode == SVT_PDB_VDW_GLOSSY && bToonSupported)
		{
		    memcpy(oToonAtom.v, m_aRenAtoms[i].getGLPos().c_data(), 12);
		    oToonAtom.v[3]    = m_aRenAtoms[i].getVDWRadius() * m_aRenAtoms[i].getVDWRadiusScaling() * 0.01f;

		    memcpy(oToonAtom.c, m_aRenAtoms[i].getColor()->getDiffuse(), 16);

		    m_pToonCont->addAtom(&oToonAtom);
		}
		else
		    m_aRenAtoms[i].prepareContainer((*m_pCont));
		break;
	    }

	    default:
	    {
		m_aRenAtoms[i].prepareContainer((*m_pCont));
		break;
	    }
	}
    }


    //
    // do any tube atoms remain?
    //
    if (aTubeAtoms.size()>0)
    {
	aTubeAtoms.push_back( aTubeAtoms.back() );
	calcTube( aTubeAtoms );
	aTubeAtoms.clear();
    }


    //
    // newtube and cartoon atoms
    //
    if (aNewTubeCA.size() > 0)
	calcNewTube(&aNewTubeCA);

    if (aNewTubeNA.size() > 0)
	calcNewTube(&aNewTubeNA);

    if (aCartoonCA.size() > 0)
	calcCartoon(&aCartoonCA, &aCartoonSS);


    //
    // bonds
    //
    for(i=0; i<m_aRenBonds.size(); i++)
        m_aRenBonds[i].prepareContainer((*m_pCont));

    //
    // Surface
    //
    m_pContainer_Surf->prepareContainer();
    m_pCont->rebuildDL();
    m_pCartoonCont->rebuildDL();

    //printf("svt_pdb::prepareContainer() %lu\n", svt_getToD()-iTime);
}

///////////////////////////////////////////////////////////////////////////////
// internal functions
///////////////////////////////////////////////////////////////////////////////

/**
 * set the tube cylinder diameter scaling (default 2.0f)
 * \param fTubeDiameter tube diameter scaling value
 */
void svt_pdb::setTubeDiameter(float fTubeDiameter)
{
    m_fTubeDiameter = fTubeDiameter;
}
/**
 * get the tube cylinder diameter scaling (default 2.0f)
 * \return tube diameter scaling value
 */
float svt_pdb::getTubeDiameter()
{
    return m_fTubeDiameter;
}

/**
 * set the number of tube segments between two CA atoms (default: 6)
 * \param iTubeSegs number of tube segments between two atoms
 */
void svt_pdb::setTubeSegs(int iTubeSegs)
{
    if (iTubeSegs > 0)
        m_iTubeSegs = iTubeSegs;
}
/**
 * get the number of tube segments between two CA atoms (default: 6)
 * \return number of tube segments between two atoms
 */
int svt_pdb::getTubeSegs()
{
    return m_iTubeSegs;
}

///////////////////////////////////////////////////////////////////////////////
// Timesteps
///////////////////////////////////////////////////////////////////////////////

/**
 * set loop
 * \param iLoop SVT_DYNAMICS_ONCE, SVT_DYNAMICS_LOOP, SVT_DYNAMICS_ROCK
 */
void svt_pdb::setLoop( unsigned int iLoop )
{
    m_iLoop = iLoop;
};
/**
 * get loop
 * \return SVT_DYNAMICS_ONCE, SVT_DYNAMICS_LOOP, SVT_DYNAMICS_ROCK
 */
unsigned int svt_pdb::getLoop()
{
    return m_iLoop;
};


/**
 * set speed
 * \param fSpeed timesteps per second
 */
void svt_pdb::setSpeed(float fSpeed)
{
    m_fSpeed = fSpeed;
    m_fSpeedTS = 0.0f;
    m_iSpeedTime = svt_getElapsedTime();
}
/**
 * get speed
 * \return timesteps per second
 */
float svt_pdb::getSpeed()
{
    return m_fSpeed;
}

/**
 * print name of scenegraph node to stdout
 */
void svt_pdb::printName()
{
    cout << "PDB with " << m_aAtoms.size() << " atoms" << "(" << m_iNodeID << ")" << endl;
};

/**
 * check if toons are supported
 * \return true if toons are supported, false otherwise
 */
bool svt_pdb::getToonSupported()
{
    return m_pToonCont->getToonSupported();
}

/**
 *
 */
void svt_pdb::setAllowShaderPrograms()
{
    if ( svt_getAllowShaderPrograms() )
    {

        // checking shader support will also init the shaders in case it didn't happen before
        m_pCartoonCont_Dynamic->checkShaderSupport();
        m_pToonCont->checkShaderSupport();


        // activate cartoon shader programs. toons are handled in prepareContainer()
        if ( (bool)getCartoonParam(CARTOON_USE_SHADER) )
        {
            if ( m_pCartoonCont_Dynamic->checkShaderSupport() )
            {
                m_pCartoonCont = m_pCartoonCont_Dynamic;
                m_pCartoonCont_Dynamic->setVisible(true);
                m_pCartoonCont_Static ->setVisible(false);
            }
        }
        if (getVDWRenderingMode() == SVT_PDB_VDW_GLOSSY)
            m_pToonCont->setVisible(true);
    }
    else
    {
	m_pCartoonCont = m_pCartoonCont_Static;
	m_pCartoonCont_Dynamic->setVisible(false);
	m_pCartoonCont_Static ->setVisible(true);
    }

    rebuildDL();
}

///////////////////////////////////////////////////////////////////////////////
// Solvent accessible surfaces
///////////////////////////////////////////////////////////////////////////////

/**
 * Set the radius of the probe sphere for the solvent accessible surfaces (default: 1.4A)
 * \param fProbeRadius radius of probe sphere
 */
void svt_pdb::setProbeRadius( Real64 fProbeRadius )
{
    m_pContainer_Surf->setProbeRadius( fProbeRadius );
    rebuildDL();
};
/**
 * Get the radius of the probe sphere for the solvent accessible surfaces (default: 1.4A)
 * \return radius of probe sphere
 */
Real64 svt_pdb::getProbeRadius( )
{
    return m_pContainer_Surf->getProbeRadius( );
};
