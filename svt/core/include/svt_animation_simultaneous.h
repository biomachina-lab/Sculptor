/***************************************************************************
                          svt_animation_simultaneous.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_SIMULTANEOUS_H
#define SVT_ANIMATION_SIMULTANEOUS_H

#include <svt_animation.h>

/** An simultaneous animation (all child animations will be executed simultaneous)
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_simultaneous : public svt_animation  {
    
public:
    svt_animation_simultaneous() : svt_animation(){};

    /// start the animation or stop it
    virtual void setOn(bool flag);
    /// get animation state
    inline bool getOn(){ return m_bOnFlag; };
    /// set loop mode
    inline void setLoop(bool flag) { m_bLoopFlag = flag; };
    /// get loop mode
    inline bool getLoop() { return m_bLoopFlag; };

    // check if one animation is finished.
    virtual void drawGL();
};

#endif
