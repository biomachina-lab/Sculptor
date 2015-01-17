/***************************************************************************
                          svt_matrix4f.cpp  -  description
                             -------------------
    begin                : Sep 03 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_matrix4f.h>
#include <svt_vector4f.h>
#include <svt_opengl.h>
// clib includes
#include <svt_iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

svt_matrix4f::svt_matrix4f(const char *mat_name)
{
  setName(mat_name);
  setToId();
}

svt_matrix4f::svt_matrix4f(Matrix4f &m, const char *mat_name)
{
  // memcpy( to, from, size );
  setName(mat_name);
  setFromMatrix(m);
}

svt_matrix4f::svt_matrix4f(const svt_matrix4f &matrix, const char *mat_name)
{
  setName(mat_name);
  setFromMatrix(matrix);
}

svt_matrix4f::~svt_matrix4f()
{
}

void svt_matrix4f::setFromMatrix(Matrix4f &m)
{
  memcpy(mat, m, sizeof(Matrix4f));
}

void svt_matrix4f::setFromMatrix(const svt_matrix4f &m)
{
  memcpy(mat, m.mat, sizeof(Matrix4f));
}


void svt_matrix4f::setName(const char *_name)
{
  name = _name;
}

void svt_matrix4f::dump(void) const
{
  int i,j;
  if (name == NULL)
      cout << "dump of matrix:" << endl;
  else
      cout << "dump of matrix '" << name << "':" << endl;
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      cout << mat[i][j];
      if (j<3) cout << ", ";
    }
    cout << endl;
  }
  cout << endl;
}

void svt_matrix4f::setToId(void)
{
  mat[0][1] = mat[0][2] = mat[0][3] =
  mat[1][0] = mat[1][2] = mat[1][3] =
  mat[2][0] = mat[2][1] = mat[2][3] =
  mat[3][0] = mat[3][1] = mat[3][2] = static_cast<Real32> (0.0);
  mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = static_cast<Real32> (1.0);
}

void svt_matrix4f::setFromQuaternion(Quaternion &q)
{
Real32 wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  // calculate coefficients
  x2 = q[0]+q[0]; y2 = q[1]+q[1]; z2 = q[2]+q[2];
  xx = q[0]*x2; xy = q[0]*y2; xz = q[0]*z2;
  yy = q[1]*y2; yz = q[1]*z2; zz = q[2]*z2;
  wx = q[3]*x2; wy = q[3]*y2; wz = q[3]*z2;
  mat[0][0] = 1.0f - (yy + zz); mat[1][0] = xy - wz;
  mat[2][0] = xz + wy;         mat[0][3] = 0.0f;
               
  mat[0][1] = xy + wz; mat[1][1] = 1.0f - (xx + zz);
  mat[2][1] = yz - wx; mat[1][3] = 0.0f;

  mat[0][2] = xz - wy; mat[1][2] = yz + wx;
  mat[2][2] = 1.0f - (xx + yy);  mat[2][3] = 0.0f;

  mat[3][0] = 0.0f; mat[3][1] = 0.0f;
  mat[3][2] = 0.0f; mat[3][3] = 1.0f;
}

void svt_matrix4f::setFromTranslation(Vector3f &v)
{
  setToId();
  mat[3][0] = v[0];
  mat[3][1] = v[1];
  mat[3][2] = v[2];
}

void svt_matrix4f::setFromTranslation(Real32 x, Real32 y, Real32 z)
{
  setToId();
  mat[3][0] = x;
  mat[3][1] = y;
  mat[3][2] = z;
}

void svt_matrix4f::setRotationFromQuat(Quaternion &q)
// sets only the upper 3x3 sub-matrix to quaternion
{
Real32 wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  // calculate coefficients
  x2 = q[0]+q[0]; y2 = q[1]+q[1]; z2 = q[2]+q[2];
  xx = q[0]*x2; xy = q[0]*y2; xz = q[0]*z2;
  yy = q[1]*y2; yz = q[1]*z2; zz = q[2]*z2;
  wx = q[3]*x2; wy = q[3]*y2; wz = q[3]*z2;
  mat[0][0] = 1.0f - (yy + zz); mat[1][0] = xy - wz;
  mat[2][0] = xz + wy;
               
  mat[0][1] = xy + wz; mat[1][1] = 1.0f - (xx + zz);
  mat[2][1] = yz - wx;

  mat[0][2] = xz - wy; mat[1][2] = yz + wx;
  mat[2][2] = 1.0f - (xx + yy);
}

void svt_matrix4f::setTranslationFromTranslation(Vector3f &v)
{
  mat[3][0] = v[0];
  mat[3][1] = v[1];
  mat[3][2] = v[2];
}

void svt_matrix4f::setRotationFromAA(int axis, Real32 alpha)
// meaning of axis values: (0,1,2) = (x, y, z)
{
  int i, j;
  for (i=0; i<4; i++) for (j=0; j<4; j++) mat[i][j] = static_cast<Real32>(0.0);
  if (axis == 0) {
    mat[0][0] = mat[3][3] = 1.0f;
    mat[1][1] = (Real32)cos(alpha); mat[1][2] = -(Real32)sin(alpha);
    mat[2][1] = (Real32)sin(alpha); mat[2][2] =  (Real32)cos(alpha);
  } else if (axis == 1) {
    mat[1][1] = mat[3][3] = 1.0f;
    mat[0][0] = (Real32)cos(alpha); mat[0][2] = -(Real32)sin(alpha);
    mat[2][0] = (Real32)sin(alpha); mat[2][2] =  (Real32)cos(alpha);
  } else {
    mat[2][2] = mat[3][3] = 1.0f;
    mat[0][0] = (Real32)cos(alpha); mat[0][1] = -(Real32)sin(alpha);
    mat[1][0] = (Real32)sin(alpha); mat[1][1] =  (Real32)cos(alpha);
  }
}

void svt_matrix4f::setFromScale(Real32 s)
{
  setToId();
  mat[0][0] = s;
  mat[1][1] = s;
  mat[2][2] = s;
}

void svt_matrix4f::setFromScale(Real32 s1, Real32 s2, Real32 s3)
{
  setToId();
  mat[0][0] = s1;
  mat[1][1] = s2;
  mat[2][2] = s3;
}

void svt_matrix4f::postMult(Matrix4f &m)
{
   register int i, j, k;
   Matrix4f temp;
   for (i=0; i<4; i++) {
      for (j=0; j<4; j++) {
         temp[i][j] = 0.0f;
         for (k=0; k<4; k++)
            temp[i][j] += mat[i][k] * m[k][j];
      }
   }
   setFromMatrix(temp);
}

void svt_matrix4f::preMult(Matrix4f &m)
{
   register int i, j, k;
   Matrix4f temp;
   for (i=0; i<4; i++) {
      for (j=0; j<4; j++) {
         temp[i][j] = 0.0f;
         for (k=0; k<4; k++)
            temp[i][j] += m[i][k] * mat[k][j];
      }
   }
   setFromMatrix(temp);
}


void svt_matrix4f::postMult(svt_matrix4f &m)
{
  postMult(m.mat);
}

void svt_matrix4f::preMult(svt_matrix4f &m)
{
  preMult(m.mat);
}

void svt_matrix4f::preAddRotation(int axis, Real32 alpha)
// meaning of axis values: (0,1,2) = (x, y, z)
{
  int i,j;
  Matrix4f tmp;
  for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j] = static_cast<Real32>(0.0);
  tmp[3][3] = 1.0;
  if (axis == 0) {
    tmp[0][0] = tmp[3][3] = 1.0f;
    tmp[1][1] = (Real32)cos(alpha); tmp[1][2] = -(Real32)sin(alpha);
    tmp[2][1] = (Real32)sin(alpha); tmp[2][2] =  (Real32)cos(alpha);
  } else if (axis == 1) {
    tmp[1][1] = tmp[3][3] = 1.0f;
    tmp[0][0] = (Real32)cos(alpha); tmp[0][2] = -(Real32)sin(alpha);
    tmp[2][0] = (Real32)sin(alpha); tmp[2][2] =  (Real32)cos(alpha);
  } else {
    tmp[2][2] = tmp[3][3] = 1.0f;
    tmp[0][0] = (Real32)cos(alpha); tmp[0][1] = -(Real32)sin(alpha);
    tmp[1][0] = (Real32)sin(alpha); tmp[1][1] =  (Real32)cos(alpha);
  }
  preMult(tmp);
}

void svt_matrix4f::postAddRotation(int axis, Real32 alpha)
// meaning of axis values: (0,1,2) = (x, y, z)
{
  int i,j;
  Matrix4f tmp;
  for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j] = static_cast<Real32>(0.0);
  tmp[3][3] = 1.0;
  if (axis == 0) {
    tmp[0][0] = tmp[3][3] = 1.0f;
    tmp[1][1] = (Real32)cos(alpha); tmp[1][2] = -(Real32)sin(alpha);
    tmp[2][1] = (Real32)sin(alpha); tmp[2][2] =  (Real32)cos(alpha);
  } else if (axis == 1) {
    tmp[1][1] = tmp[3][3] = 1.0f;
    tmp[0][0] = (Real32)cos(alpha); tmp[0][2] = -(Real32)sin(alpha);
    tmp[2][0] = (Real32)sin(alpha); tmp[2][2] =  (Real32)cos(alpha);
  } else {
    tmp[2][2] = tmp[3][3] = 1.0f;
    tmp[0][0] = (Real32)cos(alpha); tmp[0][1] = -(Real32)sin(alpha);
    tmp[1][0] = (Real32)sin(alpha); tmp[1][1] =  (Real32)cos(alpha);
  }
  postMult(tmp);
}

void svt_matrix4f::postAddRotation(Vector3f &v, Real32 fAlpha)
{
    Matrix4f tmp;
    int i,j;
    for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j] = static_cast<Real32>(0.0);
    tmp[3][3] = 1.0;

    tmp[0][0] = (Real32)cos(fAlpha) + (v[0]*v[0]*(1.0f-(Real32)cos(fAlpha)));
    tmp[0][1] = (v[0]*v[1]*(1.0f-(Real32)cos(fAlpha))) - (v[2]*(Real32)sin(fAlpha));
    tmp[0][2] = (v[0]*v[2]*(1.0f-(Real32)cos(fAlpha))) + (v[1]*(Real32)sin(fAlpha));

    tmp[1][0] = (v[0]*v[1]*(1.0f-(Real32)cos(fAlpha))) + (v[2]*(Real32)sin(fAlpha));
    tmp[1][1] = (Real32)cos(fAlpha) + (v[1]*v[1]*(1.0f-(Real32)cos(fAlpha)));
    tmp[1][2] = (v[1]*v[2]*(1.0f-(Real32)cos(fAlpha))) - (v[0]*(Real32)sin(fAlpha));

    tmp[2][0] = (v[0]*v[2]*(1.0f-(Real32)cos(fAlpha))) - (v[1]*(Real32)sin(fAlpha));
    tmp[2][1] = (v[1]*v[2]*(1.0f-(Real32)cos(fAlpha))) + (v[0]*(Real32)sin(fAlpha));
    tmp[2][2] = (Real32)cos(fAlpha) + (v[2]*v[2]*(1-(Real32)cos(fAlpha)));

    postMult(tmp);
}

void svt_matrix4f::postAddRotation(svt_vector4f &v, Real32 fAlpha)
{
    Vector3f oV = { v.x(), v.y(), v.z() };
    postAddRotation(oV, fAlpha);
}

void svt_matrix4f::preAddTranslation(Vector3f &v)
{
  mat[3][0] = v[0]*mat[0][0] + v[1]*mat[1][0] + v[2]*mat[2][0] + mat[3][0];
  mat[3][1] = v[0]*mat[1][0] + v[1]*mat[1][1] + v[2]*mat[2][1] + mat[3][1];
  mat[3][2] = v[0]*mat[2][0] + v[1]*mat[2][1] + v[2]*mat[2][2] + mat[3][2];
}

void svt_matrix4f::postAddTranslation(Vector3f &v)
{
  mat[3][0] += v[0];
  mat[3][1] += v[1];
  mat[3][2] += v[2];
}

void svt_matrix4f::preAddTranslation(Real32 x, Real32 y, Real32 z)
{
  mat[3][0] = x*mat[0][0] + y*mat[1][0] + z*mat[2][0] + mat[3][0];
  mat[3][1] = x*mat[0][1] + y*mat[1][1] + z*mat[2][1] + mat[3][1];
  mat[3][2] = x*mat[0][2] + y*mat[1][2] + z*mat[2][2] + mat[3][2];
}

void svt_matrix4f::postAddTranslation(Real32 x, Real32 y, Real32 z)
{
  mat[3][0] += x;
  mat[3][1] += y;
  mat[3][2] += z;
}

void svt_matrix4f::preAddScale(Real32 s)
{
  mat[0][0] *= s; mat[0][1] *= s; mat[0][2] *= s;
  mat[1][0] *= s; mat[1][1] *= s; mat[1][2] *= s;
  mat[2][0] *= s; mat[2][1] *= s; mat[2][2] *= s;
}

void svt_matrix4f::postAddScale(Real32 s)
{
    postAddScale(s, s, s);

    //mat[0][0] *= s; mat[0][1] *= s; mat[0][2] *= s;
    //mat[1][0] *= s; mat[1][1] *= s; mat[1][2] *= s;
    //mat[2][0] *= s; mat[2][1] *= s; mat[2][2] *= s;
    //mat[3][0] *= s; mat[3][1] *= s; mat[3][2] *= s;
}

void svt_matrix4f::postAddScale(Real32 s1, Real32 s2, Real32 s3)
{
    Matrix4f tmp;
    int i,j;
    for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j] = static_cast<Real32>(0.0f);
    tmp[3][3] = 1.0f;
    tmp[0][0] = s1;
    tmp[1][1] = s2;
    tmp[2][2] = s3;
    postMult(tmp);
}

int svt_matrix4f::decomposition(Real32 xform3[4][4], int pivot[4])
/* Performs the Lu decomposition for Gaussian elimination with partial
 * pivoting.  On entry:
 *   xform3 - array to be decomposed
 *   On exit:
 *   xform3 - contains the LU decomposition
 *   pivot - contains the pivot history...the original position numbers
 *	of the pivoted rows.
 */
{
    register int i, j, step;
    register Real32 temp, mfact;            /*temp & row multiplication factor*/
    register Real32 abs_istep, abs_maxstep; /*used to get absolute values*/
    register int itemp, max;               /*temp & maximum coefficient*/

    for (i = 0; i <= 3; i++)
	pivot[i] = i;
    for (step = 0; step <= 3; step++) {
	max = step;
	for (i = step + 1; i <= 3; i++) {
	    abs_istep = xform3[i][step];
	    if (abs_istep < 0.0f)
		abs_istep = -abs_istep;
	    abs_maxstep = xform3[max][step];
	    if (abs_maxstep < 0.0f)
		abs_maxstep = -abs_maxstep;
	    if (abs_istep > abs_maxstep)
		max = i;
	}
	if (xform3[max][step] == 0.0f) {
	    printf("Couldn't invert a transform.\n");
	    return 0;
	}
	if (max != step) {
	    for (j = 0; j <= 3; j++) {
		temp = xform3[step][j];
		xform3[step][j] = xform3[max][j];
		xform3[max][j] = temp;
	    }
	    itemp = pivot[step];
	    pivot[step] = pivot[max];
	    pivot[max] = itemp;
	}
	for (i = step + 1; i <= 3; i++) {
	    mfact = -xform3[i][step] / xform3[step][step];
	    xform3[i][step] = -mfact;
	    for (j = step + 1; j <= 3; j++)
		xform3[i][j] = mfact * xform3[step][j] + xform3[i][j];
	}
    }
    return 1;
}

