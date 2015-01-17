/***************************************************************************
                          svt_vector3.h
                          -------------
    begin                : 19.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VECTOR3_H
#define SVT_VECTOR3_H

// svt includes
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_matrix.h>
#include <svt_vector4.h>
// clib includes
#include <svt_cmath.h>

#if !defined(NOLIMITS)
#include <limits>
#else
#include <float.h>
#endif
//#include <limits>

#define EQ_EPS 0.00000000001

//
// binary operators
//

template<class T> class svt_vector3;
template<class T> class svt_vector4;


// Sum / Difference / Product
template<class T> 
inline svt_vector3<T> operator+(const svt_vector3<T>& p1, const svt_vector3<T>& p2);

template<class T> 
inline svt_vector3<T> operator+(const svt_vector3<T>& p, const T& f);

template<class T> 
inline svt_vector3<T> operator+(const T& f, const svt_vector3<T>& p);


template<class T> 
inline svt_vector3<T> operator-(const svt_vector3<T>& p1, const svt_vector3<T>& p2);

template<class T> 
inline svt_vector3<T> operator-(const svt_vector3<T>& p, const T& f);

template<class T> 
inline svt_vector3<T> operator-(const T& f, const svt_vector3<T>& p);

template<class T> 
inline svt_vector3<T> operator-(const svt_vector3<T>& p);


template<class T> 
inline svt_vector3<T> operator*(const svt_vector3<T>& p, const T& f);

template<class T> 
inline svt_vector3<T> operator*(const T& f, const svt_vector3<T>& p);

template<class T> 
inline svt_vector3<T> operator/(const svt_vector3<T>& p1, const T& f);

// Scalar Product
template<class T> 
inline T operator*(const svt_vector3<T>& p1, const svt_vector3<T>& p2);


// Vector Product
template<class T> 
inline svt_vector3<T> 
vectorProduct(const svt_vector3<T>& p1, const svt_vector3<T>& p2);

template<class T> 
inline svt_vector3<T> 
crossProduct(const svt_vector3<T>& p1, const svt_vector3<T>& p2);


///////////////////////////////////////////////////////////////////////////////
// declaration
///////////////////////////////////////////////////////////////////////////////



/** A 3 value template vector
  *@author Maik Boltes, Stefan Birmanns, Frank Delonge
  */
