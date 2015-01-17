/***************************************************************************
                          svt_point_cloud_link
			  --------------------
    begin                : 07/18/2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_POINT_CLOUD_LINK_H
#define __SVT_POINT_CLOUD_LINK_H

#include <svt_basics.h>
#include <svt_matrix4.h>
#include <svt_point_cloud_bond.h>


/**
 * A link class (link between two point cloud of atoms). 
 * A link is a bond with internal coordinates properties: frames, length, angles 
 * @author Mirabela Rusu
 */
class DLLEXPORT svt_point_cloud_link : public svt_point_cloud_bond
{
protected:

    // direction- should be a normalized vector
    svt_vector4<Real64> m_oDirection;

    //link lenk
    Real64 m_fLength;
    
    //rotation angles that are applied to the vector defined by the direction and length
    Real64 m_fPsi, m_fTheta, m_fPhi;
                
    // the transformation required to get the location of indexB relative to atom indexA
    svt_matrix4<Real64> m_oTransform;
    
    //does the transformation matrix need to be update (some parameters were changed)
    bool m_bUpdate;

public:
    
    /**
     * Constructor
     * create an link between atom a and b
     * \param pA pointer to first svt_atom object
     * \param pB pointer to second svt_atom object
     */ 
    svt_point_cloud_link(svt_point_cloud_atom* pA, svt_point_cloud_atom* pB, int iIndexA=-1,int iIndexB=-1, svt_vector4<Real64>* pVec=NULL );
 
    /**
     * set variables from vector
     * \param the vector from which to extract, len, phi and theta
     */
    void set( svt_vector4<Real64> *pVec );

    /**
     * rotate 
     * \param iIndexAxis = (0,1,2)=(x,y,z)
     * \param fAngle the angle in rad
     */
    void rotate( int iIndexAxis, Real64 fAngle );
   
    /**
     * set Length 
     * \param fLen the length 
     */
    void setLength( Real64 fLen );
     
    /**
     * get Length 
     * \return the length 
     */
    Real64 length();
 
    /**
     * set Psi 
     * \param psi
     */
    void setPsi( Real64 fPsi );

    /**
     * get the angle Psi
     * \return Psi 
     */
    Real64 getPsi();

    /**
     * set Phi 
     * \param phi
     */
    void setPhi( Real64 fPhi );

    /**
     * get the angle Phi
     * \return Phi 
     */
    Real64 getPhi();

    /**
     * set the angle theta 
     * \param Theta
     */
    void setTheta( Real64 fTheta );

    /**
     * get the angle Phi
     * \return the length 
     */
    Real64 getTheta();

    /**
     * print Link distance and angles
     */
    void print();
    
    /**
     * compute the transformation that allows the computation of the point B
     */
    void calcTransform();
    
    /**
     * get the transformation that allows the computation of the point B
     * \return the transformation 
     */
    svt_matrix4<Real64> getTransformation();
    
    /**
     * reset transformation: remove rotations keep translations
     */
    void resetTransformation();

};

#endif


