/***************************************************************************
                          svt_animation_stop.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_STOP_H
#define SVT_ANIMATION_STOP_H

#include <svt_animation.h>

/** This action stops the program. It is a useful feature if you want to create a movie out of the
  * frames saved to disk. Then you want to stop the program if the animation is over.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_stop : public svt_animation  {
public:
    svt_animation_stop(){ };

    virtual void setOn(bool flag);
};

#endif
