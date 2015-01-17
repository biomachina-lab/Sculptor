/***************************************************************************
                          svt_animation_visible.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation_visible.h>

/**
 * Constructor
 * \param pObject pointer to svt_node
 * \param bVis if true, then the object will get switched on
 */
svt_animation_visible::svt_animation_visible(svt_node* pObject, bool bVis) : svt_animation()
{
    m_bVisFlag = bVis;
    m_pObject = pObject;
};

/**
 * start the animation or stop it
 */
void svt_animation_visible::setOn(bool)
{
    m_pObject->setVisible(m_bVisFlag);
};

/**
 * get animation state
 */
bool svt_animation_visible::getOn()
{
    return m_bOnFlag;
};
