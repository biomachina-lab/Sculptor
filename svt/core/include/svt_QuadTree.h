/**
 * \file
 *
 * This file contains a collection of neccessary 
 * helper classes for the QuadTree used by svt_HeightField.
 * Due to performance reasons, the QuadTree itself is just a vector 
 * of QuadTreeNodes, that can be randomly accessed by the helper class
 * svt_Quad.
 *
 * Note that the classes specified in this file are for internal use with
 * svt_HeightField only.
 *
 * \author Frank Delonge
 */


#ifndef SVT_QUAD_TREE
#define SVT_QUAD_TREE

#include <svt_types.h>
#include <svt_stlVector.h>
#include <svt_streamableObject.h>

#include <svt_vector4.h>
#include <svt_Plane.h>


//
// internal function prototypes
//
// get position of least significant non-null bit 
static inline int leastSignificantBit(short i);



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Inernal class: svt_QuadTreeNode                                      //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class svt_QuadTreeNode : public svt_streamableObject
{
public:
  Real32 r;           // Bounding Sphere Radius
  Real32 gamma;       // Objectspace Error if quad is not further subdevided 

#ifdef SVT_HF_USE_SMALL_SPHERES
  Real32 z;           // Bounding-Sphere Z Coord
  svt_QuadTreeNode(Real32 R=0.0, Real32 Z=0.0, Real32 G=0.0) : r(R), gamma(G), z(Z) {}
  virtual void streamOutA(ostream& s) const {s << r << " " << z << " " << gamma << " ";}
  virtual void streamInA (istream& s) {s >> r >> z >> gamma;}
  virtual void streamOutB(svt_obfstream& s) const {s << r << z << gamma;}
  virtual void streamInB (svt_ibfstream& s) {s >> r >> z >> gamma;}

#else 
  svt_QuadTreeNode(Real32 R=0.0, Real32 G=0.0) : r(R), gamma(G) {}
  virtual void streamOutA(ostream& s) const {s << r << " " << gamma << " ";}
  virtual void streamInA (istream& s) {s >> r >> gamma;}
  virtual void streamOutB(svt_obfstream& s) const {s << r << gamma;}
  virtual void streamInB (svt_ibfstream& s) {s >> r >> gamma;}
#endif

  // use default copy-constuctor / destructor 


};

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_Quad                                                      //
//                                                                       //
//  Provides storage information to address a quad as well in            //
//  heightfield as in QuadTree.                                          //
//  Methods to generate neighbour+child descriptors                      //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


class DLLEXPORT svt_Quad
{

 private:

  // Lookup-table for index offsets
  // Calculation: LevelOffset(level) = (2^(2n)-1)/3
  static int sm_iLevelOffset[];

  // n: number of refinement levels as static member
  // size == 2^n+1
  // level = 0..n-1 
  static int sm_n;
  static int sm_iSize;


  // Quad storage information
  short m_i;                  // #Row in Heightfield Matrix    ( <=> Y )
  short m_j;                  // #Column in Heightfield Matrix ( <=> X ) 
  int   m_iIndex;             // index in QuadTreeNodes Vector
  char  m_iLevel;             // level in QuadTreeHirarchy

  // internal methods to compute level and index from i/j
  inline void computeLevel();
  inline void computeIndex();
  inline int rowsInLevel() const;
  inline int levelOffset() const;

  // set level to iLevel
  // if level is valid, return true
  // otherwise, mark quad as invalid and return false
  inline bool setLevel(char iLevel);


 public:


  static int N();
  static int sizeX();
  static int sizeY();
  static int levelOffset(int i);


  // default constructor (-> invalid quad)
  svt_Quad() : m_i(-1), m_j(-1), m_iIndex(-1), m_iLevel(0) {}

  // full constructor
  svt_Quad(short i, short j, int iIndex, char iLevel)
    : m_i(i) , m_j(j), m_iIndex(iIndex), 
      m_iLevel(iLevel) {}

  // constructor with matrix indices only
  // -> level and index will be computed 
  svt_Quad(short i, short j)
    : m_i(i) , m_j(j) {computeLevel(); computeIndex(); }

  // use default copy-constructor + destrcutor + assign-operator
  // memcpy of default copy-constructor should be most efficient in this case

  // init function: store n
  static void setSize(unsigned n, unsigned iSize);


  // query content
  inline short j() const;          // matrix row index of center point
  inline short jLeft() const;      // matrix row index of left points
  inline short jRight() const;     // matrix row index of right points

  inline short i() const;          // matrix column index of center point
  inline short iTop() const;       // matrix columm index of upper points 
  inline short iBottom() const;    // matrix column index of lower points

  inline char  level() const;
  inline int  index() const;
	
  // if compiler does not support NRV optimization:
  // turn following 8 methods into computational copy-constructors