void svt_matrix4f::solve(Real32 xform3[4][4], Real32 col[4], Real32 result[4], int pivot[4])
{
    register int solv, i;
    Real32 sum;              /*used for computing a sum while backsolving*/
    Real32 y[4];             /*used for computing Ly=Pb*/
    /*Solve for y in Ly=Pb*/
    for (solv = 0; solv <= 3; solv++) {
	sum = 0.0f;
	for (i = 0; i <= solv - 1; i++)
	    sum = sum + y[i] * xform3[solv][i];
	y[solv] = col[pivot[solv]] - sum;
    }
    /*Solve for x in Ux=y*/
    for (solv = 3; solv >= 0; solv--) {
	sum = 0.0f;
	for (i = solv + 1; i <= 3; i++)
	    sum += result[i] * xform3[solv][i];
	result[solv] = (y[solv] - sum) / xform3[solv][solv];
    }
}


int svt_matrix4f::compute_inverse(Real32 xform1[4][4], Real32 xform2[4][4])
{
    int pivot[4];       /*pivot history - used to remember original*/
			/*row numbers of the pivoted rows*/
    Real32 idcol[4];     /*columns of the identity matrix*/
    Real32 result[4];    /*inverse is solved one column at a time*/
    register int i, j;
    Real32 txform[4][4];    /*temp copy of xform1*/

    memcpy(txform, xform1, 16*sizeof(Real32));
    if (!decomposition(txform, pivot)) return 0;
    /*Solve system with columns of identity matrix as solutions*/
    for (i = 0; i <= 3; i++) {
	for (j = 0; j <= 3; j++)
	    idcol[j] = 0.0f;
	idcol[i] = 1.0f;
	solve(txform, idcol, result, pivot);
	/* Resultant columns are the columns of inverse matrix*/
	for (j = 0; j <= 3; j++)
	    xform2[j][i] = result[j];
    }
    return 1;
}

