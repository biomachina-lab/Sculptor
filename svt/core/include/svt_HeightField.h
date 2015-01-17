// 
// Compiler Switches:
//
//   SVT_HF_USE_SMALL_SPHERES   enables optmized spheres if enabled
//

#ifndef SVT_HEIGHT_FIELD
#define SVT_HEIGHT_FIELD


//
// includes
//

#include <svt_basics.h>
#include <svt_exception.h>
#include <svt_matrix.h>
#include <svt_stlVector.h>
#include <svt_vector4.h>
#include <svt_Plane.h>


#include <svt_contextSpecific.h>
#include <svt_time.h>

#include <svt_init.h>
#include <svt_node.h>
#include <svt_Camera.h>
#include <svt_triFan.h>

//
// includes of private classes
//
#include <svt_QuadTree.h>





///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: HF_CanvasData                                                 //
//                                                                       //
//  collection of all context-specific data within svt_HeightField       //
//  Specific context access is managed to svt_contextSensitive container //
//  class                                                                //
///////////////////////////////////////////////////////////////////////////

class HF_CanvasData {
public:
  //  svt_Camera               oCam;//@@@
  svt_QuadPerm             oQuadPerm;
  svt_QuadSelectionBuffer  oSelectedQuads;
  svt_QuadBacktraceStack   oBTStack;
  svt_QuadSubdevInfo       oSubdevInfo;
  UInt32                   iNoTriangles;
  UInt32                   iNoQuads;

  //  HF_CanvasData() : oCam(), oQuadPerm(), oSelectedQuads(), 
  //					 oBTStack(), oSubdevInfo(), iNoTriangles(0), iNoQuads(0) 
  //  {}
  HF_CanvasData() : oQuadPerm(), oSelectedQuads(), 
					 oBTStack(), oSubdevInfo(), iNoTriangles(0), iNoQuads(0) 
  {}//@@@

//   HF_CanvasData(const HF_CanvasData& that) 
// 	: oCam(that.oCam), oQuadPerm(that.oQuadPerm), oSelectedQuads(that.oSelectedQuads), 
// 	  oBTStack(that.oBTStack), oSubdevInfo(that.oSubdevInfo), 
// 	  iNoTriangles(0), iNoQuads(0)
//   {}
  HF_CanvasData(const HF_CanvasData& that) 
	: oQuadPerm(that.oQuadPerm), oSelectedQuads(that.oSelectedQuads), 
	  oBTStack(that.oBTStack), oSubdevInfo(that.oSubdevInfo), 
	  iNoTriangles(0), iNoQuads(0)
  {}//@@@
  virtual ~HF_CanvasData() {}


  // collective init method for all members:
  // n: (2^n+1 == size)
  // iCapacity:   capacity of QuadSelectionBuffer
  // bUnilimited: limitation flag of QuadSelectionBuffer
  void init(int n, int iQuadLimit=-1) {
	oSelectedQuads.init(iQuadLimit, n);
	oBTStack.init(n);
	oSubdevInfo.init(n);
  }

  void setQuadLimit(int iMaxQ) {
	oSelectedQuads.setLimit(iMaxQ);
  }

  void toggleQuadPermFreeze() {
	oQuadPerm.toggleFreeze();
  }



};





///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_HeightField                                               //
//                                                                       //
//  contains all heightfield data and the quadtree itself.               //
//  provides methods for refinement and drawing                          //
///////////////////////////////////////////////////////////////////////////

class DLLEXPORT svt_HeightField : public svt_node
{
 public:


  //
  // exported types
  //================
  //

  enum SphereMode {Small, Normal};
  enum ObjErrMode {Real,  Z};
  enum ResizeMode {Center, Interpolate};

 private:

  //
  // private members
  //=================
  //

  /**
   *  heightfield data as float vector
   */
  svt_matrix<Real32> m_oElevationData;

  Int32 m_iSizeX;
  Int32 m_iSizeY;

  /**
   * heightfield size with n^2+1 == size
   * assumes that width==height, which is currently a must
   */
  Int32 m_n;

  /**
   * gridpoint distances in m
   */
  //@{
  Real32 m_fGPDistX;
  Real32 m_fGPDistY;
  //@}

  /**
   * The QuadTree as vector of svt_QuadTreeNode.
   * Guarantees random data access
   */
  vector<svt_QuadTreeNode> m_oQuadTree;


  /**
   * all context sensitive stuff
   */
  svt_canvasSpecific<HF_CanvasData> m_oContextData;

  /**
   * error tolerance in mm
   * if <0 -> no simplification
   */
  Real32 m_fEpsilon;

  /** 
   * error tolerance, for which the current quadtree content is calculated
   * only < 0 if no calculation has been performed yet.
   * Used to restore simplifcation settings when toggling simplification
   * and to check if parameters changed in resize()
   */
  Real32 m_fCalcEpsilon; 