template<class T> class DLLEXPORT svt_vector3 : public svt_matrix<T>
{
private:
  
  T stack_data[3];

public:

  /**
   * Constructor
   * \param fX initial x coordinate
   * \param fY initial y coordinate
   * \param fZ initial z coordinate
   * \param fW initial w coordinate
   */
  svt_vector3(T fX, T fY, T fZ) 
	: svt_matrix<T>(3,1, stack_data) {
	x( fX );
	y( fY );
	z( fZ );
  }

  svt_vector3(T fValue=T(0)) 
	: svt_matrix<T>(3,1, stack_data) {
	x( fValue );
	y( fValue );
	z( fValue );
  }
  
  svt_vector3( const Point3f& rVec )
	: svt_matrix<T>(3,1, stack_data) {
	x( rVec[0] );
	y( rVec[1] );
	z( rVec[2] );
  }

  svt_vector3( svt_vector4<T> oVec4 )
	: svt_matrix<T>(3,1, stack_data) {
	x( oVec4[0] );
	y( oVec4[1] );
	z( oVec4[2] );
  }

  svt_vector3(const svt_matrix<T> &that) : svt_matrix<T>( that, stack_data ){}
  
  virtual ~svt_vector3(){}

  svt_vector3<T>& operator=(const svt_vector3<T>& that);

  svt_vector3<T>& operator=(const Point3f& that);

  T& operator[](unsigned i) {
	return svt_matrix<T>::m_pData[i];
  }

  const T& operator[](unsigned i) const {
	return svt_matrix<T>::m_pData[i];
  }

  //
  // arithmetic operators
  // These operators need to be redefined because
  // only the first 3 coordinates shall be considered
  // all operators return *this to allow daisy chaining
  //
  svt_vector3<T>& operator+=(const svt_vector3<T>& p) {
	(*this)[0] += p[0];
	(*this)[1] += p[1];
	(*this)[2] += p[2];
	return *this;
  }
	
  svt_vector3<T>& operator+=(const T& f){
	(*this)[0] += f;
	(*this)[1] += f;
	(*this)[2] += f;
	return *this;
  }
  
  svt_vector3<T>& operator-=(const svt_vector3<T>& p){
	(*this)[0] -= p[0];
	(*this)[1] -= p[1];
	(*this)[2] -= p[2];
	return *this;
  }

  svt_vector3<T>& operator-=(const T& f){
	(*this)[0] -= f;
	(*this)[1] -= f;
	(*this)[2] -= f;
	return *this;
  }
  
  svt_vector3<T>& operator*=(const T& f){
	(*this)[0] *= f;
	(*this)[1] *= f;
	(*this)[2] *= f;
	return *this;
  }

  svt_vector3<T>& operator/=(const T& f){
	(*this)[0] /= f;
	(*this)[1] /= f;
	(*this)[2] /= f;
	return *this;
  }

  /**
   * get/set methods
   */
  T x() const { return (*this)[0]; }
  void x(T value) { (*this)[0] = value; }

  T y() const { return (*this)[1]; }
  void y(T value) { (*this)[1] = value; }

  T z() const { return (*this)[2]; }
  void z(T value) { (*this)[2] = value; }

  /**
   * set all three coords of the vector at once
   * \param fX x coord
   * \param fY y coord
   * \param fZ z coord
   */
  void set(T fX, T fY, T fZ)
  {
	x( fX );
	y( fY );
	z( fZ );
  }

  void set(T value)
  {
	*this = value;
  }
  
  void set(const T* p)
  {
	memcpy(svt_matrix<T>::m_pData, p, 3*sizeof(T));
  }

  /**
   * set from a Vector4f
   * \param aNewVec new Vector4f to set the svt_vector4 to
   */
  void set(const Point3f& aNewVec)
  {
	x( aNewVec[0] );
	y( aNewVec[1] );
	z( aNewVec[2] );
  }


  /**
   * get the squares length of the vector
   * \return length^2
   */
  T lengthSq() const
  {
	return x()*x() + y()*y() +z()*z();
  } 

  /**
   * get the length of the vector
   * \return length
   */
  T length() const
  {
	return sqrt(lengthSq());
  }
  
  /**
   * calculate the distance between two vectors
   * \param oVec the other vector
   * \return distance
   */
  T distance(const svt_vector3<T>& oVec) const
  {
	return (*this-oVec).length();
  }

  /**
   * calculate the squared distance between two vectors
   * \param oVec the other vector
   * \return squared distance
   */
  T distanceSq(const svt_vector3<T>& oVec) const
  {
	return (*this-oVec).lengthSq();
  }

  /**
   * calculate the squared distance between vectors and a vector from type Vector3f
   * \param oVec the other vector
   * \return squared distance
   */
  T distanceSq(const Vector3f& oVec) const
  {
      return (x()-oVec[0])*(x()-oVec[0]) + (y()-oVec[1])*(y()-oVec[1]) + (z()-oVec[2])*(z()-oVec[2]);
  }

  /**
   * check, if vectors are nearly equal using the fast maximum norm
   * \param oVec the other vector
   * \return true if nearly equal otherwise false
   */
  bool nearEqual(const Vector3f& oVec)
  {
      if ((fabs(x()-oVec[0]) < EQ_EPS) && (fabs(y()-oVec[1]) < EQ_EPS) && (fabs(z()-oVec[2]) < EQ_EPS))
          return true;
      else
          return false;
  }

  /**
   * check, if vectors are nearly equal using the fast maximum norm
   * \param oVec the other vector
   * \return true if nearly equal otherwise false
   */
  bool nearEqual(const svt_vector3<T>& oVec)
  {
      if ((fabs(x()-oVec[0]) < EQ_EPS) && (fabs(y()-oVec[1]) < EQ_EPS) && (fabs(z()-oVec[2]) < EQ_EPS))
          return true;
      else
          return false;
  }
    
  /**
   * normalize the vector, return *this to allow daisy chaining
   */
  svt_vector3<T>& normalize()
  {
	T fLength = length();
	// 0 only for (0,0,0), so we have not to divide
	if (isPositive(fLength) )
	  (*this) /= fLength;

	return *this;
  }

  /**
   * returns data pointer for old function calls
   */
  T* cast()
  {
      return svt_matrix<T>::m_pData;//why not stack_data?
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

template<class T>
svt_vector3<T>& svt_vector3<T>::operator=(const svt_vector3<T>& that) 
{
  set(that.m_pData);
  return *this;
}

template<class T>
svt_vector3<T>& svt_vector3<T>::operator=(const Point3f& that) 
{
  set(that);
  return *this;
}

template<class T>
inline void svt_vector3<T>::streamOutA(ostream& s) const
{
  s << x() << " " << y() << " " << z();
}

template<class T>
inline void svt_vector3<T>::streamInA(istream& s)
{
  s >> (*this)[0] >> (*this)[1] >> (*this)[2];
}

template<class T>
inline void svt_vector3<T>::streamOutB(svt_obfstream& s) const
{
  s << x() << y() << z();
}

template<class T>
inline void svt_vector3<T>::streamInB(svt_ibfstream& s)
{
  s >> (*this)[0] >> (*this)[1] >> (*this)[2];
}


template<class T> 
inline svt_vector3<T> operator-(const svt_vector3<T>& p)
{
  svt_vector3<T> v;
  v[0] = -p[0];
  v[1] = -p[1];
  v[2] = -p[2];
  return v;
}


//
// arithmetic operators
// (need to be redefined because only first 3 components are taken into consideration)
//

// add 2 points
template<class T> 
inline svt_vector3<T> operator+(const svt_vector3<T>& p1, const svt_vector3<T>& p2)
{ return svt_vector3<T>(p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z()); } 


// add point and scalar
template<class T> 
inline svt_vector3<T> operator+(const svt_vector3<T>& p, const T& f)
{ return svt_vector3<T>( p.x()+f, p.y()+f, p.z()+f); }


// add scalar and point
template<class T> 
inline svt_vector3<T> operator+(const T& f, const svt_vector3<T>& p)
{ return svt_vector3<T>( p.x()+f, p.y()+f, p.z()+f); }


// substract 2 points
template<class T> 
inline svt_vector3<T> operator-(const svt_vector3<T>& p1, const svt_vector3<T>& p2)
{ return svt_vector3<T>(p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z()); } 


// substract point and scalar
template<class T> 
inline svt_vector3<T> operator-(const svt_vector3<T>& p, const T& f)
{ return svt_vector3<T>( p.x()-f, p.y()-f, p.z()-f); }


// substract scalar and point
template<class T> 
inline svt_vector3<T> operator-(const T& f, const svt_vector3<T>& p)
{ return svt_vector3<T>( f-p.x(), f-p.y(), f-p.z()); }


// multiply/devide point with scalar
template<class T> 
inline svt_vector3<T> operator*(const svt_vector3<T>& p, const T& f)
{ return svt_vector3<T>( f*p.x(), f*p.y(), f*p.z()); }


template<class T> 
inline svt_vector3<T> operator*(const T& f, const svt_vector3<T>& p)
{ return svt_vector3<T>( f*p.x(), f*p.y(), f*p.z()); }


template<class T> 
inline svt_vector3<T> operator/(const svt_vector3<T>& p, const T& f)
{ return svt_vector3<T>( p.x()/f, p.y()/f, p.z()/f); }


// Scalar Product
template<class T> 
inline T operator*(const svt_vector3<T>& p1, const svt_vector3<T>& p2)
{ return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z(); }


// Vector Product
template<class T> 
inline svt_vector3<T> vectorProduct(const svt_vector3<T>& p1, const svt_vector3<T>& p2)
{ return svt_vector3<T>( p1.y()*p2.z()-p1.z()*p2.y(),  
					     p1.z()*p2.x()-p1.x()*p2.z(),  
					     p1.x()*p2.y()-p1.y()*p2.x()); }
template<class T> 
inline svt_vector3<T> crossProduct(const svt_vector3<T>& p1, const svt_vector3<T>& p2)
{
    return vectorProduct(p1, p2);
}


#endif
