/***************************************************************************
                          svt_vector4.h  -  description
                          ------------------------------
    begin                : 08.08.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VECTOR4_H
#define SVT_VECTOR4_H

// svt includes
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_structs.h>
#include <svt_matrix4.h>
#include <svt_vector3.h>
#include <svt_vector4f.h>
#include <svt_rnd.h>
// clib includes
#include <svt_cmath.h>

#if !defined(NOLIMITS)
#include <limits>
#else
#include <float.h>
#endif

typedef svt_vector4<Real32> svt_vec4real32;
typedef svt_vector4<Real64> svt_vec4real64;

//
// binary operators
//
// NOTE: all arithmetic operators only correspond to the first 3 vector entries,
//       the w-coordinate always stays untouched

template<class T> class svt_matrix4;
template<class T> class svt_vector4;
template<class T> class svt_vector3;


// Sum / Difference / Product
template<class T> 
inline svt_vector4<T> operator+(const svt_vector4<T>& p1, const svt_vector3<T>& p2);

template<class T> 
inline svt_vector4<T> operator+(const svt_vector3<T>& p1, const svt_vector4<T>& p2);

template<class T> 
inline svt_vector4<T> operator+(const svt_vector4<T>& p1, const svt_vector4<T>& p2);

template<class T> 
inline svt_vector4<T> operator+(const svt_vector4<T>& p, const T& f);

template<class T> 
inline svt_vector4<T> operator+(const T& f, const svt_vector4<T>& p);


template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p1, const svt_vector3<T>& p2);

template<class T> 
inline svt_vector4<T> operator-(const svt_vector3<T>& p1, const svt_vector4<T>& p2);

template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p1, const svt_vector4<T>& p2);

template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p, const T& f);

template<class T> 
inline svt_vector4<T> operator-(const T& f, const svt_vector4<T>& p);

template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p);


template<class T> 
inline svt_vector4<T> operator*(const svt_vector4<T>& p, const T& f);

template<class T> 
inline svt_vector4<T> operator*(const T& f, const svt_vector4<T>& p);

template<class T> 
inline svt_vector4<T> operator*(const svt_matrix4<T>& M, const svt_vector4<T>& V);

template<class T> 
inline svt_vector4<T> operator*(const svt_vector4<T>& V, const svt_matrix4<T>& M);

template<class T> 
inline svt_vector4<T> operator/(const svt_vector4<T>& p1, const T& f);

// Scalar Product
template<class T> 
inline T operator*(const svt_vector4<T>& p1, const svt_vector4<T>& p2);


// Vector Product
template<class T> 
inline svt_vector4<T> 
vectorProduct(const svt_vector4<T>& p1, const svt_vector4<T>& p2);

// Vector Product of Quaternions
template<class T> 
inline svt_vector4<T> quaternionVectorProduct(const svt_vector4<T>& p1, const svt_vector4<T>& p2);



///////////////////////////////////////////////////////////////////////////////
// declaration
///////////////////////////////////////////////////////////////////////////////



/** A 4 value template vector
  *@author Stefan Birmanns, Frank Delonge
  */
