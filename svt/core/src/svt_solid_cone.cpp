/***************************************************************************
                          svt_solid_cone.cpp  -  description
                             -------------------
    begin                : May 10 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_solid_cone.h>
#include <svt_pos.h>
#include <svt_rotation.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fHeight height of cone
 * \param fRadius radius of the base of the cone
 */
svt_solid_cone::svt_solid_cone(svt_pos* pPos, svt_properties* pProp, Real32 fHeight, Real32 fRadius) : svt_node(pPos, pProp)
{
    preparePrimitives();
    setRadius(fRadius);
    setHeight(fHeight);
    setName("svt_solid_cone");
};
svt_solid_cone::~svt_solid_cone()
{
};

/**
 * set the height of the solid cone
 * \param fHeight height of the solid cone
 */
void svt_solid_cone::setHeight(Real32 fHeight)
{
    m_pCone->setHeight(fHeight);
}

/**
 * set the radius of the solid cone
 * \param fRadius radius of the solid cone
 */
void svt_solid_cone::setRadius(Real32 fRadius)
{
    m_pCone->setRadius(fRadius);
    m_pDisc->setRadius(fRadius);
}

/**
 * set the properties object.
 * \param pProp pointer to svt_properties object
 */
void svt_solid_cone::setProperties(svt_properties* pProp)
{
    m_pDisc->setProperties(pProp);
    m_pDisc->setDeleteProp( false );
    m_pCone->setProperties(pProp);
    m_pCone->setDeleteProp( false );

    svt_node::setProperties(pProp);
}

/**
 * prepare the svt primitives
 */
void svt_solid_cone::preparePrimitives()
{
    // the cone
    m_pCone = new svt_cone( new svt_pos(0.0f,0.0f,0.0f), NULL, 0.01f, 0.01f );
    m_pCone->setName("(svt_solid_cone)m_pCone");
    add(m_pCone);

    // the disc
    m_pDisc = new svt_disc( new svt_pos(0.0f,0.0f,0.0f), NULL, 0.01f );
    m_pDisc->setRotation( new svt_rotation(0.0f,180,0.0f) );
    m_pDisc->setName("(svt_solid_cone)m_pDisc");
    add(m_pDisc);
}
