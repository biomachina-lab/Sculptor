/***************************************************************************
                          svt_solid_cylinder.cpp  -  description
                             -------------------
    begin                : May 10 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_solid_cylinder.h>

/**
 * Constructor
 */
svt_solid_cylinder::svt_solid_cylinder(svt_pos* pPos, svt_properties* pProp, Real32 fHeight, Real32 fRadius) : svt_node(pPos, pProp)
{
    preparePrimitives();
    setRadius(fRadius);
    setHeight(fHeight);
    setName("svt_solid_cylinder");
};
svt_solid_cylinder::~svt_solid_cylinder()
{
};

/**
 * set the height of the solid_cylinder
 * \param fHeight height of the cylinder
 */
void svt_solid_cylinder::setHeight(Real32 fHeight){

    m_pCylinder->setHeight(fHeight);
    m_pDisc2->getPos()->setPos(0.0f,0.0f,fHeight);
}

/**
 * set the radius of the solid_cylinder
 * \param radius of the cylinder
 */
void svt_solid_cylinder::setRadius(Real32 fRadius){

    m_pCylinder->setRadius(fRadius);
    m_pDisc1->setRadius(fRadius);
    m_pDisc2->setRadius(fRadius);
}

/**
 * set the properties object.
 * \param pProp pointer to svt_properties object
 */
void svt_solid_cylinder::setProperties(svt_properties* pProp)
{
    m_pDisc1->setProperties(pProp);
    m_pDisc1->setDeleteProp( false );
    m_pDisc2->setProperties(pProp);
    m_pDisc2->setDeleteProp( false );
    m_pCylinder->setProperties(pProp);
    m_pCylinder->setDeleteProp( false );

    svt_node::setProperties(pProp);
}

/**
 * prepare the svt objects
 */
void svt_solid_cylinder::preparePrimitives()
{
    m_pDisc1 = new svt_disc(new svt_pos(0.0f,0.0f,0.0f), NULL, 0.01f);
    m_pDisc1->setName("(svt_solid_cylinder)m_pDisc1");
    add(m_pDisc1);
    m_pDisc2 = new svt_disc(new svt_pos(0.0f,0.0f,0.1f), NULL, 0.01f);
    m_pDisc2->setName("(svt_solid_cylinder)m_pDisc2");
    add(m_pDisc2);
    m_pCylinder = new svt_cylinder(new svt_pos(0.0f,0.0f,0.0f), NULL, 0.1f,0.01f);
    m_pCylinder->setName("(svt_solid_cylinder)m_pCylinder");
    add(m_pCylinder);
}
