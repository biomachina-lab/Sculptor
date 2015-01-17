/***************************************************************************
                          live_types.h  -  description
                             -------------------
    begin                : Wed Nov 22 2000
    copyright            : (C) 2000 by Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_TYPES_H
#define LIVE_TYPES_H

/** the definitions of own data-types and a class for a machine-dependent checking of the length of this types
  *@author Herwig Zilken
  */
#ifdef linux
typedef char Int8;
typedef unsigned char UInt8;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
#ifdef _LONG
  typedef long int Int64;
  typedef unsigned long int UInt64;
#endif
typedef float Real32;
typedef double Real64;
#endif

#ifdef WIN32
typedef char Int8;
typedef unsigned char UInt8;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
#ifdef _LONG
  typedef long int Int64;
  typedef unsigned long int UInt64;
#endif
typedef float Real32;
typedef double Real64;
#endif

#ifdef sgi
typedef char Int8;
typedef unsigned char UInt8;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
#ifdef _LONG
  typedef long int Int64;
  typedef unsigned long int UInt64;
#endif
typedef float Real32;
typedef double Real64;
#endif

#ifdef SOLARIS
typedef char Int8;
typedef unsigned char UInt8;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
#ifdef _LONG
  typedef long int Int64;
  typedef unsigned long int UInt64;
#endif
typedef float Real32;
typedef double Real64;
#endif

#endif
