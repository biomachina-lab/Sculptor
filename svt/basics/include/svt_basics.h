/***************************************************************************
                          svt_basics.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <stdio.h>

/**
\mainpage

This is the SVT Basics Main Page Introduction.<br>
A more sensefull text about what this library does should be written 
in include/svt_basics.h.
<p>
Unfortunately, this text does not appear in the latex-Documentation,
although it should (propably a doxygen-bug :( )
<p>

 */

#ifndef SVT_BASICS_H
#define SVT_BASICS_H

#ifdef WIN32_MSVC
  #include <windows.h>
  #ifndef DLLEXPORT
     #define DLLEXPORT __declspec(dllexport)
  #endif

  #ifndef GPLUSPLUS3
     #pragma warning(disable:4251)   // benoetigt DLLEXPORT ....
     #pragma warning(disable:4244)
     #pragma warning(disable:4786)
     #pragma warning(disable:4305)
  #endif

#else
  #ifndef DLLEXPORT
     #define DLLEXPORT
  #endif
#endif

#ifndef NULL
  #define NULL 0
#endif

#ifdef IRIX65_O32

  #ifndef __STL_DONT_USE_BOOL_TYPEDEF

    #define __STL_DONT_USE_BOOL_TYPEDEF
    typedef char bool;

    #ifdef false
      #undef false
    #endif
    #define false (bool(0))

    #ifdef true
      #undef true
    #endif
    #define true (!false)

  #endif



#endif

// include svt_system because of svtout
#include <svt_system.h>

// opengl profiling
#include <svt_opengl.h>
#ifdef OGLPROF
#define glVertex2d(x,y) glNormal3d(x,y,0)
#define glVertex2f(x,y) glNormal3f(x,y,0)
#define glVertex2i(x,y) glNormal3i(x,y,0)
#define glVertex2s(x,y) glNormal3s(x,y,0)
#define glVertex3d(x,y,z) glNormal3d(x,y,z)
#define glVertex3f(x,y,z) glNormal3f(x,y,z)
#define glVertex3i(x,y,z) glNormal3i(x,y,z)
#define glVertex3s(x,y,z) glNormal3s(x,y,z)
#define glVertex4d(x,y,z,w) glNormal3d(x,y,z)
#define glVertex4f(x,y,z,w) glNormal3f(x,y,z)
#define glVertex4i(x,y,z,w) glNormal3i(x,y,z)
#define glVertex4s(x,y,z,w) glNormal3s(x,y,z)
#define glVertex2dv(v) glNormal3d(v[0],v[1])
#define glVertex2fv(v) glNormal3f(v[0],v[1])
#define glVertex2iv(v) glNormal3i(v[0],v[1])
#define glVertex2sv(v) glNormal3s(v[0],v[1])
#define glVertex3dv(v) glNormal3dv(v)
#define glVertex3fv(v) glNormal3fv(v)
#define glVertex3iv(v) glNormal3iv(v)
#define glVertex3sv(v) glNormal3sv(v)
#define glVertex4dv(v) glNormal3dv(v)
#define glVertex4fv(v) glNormal3fv(v)
#define glVertex4iv(v) glNormal3iv(v)
#define glVertex4sv(v) glNormal3sv(v)
#endif

// macros to calculate the minimum and maximum
// do not rename them to min/max because they typically collide with gcc min/max defines
#define svt_max(x,y) (((x)>(y))?(x):(y))
#define svt_min(x,y) (((x)<(y))?(x):(y))

#include <typeinfo>

#endif
