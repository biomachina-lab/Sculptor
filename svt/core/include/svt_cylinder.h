/***************************************************************************
                          svt_cylinder.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CYLINDER_H
#define SVT_CYLINDER_H

#include <svt_node.h>
#include <svt_properties.h>
#include <svt_pos.h>
#include <svt_opengl.h>
#include <svt_iostream.h>

/**
 * A cylinder class.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_cylinder : public svt_node  {

protected:
    Real32 m_fRadius;
    Real32 m_fHeight;
    svt_pos *m_pRotationVector;
    Real32 m_fRotAngle;

    GLUquadricObj* m_pQuadric;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param fHeight height of the cylinder
     * \param fRadius radius of the cylinder
     */
    svt_cylinder(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fHeight =0.1f, Real32 fRadius =0.1f);
    /**
     * Constructor
     * Create a cylinder between two positions
     * \param pPosStart pointer to svt_pos object
     * \param pPosEnd pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param fRadius radius of the cylinder
     */
    svt_cylinder(svt_pos* pPosStart, svt_pos* pPosEnd, svt_properties* pProp =NULL, Real32 fRadius =0.1f);
    virtual ~svt_cylinder();

    /**
     * set the height of the cylinder
     * \param fHeight height of the cylinder
     */
    void setHeight(Real32 fHeight);
    /**
     * set the radius of the cylinder
     * \param fRadius radius of the cylinder
     */
    void setRadius(Real32 fRadius);
    /**
     * draw the cylinder
     */
    void drawGL();

protected:
    /** prepare the quadric object for glu */
    void prepareQuad();
};

#endif
