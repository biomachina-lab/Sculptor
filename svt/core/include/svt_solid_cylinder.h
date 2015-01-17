/***************************************************************************
                          svt_solid_cylinder.h  -  description
                             -------------------
    begin                : Jul 16 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SOLID_CYLINDER_H
#define SVT_SOLID_CYLINDER_H

#include <svt_node.h>
#include <svt_disc.h>
#include <svt_cylinder.h>

/** A solid cylinder (discs at both ends)
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_solid_cylinder : public svt_node {

protected:
    svt_disc* m_pDisc1;
    svt_disc* m_pDisc2;
    svt_cylinder* m_pCylinder;

public:
    /**
     * Constructor
     */
    svt_solid_cylinder(svt_pos* pPos=NULL, svt_properties* pProp=NULL, Real32 fHeight=0.1f, Real32 fRadius=0.1f);
    virtual ~svt_solid_cylinder();

    /**
     * set the height of the solid_cylinder
     * \param fHeight height of the cylinder
     */
    void setHeight(Real32 fHeight);
    /**
     * set the radius of the solid_cylinder
     * \param radius of the cylinder
     */
    void setRadius(Real32 fRadius);

    /**
     * set the properties object.
     * \param pProp pointer to svt_properties object
     */
    virtual void setProperties(svt_properties* pProp);

protected:
    /**
     * prepare the svt objects
     */
    void preparePrimitives();
};

#endif
