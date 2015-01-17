/***************************************************************************
                          svt_bond.h
			  ----------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BOND_H
#define SVT_BOND_H

#include <svt_core.h>
#include <svt_types.h>
#include <svt_vector4.h>
#include <svt_pos.h>
#include <svt_structs.h>
#include <svt_atom_colmap.h>
#include <svt_point_cloud_bond.h>

class svt_container;
class svt_atom;

enum {
    SVT_BOND_OFF,
    SVT_BOND_LINE,
    SVT_BOND_CYLINDER,
    SVT_BOND_LICORIZE
};

/**
 * A bond class (bond between two atoms).
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_bond : public svt_point_cloud_bond
{

protected:

    // how should this bond be displayed
    Int16  m_iDisplayMode;
    // cylinder
    Real32 m_fCylLength;
    Real32 m_fCylLengthF2; // 1/2 cyllength
    Real32 m_fCylUndo;
    Real32 m_fCylRotAngle;
    svt_vector4f m_oCylRotVec;
    Real32 m_fCylRad;
    // licorize radius
    Real32 m_fLicorizeRad;
    Real32 m_fLicRad;
    Real32 m_fLicUndo;
    // colormapping
    svt_atom_colmap* m_pMapping;

public:

    /**
     * Constructor
     * create an bond between atom a and b
     * \param pA pointer to first svt_atom object
     * \param pB pointer to second svt_atom object
     * \param pMapping pointer to a svt_atom_colmap object
     */
    svt_bond(svt_atom* pA=NULL, svt_atom* pB=NULL, svt_atom_colmap* pMapping = NULL);
    /**
     * Copy-Constructor
     * create an bond between atom a and b
     * \param reference to svt_point_cloud_bond object
     * \param pMapping pointer to a svt_atom_colmap object
     */
    svt_bond(svt_point_cloud_bond& rBond, svt_atom_colmap* pMapping = NULL);
    /**
     * Destructor
     */
    ~svt_bond();

    /**
     * set the display mode for this bond
     * \param iDisplayMode
     */
    void setDisplayMode(Int16 iDisplayMode);
    /**
     * get the display mode for this bond
     * \return the display mode
     */
    Int16 getDisplayMode();

    /**
     * set the radius for the licorize drawing mode
     * \param fLicorizeRad radius for the licorize mode
     */
    void setLicorizeRad(Real32 fLicorizeRad);
    /**
     * get the radius for the licorize drawing mode
     * \return radius for the licorize mode
     */
    Real32 getLicorizeRad();

    /**
     * set the radius for the cylinder drawing mode
     * \param fCylRad radius for the cylinder mode
     */
    void setCylinderRad(Real32 fCylRad);
    /**
     * get the radius for the cylinder drawing mode
     * \return radius for the cylinder mode
     */
    Real32 getCylinderRad();

    /**
     * set the svt_atom_colmap object, responsible for the colormapping
     * \param pMapping pointer to the svt_atom_colmap object
     */
    void setAtomColmap(svt_atom_colmap* pMapping);

    /**
     * prepare container
     * \param m_rCont reference to the svt_container
     * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
     */
    void prepareContainer(svt_container& m_rCont);
};

#endif
