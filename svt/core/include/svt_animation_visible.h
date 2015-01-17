/***************************************************************************
                          svt_animation_visible.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_VISIBLE_H
#define SVT_ANIMATION_VISIBLE_H

#include <svt_animation.h>

/**
 * This animation switches an object on or off.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_animation_visible : public svt_animation  {
protected:
    bool m_bVisFlag;
    svt_node* m_pObject;
    
public:
    /**
     * Constructor
     * \param pObject pointer to svt_node
     * \param bVis if true, then the object will get switched on
     */
    svt_animation_visible(svt_node* pObject, bool bVis);

    /**
     * start the animation or stop it
     */
    virtual void setOn(bool flag);
    /**
     * get animation state
     */
    bool getOn();
};

#endif
