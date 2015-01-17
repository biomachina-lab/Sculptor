/***************************************************************************
                          svt_save_framebuffer.h  -  description
                             -------------------
    begin                : Mon August 28 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SAVE_FRAMEBUFFER_H
#define SVT_SAVE_FRAMEBUFFER_H

/** This class save the content of the framebuffer
  *@author Herwig Zilken
  */

class DLLEXPORT svt_save_framebuffer {

public:
    static int save(char *filename, int width, int height);

};

#endif