  // create neighbouring quads
  inline svt_Quad rightNeighbour() const;
  inline svt_Quad leftNeighbour() const;
  inline svt_Quad topNeighbour() const;
  inline svt_Quad bottomNeighbour() const;

  // create child quads
  inline svt_Quad topLeftChild() const;
  inline svt_Quad topRightChild() const;
  inline svt_Quad bottomLeftChild() const;
  inline svt_Quad bottomRightChild() const;

  // get a child
  // parameter k: 0 1 2 3 = TopLeft BottomLeft BottomRight TopRight
  // to be called with the current child permutation
  inline svt_Quad child(int k) const;

  // get root
  static svt_Quad root();
  
  // check if this is a valid descriptor
  inline bool valid() const;

  // get quad width as number of spanned matrix indices
  inline short width() const;

  // marks the quad as invalid
  inline void setInvalid();


};

//
// inline class implementation
//

inline void svt_Quad::computeLevel() {
 m_iLevel = N() - 1 - leastSignificantBit(m_i);
}


inline void svt_Quad::computeIndex() {
  int j = jLeft() / width();  
  int i = iTop()  / width();
  m_iIndex = j+i*rowsInLevel() + levelOffset(); // #rows per level == #cols per level
}

inline bool svt_Quad::valid() const {
  return i() >= 0;
}

inline short svt_Quad::width() const {
  return 1 << (N()-level()); 
}

inline int svt_Quad::rowsInLevel() const { 
  return 1 << level(); 
}

inline int svt_Quad::levelOffset() const {
  return levelOffset(level());
}

inline short svt_Quad::j() const { 
  return m_j;
}

inline short svt_Quad::jLeft() const { 
  return j()-width()/2;
}

inline short svt_Quad::jRight() const { 
  return j()+width()/2;
}

inline short svt_Quad::i() const {
  return m_i;
}

inline short svt_Quad::iTop() const {
  return i()-width()/2;
}

inline short svt_Quad::iBottom() const {
  return i()+width()/2;
}

inline char  svt_Quad::level() const {
  return m_iLevel;
}

inline int  svt_Quad::index() const { 
  return m_iIndex;
}

inline bool svt_Quad::setLevel(char iLevel) { 
  m_iLevel=iLevel; 
  if ( level() >= N() ) { setInvalid(); return false;}
  return true;
}

inline void svt_Quad::setInvalid() {
  m_i = -1;
}


inline svt_Quad svt_Quad::topLeftChild() const {
  svt_Quad Q;
  if ( Q.setLevel(level()+1)) {
    Q.m_i = i() - width()/4;
    Q.m_j = j() - width()/4;
    Q.computeIndex();
  }
  return Q;
}

inline svt_Quad svt_Quad::topRightChild() const {
  svt_Quad Q;
  if ( Q.setLevel(level()+1)) {
    Q.m_i = i() - width()/4;
    Q.m_j = j() + width()/4;
    Q.computeIndex();
    }
  return Q;
}

inline svt_Quad svt_Quad::bottomLeftChild() const {
  svt_Quad Q;
  if ( Q.setLevel(level()+1)) {
    Q.m_i = i() + width()/4;
    Q.m_j = j() - width()/4;
    Q.computeIndex();
  }
  return Q;
}

inline svt_Quad svt_Quad::bottomRightChild() const {
  svt_Quad Q;
  if ( Q.setLevel(level()+1)) {
    Q.m_i = i() + width()/4;
    Q.m_j = j() + width()/4;
    Q.computeIndex();
  }
  return Q;
}

inline svt_Quad svt_Quad::child(int k) const { // k=0..3
  switch (k) {
    case(0): return topLeftChild(); 
    case(1): return bottomLeftChild(); 
    case(2): return bottomRightChild(); 
    case(3): return topRightChild(); 
  }
  
  // the next statement cannot be reaches -
  // it just suppressed a compiler warning...
  return topLeftChild(); 

}
  


inline svt_Quad svt_Quad::rightNeighbour() const {
  svt_Quad Q(*this);
  Q.m_j = j() + width();
  if ( Q.j() >= sizeX() ) Q.setInvalid();
  else Q.m_iIndex++;
  return Q;
}


inline svt_Quad svt_Quad::leftNeighbour() const {
  svt_Quad Q(*this);
  Q.m_j = j() - width();
  if ( Q.j() < 0  ) Q.setInvalid();
  else Q.m_iIndex--;
  return Q;
}


inline svt_Quad svt_Quad::topNeighbour() const {
  svt_Quad Q(*this);
  Q.m_i = i() - width();
  if ( Q.i() < 0 ) Q.setInvalid();
  else Q.m_iIndex -= rowsInLevel();
  return Q;
}


