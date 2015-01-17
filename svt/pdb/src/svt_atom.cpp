/***************************************************************************
                          svt_atom.cpp
			  ------------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_atom.h>
#include <svt_atom_colmap.h>
#include <svt_container.h>
#include <svt_iostream.h>
#include <svt_opengl.h>

/**
 * Constructor
 */
/**svt_atom::svt_atom(svt_atom_colmap* pMapping) : svt_point_cloud_atom()
{
    setAtomColmap(pMapping);
};*/
/**
 * Copy Constructor
 * \param reference to position vector (in angstroem)
 * \param reference to svt_point_cloud_atom object
 * \param pointer to svt_atom_colmap object
 */
/*svt_atom::svt_atom(svt_vector4<Real64>& rVec, svt_point_cloud_atom& rAtom, svt_atom_colmap* pMapping) : svt_point_cloud_atom( rAtom ),
m_iDisplayMode( SVT_ATOM_POINT )
{
    setAtomColmap( pMapping );

    // copy the coordinates and the radii
    m_pPoint = &rVec;
    m_fVDWRadGL = rAtom.getVDWRadius() * 1.0E-2;
    m_fVDWRadGLScale = 1.0f;
    m_fCPKRadGLScale = 0.2f;
    m_fLicorizeRad = 0.145f;
};*/

/**
 * Constructor
 * \param iIndex index to atom in point cloud
 * \param reference to svt_point_cloud_pdb object
 * \param reference to svt_point_cloud_atom object
 * \param pointer to svt_atom_colmap object
 */
svt_atom::svt_atom(unsigned int iIndex, svt_point_cloud_pdb< svt_vector4<Real64> >& rPDB, svt_point_cloud_atom& rAtom, svt_atom_colmap* pMapping) :
    svt_point_cloud_atom( rAtom ),
    m_iPoint( iIndex ),
    m_rPDB( rPDB ),
    m_iDisplayMode( SVT_ATOM_POINT ),
    m_iLabelMode( SVT_ATOM_LABEL_OFF )
{
    setAtomColmap( pMapping );

    // copy the coordinates and the radii
    m_fVDWRadGL = rAtom.getVDWRadius() * 1.0E-2;
    m_fVDWRadGLScale = 1.0f;
    m_fCPKRadGLScale = 0.2f;
    m_fLicorizeRad = 0.145f;
}

/**
 * Copy Constructor
 */
svt_atom::svt_atom( const svt_atom& rOther ) :
    svt_point_cloud_atom( rOther ),
    m_iPoint( rOther.m_iPoint ),
    m_rPDB( rOther.m_rPDB ),
    m_fVDWRadGL( rOther.m_fVDWRadGL ),
    m_fVDWRadGLScale( rOther.m_fVDWRadGLScale ),
    m_fCPKRadGLScale( rOther.m_fCPKRadGLScale ),
    m_iDisplayMode( rOther.m_iDisplayMode ),
    m_fLicorizeRad( rOther.m_fLicorizeRad ),
    m_pMapping( rOther.m_pMapping ),
    m_iLabelMode( rOther.m_iLabelMode )
{
};

svt_atom& svt_atom::operator=(const svt_atom& that)
{
    *this = that;

    return *this;
}

/**
 * Destructor
 */
svt_atom::~svt_atom()
{
};

/**
 * get the opengl position
 * \return svt_vector4f with the position
 */
svt_vector4<Real32> svt_atom::getGLPos()
{
    svt_vector4<Real32> oVec( m_rPDB[m_iPoint].x(), m_rPDB[m_iPoint].y(), m_rPDB[m_iPoint].z() );
    oVec *= 1.0E-2;
    oVec.w( 1.0 );

    return oVec;
};
/**
 * get the position
 * \return svt_vector4f with the position
 */
svt_vector4<Real64> svt_atom::getAtomPos()
{
    svt_vector4<Real64> oVec( m_rPDB[m_iPoint].x(), m_rPDB[m_iPoint].y(), m_rPDB[m_iPoint].z() );
    //oVec *= 1.0E-2;
    oVec.w( 1.0 );

    return oVec;
};

/**
 * set the display mode for this atom
 * \param iDisplayMode display mode
 */
void svt_atom::setDisplayMode(Int16 iDisplayMode)
{
    m_iDisplayMode = iDisplayMode;
};

/**
 * get the display mode for this atom
 * \return display mode
 */
Int16 svt_atom::getDisplayMode()
{
    return m_iDisplayMode;
};

/**
 * set the label mode for this atom
 * \param iLabelMode display mode
 */
void svt_atom::setLabelMode(Int16 iLabelMode)
{
    m_iLabelMode = iLabelMode;
};
/**
 * get the label mode for this atom
 * \return label mode
 */
