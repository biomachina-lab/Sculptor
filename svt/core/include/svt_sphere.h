/***************************************************************************
                          svt_sphere.h  -  description
                             -------------------
    begin                : Mon Feb 28 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SPHERE_H
#define SVT_SPHERE_H

#include <svt_node.h>
#include <svt_opengl.h>

/**
 * A sphere (just the glu sphere)
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_sphere : public svt_node  {

protected:
    Real32 m_fRadius;
    int m_iSlices;
    int m_iStacks;
    GLUquadricObj* m_pQuadric;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_pos object
     * \param fRadius radius of the sphere
     * \param iSlices number of slices of the sphere
     * \param iStacks number of stacks of the sphere
     */
    svt_sphere(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fRadius =0.025f, int iSlices =16, int iStacks =16);
    virtual ~svt_sphere();

public:
    /**
     * set radius of the sphere
     */
    void setRadius(Real32 fRadius);
    /**
     * get radius of the sphere
     */
    Real32 getRadius() const;

    /**
     * set the stacks
     */
    void setStacks(int iStacks);
    /**
     * get the stacks
     */
    int getStacks();
    /**
     * set the slices
     */
    void setSlices(int iSlices);
    /**
     * get the slices
     */
    int getSlices();

    /**
     * draw the sphere
     */
    void drawGL();
    /**
     * draw the sphere
     */
    void drawOBJ();

    /**
     * prints vrml code for the sphere
     */
    virtual void drawVrml();

protected:
    /**
     * prepare the quadrics object
     */
    void prepareQuad();
};

#endif
