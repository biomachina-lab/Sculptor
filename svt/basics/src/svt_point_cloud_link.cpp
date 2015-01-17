/***************************************************************************
                          svt_point_cloud_link
			  --------------------
    begin                : 07/18/2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/
///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_iostream.h>
#include <stdlib.h>
#include <svt_cmath.h>
#include <svt_point_cloud_link.h>

/**
 * Constructor
 * create an link between atom a and b
 * \param pA pointer to first svt_atom object
 * \param pB pointer to second svt_atom object
 */ 
svt_point_cloud_link::svt_point_cloud_link(svt_point_cloud_atom* pA, svt_point_cloud_atom* pB, int iIndexA, int iIndexB, svt_vector4<Real64>* pVec):svt_point_cloud_bond(pA, pB, iIndexA, iIndexB),
  m_fPsi(0.0),
  m_fTheta(0.0),
  m_fPhi(0.0)
{
    if (pVec == NULL)
	pVec = new svt_vector4<Real64>(0.0,0.0,1.0);
	    
    set( pVec );
    m_bUpdate = true;
};

/**
 * set variables from vector
 * \param the vector from which to extract, len, phi and theta
 */
void svt_point_cloud_link::set(  svt_vector4<Real64>* pVec ) 
{ 
    if (pVec == NULL)
    {
	svt_exception::ui()->info("Can set a link from a Null vector!");
	return;
    }
  
    m_fLength = pVec->length();

    m_oDirection = *pVec;
    m_oDirection.normalize();
    
    m_bUpdate = true;
};

/**
 * rotate 
 * \param iIndexAxis = (0,1,2)=(Psi,Theta,Phi)
 * \param fAngle the angle in rad
 */
void svt_point_cloud_link::rotate( int iIndexAxis, Real64 fAngle )
{
    switch(iIndexAxis)
    {
	case 0: m_fPsi += fAngle; break;
        case 1: m_fTheta += fAngle; break;
        case 2: m_fPhi += fAngle; break;
    }
    
    m_bUpdate = true;
};

/**
 * set Length 
 * \param fLen the length 
 */
void svt_point_cloud_link::setLength( Real64 fLen )
{
    m_fLength = fLen;
    m_bUpdate = true;
};

/**
 * get Length 
 * \return the length 
 */
Real64 svt_point_cloud_link::length()
{
    return m_fLength;
};

/**
 * set Psi 
 * \param psi
 */
void svt_point_cloud_link::setPsi( Real64 fPsi )
{
    m_fPsi = fPsi;
    m_bUpdate = true;
};

/**
 * get the angle Psi
 * \return psi 
 */
Real64 svt_point_cloud_link::getPsi()
{
    return m_fPsi;
};


/**
 * set Phi 
 * \param phi
 */
void svt_point_cloud_link::setPhi( Real64 fPhi )
{
    m_fPhi = fPhi;
    m_bUpdate = true;
};

/**
 * get the angle Phi
 * \return the length 
 */
Real64 svt_point_cloud_link::getPhi()
{
    return m_fPhi;
};

/**
 * set the angle theta 
 * \param Theta
 */
void svt_point_cloud_link::setTheta( Real64 fTheta )
{
    m_fTheta = fTheta;
    m_bUpdate = true;
};

/**
 * get the angle Phi
 * \return the length 
 */
Real64 svt_point_cloud_link::getTheta()
{
    return m_fTheta;
};


/**
 * print Link distance and angles
 */
void svt_point_cloud_link::print()
{
    svtout << "[" << m_fLength << "," << m_fPsi << "," << m_fTheta << "," << m_fPhi << "] --  " ;
    cout << "[" << m_fTheta << "," << m_fPhi << "]" << endl;
};

/**
 * compute the transformation that allows the computation of the point B
 */
void svt_point_cloud_link::calcTransform()
{
    svt_vector4<Real64> oVec = m_oDirection;
    oVec *= m_fLength;
    
    //set transformation
    m_oTransform.loadIdentity();    
    // set rotations
    m_oTransform.rotatePTP(m_fPsi, m_fTheta, m_fPhi );
    //set translation
    m_oTransform.setTranslation( m_oTransform * oVec );
    
    m_bUpdate = false;
};

/**
 * get the transformation that allows the computation of the point B
 * \return the transformation 
 */
svt_matrix4<Real64> svt_point_cloud_link::getTransformation()
{
    if (m_bUpdate)
        calcTransform();
    
    return m_oTransform;
};

/**
 * reset transformation: remove rotations keep translations
 */
void svt_point_cloud_link::resetTransformation()
{
    
   // m_fLength = 0.0f;
    m_fPsi    = 0.0f;
    m_fTheta  = 0.0f;
    m_fPhi    = 0.0f;
    
    m_bUpdate = true;
};
