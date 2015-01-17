/***************************************************************************
                          svt_animation_translation.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ANIMATION_TRANSLATION_H
#define SVT_ANIMATION_TRANSLATION_H

#include <svt_animation_time.h>
#include <svt_node.h>
#include <svt_pos.h>

/** A translation animation (just moving an object from one place to another)
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_animation_translation : public svt_animation_time  {

protected:

    svt_node* m_pObject;
    svt_pos* m_pDestPos;

    // startpos
    float m_fStartX;
    float m_fStartY;
    float m_fStartZ;
    
public:
    svt_animation_translation(svt_node* _tobj =NULL, svt_pos* _tpos =NULL, int time = 1000);

    void setObject(svt_node* _tobj);
    void setDestPos(svt_pos* _tpos);
    
    virtual void setOn(bool flag);

    virtual void drawGL();
};

#endif
