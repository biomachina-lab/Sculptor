/***************************************************************************
                          svt_container_surf
			  -------
    begin                : 10/07/2010
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#define EXTERN
#include <svt_container_surf.h>
#include <surf.h>
#include <chull.h>
#include <svt_atom_type_colmap.h>

extern void init_and_compute();
extern void meta_clean();

svt_container_surf* g_pSurf = NULL;

/**
 * Constructor
 */
svt_container_surf::svt_container_surf( ) : m_fProbeRadius( 1.4 ),
    m_bChanged( true ),
    m_pPalette( NULL )
{
    m_pCont = new svt_container( NULL, NULL );
    m_pCont->setName("(svt_container_surf)m_pCont");
    m_pProp = new svt_properties();
    m_pProp->setTwoSided( true );
    m_pCont->setProperties( m_pProp );
    this->add(m_pCont);
};

/**
 * Destructor
 */
svt_container_surf::~svt_container_surf()
{
    g_pSurf = NULL;
};

/**
 * Add an atom to the container
 * \param fX x coordinate
 * \param fY y coordinate
 * \param fZ z coordinate
 * \param fRadius vdw radius of atom
 * \param iType atom type
 */
void svt_container_surf::addAtom( Real64 fX, Real64 fY, Real64 fZ, Real64 fRadius, int iType )
{
    Gp_Atom oAtom;

    oAtom.radius = fRadius;
    oAtom.center[X] = fX;
    oAtom.center[Y] = fY;
    oAtom.center[Z] = fZ;
    oAtom.type = (short)iType;

    m_aAtoms.push_back( oAtom );
}

/**
 * Set the palette. For now, the atom type will directly be used to index the palette.
 * \param pPalette pointer to svt_palette object
 */
void svt_container_surf::setPalette( svt_palette* pPalette )
{
    m_pPalette = pPalette;
};

/**
 * Compute triangles
 */
void svt_container_surf::prepareContainer()
{
    if (m_aAtoms.size() == 0)
        return;

    Checks_On = FALSE;
    Write_Option = 1;
    Num_atoms = 0;
    Probe_radius = -1;
    Max_Tess_Len = -1;
    Max_Gp_Polys = 0;

    /**
     * Copy all atoms
     */
    atoms = &m_aAtoms[0];
    Num_atoms = m_aAtoms.size();

    /**
     * Set probe radius
     */
    Probe_radius = m_fProbeRadius;

    /**
     * Initialize triangle container
     */
    m_pCont->delAll();
    m_pCont->beginTriangles();
    if (Write_Option == 1)
        g_pSurf = this;

    /**
     * Initialize and compute the molecular surface
     */
    init_and_compute();

    /**
     * Pump the triangles into the container
     */
    if (Write_Option == 2)
    {
        for(int i = 0; i < Num_polys; i++)
            addTriangle( verts[i][0], verts[i][1], verts[i][2], atom_type[i] );
    }

    /**
     * OK, we are done now
     */
    m_pCont->endTriangles();
    if (Write_Option == 1)
        g_pSurf = NULL;

    /**
     * Clean up
     */
    meta_clean();
    free( atom_type );
};

/**
 * Delete all data (triangles, atoms, etc)
 */
void svt_container_surf::delAll()
{
    m_aAtoms.clear();
    m_pCont->delAll();
    m_pCont->rebuildDL();
};

/**
 * Set the probe radius
 * \param pRadius the radius of the probe
 */
void svt_container_surf::setProbeRadius( Real64 fProbeRadius )
{
    if (fProbeRadius != m_fProbeRadius)
    {
        m_fProbeRadius = fProbeRadius;
        m_bChanged = true;
    }
};
/**
 * Get the probe radius
 * \return the radius of the probe
 */
Real64 svt_container_surf::getProbeRadius()
{
    return m_fProbeRadius;
};

/**
 * Add a triangle to the internal container
 */
void svt_container_surf::addTriangle( VertexType oV1, VertexType oV2, VertexType oV3, int iAtomNum )
{
    // prepare svt vertex
    svt_c4ub_n3f_v3f p3D;
    p3D.m_oC[0] = 255.0f;
    p3D.m_oC[1] =   0.0f;
    p3D.m_oC[2] =   0.0f;
    p3D.m_oC[3] = 255.0f;
    if (m_pPalette)
    {
        svt_color* pColor = m_pPalette->getColor( m_aAtoms[iAtomNum].type );
        if (pColor)
        {
            p3D.m_oC[0] = pColor->getR() * 255.0;
            p3D.m_oC[1] = pColor->getG() * 255.0;
            p3D.m_oC[2] = pColor->getB() * 255.0;
        }
    }

    // add first vertex
    p3D.setV( oV1.Coord[X] * 1.0E-2,  oV1.Coord[Y] * 1.0E-2,  oV1.Coord[Z] * 1.0E-2 );
    p3D.setN( oV1.Normal[X],          oV1.Normal[Y],          oV1.Normal[Z] );
    m_pCont->addVertex( p3D );
    // add second vertex
    p3D.setV( oV2.Coord[X] * 0.01,  oV2.Coord[Y] * 0.01,  oV2.Coord[Z] * 0.01 );
    p3D.setN( oV2.Normal[X],        oV2.Normal[Y],        oV2.Normal[Z] );
    m_pCont->addVertex( p3D );
    // add first vertex
    p3D.setV( oV3.Coord[X] * 0.01,  oV3.Coord[Y] * 0.01,  oV3.Coord[Z] * 0.01 );
    p3D.setN( oV3.Normal[X],        oV3.Normal[Y],        oV3.Normal[Z] );
    m_pCont->addVertex( p3D );
};
