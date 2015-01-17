/***************************************************************************
                          svt_animation_pause.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_PAUSE_H
#define SVT_ANIMATION_PAUSE_H

#include <svt_animation_time.h>

/** A pause for an animation.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_pause : public svt_animation_time  {
    
public:
    /// how long should the animation pause?
    svt_animation_pause(int time = 1000) : svt_animation_time(time){ };

    virtual void setOn(bool flag);
};

#endif
