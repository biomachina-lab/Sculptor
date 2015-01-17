/***************************************************************************
                          svt_animation_switch_scene.cpp  -  description
                             -------------------
    begin                : Wed 13.12 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_animation_switch_scene.h>
#include <svt_init.h>

#include <svt_iostream.h>

void svt_animation_switch_scene::setOn(bool)
{
    if (m_pNewScene != NULL)
    {
        m_pNewScene->buildScene();
        svt_setScene(m_pNewScene);
    } else
        cout << "Error: No new scene specified!" << endl;
};
