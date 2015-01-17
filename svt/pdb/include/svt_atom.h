/***************************************************************************
                          svt_atom.h
			  ----------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ATOM_H
#define SVT_ATOM_H

// svt includes
#include <svt_core.h>
#include <svt_types.h>
#include <svt_color.h>
#include <svt_atom_colmap.h>
#include <svt_array.h>
#include <svt_vector4.h>
#include <svt_point_cloud_pdb.h>
#include <svt_point_cloud_atom.h>

class svt_container;

enum {
    SVT_ATOM_OFF,
    SVT_ATOM_CPK,
    SVT_ATOM_POINT,
    SVT_ATOM_VDW,
    SVT_ATOM_DOTTED,
    SVT_ATOM_LICORIZE,
    SVT_ATOM_TUBE,
    SVT_ATOM_CARTOON,
    SVT_ATOM_NEWTUBE,
    SVT_ATOM_TOON,
    SVT_ATOM_SURF
};

enum {
    SVT_ATOM_LABEL_OFF,
    SVT_ATOM_LABEL_INDEX,
    SVT_ATOM_LABEL_TYPE,
    SVT_ATOM_LABEL_RESNAME,
    SVT_ATOM_LABEL_RESID,
    SVT_ATOM_LABEL_MODEL,
    SVT_ATOM_LABEL_CHAINID
};

/** An atom rendering class.
  *@author Stefan Birmanns
  */

class DLLEXPORT svt_atom : public svt_point_cloud_atom
{

protected:

    // index of the atom in the larger pointcloud that encapsulates the entire molecule
    unsigned int m_iPoint;
    // pointer to the molecule
    svt_point_cloud_pdb< svt_vector4<Real64> >& m_rPDB;

    // atom van der waals radius
    Real32 m_fVDWRadGL;
    Real32 m_fVDWRadGLScale;
    Real32 m_fCPKRadGLScale;

    // how should this atom be displayed
    Int16  m_iDisplayMode;
    // licorize radius
    Real32 m_fLicorizeRad;
    // colormapping
    svt_atom_colmap* m_pMapping;
    // labeling
    Int16 m_iLabelMode;

public:

    /**
     * Constructor
     * \param pMapping pointer to svt_atom_colmap object
     */
    //svt_atom( svt_atom_colmap* pMapping=NULL );
    /**
     * Copy Constructor
     * \param reference to position vector (in angstroem)
     * \param reference to svt_point_cloud_atom object
     * \param pointer to svt_atom_colmap object
     */
    //svt_atom(svt_vector4<Real64>& rVec, svt_point_cloud_atom& rAtom, svt_atom_colmap* pMapping=NULL);

    /**
     * Constructor
     * \param iIndex index to atom in point cloud
     * \param reference to svt_point_cloud_pdb object
     * \param reference to svt_point_cloud_atom object
     * \param pointer to svt_atom_colmap object
     */
    svt_atom(unsigned int iIndex, svt_point_cloud_pdb< svt_vector4<Real64> >& rPDB, svt_point_cloud_atom& rAtom, svt_atom_colmap* pMapping=NULL);

    /**
     * Copy Constructor
     */
    svt_atom( const svt_atom& rOther );

    svt_atom& operator=(const svt_atom& that);

    /**
     * Destructor
     */
    virtual ~svt_atom();

    /**
     * set the physical atom position
     * \param oVec vector with the position
     */
    void setAtomPos(svt_vector4<Real64> oVec);
    /**
     * get the physical atom position
     * \return svt_vector4 with the position
     */
    svt_vector4<Real64> getAtomPos();

    /**
     * get the opengl position
     * \return svt_vector4 with the position
     */
    svt_vector4<Real32> getGLPos();

    /**
     * set the display mode for this atom
     * \param iDisplayMode display mode
     */
    void setDisplayMode(Int16 iDisplayMode);
    /**
     * get the display mode for this atom
     * \return display mode
     */
    Int16 getDisplayMode();

    /**
     * set the label mode for this atom
     * \param iLabelMode display mode
     */
    void setLabelMode(Int16 iLabelMode);
    /**
     * get the label mode for this atom
     * \return label mode
     */
    Int16 getLabelMode();

    /**
     * set the vdw radius scaling (default: 1.0f)
     * /param fVDWRadGLScale scaling factor for the vdw radius
     */
    void setVDWRadiusScaling(Real32 fVDWRadGLScale);
    /**
     * get the vdw radius scaling (default: 1.0f)
     * /return scaling factor for the vdw radius
     */
    Real32 getVDWRadiusScaling();
    /**
     * set the cpk radius scaling (default: 0.2f)
     * \param fCPKRadGLScale scaling factor for atom radius in the cpk mode
     */
    void setCPKRadiusScaling(Real32 fCPKRadGLScale);
    /**
     * get the cpk radius scaling (default: 0.2f)
     * \return scaling factor for atom radius in the cpk mode
     */
    Real32 getCPKRadiusScaling();

    /**
     * set the radius for the licorize drawing mode
     * \param fLicorizeRad radius for the licorize drawing mode
     */
    void setLicorizeRad(Real32 fLicorizeRad);
    /**
     * get the radius for the licorize drawing mode
     * \return radius for the licorize drawing mode
     */
    Real32 getLicorizeRad();

    /**
     * set the color mapping object
     * \param pMapping pointer to the svt_atom_colmap object
     */
    void setAtomColmap(svt_atom_colmap* pMapping);
    /**
     * get the color mapping object
     * \return pointer to the svt_atom_colmap object
     */
    svt_atom_colmap* getAtomColmap() const;
    /**
     * get the color
     * \return pointer to svt_color object
     */
    svt_color* getColor();

    /**
     * prepare container
     * \param m_rCont reference to the svt_container
     * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
     */
    void prepareContainer(svt_container& m_rCont);
};

#endif
