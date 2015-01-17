/***************************************************************************
                          svt_animation_time.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation_time.h>
#include <svt_init.h>
#include <svt_time.h>

float svt_animation_time::getFraction()
{
    float curr = 0.0f;

    // currently saving a pictures to disk (to create a movie)? Then we cannot use the time as criteria...
    if (svt_isSavingFrames() == false)
        curr = svt_getElapsedTime() - m_iStartTime;
    else {
        curr = ((float)svt_getFrameNum() * ((float)1/(float)svt_getFramesPerSecond())) * 1000;
        curr -= m_iStartTime;
    }

    float frac = curr / m_iDuration;

    if (frac > 1.0f) frac = 1.0f;

    return frac;
}

void svt_animation_time::setOn(bool flag)
{    
    if (flag == true)
    {
        if (svt_isSavingFrames() == false)
            setStartTime(svt_getElapsedTime());
        else
            setStartTime((int)((float)svt_getFrameNum() * ((float)1/(float)svt_getFramesPerSecond())) * 1000);
    }

    m_bOnFlag = flag;
}

int svt_animation_time::isOver()
{
    if (getFraction() >= 1.0f)
    {
        setOn(false);
        return true;
    } else
        return false;
}

void svt_animation_time::drawGL()
{
    if (getOn())
    {
        isOver();
    }
}