inline svt_Quad svt_Quad::bottomNeighbour() const {
  svt_Quad Q(*this);
  Q.m_i = i() + width();
  if ( Q.i() >= sizeY() ) Q.setInvalid();
  else Q.m_iIndex += rowsInLevel();
  return Q;
}




///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_TrianglePlane                                             //
//                                                                       //
//  class used to compute distance between a triangle and a point        //
//  2 distance methods are provided:                                     //
//      PointZDist(svt_vector4<Real32>) -> distance in z coordinate      //
//      PointDist(svt_vector4<Real32>)  -> real distance                 //
//                                                                       //
//  triangle points in contructor and init()-method must be specified    //
//  conter-clock-wise                                                    //
///////////////////////////////////////////////////////////////////////////

class svt_TrianglePlane {
  
 private:

  svt_Plane m_oTrianglePlain;

  // check if a given point is over the triangle
  // that is, the point is in the 3-sided column of the given triangle that
  // has the same direction as the triangle´s normal
  bool pointOverTriangle(const svt_vector4<Real32>& P) const;

  // planes that make up the 3-sided column
  svt_Plane m_oColPlane1; 
  svt_Plane m_oColPlane2; 
  svt_Plane m_oColPlane3; 

  //                                    ----> 
  // return distance of Point P to line P1 P2
  Real32 edgeDist(const svt_vector4<Real32>& P, const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2) const;

 public:

  svt_TrianglePlane(const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2, const svt_vector4<Real32>& P3) 
    { init(P1, P2, P3); }

  svt_TrianglePlane(const svt_TrianglePlane& that) 
    : m_oTrianglePlain(that.m_oTrianglePlain), m_oColPlane1(that.m_oColPlane1),
      m_oColPlane2(that.m_oColPlane2), m_oColPlane3(that.m_oColPlane3) {}

  virtual ~svt_TrianglePlane() {}

  void init(const svt_vector4<Real32>& P1, const svt_vector4<Real32>& P2, const svt_vector4<Real32>& P3);

  Real32 pointDist(const svt_vector4<Real32>& P) const;
  Real32 pointZDist(const svt_vector4<Real32>& P) const;


};



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_QuadSelectionBuffer                                       //
//                                                                       //
//  buffer to store selected quads                                       //
//  used to be in control over verctor resizing                          //
///////////////////////////////////////////////////////////////////////////



class svt_QuadSelectionBuffer {

 private:
  
  int      m_iSize;      // currently used size, not vector capacity!
  int      m_iLimit;
  int      m_iReserve;   // resreve to flush rest of backtrace queue
  unsigned m_iCapacity;

  vector<svt_Quad> m_oBuffer;

 


 public:

  //
  // constructors
  //

  // contructor with argunemts:
  // iCapacity:  initial capacity, will be enlarged if bUnilimited is true
  // bUnlimited: if true, enlarge capacity when neccessary
  //             otherwise, backtracing is stopped when limit is reached
  // n:          (2^n+1==size) needed to calculate reserve
  //             only neccessary when bUnlimited is false  
  svt_QuadSelectionBuffer(int iLimit=-1, int n=1) 
    : m_iSize(0), m_iLimit(iLimit), m_iReserve(3*(n-1)), m_iCapacity(50000) 
  {
	m_oBuffer.reserve(m_iCapacity);
	m_oBuffer.resize(m_iCapacity);
  }

  svt_QuadSelectionBuffer(const svt_QuadSelectionBuffer& that)
	: m_iSize(0), m_iLimit(that.m_iLimit), m_iReserve(that.m_iReserve), 
	  m_iCapacity(that.m_iCapacity)
  {
	m_oBuffer.reserve(m_iCapacity);
	m_oBuffer.resize(m_iCapacity);
  }

  virtual ~svt_QuadSelectionBuffer() {}

  void init(int iLimit=-1, int n=1) {
	m_iSize=0; m_iReserve=3*(n-1);
	m_oBuffer.reserve(m_iCapacity);
	m_oBuffer.resize(m_iCapacity);
	m_iLimit=iLimit;
  }
	
  void setLimit(unsigned iLimit) {
	m_iLimit=iLimit;
  }

  unsigned limit() const {return m_iLimit;}

  bool unlimited() const {return m_iLimit<=0;}

  void push(const svt_Quad& Q) {
	if (m_iSize >= int(m_oBuffer.size())-1)
	  {
		m_oBuffer.reserve(m_oBuffer.size()+m_iCapacity);
		m_oBuffer.resize(m_oBuffer.size()+m_iCapacity);
	  }
	m_oBuffer[m_iSize++] = Q;   
  }

  void clear() { m_iSize=0;}

  unsigned size() const {return m_iSize;}

  const svt_Quad& operator[] (unsigned i) const {return m_oBuffer[i];}

