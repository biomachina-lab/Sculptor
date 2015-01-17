/***************************************************************************
                          svt_cone.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONE_H
#define SVT_CONE_H

#include <svt_node.h>
#include <svt_pos.h>
#include <svt_properties.h>
#include <svt_iostream.h>
#include <svt_opengl.h>

/**
 * A cone class.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_cone : public svt_node  {

protected:
    Real32 m_fRadius;
    Real32 m_fHeight;
    svt_pos* m_pRotationVector;
    Real32 m_fRotAngle;
    GLUquadricObj* m_pQuadric;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param fHeight height of the cone
     * \param fRadius radius of the cone
     */
    svt_cone(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fHeight =0.1, Real32 fRadius =0.1);
    /**
     * Constructor
     * Create a cone between two positions
     * \param pPosStart pointer to svt_pos object
     * \param pPosEnd pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_cone(svt_pos* pPosStart, svt_pos* pPosEnd, svt_properties* pProp =NULL, Real32 fRadius=0.1);
    virtual ~svt_cone();

    /**
     * set the height of the cone
     * \param fHeight the height of the cone
     */
    void setHeight(Real32 fHeight);
    /**
     * set the radius of the cone
     * \param fRadius the radius of the cone
     */
    void setRadius(Real32 fRadius);

    /**
     * draw the cone
     */
    void drawGL();

protected:
    /** prepare the quadrics object */
    void prepareQuad();
};

#endif
