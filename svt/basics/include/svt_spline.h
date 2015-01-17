//
// create intermediate points between a sequence of points
// with interpolating splines
// currently, only Catmull-Rom splines are implemented
// (see Foley - van Dam - Feiner - Hughes: 
//      "Computer Graphics: Principles and Practice", Chapter 11
//  for further explanations
//
// author: Frank Delonge
//

#ifndef SVT_SPLINE_H
#define SVT_SPLINE_H

#include <svt_basics.h>



#include <svt_stlVector.h>



#include <svt_matrix4.h>
#include <svt_vector4.h>



class DLLEXPORT svt_spline {

private:

  svt_matrix4<Real32> m_Mcr;     // spline basis matrix
  svt_matrix4<Real32> m_McrGbs;  // matrix product of m_Mcr and currently used Gbs point matrix

  vector < svt_vector4 < Real32 > > m_oPoints; // point sequence to interpolate

  int m_iLoop; // for loop mode: index of node to return to at end-of-spline
               // negative for non-loop-mode

  int     m_iCurrIndex; // index of prior point
  Real32  m_fCurrTotDist;  // total distance of prior point

  vector<int> m_Pi;     // current point indexes

  void recalcMat();
  bool advance();
  void initStart();

  Real32 xy_dist(int i, int j);

  bool m_bAtEnd;

public:

  svt_spline(const char* pszFilename=NULL);
  svt_spline(const svt_spline& that) 
	: m_Mcr(that.m_Mcr), m_McrGbs(that.m_McrGbs), m_oPoints(that.m_oPoints),
	  m_iLoop(that.m_iLoop), m_iCurrIndex(that.m_iCurrIndex), 
	  m_Pi(that.m_Pi), m_bAtEnd(that.m_bAtEnd) {}
  virtual ~svt_spline() {}

  bool load(const char* pszFilename);
  

  //
  // 2 methods to get point at specified total distance
  // at end-of-spline, the w-component of returned point is set to -1.0,
  // otherwise 1.0
  //
  svt_vector4<Real32>& point(Real32 fDist, svt_vector4<Real32>& p);
  svt_vector4<Real32>  point(Real32 fDist) {
	svt_vector4<Real32> p; return point(fDist, p); }

  const svt_vector4<Real32>& operator[] (int i) const {
	return m_oPoints[i]; }
 
  unsigned size() const {return m_oPoints.size();}

  int currentLeftPoint() const {return m_iCurrIndex;}


};

#endif