Int16 svt_atom::getLabelMode()
{
    return m_iLabelMode;
};

/**
 * set the vdw radius scaling (default: 1.0f)
 * /param fVDWRadGLScale scaling factor for the vdw radius
 */
void svt_atom::setVDWRadiusScaling(Real32 fVDWRadGLScale)
{
    m_fVDWRadGLScale = fVDWRadGLScale;
};

/**
 * get the vdw radius scaling (default: 1.0f)
 * /return scaling factor for the vdw radius
 */
Real32 svt_atom::getVDWRadiusScaling()
{
    return m_fVDWRadGLScale;
};

/**
 * set the cpk radius scaling (default: 0.2f)
 * \param fCPKRadGLScale scaling factor for atom radius in the cpk mode
 */
void svt_atom::setCPKRadiusScaling(Real32 fCPKRadGLScale)
{
    m_fCPKRadGLScale = fCPKRadGLScale;
};
/**
 * get the cpk radius scaling (default: 0.2f)
 * \return scaling factor for atom radius in the cpk mode
 */
Real32 svt_atom::getCPKRadiusScaling()
{
    return m_fCPKRadGLScale;
};

/**
 * set the radius for the licorize drawing mode
 * \param fLicorizeRad radius for the licorize drawing mode
 */
void svt_atom::setLicorizeRad(Real32 fLicorizeRad)
{
    m_fLicorizeRad = fLicorizeRad;
};
/**
 * get the radius for the licorize drawing mode
 * \return radius for the licorize drawing mode
 */
Real32 svt_atom::getLicorizeRad()
{
    return m_fLicorizeRad;
};

/**
 * set the color mapping object
 * \param pMapping pointer to the svt_atom_colmap object
 */
void svt_atom::setAtomColmap(svt_atom_colmap* pMapping)
{
    m_pMapping = pMapping;
};
/**
 * get the color mapping object
 * \return pointer to the svt_atom_colmap object
 */
svt_atom_colmap* svt_atom::getAtomColmap() const
{
    return m_pMapping;
};

/**
 * get the color 
 * \return pointer to svt_color object
 */
svt_color* svt_atom::getColor()
{
    return m_pMapping->getColor(this);
};

/**
 * prepare container
 * \param m_rCont reference to the svt_container
 * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
 */
void svt_atom::prepareContainer(svt_container& m_rCont)
{
    svt_c3f_v3f oVec;

    switch(m_iDisplayMode)
    {
    case SVT_ATOM_POINT:
	oVec.set( m_pMapping->getColor(this)->getR(),
		  m_pMapping->getColor(this)->getG(),
                 m_pMapping->getColor(this)->getB(),
                 getGLPos().x(),
                 getGLPos().y(),
                 getGLPos().z() );

		 // (Real32)(m_rPDB[m_iPoint].x())*1.0E-2f,
		 // (Real32)(m_rPDB[m_iPoint].y())*1.0E-2f,
		 // (Real32)(m_rPDB[m_iPoint].z())*1.0E-2f );

        m_rCont.addPoint( oVec );
	break;

    case SVT_ATOM_DOTTED:
        m_rCont.addDottedSphere(m_rPDB[m_iPoint].x()*1.0E-2, m_rPDB[m_iPoint].y()*1.0E-2, m_rPDB[m_iPoint].z()*1.0E-2,
                                       m_fVDWRadGL*m_fVDWRadGLScale,
                                       *(m_pMapping->getColor(this))
                                      );
	break;

    case SVT_ATOM_VDW:
        m_rCont.addSphere(m_rPDB[m_iPoint].x()*1.0E-2, m_rPDB[m_iPoint].y()*1.0E-2, m_rPDB[m_iPoint].z()*1.0E-2,
                                       m_fVDWRadGL*m_fVDWRadGLScale,
                                       *(m_pMapping->getColor(this))
                                      );
	break;

    case SVT_ATOM_LICORIZE:
        m_rCont.addSphere(m_rPDB[m_iPoint].x()*1.0E-2, m_rPDB[m_iPoint].y()*1.0E-2, m_rPDB[m_iPoint].z()*1.0E-2,
                                       m_fVDWRadGL*m_fLicorizeRad,
                                       *(m_pMapping->getColor(this))
                                      );
	break;

    case SVT_ATOM_CPK:
        m_rCont.addSphere(m_rPDB[m_iPoint].x()*1.0E-2, m_rPDB[m_iPoint].y()*1.0E-2, m_rPDB[m_iPoint].z()*1.0E-2,
                                       m_fVDWRadGL*m_fCPKRadGLScale,
                                       *(m_pMapping->getColor(this))
                                      );
        break;
    }
}
