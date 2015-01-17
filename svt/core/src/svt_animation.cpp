/***************************************************************************
                          svt_animation.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation.h>

svt_animation::svt_animation()
{
    m_pCurrentAnimation = NULL;
    m_bOnFlag = false;
    setLoop(false);
};

void svt_animation::setOn(bool flag)
{
    if (flag)
    {
        if (getSon() != NULL)
        {
            m_pCurrentAnimation = (svt_animation*)getSon();
            m_pCurrentAnimation->setOn(true);
        } else
            return;
    } else {
        if (m_pCurrentAnimation != NULL)
        {
            m_pCurrentAnimation->setOn(false);
        }
    }

    m_bOnFlag = flag;
};

// check if currentAnimation is still running.
void svt_animation::drawGL()
{
    if (getOn() == true)
    {
        if (m_pCurrentAnimation->getOn() == false)
        {
            // switch to the next action (if there is any)
            m_pCurrentAnimation = (svt_animation*)m_pCurrentAnimation->getNext();
            if (m_pCurrentAnimation == NULL)
            {
                // animation finished (last action completed)
                if (getLoop() == false)
                    setOn(false);
                else
                    setOn(true);
            } else
                m_pCurrentAnimation->setOn(true);
        }
    }
};
