/***************************************************************************
                          svt_container_surf
			  ------------------
    begin                : 10/07/2010
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SURF_H
#define SVT_SURF_H

#include <satom.h>
#include <svt_basics.h>
#include <svt_container.h>
#include <svt_palette.h>

#define SURFOUT cout << svt_trimName("svt_container_surf")

/**
 * A special container for the surface accessible surfaces.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_container_surf : public svt_node
{
protected:

    svt_container* m_pCont;
    Real64 m_fProbeRadius;
    bool m_bChanged;
    vector<Gp_Atom> m_aAtoms;
    svt_palette* m_pPalette;
    svt_properties* m_pProp;

public:

    /**
     * Constructor
     */
    svt_container_surf( );

    /**
     * Destructor
     */
    virtual ~svt_container_surf();

    /**
     * Add an atom to the container
     * \param fX x coordinate
     * \param fY y coordinate
     * \param fZ z coordinate
     * \param fRadius vdw radius of atom
     * \param iType atom type
     */
    void addAtom( Real64 fX, Real64 fY, Real64 fZ, Real64 fRadius, int iType );

    /**
     * Set the palette. For now, the atom type will directly be used to index the palette.
     * \param pPalette pointer to svt_palette object
     */
    void setPalette( svt_palette* pPalette );

    /**
     * Compute triangles
     */
    void prepareContainer();

    /**
     * Delete all data (triangles, atoms, etc)
     */
    void delAll();

    /**
     * Set the probe radius
     * \param pRadius the radius of the probe
     */
    void setProbeRadius( Real64 fProbeRadius );
    /**
     * Get the probe radius
     * \return the radius of the probe
     */
    Real64 getProbeRadius();

    /**
     * Add a triangle to the internal container
     */
    void addTriangle( VertexType oV1, VertexType oV2, VertexType oV3, int iAtomType );
};

#endif
