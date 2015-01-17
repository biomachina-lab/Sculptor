/***************************************************************************
                          svt_matrix4f.h  -  description
                          ------------------------------
    begin                : Sep 03 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_MATRIX4F___H
#define SVT_MATRIX4F___H

#include <svt_basics.h>
#include <svt_types.h>
#include <math.h>
#include <svt_iostream.h>

class svt_vector4f;

#ifndef PI
#define PI 3.1415926535
#endif

/** A 4x4 matrix
  *@author Herwig Zilken
  */
class DLLEXPORT svt_matrix4f 
{
protected:
  int decomposition(Real32 xform3[4][4], int pivot[4]);
  void solve(Real32 xform3[4][4], Real32 col[4], Real32 result[4], int pivot[4]);
  int compute_inverse(Real32 xform1[4][4], Real32 xform2[4][4]);

  void setName(const char *);

  const char* name;

public:
    Matrix4f mat;
    svt_matrix4f(const char *mat_name = NULL);
    svt_matrix4f(Matrix4f &m, const char *mat_name=NULL);
    svt_matrix4f(const svt_matrix4f &matrix, const char *mat_name=NULL);
    virtual ~svt_matrix4f();

  inline const Real32 & operator()(int i, int j)
  {
    return mat[i][j];
  };

  inline Real32 getAt(int i, int j) const
  {
    return mat[i][j];
  };

  inline void setMat(int i, int j, Real32 value )
  {
    mat[i][j] = value;
  };

  /** dumps the value of the matrix */
  void dump(void) const;

  /** sets the matrix to identity */
  virtual void setToId(void);

  /** sets the matrix from a matrix4f */
  virtual void setFromMatrix(Matrix4f &m);

  /** sets the matrix from a svt_matrix4f */
  virtual void setFromMatrix(const svt_matrix4f &m);

  /** sets the matrix from a quaternion */
  virtual void setFromQuaternion(Quaternion &q);

  /** sets the matrix from a translation (vector) */
  virtual void setFromTranslation(Vector3f &v);

  /** sets the matrix from a translation (vector) */
  virtual void setFromTranslation(Real32, Real32, Real32);

  /** sets the rotation part of the matrix from a quaternion */
  virtual void setRotationFromQuat(Quaternion &q);

  /** sets the translation part of the matrix from a translation */
  virtual void setTranslationFromTranslation(Vector3f &v);

  /** the rotation part of the matrix from axis/angle */
  virtual void setRotationFromAA(int axis, Real32 alpha);

  /** sets the matrix to a uniform scaling-matrix */
  virtual void setFromScale(Real32 s);

  /** sets the matrix to a non-uniform scaling-matrix */
  virtual void setFromScale(Real32 s1, Real32 s2, Real32 s3);

  /** multiplies a matrix from right */
  virtual void postMult(Matrix4f &m);
  
  /** multiplies a matrix from left */
  virtual void preMult(Matrix4f &m);

  /** multiplies a svt_matrix4f from right */
  virtual void postMult(svt_matrix4f &m);

  /** multiplies a svt_matrix4f from left */
  virtual void preMult(svt_matrix4f &m);

  /** adds a rotation (from left) */
  virtual void preAddRotation(int axis, Real32 alpha);

  /** adds a rotation (from right) */
  virtual void postAddRotation(int axis, Real32 alpha);

  /** adds a rotation around a vector */
  virtual void postAddRotation(Vector3f &v, Real32 fAlpha);

  /** adds a rotation around a vector */
  virtual void postAddRotation(svt_vector4f &v, Real32 fAlpha);

  /** adds a translation (from left) */
  virtual void preAddTranslation(Vector3f &v);

  /** adds a translation (from right) */
  virtual void postAddTranslation(Vector3f &v);

  /** adds a translation (from left) */
  virtual void preAddTranslation(Real32, Real32, Real32);

  /** adds a translation (from right) */
  virtual void postAddTranslation(Real32, Real32, Real32);

  /** adds a scaling (from left) */
  virtual void preAddScale(Real32 s);

  /** adds a scaling (from right) */
  virtual void postAddScale(Real32 s);
  /** adds a scaling (from right) */
  void postAddScale(Real32 s1, Real32 s2, Real32 s3);

  /** inverts a matrix */
  virtual void invert(void);

  /** normalizes the rows of the matrix */
  virtual void normalizeRows();

  /** parses a string of 16 Real32 values and stores it in the matrix */
  virtual void setFromString(const char *str);

  /** copies the matrix into m */
  void getMatrix(Matrix4f &m);

  /** copies the translation-part into the vector v */
  void getTranslation(Vector3f &v);

  /** returns the x translation */
  Real32 getTranslationX();
  /** sets the x translation */
  void setTranslationX(Real32 x);

  /** returns the y translation */
  Real32 getTranslationY();
  /** sets the x translation */
  void setTranslationY(Real32 y);

  /** returns the z translation */
  Real32 getTranslationZ();
  /** sets the x translation */
  void setTranslationZ(Real32 z);

  /** converts the rotation-part of the matrix to a quaternion */
  void getQuat(Quaternion &quat);

  /** multiplies the matrix to the opengl modelview-matrix */
  void applyToGL(void);

  /** decomposition of the matrix in rotation, translation and scale */
  void unmake_matrix( Real32 *rotx, Real32 *roty, Real32 *rotz, Real32 *scale, Real32 *transx, Real32 *transy, Real32 *transz );

  

};





#endif
