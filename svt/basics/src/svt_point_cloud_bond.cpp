/***************************************************************************
                          svt_point_cloud_bond
			  --------------------
    begin                : 04/14/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_iostream.h>
#include <stdlib.h>
#include <svt_point_cloud_bond.h>


/**
 * Constructor
 * create an bond between atom a and b
 * \param pA pointer to first svt_atom object
 * \param pB pointer to second svt_atom object
 */
svt_point_cloud_bond::svt_point_cloud_bond(svt_point_cloud_atom* pA, svt_point_cloud_atom* pB, int iIndexA, int iIndexB) :
    m_pAtomA( pA ),
    m_pAtomB( pB ),
    m_iIndexA( iIndexA ),
    m_iIndexB( iIndexB )
{
}
svt_point_cloud_bond::~svt_point_cloud_bond()
{
}

/**
 * set the first atom
 * \param pA pointer to svt_atom object
 */
void svt_point_cloud_bond::setAtomA(svt_point_cloud_atom* pA)
{
    m_pAtomA = pA;
}
/**
 * get the first atom
 * \return pointer to svt_atom object
 */
svt_point_cloud_atom* svt_point_cloud_bond::getAtomA()
{
    return m_pAtomA;
}
/**
 * set the second atom
 * \param pB pointer to svt_atom object
 */
void svt_point_cloud_bond::setAtomB(svt_point_cloud_atom* pB)
{
    m_pAtomB = pB;
}
/**
 * get the second atom
 * \return pointer to svt_atom object
 */
svt_point_cloud_atom* svt_point_cloud_bond::getAtomB()
{
    return m_pAtomB;
}
/**
 * get the index of atom a
 */
int svt_point_cloud_bond::getIndexA()
{
    return m_iIndexA;
}
/**
 * get the index of atom b
 */
int svt_point_cloud_bond::getIndexB()
{
    return m_iIndexB;
}
/**
 * set the index of atom a
 */
void svt_point_cloud_bond::setIndexA(int iIndexA)
{
    m_iIndexA = iIndexA;
}
/**
 * set the index of atom b
 */
void svt_point_cloud_bond::setIndexB(int iIndexB)
{
    m_iIndexB = iIndexB;
}
