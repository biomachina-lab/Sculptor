
#include <svt_basics.h>


#include <svt_cmath.h>
#include <svt_matrix4.h>
#include <svt_vector4.h>


#include <svt_Camera.h>




UInt32 svt_Camera::sm_InitialCullInfo=0x40u;
//UInt32 svt_Camera::sm_fullyInside=0x7fu;
UInt32 svt_Camera::sm_fullyInside=0x4fu;


UInt32 svt_Camera::initialCullInfo() {return sm_InitialCullInfo; }
UInt32 svt_Camera::fullyInside() {return sm_fullyInside; }

void svt_Camera::update()
{

  //
  // update current GL matrixes
  // note that these matrices appear transposed
  //
  svt_matrix4<Real32> M, P;

  m_M.loadModelviewMatrix();

  M=m_M;
  P.loadProjectionMatrix();

  // 
  // we will need M^(-1) and P*M
  // therefore, we first do the matrix multiplication, 
  // then replace M with its inverse
  //
  
  P *= M;
  M.invert();

  //
  // get position and viewing direction
  // ----------------------------------
  //
  // Camera position Pc and viewing direction Vc in camera-space are always
  //
  //    Pc = (0,0,0, 1)        and        Vc = (0,0,-1,0)
  //
  // (That's why its called camera space - seems allso easy :) )
  //
  // The current Modelview-Matrix M transforms vertices from a local coordinate
  // system of an object (=object space) into camera space.
  // Therefore, we use M^(-1) to get camera position and direction
  // in object space. Taking advantage of the sparce structure of Pc and Vc
  // gives:
  //
  //     Po = M^(-1)*Pc        and        Vo =  M^(-1)*Vc 
  //        = M^(-1) (*,3)                   = -M^(-1) (*,2)
  //
  // (index counting begins c-like with 0)

  int i;
  for ( i=0; i<4; i++)
	{
	m_vDir[i] = -(M[i][2]);
	m_vPos[i] = M[i][3];
	}


  //
  // get frustum planes
  // ------------------
  //
  // for computation of the frustum planes, we need P*M.
  // See "Mathematics for 3D programming" p103 for further explanations...
  //
  // postponed: if we are in stereo mode, we should extend the view frustum
  //            by the additional space that is visible for the other eye.
  //            When first calculated for the left eye, we only need to 
  //            adjust left plane to cover the full (combined) view volume
  //

  for (i=0; i<6; i++)
	{
	//
	// grep plane parameters
	//
	int k=i/2;
	Real32 fSgn = (i&1) ? -1.0 : 1.0;
	for (int j=0; j<4; j++)
	  m_vPlanes[i][j] = P[3][j] + fSgn*(P[k][j]);

	//
	// rescale
	//
	m_vPlanes[i].normalize4();
	}

  //
  // get up-direction
  // (perp. to view-direction and normal vector of left plane)
  //
  m_vUp = vectorProduct(plane(0), viewDir());
  m_vUp.normalize();


}


UInt32 svt_Camera::sphereInViewingVolume
(const svt_vector4<Real32>& oSphere, UInt32& iOldCullInfo) const
{

  //  for (int i=0; i<6; i++)
    for (int i=0; i<4; i++)

    if (!(iOldCullInfo & (1u << i))) 
	  {

	  Real32 d = m_vPlanes[i]*oSphere + m_vPlanes[i][3];

	  //
	  // return 0 if completely outside 
	  //
      if (d < -(oSphere[3]) )
		{
		  iOldCullInfo=0;
		  return 0;        
		}   

	  //
	  // set bit if completely on interiour side of plane
	  //
      if (d > oSphere[3])
        iOldCullInfo |= (1u << i);

    }

  return iOldCullInfo;
  
}
