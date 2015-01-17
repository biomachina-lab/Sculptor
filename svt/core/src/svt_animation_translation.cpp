/***************************************************************************
                          svt_animation_translation.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation_translation.h>

svt_animation_translation::svt_animation_translation(svt_node* _tobj, svt_pos* _tpos, int time) : svt_animation_time(time)
{
    setObject(_tobj);
    setDestPos(_tpos);
};

void svt_animation_translation::setOn(bool flag)
{
    m_fStartX = m_pObject->getPos()->getX();
    m_fStartY = m_pObject->getPos()->getY();
    m_fStartZ = m_pObject->getPos()->getZ();

    svt_animation_time::setOn(flag);
};

void svt_animation_translation::drawGL()
{
    if (getOn() == true)
    {
        float dist_x = getFraction() * (m_pDestPos->getX() - m_fStartX);
        float dist_y = getFraction() * (m_pDestPos->getY() - m_fStartY);
        float dist_z = getFraction() * (m_pDestPos->getZ() - m_fStartZ);

        m_pObject->getPos()->setX(m_fStartX + dist_x);
        m_pObject->getPos()->setY(m_fStartY + dist_y);
        m_pObject->getPos()->setZ(m_fStartZ + dist_z);
    }
    
    svt_animation_time::drawGL();
};

void svt_animation_translation::setObject(svt_node* _tobj)
{
    m_pObject = _tobj;
};

void svt_animation_translation::setDestPos(svt_pos* _tpos)
{
    m_pDestPos = _tpos;
};
