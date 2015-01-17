/***************************************************************************
                          svt_animation_rotation.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_ROTATION_H
#define SVT_ANIMATION_ROTATION_H

#include <svt_animation_time.h>
#include <svt_node.h>
#include <svt_rotation.h>

/** A translation animation (just moving an object from one place to another)
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_rotation : public svt_animation_time  {
protected:
    svt_node* object;
    svt_rotation* destRot;

    // startpos
    float start_x;
    float start_y;
    float start_z;
    
public:
    ///
    svt_animation_rotation(svt_node* _tobj =NULL, svt_rotation* _trot =NULL, int time = 1000) : svt_animation_time(time){ setObject(_tobj); setDestRotation(_trot); };

    void setObject(svt_node* _tobj){ object = _tobj; };
    void setDestRotation(svt_rotation* _trot){ destRot = _trot; };
    
    virtual void setOn(bool flag);

    virtual void drawGL();
};

#endif
