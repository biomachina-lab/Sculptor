/***************************************************************************
                          svt_types.h  -  description
                             -------------------
    begin                : Wed Nov 22 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TYPES_H
#define SVT_TYPES_H

#include <svt_basics.h>
#include <string.h>

/**
 * the definitions of own data-types and a class for a machine-dependent checking of the length of this types
 *@author Herwig Zilken
 */

// LINUX + MACOSX
#if (defined LINUX || defined MACOSX)
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;

#ifdef _LONG
  typedef long int Int64;
  typedef unsigned long int UInt64;
#endif

#endif

#ifdef LINUX
typedef char Int8;
typedef short Int16;
typedef int Int32;
typedef float Real32;
typedef double Real64;
#endif

// MS Windows
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

// SGI Irix
#ifdef IRIX
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

// SUN Solaris
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

// DEC OSF1
#ifdef OSF1
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


// Datatypes
typedef Real32 Point2f[2];
typedef Real32 Point3f[3];
typedef Real32 Vector3f[3];
typedef Real32 Vector4f[4];
typedef Real32 Matrix4f[4][4];
typedef Real32 Quaternion[4];
typedef struct
{
    Vector3f m_fP[3];
} Triangle;
typedef struct
{
    Point2f m_fP[3];
} TexTriangle;

/**
 * a class for a machine-dependent checking of the length of this types
 * @author Herwig Zilken
 */
class DLLEXPORT svt_types
{
protected:

  static Int16 sm_swapTest;

  bool   m_isBigEndian;


public:

  svt_types();
  virtual ~svt_types() {};
  
  /** comment */
  void checkTypes();
  bool isBigEndian() const;

  static bool BigEndian();

};

#endif