template<class T> class DLLEXPORT svt_vector4 : public svt_matrix<T>
{
private:
  
  T stack_data[4];

public:

  /**
   * Constructor
   * \param fX initial x coordinate
   * \param fY initial y coordinate
   * \param fZ initial z coordinate
   * \param fW initial w coordinate
   */
  svt_vector4(T fX, T fY, T fZ, T fW = T(1)) : svt_matrix<T>(4,1, stack_data)
  {
	x( fX );
	y( fY );
	z( fZ );
	w( fW );
  }

  svt_vector4(T fValue=T(0), T fW =T(1)) : svt_matrix<T>(4,1, stack_data)
  {
	x( fValue );
	y( fValue );
	z( fValue );
	w( fW );
  }
  
  svt_vector4( const svt_vector4f& rVec ) : svt_matrix<T>(4,1, stack_data)
  {
	x( rVec.x() );
	y( rVec.y() );
	z( rVec.z() );
	w( rVec.w() );
  }

  svt_vector4( const svt_p3d& rVec, T fW =T(1) ) : svt_matrix<T>(4,1, stack_data)
  {
        x( rVec.x );
        y( rVec.y );
        z( rVec.z );
        w( fW );
  }

  svt_vector4(const svt_matrix<T> &that) : svt_matrix<T>( that, stack_data )
  {}
  
  virtual ~svt_vector4()
  {
  };

  svt_vector4<T>& operator=(const svt_vector4<T>& that);

  inline T& operator[](unsigned i) {
	return svt_matrix<T>::m_pData[i];
  }

  inline const T& operator[](unsigned i) const {
      return svt_matrix<T>::m_pData[i];
  }

  //
  // arithmetic operators
  // These operators need to be redefined because
  // only the first 3 coordinates shall be considered
  // all operators return *this to allow daisy chaining
  //
  svt_vector4<T>& operator+=(const svt_vector4<T>& p) {
	(*this)[0] += p[0];
	(*this)[1] += p[1];
	(*this)[2] += p[2];
	return *this;
  }
	
  svt_vector4<T>& operator+=(const T& f){
	(*this)[0] += f;
	(*this)[1] += f;
	(*this)[2] += f;
	return *this;
  }
  
  svt_vector4<T>& operator-=(const svt_vector4<T>& p){
	(*this)[0] -= p[0];
	(*this)[1] -= p[1];
	(*this)[2] -= p[2];
	return *this;
  }

  svt_vector4<T>& operator-=(const T& f){
	(*this)[0] -= f;
	(*this)[1] -= f;
	(*this)[2] -= f;
	return *this;
  }
  
  svt_vector4<T>& operator*=(const T& f){
	(*this)[0] *= f;
	(*this)[1] *= f;
	(*this)[2] *= f;
	return *this;
  }

  svt_vector4<T>& operator/=(const T& f){
	(*this)[0] /= f;
	(*this)[1] /= f;
	(*this)[2] /= f;
	return *this;
  }

  /**
   * performs oMat * this, and stores result in this
   */
  svt_vector4<T>& operator*=(const svt_matrix4<T>& oMat);

  /**
   * get/set methods
   */
  inline T x() const { return (*this)[0]; }
  inline void x(T value) { (*this)[0] = value; }

  inline T y() const { return (*this)[1]; }
  inline void y(T value) { (*this)[1] = value; }

  inline T z() const { return (*this)[2]; }
  inline void z(T value) { (*this)[2] = value; }

  inline T w() const { return (*this)[3]; }
  inline void w(T value) { (*this)[3] = value; }
  
  /**
   * set all three coords of the vector at once
   * \param fX x coord
   * \param fY y coord
   * \param fZ z coord
   */
  inline void set(T fX, T fY, T fZ, T fW=T(1))
  {
	x( fX );
	y( fY );
	z( fZ );
	w( fW);
  }

  inline void set(T value, T fW=T(1))
  {
	*this = value;
	w(fW);
  }
  
  /**
   * set from a Vector4f
   * \param aNewVec new Vector4f to set the svt_vector4 to
   */
  inline void set(const Vector4f& aNewVec)
  {
	x( aNewVec[0] );
	y( aNewVec[1] );
	z( aNewVec[2] );
	w( aNewVec[3] );
  }

  inline void set(const T* p)
  {
	memcpy(svt_matrix<T>::m_pData, p, 4*sizeof(T));
  }

  inline void set(const svt_p3d oVec)
  {
      x( oVec.x );
      y( oVec.y );
      z( oVec.z );
      w( T(1) );
  }
  
  /**
   * initialize the quaternion with a random uniform distribution rotation. Note that x^2+y^2+z^2+w^2 = 1
   * see Shoemake, Graphics Gems III.6, pp.124-132, "Uniform Random Rotations",
   * the vectors describe a ball with radius 1
   *
   */
  inline void initUniformQuaternion()
  {
    T fAlpha1,fAlpha2, fR1, fR2, fX0;
    
    fAlpha1 = svt_genrand() * 2.0f * PI; // no between 0 and 2PI
    fX0  = svt_genrand();
    fR1  = sqrt(1.0f - fX0);
    
    x( sin(fAlpha1) * fR1 );
    y( cos(fAlpha1) * fR1 );
    
    fAlpha2 = svt_genrand() * 2.0f * PI; // no between 0 and 2PI
    fR2  = sqrt(fX0);
    
    z( sin(fAlpha2) * fR2 );
    w( cos(fAlpha2) * fR2 );
      
  }
  
  /**
   * convert the unit quaternion (x^2+y^2+z^2+w^2 = 1) into one with a normal vector and an angle  between -pi and pi
   */
  inline void setQuat2Rot()
  {
      T fW = w();
      if ( fabs( fW ) > 1.0f)
      {
          svt_exception::ui()->error("w should be between -1 and 1");
          return;
      }
      
      Real64 fAngle = 2.0f * acos( fW ); // value between 0 and 360
      if (fW == 1.0f)
      {
          x( 1.0 );
          y( 0.0 );
          z( 0.0 );
      }else
      {
          Real64 fInvSinHalfAngle = 1.0/ sin( fAngle/2.0 );
          
          x( x()*fInvSinHalfAngle );
          y( y()*fInvSinHalfAngle );
          z( z()*fInvSinHalfAngle );
          
          normalize();
      }
      
      //convert angle to -180, 180 interval
/*      if (fAngle>PI)
          fAngle -= 2.0*PI;
      else if (fAngle<-PI)
          fAngle += 2.0*PI;*/
      
      w ( fAngle );
  }


  /**
   * get the squares length of the vector
   * \return length^2
   */
  inline T lengthSq() const
  {
	return x()*x() + y()*y() +z()*z();
  } 

  /**
   * get the length of the vector
   * \return length
   */
  inline T length() const
  {
	return sqrt(lengthSq());
  }
  
   /**
   * get the length of the quaternion
   * \return length
   */
  inline T quaternionLengthSq() const
  {
	return x()*x() + y()*y() + z()*z() + w()*w() ;
  }
  
  /**
   * calculate the distance between two vectors
   * \param oVec the other vector
   * \return distance
   */
  inline T distance(const svt_vector4<T>& oVec) const
  {
      T *oTmp = oVec.c_data();
      T *oThis = (*this).c_data();
      
      T fDiffX = oThis[0] - oTmp[0];
      T fDiffY = oThis[1] - oTmp[1];
      T fDiffZ = oThis[2] - oTmp[2];
      
      T fLengthSq = fDiffX*fDiffX + fDiffY*fDiffY + fDiffZ*fDiffZ;
      T fLength =  sqrt(fLengthSq);
      
      return fLength;
  }

  /**
   * calculate the squared distance between two vectors
   * \param oVec the other vector
   * \return distance
   */
  inline T distanceSq(const svt_vector4<T>& oVec) const
  {
	return (*this-oVec).lengthSq();
  }

  /**
   * normalize the vector, return *this to allow daisy chaining
   */
  inline svt_vector4<T>& normalize()
  {
	T fLength = length();
	
	if (isPositive(fLength) )
	  (*this) /= fLength;

	return *this;
  }

  /**
   * normalize the vector, include w-coordinate in scaling
   * return *this to allow daisy chaining
   */
  svt_vector4<T>& normalize4()
  {
	T fLength = length();
	if (isPositive(fLength) )
	  {
	  *this /= fLength;
	  (*this)[3] /= fLength;
	  } 
	return *this;
  }   


protected:
  // 
  // stream operators
  //

  /// virtual stream methods used by operators << and >>
  virtual void streamOutA(ostream& s) const;
  virtual void streamInA(istream& s);



  /// binary i/o methods
  virtual void streamOutB(svt_obfstream& s) const;
  virtual void streamInB(svt_ibfstream& s);




};




