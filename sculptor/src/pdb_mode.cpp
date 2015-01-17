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

#include <pdb_mode.h>

#include <svt_atom_colmap.h>

/**
 * Constructor
 * \param iSelMode the selection mode (e.g. MODE_ALL)
 * \param iSelName the selection name (e.g. C, but here as an index to the list of all available names in the pdb file)
 * \param iGraphicsMode the graphics mode (e.g. GM_LINE)
 * \param iColmapType the colormapping type (e.g. COLMAP_RESNAME)
 * \param iLabelMode the text label mode
 */
pdb_mode::pdb_mode(int iSelMode, int iSelName, int iGraphicsMode, int iColmapType, int iLabelMode)
{
    m_iSelMode = iSelMode;
    m_iSelName = iSelName;
    m_iSelTo = 0;
    m_iGraphicsMode = iGraphicsMode;
    m_iColmapType = iColmapType;
    m_pAtomColmap = NULL;
    m_fTubeDiameter = 2.0f;
    m_iTubeSegments = 4;
    m_iTubeProfileCorners = 5;
    m_iLabelMode = iLabelMode;

    m_fSphereScale_CPK = 0.2f;
    m_fSphereScale_NonCPK = 1.0f;

    switch(m_iGraphicsMode)
    {
	case GM_CPK:
	    m_fSphereScale = 0.2f;
	    break;
	default:
	    m_fSphereScale = 1.0f;
	    break;
    }

    m_fProbeRadius = 1.4;
}

/**
 * get the selection mode
 * \return the selection mode (e.g. MODE_ALL)
 */
int pdb_mode::getSelectionMode()
{
    return m_iSelMode;
}
/**
 * set the selection mode
 * \param iSelMode the selection mode (e.g. MODE_ALL)
 */
void pdb_mode::setSelectionMode(int iSelMode)
{
    m_iSelMode = iSelMode;
}

/**
 * get the selection name.
 * \return an index to the array of all available names (svt_pdb provides this, e.g. getAtomTypes())
 */
int pdb_mode::getSelectionName()
{
    return m_iSelName;
}

/**
 * set the selection name
 * \param iSelName an index to the selected name
 */
void pdb_mode::setSelectionName(int iSelName)
{
    m_iSelName = iSelName;
}

/**
 * get the selection "to" limit
 * \return the selection "to" limit
 */
int pdb_mode::getSelectionTo()
{
    return m_iSelTo;
}
/**
 * get the selection "to" limit
 * \param iTo the selection "to" limit
 */
void pdb_mode::setSelectionTo(int iSelTo)
{
    m_iSelTo = iSelTo;
}

/**
 * get the graphics mode
 * \return the graphics mode (e.g. GM_LINE)
 */
int pdb_mode::getGraphicsMode()
{
    return m_iGraphicsMode;
}
/**
 * set the graphics mode
 * \param iGraphicsMode the graphics mode (e.g. GM_LINE)
 */
void pdb_mode::setGraphicsMode(int iGraphicsMode)
{
    m_iGraphicsMode = iGraphicsMode;

    switch(m_iGraphicsMode)
    {
	case GM_CPK:
	    m_fSphereScale = m_fSphereScale_CPK;
	    break;
	default:
	    m_fSphereScale = m_fSphereScale_NonCPK;
	    break;
    }
}

/**
 * get the colormapping type
 * \return the colormapping type (e.g. COLMAP_RESNAME)
 */
int pdb_mode::getColmapType()
{
    return m_iColmapType;
}
/**
 * set the colormapping type
 * \param iColmapType the colormapping type (e.g. COLMAP_RESNAME)
 */
void pdb_mode::setColmapType(int iColmapType)
{
    m_iColmapType = iColmapType;
}

/**
 * get the text label mode
 * \return the text label mode (e.g. LABEL_INDEX)
 */
int pdb_mode::getLabelMode()
{
    return m_iLabelMode;
};
/**
 * set the text label mode
 * \param iLabelMode the text label mode (e.g. LABEL_INDEX)
 */
void pdb_mode::setLabelMode(int iLabelMode)
{
    m_iLabelMode = iLabelMode;
};

/**
 * get the sphere scaling
 * \return iSphereScale the scaling of the spheres
 */
float pdb_mode::getSphereScale()
{
    switch (m_iGraphicsMode)
    {
	case GM_CPK:
	    return m_fSphereScale_CPK;
	    break;
	default:
	    return m_fSphereScale_NonCPK;
	    break;
    }
}
/**
 * set the sphere scaling
 * \param fSphereScale the scaling of the sphere (according to the VDW radius of the atoms. In the GM_VDW graphics mode the default sphere scaling factor is 1.0f, in GM_CPK it is 0.2f)
 */
void pdb_mode::setSphereScale(float fSphereScale)
{
    switch (m_iGraphicsMode)
    {
	case GM_CPK:
	    m_fSphereScale_CPK = fSphereScale;
	    break;
	default:
	    m_fSphereScale_NonCPK = fSphereScale;
	    break;
    }
}

/**
 * get the tube diameter
 * \return the tube diameter scaling value
 */
float pdb_mode::getTubeDiameter()
{
    return m_fTubeDiameter;
}
/**
 * set the tube diameter
 * \param fTubeDiameter the tube diameter scaling value
 */
void pdb_mode::setTubeDiameter(float fTubeDiameter)
{
    m_fTubeDiameter = fTubeDiameter;
}
int pdb_mode::getTubeSegments()
{
    return m_iTubeSegments;
}
/**
 * set the number of tub segments
 * \param iTubeSegments the number of segments
 */
void pdb_mode::setTubeSegments(int iTubeSegments)
{
    m_iTubeSegments = iTubeSegments;
}
/**
 * get the number of tube profile corners
 * \return the number of tube profile corners
 */
int pdb_mode::getTubeProfileCorners()
{
    return m_iTubeProfileCorners;
}
/**
 * set the number of tub profile corners
 * \param iTubeSegments the number of tube profile corners
 */
void pdb_mode::setTubeProfileCorners(int iTubeProfileCorners)
{
    m_iTubeProfileCorners = iTubeProfileCorners;
}

/**
 * Set the radius of the probe sphere for the solvent accessible surfaces (default: 1.4A)
 * \param fProbeRadius radius of probe sphere
 */
void pdb_mode::setProbeRadius( Real64 fProbeRadius )
{
    m_fProbeRadius = fProbeRadius;
};
/**
 * Get the radius of the probe sphere for the solvent accessible surfaces (default: 1.4A)
 * \return radius of probe sphere
 */
Real64 pdb_mode::getProbeRadius( )
{
    return m_fProbeRadius;
};
