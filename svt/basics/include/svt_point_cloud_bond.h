/***************************************************************************
                          svt_point_cloud_bond
			  --------------------
    begin                : 04/14/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_POINT_CLOUD_BOND_H
#define __SVT_POINT_CLOUD_BOND_H

#include <svt_basics.h>

class svt_point_cloud_atom;

/**
 * A bond class (bond between two atoms).
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_point_cloud_bond
{

protected:

    // bond between atom a and b
    svt_point_cloud_atom* m_pAtomA;
    svt_point_cloud_atom* m_pAtomB;

    int m_iIndexA;
    int m_iIndexB;

public:

    /**
     * Constructor
     * create an bond between atom a and b
     * \param pA pointer to first svt_atom object
     * \param pB pointer to second svt_atom object
     */
    svt_point_cloud_bond(svt_point_cloud_atom* pA, svt_point_cloud_atom* pB, int iIndexA=-1, int iIndexB=-1);
    ~svt_point_cloud_bond();

    /**
     * set the first atom
     * \param pA pointer to svt_atom object
     */
    void setAtomA(svt_point_cloud_atom* pA);
    /**
     * get the first atom
     * \return pointer to svt_atom object
     */
    svt_point_cloud_atom* getAtomA();
    /**
     * get the index of atom a
     */
    int getIndexA();
    /**
     * get the index of atom b
     */
    int getIndexB();
    /**
     * set the index of atom a
     */
    void setIndexA(int iIndexA);
    /**
     * set the index of atom b
     */
    void setIndexB(int iIndexB);
    /**
     * set the second atom
     * \param pB pointer to svt_atom object
     */
    void setAtomB(svt_point_cloud_atom* pB);
    /**
     * get the second atom
     * \return pointer to svt_atom object
     */
    svt_point_cloud_atom* getAtomB();
};

#endif
