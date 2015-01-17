/***************************************************************************
                          svt_cmath.h
                          ------------------------------
    begin                : 19.08.2002
    author               : Frank Delonge
    email                : sculptor@biomachina.org

    This file contains several simple math functions and constants

 ***************************************************************************/

#ifndef SVT_CMATH_H
#define SVT_CMATH_H

#include <svt_basics.h>
#include <svt_types.h>

#include <math.h>

#ifndef IRIX 
#  include <cmath>
#endif

#ifdef SVT_NEW_STL
#  include <algorithm>
using namespace std;
#else
#  include <stl_algobase.h>
#endif

/**
 * PI as float constant
 */

const Real32 fPI = 3.1415926535f;

/**
 * PI as double constant
 */
const Real64 dPI = 3.141592653589793116;

/**
 * EPSILON
 */
const Real64 EPSILON = 1.0E-6;

/**
 * convert degrees to radian (float version)
 */
inline Real32 deg2rad(Real32 f) {return fPI* (f / 180.0f); }

/**
 * convert degrees to radian (double version)
 */
inline Real64 deg2rad(Real64 f) {return dPI* (f / 180.0 ); }
inline Real64 deg2rad(int f) {return dPI* (f / 180.0 ); }


/**
 * convert radian to degrees (float version)
 */
inline Real32 rad2deg(Real32 f) {return 180.0f *(f/fPI); }

/**
 * convert radian to degrees (double version)
 */
inline Real64 rad2deg(Real64 f) {return 180.0  *(f/dPI); }
inline Real64 rad2deg(int f) {return 180.0  *(f/dPI); }

/**
 * swap content of to values as template function
 * (same as STL swap(), which does not seem to work everywhere)
 */
template <class T>
inline void svt_swap_values(T& a, T& b) {T tmp=a; a=b; b=tmp; }


/** 
 * get next power-of-2 value of given value,
 * i.e. given i, compute and return the smallest value j=2^n with i<=j.
 * n is returned by reference, if specified.
 */
inline UInt32 svt_next_pow2(UInt32 i, UInt32& n)
{
  n=0;
  while ( i > (1u<<n) ) n++;
  return (1u<<n);
}

inline UInt32 svt_next_pow2(UInt32 i)
{ UInt32 n; return svt_next_pow2(i,n);}





#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif



// NOTE: win32-msvc defines macros min/max instead of template functions.
//       in order to guarentee portability, these macros get undefined here
//       and are replaced by the usual template functions.

#if defined(WIN32_MSVC) 

/**
 *  min template function with 2 args
 */
template <class T>
inline const T& min(const T& a, const T& b) { return (a<b)?a:b; }

/**
 *  max template function with 2 args
 */
template <class T>
inline const T& max(const T& a, const T& b) { return (a<b)?b:a; }

// make sure that the macors won't get redefined, at least not from
// VC's minmax.h
#define _INC_MINMAX



#endif




/**
 *  min template function with 3 args
 */
template <class T>
inline const T& min(const T& a, const T& b, const T& c) {
  return min(min(a,b),c); 
}

/**
 *  max template function with 3 args
 */
template <class T>
inline const T& max(const T& a, const T& b, const T& c) {
  return max(max(a,b),c); 
}



/**
 *  min template function with 4 args
 */
template <class T>
inline const T& min(const T& a, const T& b, const T& c, const T& d) { 
  return min(min(a,b),min(c,d)); 
}

/**
 *  max template function with 4 args
 */
template <class T>
inline const T& max(const T& a, const T& b, const T& c, const T& d) { 
  return max(max(a,b),max(c,d)); 
}



/** 
 * Check if a float value is significantly greater that 0 
 * (seems like all supported compilers can now handle numeric_limits, 
 *  but anyhow...)
 */
#if !defined(WIN32_MSVC) && !defined(IRIX) && !defined(GPLUSPLUS2)

#include <limits>



template <class T> 
inline bool isPositive(const T& value) {
  return value > numeric_limits<T>::epsilon();
}

#else
#  include <float.h>

inline bool isPositive(float f) {
   return f>FLT_EPSILON;
}

inline bool isPositive(double f) {
   return f>DBL_EPSILON;
}

inline bool isPositive(long double f) {
   return f>LDBL_EPSILON;
}

#endif

/**
 * clamp a to the range [mn, mx]
 */
template <class T>
inline void svt_clamp(T& a, const T& mn, const T& mx) {
    a = min( mx, max(mn, a) );
    //a = a < mn ? mn : (a > mx ? mx : a);
}

#endif // SVT_CMATH_H
