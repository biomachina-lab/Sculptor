/***************************************************************************
                          svt_arrow.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ARROW_H
#define SVT_ARROW_H

#include <svt_node.h>
#include <svt_solid_cone.h>
#include <svt_solid_cylinder.h>

/**
 * A arrow (consisting of a cylinder and a cone)
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_arrow : public svt_node  {

protected:
    svt_solid_cone* m_pCone;
    svt_solid_cylinder* m_pCylinder;

    Real32 m_fTotHeight;
    Real32 m_fTailRadius;
    Real32 m_fHeadHeightScale;
    Real32 m_fHeadRadiusScale;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param fHeight total height of the arrow
     * \param fRadius radius of the tail
     */
    svt_arrow(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fHeight =0.1f, Real32 fRadius =0.015f);
    virtual ~svt_arrow();

    /**
     * set the total height of the arrow
     * \param fHeight height of the arrow
     */
    void setHeight(Real32 fHeight);
    /**
     * set the radius of the arrow tail
     * \param fRadius radius of the arrow tail
     */
    void setRadius(Real32 fRadius);
    /**
     * set the portion of the arrow which will
     * be taken by the head
     * \param fScale head portion
     */
    void setHeadHeightScale(Real32 fScale);
    /**
     * set how much larger the arrow head radius is
     * compared to the tail
     * \param fScale radius scaling of the head
     */
    void setHeadRadiusScale(Real32 fScale);

    /**
     * set the properties object.
     * \param pProp pointer to svt_properties object
     */
    virtual void setProperties(svt_properties* pProp);

protected:
    /**
     * prepare the primitives for the arrow
     */
    void preparePrimitives();
};

#endif
