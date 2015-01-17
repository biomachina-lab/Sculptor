/***************************************************************************
                          svt_arrow.cpp  -  description
                             -------------------
    begin                : May 10 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_arrow.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fHeight height of the arrow (height of head is 10% of the complete height)\
 * \param fRadius radius of the head
 */
svt_arrow::svt_arrow(svt_pos* pPos, svt_properties* pProp, Real32 fHeight, Real32 fRadius) : svt_node(pPos, pProp)
{
    preparePrimitives();
    setHeight(fHeight);
    setRadius(fRadius);
    setHeadHeightScale(0.25f);
    setHeadRadiusScale(1.6f);

    setName("svt_arrow");
};
svt_arrow::~svt_arrow()
{
};

/**
 * set the total height of the arrow
 * \param fHeight height of the arrow
 */
void svt_arrow::setHeight(Real32 fHeight)
{
    m_fTotHeight = fHeight;
    m_pCylinder->setHeight(m_fTotHeight * (1.0-m_fHeadHeightScale) );
    m_pCone->setHeight(m_fTotHeight * m_fHeadHeightScale);
    m_pCone->getPos()->setPos(0.0f,0.0f,m_fTotHeight * (1.0-m_fHeadHeightScale));
}

/**
 * set the radius of the arrow tail
 * \param fRadius radius of the arrow tail
 */
void svt_arrow::setRadius(Real32 fRadius)
{
    m_fTailRadius = fRadius;
    m_pCylinder->setRadius(m_fTailRadius);
    m_pCone->setRadius(m_fTailRadius * m_fHeadRadiusScale);
}

/**
 * set the portion of the arrow which will
 * be taken by the head
 * \param fScale head portion
 */
void svt_arrow::setHeadHeightScale(Real32 fScale)
{
    m_fHeadHeightScale = fScale;
    setHeight(m_fTotHeight);
}

/**
 * set how much larger the arrow head radius is
 * compared to the tail
 * \param fScale radius scaling of the head
 */
void svt_arrow::setHeadRadiusScale(Real32 fScale)
{
    m_fHeadRadiusScale = fScale;
    setRadius(m_fTailRadius);
}

/**
 * set the properties object.
 * \param pProp pointer to svt_properties object
 */
void svt_arrow::setProperties(svt_properties* pProp)
{
    m_pCylinder->setProperties(pProp);
    m_pCylinder->setDeleteProp( false );
    m_pCone->setProperties(pProp);
    m_pCone->setDeleteProp( false );

    svt_node::setProperties(pProp);
}

/**
 * prepare primitives
 */
void svt_arrow::preparePrimitives()
{
    //the cone
    m_pCone = new svt_solid_cone(new svt_pos(0.0f, 0.0f, 0.0f), NULL, 0.01f, 0.01f);
    m_pCone->setRotation(new svt_rotation(0.0f,0.0f,0.0f));
    m_pCone->setName("(svt_arrow)m_pCone");
    add(m_pCone);

    //the cylinder
    m_pCylinder = new svt_solid_cylinder(new svt_pos(0.0f,0.0f,0.0f), NULL, 0.1f, 0.005f);
    m_pCylinder->setName("(svt_arrow)m_pCylinder");
    add(m_pCylinder);
}
