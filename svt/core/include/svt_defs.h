/***************************************************************************
                          svt_defs.h  -  description
                             -------------------
    begin                : Tue Feb 29 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_DEFS_H
#define SVT_DEFS_H

#ifndef MESA
#define ENUM_GLENUM int
#else
#define ENUM_GLENUM enum GLenum
#endif

#ifndef PI
#define PI 3.1415926535
#endif

#define SVT_DISPLAY_MONO        1
#define SVT_DISPLAY_STEREO      2
#define SVT_DISPLAY_WORKBENCH   3

#endif
