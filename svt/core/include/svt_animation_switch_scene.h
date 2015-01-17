/***************************************************************************
                          svt_animation_switch_scene.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_SWITCH_SCENE_H
#define SVT_ANIMATION_SWITCH_SCENE_H

#include <svt_animation.h>
#include <svt_scene.h>

/** This action switchs to another scene
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_switch_scene : public svt_animation  {

protected:
    svt_scene* m_pNewScene;

public:
    svt_animation_switch_scene(svt_scene* _tscene){ m_pNewScene = _tscene; };

    virtual void setOn(bool flag);
};

#endif
