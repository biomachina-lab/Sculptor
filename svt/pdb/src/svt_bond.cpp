/***************************************************************************
                          svt_bond.h
			  ----------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_bond.h>
#include <svt_atom.h>
#include <svt_atom_colmap.h>
#include <svt_container.h>

///////////////////////////////////////////////////////////////////////////////
// member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * create an bond between atom a and b
 * \param pA pointer to first svt_atom object
 * \param pB pointer to second svt_atom object
 * \param pMapping pointer to a svt_atom_colmap object
 */
svt_bond::svt_bond(svt_atom* pA, svt_atom* pB, svt_atom_colmap* pMapping) : svt_point_cloud_bond( pA, pB ),
    m_fCylRad( 1.0 )
{
    setAtomColmap(pMapping);
    setDisplayMode(SVT_BOND_LINE);
    setLicorizeRad(0.002f);
}
/**
 * Copy-Constructor
 * create an bond between atom a and b
 * \param reference to svt_point_cloud_bond object
 * \param pMapping pointer to a svt_atom_colmap object
 */
svt_bond::svt_bond(svt_point_cloud_bond& rBond, svt_atom_colmap* pMapping) : svt_point_cloud_bond( rBond ),
    m_fCylRad( 1.0 )
{
    setAtomColmap(pMapping);
    setDisplayMode(SVT_BOND_LINE);
    setLicorizeRad(0.002f);
}
/**
 * Destructor
 */
svt_bond::~svt_bond()
{
}

/**
 * set the display mode for this bond
 * \param iDisplayMode
 */
void svt_bond::setDisplayMode(Int16 iDisplayMode)
{
    m_iDisplayMode = iDisplayMode;
}
/**
 * get the display mode for this bond
 * \return the display mode
 */
Int16 svt_bond::getDisplayMode()
{
    return m_iDisplayMode;
}

/**
 * set the radius for the licorize drawing mode
 * \param fLicorizeRad radius for the licorize mode
 */
void svt_bond::setLicorizeRad(Real32 fLicorizeRad)
{
    m_fLicorizeRad = fLicorizeRad;

    m_fLicRad = (m_fLicorizeRad / 0.001f) + 0.0001f;
    m_fLicUndo = 1 / m_fLicRad;
}
/**
 * get the radius for the licorize drawing mode
 * \return radius for the licorize mode
 */
Real32 svt_bond::getLicorizeRad()
{
    return m_fLicorizeRad;
}

/**
 * set the radius for the cylinder drawing mode
 * \param fCylRad radius for the cylinder mode
 */
void svt_bond::setCylinderRad(Real32 fCylRad)
{
    m_fCylRad = fCylRad;
}
/**
 * get the radius for the cylinder drawing mode
 * \return radius for the cylinder mode
 */
Real32 svt_bond::getCylinderRad()
{
    return m_fCylRad;
}

/**
 * set the svt_atom_colmap object, responsible for the colormapping
 * \param pMapping pointer to the svt_atom_colmap object
 */
void svt_bond::setAtomColmap(svt_atom_colmap* pMapping)
{
    m_pMapping = pMapping;
}

/**
 * prepare container
 * \param m_rCont reference to the svt_container
 * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
 */
void svt_bond::prepareContainer(svt_container& m_rCont)
{
    if (m_iDisplayMode != SVT_BOND_LINE && m_iDisplayMode != SVT_BOND_CYLINDER && m_iDisplayMode != SVT_BOND_LICORIZE )
        return;

    svt_atom* pAtomA = (svt_atom*)m_pAtomA;
    svt_atom* pAtomB = (svt_atom*)m_pAtomB;

    if (pAtomA == NULL || pAtomB == NULL || pAtomA->getDisplayMode() == SVT_ATOM_OFF || pAtomB->getDisplayMode() == SVT_ATOM_OFF)
        return;

    svt_c3f_v3f oVecA, oVecB, oMidA, oMidB;
    svt_vector4<Real32> oMidPt;

    switch(m_iDisplayMode)
    {
    case SVT_BOND_LINE:

	oVecA[0] = m_pMapping->getColor(pAtomA)->getR();
	oVecA[1] = m_pMapping->getColor(pAtomA)->getG();
	oVecA[2] = m_pMapping->getColor(pAtomA)->getB();
	oVecA[3] = pAtomA->getGLPos().x();
	oVecA[4] = pAtomA->getGLPos().y();
	oVecA[5] = pAtomA->getGLPos().z();

	oVecB[0] = m_pMapping->getColor(pAtomB)->getR();
	oVecB[1] = m_pMapping->getColor(pAtomB)->getG();
	oVecB[2] = m_pMapping->getColor(pAtomB)->getB();
	oVecB[3] = pAtomB->getGLPos().x();
	oVecB[4] = pAtomB->getGLPos().y();
	oVecB[5] = pAtomB->getGLPos().z();

	oMidA[0] = oVecA[0];
	oMidA[1] = oVecA[1];
	oMidA[2] = oVecA[2];
	oMidA[3] = ( (pAtomA->getGLPos().x() + pAtomB->getGLPos().x()) / 2 );
	oMidA[4] = ( (pAtomA->getGLPos().y() + pAtomB->getGLPos().y()) / 2 );
	oMidA[5] = ( (pAtomA->getGLPos().z() + pAtomB->getGLPos().z()) / 2 );

	oMidB[0] = oVecB[0];
	oMidB[1] = oVecB[1];
	oMidB[2] = oVecB[2];
	oMidB[3] = oMidA[3];
	oMidB[4] = oMidA[4];
	oMidB[5] = oMidA[5];

	m_rCont.addLine( oVecA, oMidA );
	m_rCont.addLine( oMidB, oVecB );
        break;

    case SVT_BOND_CYLINDER:

	oMidPt = ( (pAtomA->getGLPos() + pAtomB->getGLPos()) * 0.5f );

        m_rCont.addCylinder( pAtomA->getGLPos(),
			     oMidPt,
			     m_fCylRad * 0.001f,
			     *m_pMapping->getColor(pAtomA)
			   );
	m_rCont.addCylinder( oMidPt,
			     pAtomB->getGLPos(),
			     m_fCylRad * 0.001f,
			     *m_pMapping->getColor(pAtomB)
			   );
        break;

    case SVT_BOND_LICORIZE:

	oMidPt = ( (pAtomA->getGLPos() + pAtomB->getGLPos()) * 0.5f );

        m_rCont.addCylinder( pAtomA->getGLPos(),
			     oMidPt,
			     m_fLicRad * 0.001f,
			     *m_pMapping->getColor(pAtomA)
			   );
	m_rCont.addCylinder( oMidPt,
			     pAtomB->getGLPos(),
			     m_fLicRad * 0.001f,
			     *m_pMapping->getColor(pAtomB)
			   );
        break;

    default:
        break;
    }
}