///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////

#include <svt_matrix4.h>

template<class T>
svt_vector4<T>& svt_vector4<T>::operator=(const svt_vector4<T>& that) 
{
  x( that.x() );
  y( that.y() );
  z( that.z() );
  w( that.w() );
  return *this;
}


template<class T>
inline void svt_vector4<T>::streamOutA(ostream& s) const
{
  s << x() << " " << y() << " " << z() << " " << w();
}

template<class T>
inline void svt_vector4<T>::streamInA(istream& s)
{
  s >> (*this)[0] >> (*this)[1] >> (*this)[2] >> (*this)[3];
}

template<class T>
inline void svt_vector4<T>::streamOutB(svt_obfstream& s) const
{
  s << x() << y() << z() << w();
}

template<class T>
inline void svt_vector4<T>::streamInB(svt_ibfstream& s)
{
  s >> (*this)[0] >> (*this)[1] >> (*this)[2] >> (*this)[3];
}


template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p)
{
  svt_vector4<T> v;
  v[0] = -p[0];
  v[1] = -p[1];
  v[2] = -p[2];
  v[3] =  p[3];
  return v;
}

template<class T> 
inline svt_vector4<T>& svt_vector4<T>::operator*=(const svt_matrix4<T>& oMat)
{
  *this=oMat*(*this);
  return *this;
}


