/***************************************************************************
                          svt_pipette.cpp
                          -------------------
    begin                : 26.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_pipette.h>

/**
 * Constructor
 */
svt_pipette::svt_pipette(svt_pos* pPos, svt_properties* pProp) : svt_node(pPos, pProp)
{
    setName("svt_pipette");

    m_pCone1 = new svt_solid_cone(new svt_pos(0.0f, 0.0f, 0.0f), NULL, 0.5f, 0.05f);
    m_pCone1->setRotation(new svt_rotation(0.0f,180.0f,0.0f));
    //m_pCone1->setProperties(svt_properties_solid_white);
    m_pCone1->setName("(svt_pipette)m_pCone1");
    m_pCone1->setProperties(pProp);
    add(m_pCone1);
    m_pCone2 = new svt_solid_cone(new svt_pos(0.0f, 0.07f, -0.05f), NULL, 0.05f, 0.02f);
    m_pCone2->setRotation(new svt_rotation(90.0f,180.0f,0.0f));
    m_pCone2->setProperties(pProp);
    m_pCone2->setName("(svt_pipette)m_pCone2");
    add(m_pCone2);
}
svt_pipette::~svt_pipette()
{
}

/**
 * set the properties object.
 * \param pProp pointer to svt_properties object
 */
void svt_pipette::setProperties(svt_properties* pProp)
{
    m_pCone1->setProperties(pProp);
    m_pCone2->setProperties(pProp);

    svt_node::setProperties(pProp);
}
