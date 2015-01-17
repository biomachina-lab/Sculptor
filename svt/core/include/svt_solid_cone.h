/***************************************************************************
                          svt_solid_cone.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SOLID_CONE_H
#define SVT_SOLID_CONE_H

#include <svt_node.h>
#include <svt_cone.h>
#include <svt_disc.h>

#include <svt_iostream.h>

/**
 * A solid_cone
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_solid_cone : public svt_node  {

protected:
    svt_cone* m_pCone;
    svt_disc* m_pDisc;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param fHeight height of cone
     * \param fRadius radius of the base of the cone
     */
    svt_solid_cone(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fHeight =0.1f, Real32 fRadius =0.1f);
    virtual ~svt_solid_cone();

    /**
     * set the height of the solid cone
     * \param fHeight height of the solid cone
     */
    void setHeight(Real32 fHeight);
    /**
     * set the radius of the solid cone
     * \param fRadius radius of the solid cone
     */
    void setRadius(Real32 fRadius);

    /**
     * set the properties object.
     * \param pProp pointer to svt_properties object
     */
    virtual void setProperties(svt_properties* pProp);

protected:
    /**
     * prepare the svt primitives
     */
    void preparePrimitives();
};

#endif
