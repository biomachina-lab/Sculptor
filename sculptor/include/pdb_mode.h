/***************************************************************************
                          pdb_mode.h  -  description
                             -------------------
    begin                : 15.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef PDB_MODE_H
#define PDB_MODE_H

#include <svt_basics.h>
#include <svt_types.h>

class svt_atom_colmap;

enum {
    GM_OFF,
    GM_POINT,
    GM_LINE,
    GM_CPK,
    GM_VDW,
    GM_DOTTED,
    GM_LICORIZE,
    GM_TUBE,
    GM_NEWTUBE,
    GM_CARTOON,
    GM_SURF,
    GM_TOON
};

enum {
    MODE_ALL,
    MODE_INDEX,
    MODE_TYPE,
    MODE_RESNAME,
    MODE_RESSEQ,
    MODE_CHAINID,
    MODE_SEGID,
    MODE_MODEL,
    MODE_NUCLEOTIDE,
    MODE_HET,
    MODE_WATER
};

enum {
    COLMAP_TYPE,
    COLMAP_RESNAME,
    COLMAP_CHAINID,
    COLMAP_SEGID,
    COLMAP_SOLID,
    COLMAP_STRUCTURE,
    COLMAP_MODEL,
    COLMAP_BETA
};

enum {
    LABEL_OFF,
    LABEL_INDEX,
    LABEL_TYPE,
    LABEL_RESNAME,
    LABEL_RESID,
    LABEL_MODEL,
    LABEL_CHAINID
};

/** This class encapsulates a pdb drawing mode
 * @author Stefan Birmanns
 */
class pdb_mode
{
protected:
    // selection mode (all atoms, only the atoms of a certain chain, index, etc.)
    int m_iSelMode;
    // the selected atom name, or chain id, or the selected "from" value
    int m_iSelName;
    // selected "to" limit
    int m_iSelTo;
    // the selected graphics mode
    int m_iGraphicsMode;
    // the sphere scaling
    float m_fSphereScale;
    float m_fSphereScale_CPK;
    float m_fSphereScale_NonCPK;
    // the tube diameter
    float m_fTubeDiameter;
    // the number of tube segments
    int m_iTubeSegments;
    // the number of tube profile corners
    int m_iTubeProfileCorners;
    // the color mapping type
    int m_iColmapType;
    // pointer to the actual color mapping object
    svt_atom_colmap* m_pAtomColmap;
    // the label mode
    int m_iLabelMode;
    // probe radius
    Real64 m_fProbeRadius;

public:
    /**
     * Constructor
     * \param iSelMode the selection mode (e.g. MODE_ALL)
     * \param iSelName the selection name (e.g. C, but here as an index to the list of all available names in the pdb file)
     * \param iGraphicsMode the graphics mode (e.g. GM_LINE)
     * \param iColmapType the colormapping type (e.g. COLMAP_RESNAME)
     * \param iLabelMode the text label mode
     */
    pdb_mode(int iSelMode =MODE_ALL, int iSelName =0, int iGraphicsMode =GM_LINE, int iColmapType =COLMAP_TYPE, int iLabelMode =LABEL_OFF);

    /**
     * get the selection mode
     * \return the selection mode (e.g. MODE_ALL)
     */
    int getSelectionMode();
    /**
     * set the selection mode
     * \param iSelMode the selection mode (e.g. MODE_ALL)
     */
    void setSelectionMode(int iSelMode);

    /**
     * get the selection name.
     * \return an index to the array of all available names (svt_pdb provides this, e.g. getAtomTypes())
     */
    int getSelectionName();
    /**
     * set the selection name
     * \param iSelName an index to the selected name
     */
    void setSelectionName(int iSelName);

    /**
     * get the selection "to" limit
     * \return the selection "to" limit
     */
    int getSelectionTo();
    /**
     * get the selection "to" limit
     * \param iTo the selection "to" limit
     */
    void setSelectionTo(int iTo);

    /**
     * get the graphics mode
     * \return the graphics mode (e.g. GM_LINE)
     */
    int getGraphicsMode();
    /**
     * set the graphics mode
     * \param iGraphicsMode the graphics mode (e.g. GM_LINE)
     */
    void setGraphicsMode(int iGraphicsMode);

    /**
     * get the colormapping type
     * \return the colormapping type (e.g. COLMAP_RESNAME)
     */
    int getColmapType();
    /**
     * set the colormapping type
     * \param iColmapType the colormapping type (e.g. COLMAP_RESNAME)
     */
    void setColmapType(int iColmapType);

    /**
     * get the text label mode
     * \return the text label mode (e.g. LABEL_INDEX)
     */
    int getLabelMode();
    /**
     * set the text label mode
     * \param iLabelMode the text label mode (e.g. LABEL_INDEX)
     */
    void setLabelMode(int iLabelMode);

    /**
     * get the sphere scaling
     * \return iSphereScale the scaling of the spheres
     */
    float getSphereScale();
    /**
     * set the sphere scaling
     * \param fSphereScale the scaling of the sphere (according to the VDW radius of the atoms. In the GM_VDW graphics mode the default sphere scaling factor is 1.0f, in GM_CPK it is 0.2f)
     */
    void setSphereScale(float fSphereScale);

    /**
     * get the tube diameter
     * \return the tube diameter scaling value
     */
    float getTubeDiameter();
    /**
     * set the tube diameter
     * \param fTubeDiameter the tube diameter scaling value
     */
    void setTubeDiameter(float fTubeDiameter);
    /**
     * get the number of tube segments
     * \return the number of tube segments
     */
    int getTubeSegments();
    /**
     * set the number of tub segments
     * \param iTubeSegments the number of segments
     */
    void setTubeSegments(int iTubeSegments);
    /**
     * get the number of tube profile corners
     * \return the number of tube profile corners
     */
    int getTubeProfileCorners();
    /**
     * set the number of tub profile corners
     * \param iTubeSegments the number of tube profile corners
     */
    void setTubeProfileCorners(int iTubeProfileCorners);

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
};

#endif
