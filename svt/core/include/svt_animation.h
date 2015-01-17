/***************************************************************************
                          svt_animation.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_H
#define SVT_ANIMATION_H

#include <svt_node_noautodl.h>

/** An animation class. Most of the time this is only the container for other animation objects.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation : public svt_node_noautodl  {
protected:
    svt_animation* m_pCurrentAnimation;
    bool m_bOnFlag;
    bool m_bLoopFlag;
    
public:
    svt_animation();
    virtual ~svt_animation() { };

    /// start the animation or stop it
    virtual void setOn(bool flag);
    /// get animation state
    inline bool getOn(){ return m_bOnFlag; };
    /// set loop mode
    inline void setLoop(bool flag) { m_bLoopFlag = flag; };
    /// get loop mode
    inline bool getLoop() { return m_bLoopFlag; };

    /// this function draws nothing, it checks if currentAnimation is still running. If not it switchs to the next anim.
    virtual void drawGL();
};

#endif
