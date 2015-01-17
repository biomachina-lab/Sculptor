#include <svt_cmath.h>

#include <svt_QuadTree.h>



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_Quad                                                      //
//                                                                       //
//  Provides storage information to address a quad as well in            //
//  heightfield as in QuadTree.                                          //
//  Methods to generate neighbour+child descriptors                      //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


//
// init static members
//




int svt_Quad::sm_iLevelOffset[] = 
  {0, 1, 5, 21, 85, 341, 1365, 5461, 21845, 87381, 
   349525, 1398101, 5592405, 22369621, 89478485, 357913941};

int svt_Quad::sm_n = 0;
int svt_Quad::sm_iSize = 0;



//
// implementation of static methods
//

void svt_Quad::setSize(unsigned n, unsigned iSize)
{ sm_n = n; sm_iSize = iSize;}


svt_Quad svt_Quad::root() 
{ return svt_Quad((sm_iSize-1)/2, (sm_iSize-1)/2, 0, 0); }

int svt_Quad::levelOffset(int i) {return sm_iLevelOffset[i];}
int svt_Quad::N() {return sm_n;}
int svt_Quad::sizeX() {return sm_iSize;}
int svt_Quad::sizeY() {return sm_iSize;}


///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class Implementation: svt_TrianglePlane                              //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

void svt_TrianglePlane::init
  (const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2, const svt_vector4<Real32>& P3)  
{
  // triagle plane
  m_oTrianglePlain.init(P1,P2,P3);

  // column bounding planes, normals pointing to interiour
  m_oColPlane1.init(P1, m_oTrianglePlain.normal(), P2-P1, false);
  m_oColPlane2.init(P2, m_oTrianglePlain.normal(), P3-P2, false);
  m_oColPlane3.init(P3, m_oTrianglePlain.normal(), P1-P3, false);
  
}

Real32 svt_TrianglePlane::pointDist
  (const svt_vector4<Real32>& P) const
{



  if (pointOverTriangle(P))
    return m_oTrianglePlain.dist(P);

  Real32 fDist = edgeDist(P, m_oTrianglePlain.P1(), m_oTrianglePlain.P2());
  fDist = min(fDist, edgeDist(P, m_oTrianglePlain.P1(), m_oTrianglePlain.P3()));
  fDist = min(fDist, edgeDist(P, m_oTrianglePlain.P2(), m_oTrianglePlain.P3()));
 
  return fDist;

}

Real32 svt_TrianglePlane::pointZDist
  (const svt_vector4<Real32>& P) const 
{
  return fabs( m_oTrianglePlain.z(P.x(), P.y())-P.z() );
}


bool svt_TrianglePlane::pointOverTriangle
  (const svt_vector4<Real32>& P) const
{
  if (isPositive(-m_oColPlane1.signedDist(P))) return false;
  if (isPositive(-m_oColPlane2.signedDist(P))) return false;
  if (isPositive(-m_oColPlane3.signedDist(P))) return false;
  return true;
}


Real32 svt_TrianglePlane::edgeDist
  ( const svt_vector4<Real32>& P, const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2) const
{
  //                   --->      ----> 
  // Pp: Projection of P1 P onto P1 P2
  //

  svt_vector4<Real32> D1 = P-P1;
  svt_vector4<Real32> D2 = P2-P1;

  svt_vector4<Real32> Pp= P1 + (( (D1)*(D2) / D2.lengthSq() ) * D2);

  // now, (Pp-P1= = a(P2-P1) holds.
  // with 0<=a<=1, Pp is inbetween P1 P2  -> return |P-Pp|
  //      a>1    , Pp right of P2         -> retrun |P-P2|
  //      a<0    , Pp left of P1          -> retrun |P-P1|

  Real32 a;
  if (isPositive(fabs(P2.x()-P1.x())))
	a = (Pp.x()-P1.x()) / (P2.x()-P1.x());
  else
	a = (Pp.y()-P1.y()) / (P2.y()-P1.y());


  if (a<0)   return (P-P1).length();
  if (a>1)   return (P-P2).length();
  return (P-Pp).length();

}


///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class Implementation: svt_QuadPerm                                   //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

void svt_QuadPerm::set(const svt_vector4<Real32>& oViewDir) 
{
  if (!m_bFreeze)
	{
	if (oViewDir.x() >= 0) 
	  {
	  if (oViewDir.y() >= 0) 
		{ vPerm[0]=1; vPerm[1]=2; vPerm[2]=0; vPerm[3]=3;} 
	  else 
		{ vPerm[0]=0; vPerm[1]=1; vPerm[2]=3; vPerm[3]=2; }
	  } 
	else 
	  {
	  if (oViewDir.y() >= 0) 
		{ vPerm[0]=2; vPerm[1]=3; vPerm[2]=1; vPerm[3]=0; }
	  else 
		{ vPerm[0]=3; vPerm[1]=0; vPerm[2]=2; vPerm[3]=1; }
	  }
	}
}