  bool almostFull() const { 
	return ( (!unlimited()) && (m_iSize+m_iReserve >= m_iLimit));
  }

};



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_QuadBacktraceStack                                        //
//                                                                       //
//  buffer manage backtracing the refinement of the quadtree             //
//  (avoids recursive calls)                                             //
///////////////////////////////////////////////////////////////////////////



struct svt_QuadExt {
  svt_Quad  Q;
  UInt32    cullInfo;
};


class svt_QuadBacktraceStack {
 private:
  unsigned m_iPos;
  vector<svt_QuadExt> m_oStack;

//   unsigned next(unsigned& i) {
// 	i++; if (i>=m_oQueue.size()) i=0;
// 	return i;
//  }

 public:
  svt_QuadBacktraceStack() 
	: m_iPos(0), m_oStack(0) 
  {}

  svt_QuadBacktraceStack(int n) 
	: m_oStack((n-1)*3) 
  {clear();}

  svt_QuadBacktraceStack(const svt_QuadBacktraceStack& that) 
	: m_oStack(that.m_oStack.size()) 
  {clear();}

  virtual ~svt_QuadBacktraceStack() {};

  void init(int n) {
	m_oStack.reserve((n-1)*3);
	m_oStack.resize((n-1)*3);
	clear();
  }
  
  void push(const svt_Quad& Q, int iCullInfo) {
	m_iPos++;
	m_oStack[m_iPos].Q = Q; m_oStack[m_iPos].cullInfo = iCullInfo;
  }

  const svt_QuadExt& pop() {
	m_iPos--;
	return m_oStack[m_iPos+1];
  }

  void clear() { 
	m_iPos = 0;
	if (m_oStack.size()>0) m_oStack[m_iPos].Q.setInvalid();
  }


};


///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_QuadSubdevInfo                                            //
//                                                                       //
//  stores subdevision information of all quadTree nodes                 //
//  This information needed to be extracted from svt_QuadTreeNode        //
//  because in contrast to the everything else in the QuadTree,          //
//  this information refers  to the current refinement                   //
//  and is therefore conext specific                                     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class svt_QuadSubdevInfo {

private:

  vector<UInt32> m_vNodeFlags;
  UInt32         m_iCurrSubdevFlag;
  

  
public:
  
  svt_QuadSubdevInfo() 
	: m_vNodeFlags(0), m_iCurrSubdevFlag(0) 
  {}

  svt_QuadSubdevInfo(const svt_QuadSubdevInfo& that) 
	: m_vNodeFlags(that.m_vNodeFlags.size()), m_iCurrSubdevFlag(0)
  {}
 
  virtual ~svt_QuadSubdevInfo()
  {}

  void init(UInt32 n) {
	m_vNodeFlags.resize( ((1<<(2*n))-1) / 3 ); 
	for (UInt32 i=0;i<m_vNodeFlags.size();i++)
	  m_vNodeFlags[i]=0; 
	m_iCurrSubdevFlag=1;
  }

  void advance() {m_iCurrSubdevFlag++;}
  void setSubdevided(UInt32 i) {m_vNodeFlags[i]=m_iCurrSubdevFlag;}
  bool subdevided(UInt32 i) const {return (m_vNodeFlags[i]==m_iCurrSubdevFlag);}

};

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: svt_QuadPerm                                                  //
//                                                                       //
//  stores permutation of current Quad Permutation depending on          //
//  current viewing directiono be extracted from svt_QuadTreeNode        //
//  because in contrast to the everything else in the QuadTree,          //
//  this information refers  to the current refinement                   //
//  and is therefore conext specific                                     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class svt_QuadPerm {

private:
  int vPerm[4];
  bool m_bFreeze;

public:
  svt_QuadPerm() : m_bFreeze(false) 
  {vPerm[0]=0; vPerm[1]=1; vPerm[2]=2; vPerm[3]=3;}

  // default copy constructor should work well

  virtual ~svt_QuadPerm() {}

  int operator[] (int i) const {return vPerm[i];}

  void set(const svt_vector4<Real32>& oViewDir);

  void freeze(bool b) {m_bFreeze=b;}
  bool freeze() const {return m_bFreeze;}
  bool toggleFreeze() {
	m_bFreeze=!m_bFreeze;
	if (m_bFreeze) cout << "freezing quad permutation at (" 
						<< vPerm[0] << " " << vPerm[1] << " " 
						<< vPerm[2] << " " << vPerm[3] << ")" << endl;
	else cout << "de-freezed quad permutation" << endl;
	return m_bFreeze;
  }

};

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Inernal Functions                                                    //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


// get position of least significant non-null bit 
static inline int leastSignificantBit(short i) {
  if (i<=0) return -1;
  for (unsigned j=0; j<sizeof(short)+8; j++) 
    if (i & (1<<j)) return j;
  return -1;
}

#endif