void svt_matrix4f::getMatrix(Matrix4f &m)
{
  memcpy(m, mat, sizeof(Matrix4f));
}

void svt_matrix4f::invert(void)
{
  Matrix4f inv;
  compute_inverse(mat, inv);
  memcpy(mat, inv, sizeof(Matrix4f));
}

void svt_matrix4f::getTranslation(Vector3f &v)
{
    v[0] = mat[3][0];
    v[1] = mat[3][1];
    v[2] = mat[3][2];
}

Real32 svt_matrix4f::getTranslationX()
{
    return mat[3][0];
}
void svt_matrix4f::setTranslationX(Real32 x)
{
    mat[3][0] = x;
}

Real32 svt_matrix4f::getTranslationY()
{
    return mat[3][1];
}
void svt_matrix4f::setTranslationY(Real32 y)
{
    mat[3][1] = y;
}

Real32 svt_matrix4f::getTranslationZ()
{
    return mat[3][2];
}
void svt_matrix4f::setTranslationZ(Real32 z)
{
    mat[3][2] = z;
}

void svt_matrix4f::normalizeRows()
{
    Real32 s;
    s = (Real32)sqrt((*this)(0,0)*(*this)(0,0) + (*this)(0,1)*(*this)(0,1) + (*this)(0,2)*(*this)(0,2));
    setMat(0,0, (*this)(0,0)/s); setMat(0,1, (*this)(0,1)/s); setMat(0,2, (*this)(0,2)/s);

    s = (Real32)sqrt((*this)(1,0)*(*this)(1,0) + (*this)(1,1)*(*this)(1,1) + (*this)(1,2)*(*this)(1,2));
    setMat(1,0, (*this)(1,0)/s); setMat(1,1, (*this)(1,1)/s); setMat(1,2, (*this)(1,2)/s);

    s = (Real32)sqrt((*this)(2,0)*(*this)(2,0) + (*this)(2,1)*(*this)(2,1) + (*this)(2,2)*(*this)(2,2));
    setMat(2,0, (*this)(2,0)/s); setMat(2,1, (*this)(2,1)/s); setMat(2,2, (*this)(2,2)/s);
}

