/***************************************************************************
                          svt_animation_rotation.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation_rotation.h>

void svt_animation_rotation::setOn(bool flag)
{
    if (object->getRotation() == NULL)
        return;
    
    start_x = object->getRotation()->getX();
    start_y = object->getRotation()->getY();
    start_z = object->getRotation()->getZ();

    svt_animation_time::setOn(flag);
};

void svt_animation_rotation::drawGL()
{
    if (getOn() == true)
    {
        float dist_x = getFraction() * (destRot->getX() - start_x);
        float dist_y = getFraction() * (destRot->getY() - start_y);
        float dist_z = getFraction() * (destRot->getZ() - start_z);

        object->getRotation()->setX(start_x + dist_x);
        object->getRotation()->setY(start_y + dist_y);
        object->getRotation()->setZ(start_z + dist_z);
    }
    
    svt_animation_time::drawGL();
};