  /**
   * flag weather to display rendering statistics
   */
  bool m_bDrawStat;

  /**
   * bounding box
   */
  svt_matrix<Real32> m_Box;


  /**
   * texture
   */
  svt_texture m_oTexture;


  /**
   * QuadTree Filename
   */
  svt_str m_strQTFile;


  /**
   * currently used object error mode
   */
  ObjErrMode m_eObjErrMode;

  /**
   * matrix resize mode
   */
  ResizeMode m_eResizeMode;

  int m_iDownsample;

  bool m_bFreeze;

  bool m_bCulling;

  int m_iTriangleLimit;

  bool m_bBenchmarkRun;
  const char* m_pDataDir;

  //
  // private methods
  //=================
  //

  /**
   * read heightfield data
   */
  void readHeightfield(const char* pszFilename,  
					   Real32 fGPDistX=1.0, Real32 fGPDistY=1.0, 
					   Real32 fHeightScale=1.0);

  /** 
   * Precalculate per-quad for given quad.
   * This method is called recursively from precompute()
   */
  void precompute(const svt_Quad& Q, ObjErrMode eErrMod);
  void precomputeObjErr(const svt_Quad& Q, ObjErrMode eErrMode);
  void precomputeSphere(const svt_Quad& Q);


  /**
   * draw a complete fan by resolving neighbourship dependencies
   */
  inline void drawFan(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const;


  /**
   * methods to insert intermediate edge points in triangle fan, 
   * if neighbours are further subdevided
   */
  //@{
  inline void drawRightEdge(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const;
  inline void drawLeftEdge(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const;
  inline void drawBottomEdge(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const;  
  inline void drawTopEdge(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const;
  //@}

  /**
   * draw a single point of a trinagle fan
   */
  void drawPoint(svt_triFan& oFan, short i, short j) const;

  /**
   * construct 3D point of given matrix indices
   */
  svt_vector4<Real32> point(short i, short j) const;

  /**
   * construct special quad points: center and corners
   */
  svt_vector4<Real32> centerPoint(const svt_Quad& Q) const;
  svt_vector4<Real32> topLeftPoint(const svt_Quad& Q) const;
  svt_vector4<Real32> topRightPoint(const svt_Quad& Q) const;
  svt_vector4<Real32> bottomLeftPoint(const svt_Quad& Q) const;
  svt_vector4<Real32> bottomRightPoint(const svt_Quad& Q) const;

  /**
   * draw redfinement / rendering statistics
   */
  void drawStat();

  /** 
   * draw simplified heightfield
   * includes refinement and rendering
   */
  //@{
  inline void drawSimplified();
  void refineQT();
  void renderQT();

  //@}

  /**
   * draw unsimplified heightfield
   */
  void drawAll();

 public:


  //
  // constructors / destructors
  //============================
  //

  /**
   * Class Constructor.
   * \param pszHeightFile   The filename of the Heightfield data. Usually, this is a .height-file,
   *                        which already contains Gridpoint distances an height scale parameters,
   *                        but you can also use .tga / .bmp files (which will be converted).
   * \param pszQTFile       The filename of the precomputed quadtree data. If you specify a
   *                        filename that does not exist so far, a new file will be created,
   *                        but please note the precomputing is very time-conuming. If this parameter
   *                        is omitted, a default filename will be picked.
   * \param pszTextureFile  The filename of the texture to use. If omitted, no texture will be used.
   * \param fEpsilon        Initial sceen-space error tolerance in mm. If omitted or set to a value <=0,
   *                        no simplification will be performed. This parameter can be changed
   *                        during excution by the setEpsilon()-method.
   * \param eResizeMode     Resize mode when resizing the heightfield matrix to the next possible
   *                        size s that satisfies s=2^n+1. Possible values are svt_HeightField::Center and
   *                        svt_HeightField::Interpolate. 
   * \param iMaxTriangles   If set to a positive number, the number of triangles per screen
   *                        is limited to (approximately) the specified value.
   * \param eObjErrMode     Specifies Object error mode to use when precomputing becomes
   *                        neccessary. That is, when epsilon becomes >0 and no qt-file was
   *                        specified or the specified file does not exist.
   *                        Possible values are svt_HeightField::Real or svt_HeightField::Z.
   * \param fGPDistX        The Gridpoint distance in x direction in m.
   *                        The value is only taken into account if pszHeightFile is a tga or bmp file
   *                        (.height-files already contain this information)
   * \param fGPDistY        The Gridpoint distance in y direction in m.
   *                        The value is only taken into account if pszHeightFile is a tga or bmp file
   *                        (.height-files already contain this information)
   * \param fHeightScale    The height in m that corresponds to the height distance 1 in a tga/bmp file.
   *                        The value is only taken into account if pszHeightFile is a tga or bmp file
   *                        (.height-files already contain this information)
   */

  svt_HeightField(const char* pszHeightFile, const char* pszQTFile=NULL,
				  const char* pszTextureFile=NULL, Real32 fEpsilon=-1.0f,
				  ResizeMode eResizeMode=Center, int iMaxTriangles=-1,
				  ObjErrMode eObjErrMode=Real,
				  Real32 fGPDistX=1.0, Real32 fGPDistY=1.0, 
				  Real32 fHeightScale=1.0);


  svt_HeightField(const svt_HeightField& that);
  virtual ~svt_HeightField() {}


  //
  // settings
  //==========
  //

  /**
   * get / set / toggle statistics
   */
  //@{
  bool statistics() const {return m_bDrawStat; }
  void setStatistics(bool b) {m_bDrawStat=b;}
  bool toggleStatistics() {m_bDrawStat=!m_bDrawStat; return m_bDrawStat;}
  //@}
  

  /**
   * get bounding box 
   */
  const svt_matrix<Real32>& box() const {return m_Box;}


  /**
   * get simplification settings
   */
  bool       simplify()   const {return isPositive(m_fEpsilon);}
  ObjErrMode objErrMode() const {return m_eObjErrMode;}
  SphereMode sphereMode() const {
#ifdef SVT_HF_USE_SMALL_SPHERES
	return Small;
#else
	return Normal;
#endif
  }

  //
  // virtual methods inherited from svt_node
  //=========================================
  //

  virtual void drawGL();
  virtual void printName() {cout << "Heightfield" << endl;}


  //
  // methods to prepare the quadtree
  //=================================
  //

  /** 
   * Precalculate per-quad information and store them in oQuadTree.
   * Precalculations include delta and sphere parameters.
   */
  void precompute();

  /**
   * set epsilon (in mm)
   * will re-read the quadtree file, which contains per-quad informatin
   * about delta and sphere parameters.
   * Based upon delta, gamma will be computed 
   * (see chapter 4.3 of my diplom thesis for details)
   */
  void setEpsilon(Real32 fEpsilon);

  /**
   * get current epsilon (in mm).
   * <0 if no simplification in applied
   */
  Real32 getEpsilon() const {return m_fEpsilon;}

  /**
   * toggle simplification.
   * returns current simplification status
   */
  bool toggleSimplification() {
	if (m_fEpsilon<0) { setEpsilon(m_fCalcEpsilon); return (m_fEpsilon>0);}
	else {setEpsilon(-1.0); return false;}
  }

  Int32 sizeX() const {return m_iSizeX;}
  Int32 sizeY() const {return m_iSizeX;}

  ResizeMode resizeMode() const {return m_eResizeMode;}

  const char* qtFile() const{ return m_strQTFile; }

  void increaseError();
  void decreaseError();

  bool freeze() const { return m_bFreeze;}
  void freeze(bool b) { m_bFreeze=b;}
  bool toggleFreeze() {m_bFreeze=!m_bFreeze; return m_bFreeze;}

  bool culling() const { return m_bCulling;}
  void culling(bool b) { m_bCulling=b;}
  bool toggleCulling() {m_bCulling=!m_bCulling; return m_bCulling;}

  int triangleLimit() const {return m_iTriangleLimit;}
  void triangleLimit(int iMaxT);

  void toggleQuadPermFreeze();

  void benchmark(const char* data_dir) {
	m_pDataDir=data_dir;
	m_bBenchmarkRun=true;
  }

  Real32 height(Real32 x, Real32 y);

protected:
  virtual void do_precomputations() {refineQT();}
  void run_benchmark();

};

///////////////////////////////////////////////////////////////////////////
//                     Inline Definitions                                //
///////////////////////////////////////////////////////////////////////////


inline void svt_HeightField::drawPoint
(svt_triFan& oFan, short i, short j) const 
{
  oFan.point( j *m_fGPDistX, (sizeY()-i-1)*m_fGPDistY, m_oElevationData[i][j]);
}

inline void svt_HeightField::drawFan
(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const
{ 

  if ( oCData.oSubdevInfo.subdevided(Q.index()) )
	{
	// 
	// draw full fan (8 triangles) for highest level quads
	//
	drawPoint(oFan, Q.i(),   Q.j()  ); drawPoint(oFan, Q.i()-1, Q.j()-1);
    drawPoint(oFan, Q.i()  , Q.j()-1); drawPoint(oFan, Q.i()+1, Q.j()-1);
	drawPoint(oFan, Q.i()+1, Q.j()  ); drawPoint(oFan, Q.i()+1, Q.j()+1);
	drawPoint(oFan, Q.i()  , Q.j()+1); drawPoint(oFan, Q.i()-1, Q.j()+1);
	drawPoint(oFan, Q.i()-1, Q.j()  ); drawPoint(oFan, Q.i()-1, Q.j()-1);
	}
  else 
	{
    // 
	// draw partial fan (4 + intermediate triangles) for other quads
	//


	drawPoint(oFan, Q.i(),       Q.j()     ); 
	drawPoint(oFan, Q.iTop(),    Q.jLeft() ); drawRightEdge(oFan, oCData, Q.leftNeighbour());
	drawPoint(oFan, Q.iBottom(), Q.jLeft() ); drawTopEdge(oFan, oCData, Q.bottomNeighbour());
    drawPoint(oFan, Q.iBottom(), Q.jRight()); drawLeftEdge(oFan, oCData, Q.rightNeighbour());
    drawPoint(oFan, Q.iTop(),    Q.jRight()); drawBottomEdge(oFan, oCData, Q.topNeighbour());
	drawPoint(oFan, Q.iTop(),    Q.jLeft() );
	}
}
  
  
inline void svt_HeightField::drawRightEdge
(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const
{ 
  if (Q.valid())
    if (oCData.oSubdevInfo.subdevided(Q.index()))
	  {
	  drawRightEdge(oFan, oCData, Q.topRightChild());
	  drawPoint(oFan, Q.i(), Q.jRight());
      drawRightEdge(oFan, oCData, Q.bottomRightChild());
	  }
}


inline void svt_HeightField::drawLeftEdge
(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const
{
  if (Q.valid())
    if (oCData.oSubdevInfo.subdevided(Q.index()))
	  {
      drawLeftEdge(oFan, oCData, Q.bottomLeftChild());
      drawPoint(oFan, Q.i(), Q.jLeft());  
      drawLeftEdge(oFan, oCData, Q.topLeftChild());
	  }
}


inline void svt_HeightField::drawBottomEdge
(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const
{
  if (Q.valid())
    if (oCData.oSubdevInfo.subdevided(Q.index()))
	  {
	  drawBottomEdge(oFan, oCData, Q.bottomRightChild());
	  drawPoint(oFan, Q.iBottom(), Q.j());  
	  drawBottomEdge(oFan, oCData, Q.bottomLeftChild());
	  }
}


inline void svt_HeightField::drawTopEdge
(svt_triFan& oFan, const HF_CanvasData& oCData, const svt_Quad& Q) const
{ 
  if (Q.valid())
    if (oCData.oSubdevInfo.subdevided(Q.index()))
	  {
      drawTopEdge(oFan, oCData, Q.topLeftChild());
      drawPoint(oFan, Q.iTop(), Q.j());  
      drawTopEdge(oFan, oCData, Q.topRightChild());
    }
}



inline svt_vector4<Real32> svt_HeightField::point(short i, short j) const{
  return svt_vector4<Real32>( j *m_fGPDistX, (sizeY()-i-1)*m_fGPDistY, m_oElevationData[i][j]);
}

inline svt_vector4<Real32> svt_HeightField::centerPoint(const svt_Quad& Q) const {
  return point(Q.i(), Q.j());
}

inline svt_vector4<Real32> svt_HeightField::topLeftPoint(const svt_Quad& Q) const {
  return point(Q.iTop(), Q.jLeft());
}

inline svt_vector4<Real32> svt_HeightField::topRightPoint(const svt_Quad& Q) const {
  return point(Q.iTop(), Q.jRight());
}

inline svt_vector4<Real32> svt_HeightField::bottomLeftPoint(const svt_Quad& Q) const {
  return point(Q.iBottom(), Q.jLeft());
}

inline svt_vector4<Real32> svt_HeightField::bottomRightPoint(const svt_Quad& Q) const {
  return point(Q.iBottom(), Q.jRight());
}

inline void svt_HeightField::drawGL() 
{

  if (m_bBenchmarkRun)
	{
	run_benchmark();
	m_bBenchmarkRun=false;
	}

  if (isPositive(m_fEpsilon) && m_oQuadTree.size() > 0)
	drawSimplified();
  else
	drawAll();

  if (m_bDrawStat)
	drawStat();

}


inline void svt_HeightField::drawSimplified() 
{
  //
  // refine quadtree only for left eye, re-use refinement for right eye
  //
  if (!svt_renderingForRightEye() && !freeze() && !m_bAlreadyDrawn)
	refineQT();

  renderQT();

}

void svt_HeightField::increaseError()
{



  if (simplify())
	{
	  Real32 fEps = m_fEpsilon + 0.1;
	  setEpsilon(fEps);
	}
  else
	if (m_iDownsample+1 < sizeX())
	  m_iDownsample *= 2;

}

void svt_HeightField::decreaseError()
{
  if (simplify())
	{
	  Real32 fEps = m_fEpsilon - 0.1;
	  if (fEps>0)
		setEpsilon(fEps);
	}
  else
	if (m_iDownsample>=2)
	  m_iDownsample /= 2;

}


#endif
