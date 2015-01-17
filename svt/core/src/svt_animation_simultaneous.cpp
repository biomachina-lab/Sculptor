/***************************************************************************
                          svt_animation_simultaneous.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation_simultaneous.h>

// check if one animation is finished.
void svt_animation_simultaneous::drawGL()
{
    if (getOn() == true)
    {
        svt_animation* current = (svt_animation*)getSon();

        while(current != NULL)
        {
            if (current->getOn() == false)
            {
                // animation finished
                if (getLoop() == false)
                    setOn(false);
                else {
                    setOn(false);
                    setOn(true);
                }
            }
            current = (svt_animation*)current->getNext();
        }
    }
};

void svt_animation_simultaneous::setOn(bool flag)
{
    if (flag)
    {
        // switch all sons on
        svt_animation* current = (svt_animation*)getSon();

        while(current != NULL)
        {
            current->setOn(true);
            current = (svt_animation*)current->getNext();
        }
    } else {
        // switch all sons off
        svt_animation* current = (svt_animation*)getSon();

        while(current != NULL)
        {
            current->setOn(false);
            current = (svt_animation*)current->getNext();
        }
    }

    m_bOnFlag = flag;
};
