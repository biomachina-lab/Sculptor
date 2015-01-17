/***************************************************************************
                          svt_animation_stop.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_init.h>
#include <svt_animation_stop.h>

#include <stdlib.h>

void svt_animation_stop::setOn(bool)
{
    svt_exit(0);
};
