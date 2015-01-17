/***************************************************************************
                          svt_animation_time.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_TIME_H
#define SVT_ANIMATION_TIME_H

#include <svt_animation.h>

/** Every animation class which runs a certain amount of time (like svt_animation_translation) ist derived from this class
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_time : public svt_animation  {
protected:
    int m_iStartTime;
    int m_iDuration;

public:
    svt_animation_time(int _time){ setDuration(_time); };

    /// set the start time of the animation
    inline void setStartTime(int _time){ m_iStartTime = _time; };
    /// set the duration of the animation (in microseconds)
    inline void setDuration(int _time){ m_iDuration = _time; };
    /// get the fraction of time
    float getFraction();

    virtual void setOn(bool flag);

    virtual void drawGL();

protected:
    int isOver();
};

#endif
