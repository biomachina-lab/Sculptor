
#ifndef SVT_TRI_TAN
#define SVT_TRI_TAN


#include <svt_vector4.h>





/**
 * A class a draw a tringle fan with automatic normal generation.
 * Note that this class is not thread-safe, because it uses
 * a triangle buffer that must exist in every rendering thread.
 * Therefore, use the macro-functions of svt_system's svt_window_object
 * must be used to create instances of this class for every rendering context.
 * See svt_sky as an example.
 * This class is pure inline.
 */
class svt_triFan {

private:
  /**
   * count variable for the number of triangles used within the fan
   */
  int    m_iNoTriangles;

  /**
   * Flag indicating whether lighning is currently used.
   * If set to false, the computation of triangle normals will be avoided.
   */
  bool m_bLighted;

  /**
   * buffer for 3 points forming the current triangle
   */
  Real32 m_vfVertexBuffer[9];   

  /**
   * vector for the current normal
   */
  Real32 m_vfNormal[3];

  /**
   * internal pointers to the single points (point into m_vfVertexBuffer)
   */
  //@{
  Real32* m_p0;
  Real32* m_p1;
  Real32* m_p2;
  //@}
  
  Real32 m_fNormalScale;

  /**
   * computes the current normal, stores it in m_vfNormal and passes it
   * to GL 
   */
  inline Real32* normal();


public:

  /**
   * class constructor
   */
  svt_triFan(bool bLighted=true) : m_bLighted(bLighted) {} 

  /**
   * class destructor
   */
  virtual ~svt_triFan() {}

  /**
   * Start a triangle fan.
   * Normals will be computed automatically and passed to OpenGL 
   * if argument \c bLighted is set to \c true. 
   */
  inline void begin(bool bLighted) {m_bLighted=bLighted; begin();};
  inline void begin();
  inline void beginNormals(Real32 fScaleFactor=1.0);

  /**
   * closes the triangle fan. The Nuber of drawn trinagles within the fan is 
   * returned.
   */
  inline int end();
  inline int endNormals();

  /**
   * draws the specified point
   */
  //@{
  inline void point(Real32 x, Real32 y, Real32 z);
  inline void point(const Real32* ptr);
  inline void point(const svt_vector4<Real32>& v);
  //@}


  void pointNormal(Real32 x, Real32 y, Real32 z);
  void pointNormal(const Real32* ptr);
  void pointNormal(const svt_vector4<Real32>& v);



};


inline void svt_triFan::begin()
{ 
  m_p0=m_vfVertexBuffer; m_p1=m_vfVertexBuffer+3; m_p2=m_vfVertexBuffer+6;
  m_iNoTriangles=-2; 
  
  glBegin(GL_TRIANGLE_FAN);
}


inline void svt_triFan::beginNormals(Real32 fScaleFactor)
{ 
  m_p0=m_vfVertexBuffer; m_p1=m_vfVertexBuffer+3; m_p2=m_vfVertexBuffer+6;
  m_iNoTriangles=-2; m_fNormalScale=fScaleFactor;
  
  glBegin(GL_LINES);
}

inline void svt_triFan::point(Real32 x, Real32 y, Real32 z)
{
  if (m_bLighted)
	{
	  Real32* p; 
	  if (m_iNoTriangles>=-1) { p=m_p1; m_p1=m_p2; m_p2=p; }
	  else p=m_p0;
	  p[0]=x; p[1]=y; p[2]=z;
	  if (m_iNoTriangles>=0) 
		{
		  glNormal3fv(normal());
		  if (m_iNoTriangles==0)
			{
			  glVertex3fv(m_p0);
			  glVertex3fv(m_p1);
			}
		  glVertex3fv(p);
		}
	}
  else
	glVertex3f(x,y,z);
  m_iNoTriangles++;
}

inline void svt_triFan::point(const Real32* ptr)
{
  if (m_bLighted)
	{
	  Real32* p; 
	  if (m_iNoTriangles>=-1) { p=m_p1; m_p1=m_p2; m_p2=p; }
	  else p=m_p0;
	  memcpy(p, ptr, 3*sizeof(Real32));
	  if (m_iNoTriangles>=0)
		{
		  glNormal3fv(normal());
		  if (m_iNoTriangles==0)
			{
			  glVertex3fv(m_p0);
			  glVertex3fv(m_p1);
			}
		  glVertex3fv(p);
		}
	}
  else
	glVertex3fv(ptr);
  m_iNoTriangles++;
}

inline void svt_triFan::point(const svt_vector4<Real32>& v)
{ point(v.c_data()); }


inline void svt_triFan::pointNormal(Real32 x, Real32 y, Real32 z)
{
  Real32* p; 
  if (m_iNoTriangles>=-1) { p=m_p1; m_p1=m_p2; m_p2=p; }
  else p=m_p0;
  p[0]=x; p[1]=y; p[2]=z;
  if (m_iNoTriangles>=0) 
	{
	  Real32 point[3];
	  normal();
	  point[0] = (m_p0[0]+m_p1[0]+m_p2[0])/3;
	  point[1] = (m_p0[1]+m_p1[1]+m_p2[1])/3;
	  point[2] = (m_p0[2]+m_p1[2]+m_p2[2])/3;
	  glVertex3fv(point);
	  point[0] += m_vfNormal[0] * m_fNormalScale;
	  point[1] += m_vfNormal[1] * m_fNormalScale;
	  point[2] += m_vfNormal[2] * m_fNormalScale;
	  glVertex3fv(point);
	}
  m_iNoTriangles++;
}

inline void svt_triFan::pointNormal(const Real32* ptr)
{
  Real32* p; 
  if (m_iNoTriangles>=-1) { p=m_p1; m_p1=m_p2; m_p2=p; }
  else p=m_p0;
  memcpy(p, ptr, 3*sizeof(Real32));
  if (m_iNoTriangles>=0) 
	{
	  Real32 point[3];
	  normal();
	  point[0] = (m_p0[0]+m_p1[0]+m_p2[0])/3;
	  point[1] = (m_p0[1]+m_p1[1]+m_p2[1])/3;
	  point[2] = (m_p0[2]+m_p1[2]+m_p2[2])/3;
	  glVertex3fv(point);
	  point[0] += m_vfNormal[0] * m_fNormalScale;
	  point[1] += m_vfNormal[1] * m_fNormalScale;
	  point[2] += m_vfNormal[2] * m_fNormalScale;
	  glVertex3fv(point);
	}
  m_iNoTriangles++;
}

inline void svt_triFan::pointNormal(const svt_vector4<Real32>& v)
{ pointNormal(v.c_data()); }

inline int svt_triFan::end()
{ glEnd(); return m_iNoTriangles; }

inline int svt_triFan::endNormals()
{ glEnd(); return m_iNoTriangles; }


inline Real32* svt_triFan::normal() {
  m_vfNormal[0]=  (m_p1[1]-m_p0[1])*(m_p2[2]-m_p0[2])-(m_p1[2]-m_p0[2])*(m_p2[1]-m_p0[1]);
  m_vfNormal[1]= -(m_p1[0]-m_p0[0])*(m_p2[2]-m_p0[2])+(m_p1[2]-m_p0[2])*(m_p2[0]-m_p0[0]);
  m_vfNormal[2]=  (m_p1[0]-m_p0[0])*(m_p2[1]-m_p0[1])-(m_p1[1]-m_p0[1])*(m_p2[0]-m_p0[0]);
  return m_vfNormal;
}

#endif
