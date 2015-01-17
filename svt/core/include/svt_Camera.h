/********************************************************************
 *                                                                  *
 *  file: svt_Camera.h                                              *
 *                                                                  *
 *  Class to calculate camera position und viewing direction        *
 *  within local object coordinate systems given by current         *
 *  Modelview Matrix.                                               *
 *                                                                  *
 *  Also, a method to check if a sphere given in local object       *
 *  coordintes falls into view frustum is included. This            *
 *  information is based upon the current Projection matrix.        *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_stlVector.h>
#include <svt_vector4.h>

#ifndef SVT_CAMERA
#define SVT_CAMERA

#define FULLY_INSIDE 0x4fu

/**
 *  
 *  Class to calculate camera position und viewing direction        
 *  within local object coordinate systems given by current         
 *  Modelview Matrix.                                               
 *                                                                  
 *  Also, a method to check if a sphere given in local object      
 *  coordintes falls into view frustum is included. This            
 *  information is based upon the current Projection matrix.
 */
class DLLEXPORT svt_Camera {

private:

  /**
   *  Camera position and view direction in object coordinates
   */
  svt_vector4<Real32> m_vPos;
  svt_vector4<Real32> m_vDir;
  svt_vector4<Real32> m_vUp;

  svt_matrix4<Real32> m_M;
  

  /**
   * Vector of frustum plane parameters.
   * (left,right,bottom,top,near,far)
   */
  vector< svt_vector4<Real32> > m_vPlanes;



  static UInt32 sm_InitialCullInfo;
  static UInt32 sm_fullyInside;


public:

  /**
   * Get initial cull info to initialize use of nested spheres
   */
  static UInt32 initialCullInfo();

  /**
   * get bit mask for "fully inside"
   */
  static UInt32 fullyInside();

  /**
   *  Constructor.
   *  Both Matrixes will be loaded with the identity matrix
   */
  svt_Camera() {
	m_vPlanes.resize(6); 
  }


  /**
   *  Copy-Constructor.
   */
  svt_Camera(const svt_Camera& that) 
	: m_vPos(that.m_vPos), m_vDir(that.m_vDir), m_vPlanes(that.m_vPlanes) {}

  /** 
   *  Destructor.
   */
  virtual ~svt_Camera() {}


  /**
   *  update all settings with the current OpenGL matrixes
   */
  void update();


  /**
   *  Get the current camera position transformed into object coordinates, 
   *  based upon the modelview matrix of the last call to update()
   */
  const svt_vector4<Real32>& viewPos() const {return m_vPos;}

  /**
   *  Get the current viewing direction transformed into object coordinates,
   *  based upon the modelview matrix of the last call to update()
   */
  const svt_vector4<Real32>& viewDir() const {return m_vDir;}

  /**
   *  Get the current up direction
   *  based upon the modelview matrix of the last call to update()
   */
  const svt_vector4<Real32>& up() const {return m_vUp;}


  /**
   *  get frustum plane equation parameters for plane i
   *  (0-left, 1-right, 2-bottom, 3-top, 4-near, 5-far)
   *  based upon the modelview matrix of the last call to update()
   */
  const svt_vector4<Real32>& plane(int i) const {return m_vPlanes[i];}


  /**
   *  Test is a given sphere is in the current viewing volume.
   *  The sphere parameters have to be specified in object coordinates
   *  as follows: 
   *  oSphere[0..2] The 3D-Coordinates of the Sphere's midpoint
   *  oSphere[3]    Radius of the sphere
   *  Use the parameter oldCullInfo if you are working with nested 
   *  spheres and you want to reuse old culling information of 
   *  a larger sphere. 
   */
   UInt32 sphereInViewingVolume(const svt_vector4<Real32>& oSphere, 
 							   UInt32& oldCullInfo) const;

  UInt32 sphereInViewingVolume(const svt_vector4<Real32>& oSphere) const
  {
	UInt32 i=initialCullInfo();
	return sphereInViewingVolume(oSphere, i);
  }


  const svt_matrix4<Real32>& ModelviewMatrix() const {return m_M;}
  svt_matrix4<Real32>& ModelviewMatrix() {return m_M;}


};

#endif
