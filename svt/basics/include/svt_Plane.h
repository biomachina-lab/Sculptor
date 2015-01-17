#ifndef SVT_PLANE
#define SVT_PLANE

#include <svt_vector4.h>

class DLLEXPORT svt_Plane {
  
 private:

  // 3 Points within Plane
  svt_vector4<Real32> m_P1;
  svt_vector4<Real32> m_P2;
  svt_vector4<Real32> m_P3;
  
  // normal vector
  svt_vector4<Real32> m_N;
  
  // distance to origin
  Real32 m_d;

 public:

  //
  // constructors
  //
  svt_Plane() {}

  svt_Plane(const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2, const svt_vector4<Real32>& P3,
	    bool bPoints=true) 
    { init(P1, P2, P3, bPoints); }

  svt_Plane (const svt_Plane& that)
    : m_P1(that.m_P1), m_P2(that.m_P2), m_P3(that.m_P3), m_N(that.m_N), 
      m_d(that.m_d) {}

  virtual ~svt_Plane() {}

  inline void init(const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2, 
			const svt_vector4<Real32>& P3, bool bPoints=true);

  // compute value of plane function at given (x,y)
  // (only use this method if plane is not vertical!)
  Real32 z(Real32 x, Real32 y) const;

  // returns signed distance to a given point
  //  result>0: point is on upper side of triangle 
  //  result<0: point is on lower side of triangle
  // (upwards is in direction of plane normal)  
  Real32 signedDist(const svt_vector4<Real32>& P) const;

  // returns distance of specified point
  // == fabs(signedDist(P))
  Real32 dist(const svt_vector4<Real32>& P) const;


  // get normal
  const svt_vector4<Real32>& normal() const;  

  // get points
  const svt_vector4<Real32>& P1() const;
  const svt_vector4<Real32>& P2() const;
  const svt_vector4<Real32>& P3() const;

  // get direction vectors V1=P2-P1, V2=P3-P1
  svt_vector4<Real32> V1() const;
  svt_vector4<Real32> V2() const;

};

///////////////////////////////////////////////////////////////////////////
//                     Inline Definitions                                //
///////////////////////////////////////////////////////////////////////////

inline void svt_Plane::init
(const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2, const svt_vector4<Real32>& P3, bool bPoints) {
  if (bPoints) {
    // treat arguments as 3 points on plane
    m_P1 = P1; m_P2 = P2; m_P3 = P3;}

  else {
    // treat arguments as: Point, DirectionVector1, DirectionVector2
    m_P1 = P1; m_P2=P1+P2; m_P3=P1+P3;}

  // calculate normalized normal vector N and (signed) distance d to origin 
  // so that <x,N>=d is the plane equation
  m_N = vectorProduct(m_P2-m_P1, m_P3-m_P1);
  m_N.normalize();
  m_d = m_N*P1;

}

inline Real32 svt_Plane::z(Real32 x, Real32 y) const {
  return (m_d - x*m_N.x() - y*m_N.y()) / m_N.z();
}

inline Real32 svt_Plane::signedDist(const svt_vector4<Real32>& P) const {
  return (P-m_P1)*m_N;
}

inline Real32 svt_Plane::dist(const svt_vector4<Real32>& P) const {
  return fabs(signedDist(P));
}

inline const svt_vector4<Real32>& svt_Plane::normal() const {
  return m_N ;
}

inline const svt_vector4<Real32>& svt_Plane::P1() const {
  return m_P1;
}

inline const svt_vector4<Real32>& svt_Plane::P2() const {
  return m_P2;
}

inline const svt_vector4<Real32>& svt_Plane::P3() const {
  return m_P3;
}

inline   svt_vector4<Real32> svt_Plane::V1() const {
  return m_P2-m_P1;
}

inline   svt_vector4<Real32> svt_Plane::V2() const {
  return m_P3-m_P1;
}



#endif