void svt_matrix4f::getQuat(Quaternion &quat)
{
    Real32  tr, s, q[4];
    int    i, j, k;
    int nxt[3] = {1, 2, 0};
    tr = mat[0][0] + mat[1][1] + mat[2][2];
    // check the diagonal
    if (tr > 0.0) {
        s = (Real32)sqrt (tr + 1.0f);
        quat[3] = s / 2.0f;
        s = 0.5f / s;
        quat[0] = (mat[1][2] - mat[2][1]) * s;
        quat[1] = (mat[2][0] - mat[0][2]) * s;
        quat[2] = (mat[0][1] - mat[1][0]) * s;
    } else {
        // diagonal is negative
        i = 0;
        if (mat[1][1] > mat[0][0]) i = 1;
        if (mat[2][2] > mat[i][i]) i = 2;
        j = nxt[i];
        k = nxt[j];
        s = (Real32)sqrt ((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0f);
        q[i] = s * 0.5f;
        if (s != 0.0f) s = 0.5f / s;
        q[3] = (mat[j][k] - mat[k][j]) * s;
        q[j] = (mat[i][j] + mat[j][i]) * s;
        q[k] = (mat[i][k] + mat[k][i]) * s;
        quat[0] = q[0];
        quat[1] = q[1];
        quat[2] = q[2];
        quat[3] = q[3];
    }
}

void svt_matrix4f::applyToGL(void)
{
  glMultMatrixf(static_cast<const Real32 *> (&(mat[0][0])) );
}

void svt_matrix4f::setFromString(const char *str)
{
    unsigned int i = 0;
    int j, k;
    char tmpstr[256];
    int tmpptr = 0;
    int valcnt = 0;
    Matrix4f m = { {1.0f, 0.0f, 0.0f, 0.0f},
                   {0.0f, 1.0f, 0.0f, 0.0f},
                   {0.0f, 0.0f, 1.0f, 0.0f},
                   {0.0f, 0.0f, 0.0f, 1.0f} };

    while(i<strlen(str) && valcnt < 16)
    {
        j = valcnt / 4;
        k = valcnt % 4;
        if (str[i] != ' ' && i != strlen(str)-1)
        {
            tmpstr[tmpptr++] = str[i];
        } else {
            if (tmpptr != 0)
            {
                tmpstr[tmpptr] = 0;
                m[j][k] = static_cast<Real32>(atof(tmpstr));
                //printf("m[%i] = %f\n", valcnt,m[valcnt]);
                valcnt++;
            }
            tmpptr = 0;
        }
        i++;
    }
    setFromMatrix(m);
}

#define EPS 0.000001

void svt_matrix4f::unmake_matrix( Real32 *rotx, Real32 *roty, Real32 *rotz, Real32 *scale, Real32 *transx, Real32 *transy, Real32 *transz )
{
  Real32  sx, sy, sz, cx, cy, cz;
  int i, j;
  Matrix4f nat;

  mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
  mat[1][0] = 0; mat[1][1] = 0; mat[1][2] = -1;
  mat[2][0] = 0; mat[2][1] = 1; mat[2][2] = 0;

  Real32 scalex, scaley, scalez, scaleinv, cxa, cxb, cxinv;

  /* translation */
  *transx = mat[3][0];
  *transy = mat[3][1];
  *transz = mat[3][2];

  /* scale */
  scalex = scaley = scalez = 0.0;
  for (i=0; i<3; i++) {
    scalex += mat[0][i] * mat[0][i];
    scaley += mat[1][i] * mat[1][i];
    scalez += mat[2][i] * mat[2][i];
  }
  //if (fabs(scalex - scaley) > EPS || fabs(scalex - scalez) > EPS) {
  //  printf("problem1 %f %f %f\n", scalex, scaley, scalez);
  //}
  *scale = (Real32)sqrt((scalex + scaley + scalez)/3.0);
  scaleinv = (Real32)fabs(*scale) > EPS ? 1.0f / *scale : 1.0f / EPS;

  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      nat[j][i] = scaleinv * mat[j][i];
    }
  }

  /* rotation */
  sx = -nat[2][1];

  cxa = (Real32)sqrt(nat[2][0]*nat[2][0] + nat[2][2]*nat[2][2]);
  cxb = (Real32)sqrt(nat[0][1]*nat[0][1] + nat[1][1]*nat[1][1]);

  //if (fabs(cxa - cxb) > EPS) {
  //  printf("problem2 %f %f\n", cxa, cxb);
  //}
  /* the sign of cx does not matter;
     it is an ambiguity in 3-D rotations:
     (rotz, rotx, roty) = (180+rotz, 180-rotx, 180+roty) */
  cx = (cxa + cxb) / 2.0f;
  if (fabs(cx) > EPS) {
    cxinv = 1.0f / cx;
    sy = nat[2][0] * cxinv;
    cy = nat[2][2] * cxinv;
    sz = nat[0][1] * cxinv;
    cz = nat[1][1] * cxinv;
  }
  else {
    /* if cx == 0 then roty and rotz are ambiguous:
       assume rotx = 0.0 */
    sy = 0.0f;
    cy = 1.0f;
    sz = nat[0][2];
    cz = nat[1][2];
  }

  //printf("Sinus: %f, %f, %f\n", sx, sy, sz);
  //printf("Cosinus: %f, %f, %f\n", cx, cy, cz);

  *rotx = 57.2957f * (Real32)atan2(sx, cx);
  *roty = 57.2957f * (Real32)atan2(sy, cy);
  *rotz = 57.2957f * (Real32)atan2(sz, cz);
  // setFromMatrix(static_cast<Matrix4f &>(&m));
  // setFromMatrix(m);
}