//
// arithmetic operators
// (need to be redefined because only first 3 components are taken into consideration)
//

// add point and vector
template<class T> 
inline svt_vector4<T> operator+(const svt_vector4<T>& p1, const svt_vector3<T>& p2)
{ return svt_vector4<T>(p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z()); } 


// add vector and point
template<class T> 
inline svt_vector4<T> operator+(const svt_vector3<T>& p1, const svt_vector4<T>& p2)
{ return svt_vector4<T>(p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z()); } 


// add 2 points
template<class T> 
inline svt_vector4<T> operator+(const svt_vector4<T>& p1, const svt_vector4<T>& p2)
{ return svt_vector4<T>(p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z()); } 


// add point and scalar
template<class T> 
inline svt_vector4<T> operator+(const svt_vector4<T>& p, const T& f)
{ return svt_vector4<T>( p.x()+f, p.y()+f, p.z()+f); }


// add scalar and point
template<class T> 
inline svt_vector4<T> operator+(const T& f, const svt_vector4<T>& p)
{ return svt_vector4<T>( p.x()+f, p.y()+f, p.z()+f); }


// subtract point and vector
template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p1, const svt_vector3<T>& p2)
{ return svt_vector4<T>(p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z()); } 


// subtract vector and point
template<class T> 
inline svt_vector4<T> operator-(const svt_vector3<T>& p1, const svt_vector4<T>& p2)
{ return svt_vector4<T>(p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z()); } 


// substract 2 points
template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p1, const svt_vector4<T>& p2)
{ return svt_vector4<T>(p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z()); } 


// substract point and scalar
template<class T> 
inline svt_vector4<T> operator-(const svt_vector4<T>& p, const T& f)
{ return svt_vector4<T>( p.x()-f, p.y()-f, p.z()-f); }


// substract scalar and point
template<class T> 
inline svt_vector4<T> operator-(const T& f, const svt_vector4<T>& p)
{ return svt_vector4<T>( f-p.x(), f-p.y(), f-p.z()); }


// multiply/devide point with scalar
template<class T> 
inline svt_vector4<T> operator*(const svt_vector4<T>& p, const T& f)
{ return svt_vector4<T>( f*p.x(), f*p.y(), f*p.z()); }


template<class T> 
inline svt_vector4<T> operator*(const T& f, const svt_vector4<T>& p)
{ return svt_vector4<T>( f*p.x(), f*p.y(), f*p.z()); }


template<class T> 
inline svt_vector4<T> operator/(const svt_vector4<T>& p, const T& f)
{ return svt_vector4<T>( p.x()/f, p.y()/f, p.z()/f); }


// Scalar Product
template<class T> 
inline T operator*(const svt_vector4<T>& p1, const svt_vector4<T>& p2)
{ return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z(); }


// Vector Product
template<class T> 
inline svt_vector4<T> vectorProduct(const svt_vector4<T>& p1, const svt_vector4<T>& p2)
{
    return svt_vector4<T>( p1.y()*p2.z()-p1.z()*p2.y(),
			   p1.z()*p2.x()-p1.x()*p2.z(),
			   p1.x()*p2.y()-p1.y()*p2.x());
}

// Vector Product of Quaternions
template<class T> 
inline svt_vector4<T> quaternionVectorProduct(const svt_vector4<T>& p1, const svt_vector4<T>& p2)
{
    return svt_vector4<T>( p1.w()*p2.x() + p1.x()*p2.w() + p1.y()*p2.z() - p1.z()*p2.y(),
			   p1.w()*p2.y() + p1.y()*p2.w() + p1.z()*p2.x() - p1.x()*p2.z(),
			   p1.w()*p2.z() + p1.z()*p2.w() + p1.x()*p2.y() - p1.y()*p2.x(),
                           p1.w()*p2.w() - p1.x()*p2.x() - p1.y()*p2.y() - p1.z()*p2.z());
}

// Product of matrix and vector
template<class T> 
inline svt_vector4<T> operator*(const svt_matrix4<T>& M, const svt_vector4<T>& V)
{
  svt_vector4<T> oVec(V);
  return mat_mult(M,V,oVec);
}


// Product of matrix and vector
// ...does the same as the method above, although not aritmnetically correct...
// ...but usefull :)
template<class T> 
inline svt_vector4<T> operator*(const svt_vector4<T>& V, const svt_matrix4<T>& M)
{
  svt_vector4<T> oVec(V);
  return mat_mult(M,V,oVec);
}




#endif
