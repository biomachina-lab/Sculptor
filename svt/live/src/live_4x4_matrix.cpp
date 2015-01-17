/***************************************************************************
                          live_4x4_matrix.cpp  -  description
                             -------------------
    begin                : Sep 03 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// includes
///////////////////////////////////////////////////////////////////////////////

#include <live.h>
#include <live_4x4_matrix.h>

#include <svt_iostream.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
#include <direct.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// public member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 * \param pMatName pointer to the name of the matrix.
 */
live_4x4_matrix::live_4x4_matrix(char *pMatName)
{
    name = pMatName;
    setId();
}

/**
 * Constructor.
 * \param pM pointer to a float array with 16 values (the initial values are copied from there)
 * \param pMatName pointer to the name of the matrix.
 */
live_4x4_matrix::live_4x4_matrix(float *pM, char *pMatName)
{
  name = pMatName;
  memcpy( mat, pM, 16*sizeof(float) );
}

/**
 * Constructor.
 * \param pMatrix pointer to another live_4x4_matrix object (the initial values are copied from there)
 * \param pMatName pointer to the name of the matrix.
 */
live_4x4_matrix::live_4x4_matrix(live_4x4_matrix *pMatrix, char *pMatName)
{
  name = pMatName;
  memcpy(mat, pMatrix->mat, 16*sizeof(float));
}

/**
 * print the content of the matrix to stdout.
 */
void live_4x4_matrix::print()
{
    int i,j;
    cout << "Matrix '" << name << "':" << endl;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            cout << mat[i][j];
            if (j<3) cout << ", ";
        }
        cout << endl;
    }
}

/**
 * set matrix to another matrix
 * \param pMat pointer to 16 float values from where the new data will be copied.
 */
void live_4x4_matrix::setToMatrix(float *pMat)
{
    memcpy( mat, pMat, 16*sizeof(float) );
}
/**
 * set matrix to a quaternian rotation.
 * Attention: The complete matrix will get overwritten!
 * \param fX the x rotation
 * \param fY the y rotation
 * \param fZ the z rotation
 * \param fW the w rotation
 */
void live_4x4_matrix::setToQuat(float fX, float fY, float fZ, float fW)
{
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = fX+fX; y2 = fY+fY; z2 = fZ+fZ;
    xx = fX*x2; xy = fX*y2; xz = fX*z2;
    yy = fY*y2; yz = fY*z2; zz = fZ*z2;
    wx = fW*x2; wy = fW*y2; wz = fW*z2;
    mat[0][0] = 1.0 - (yy + zz); mat[1][0] = xy - wz;
    mat[2][0] = xz + wy;         mat[0][3] = 0.0;

    mat[0][1] = xy + wz; mat[1][1] = 1.0 - (xx + zz);
    mat[2][1] = yz - wx; mat[1][3] = 0.0;

    mat[0][2] = xz - wy; mat[1][2] = yz + wx;
    mat[2][2] = 1.0 - (xx + yy);  mat[2][3] = 0.0;

    mat[3][0] = 0; mat[3][1] = 0;
    mat[3][2] = 0; mat[3][3] = 1;
}

/**
 * set rotational part (upper 3x3 matrix) of the matrix to a quaternian rotation.
 * \param fX the x rotation
 * \param fY the y rotation
 * \param fZ the z rotation
 * \param fW the w rotation
 */
void live_4x4_matrix::setRotationToQuat(float fX, float fY, float fZ, float fW)
{
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = fX+fX; y2 = fY+fY; z2 = fZ+fZ;
    xx = fX*x2; xy = fX*y2; xz = fX*z2;
    yy = fY*y2; yz = fY*z2; zz = fZ*z2;
    wx = fW*x2; wy = fW*y2; wz = fW*z2;
    mat[0][0] = 1.0 - (yy + zz); mat[1][0] = xy - wz;
    mat[2][0] = xz + wy;

    mat[0][1] = xy + wz; mat[1][1] = 1.0 - (xx + zz);
    mat[2][1] = yz - wx;

    mat[0][2] = xz - wy; mat[1][2] = yz + wx;
    mat[2][2] = 1.0 - (xx + yy);
}


/**
 * set matrix to another matrix
 * \param pMat pointer to 16 float values from where the new data will be copied.
 */
void live_4x4_matrix::setId()
{
    int i;
    float *m;

    m = (float *) mat;
    for (i=0; i<16; i++) *m++ = (float) 0.0;
    mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = (float) 1.0;
}

/**
 * set matrix to a rotation matrix.
 * Attention: The complete matrix will get overwritten!
 * \param iAxis the axis (0 - x axis, 1 - y axis, 2 - z axis)
 * \param fAlpha rotation value (radian value. If you have an angle value and want to calculate the radian: (angle*PI)/180.0f)
 */
void live_4x4_matrix::setRotation(int iAxis, float fAlpha)
{
    int i;
    float *m;
    m = (float *) mat;
    for (i=0; i<16; i++) *m++ = (float) 0.0;
    mat[3][3] = 1.0;
    if (iAxis == 0) {
        mat[0][0] = mat[3][3] = 1.0;
        mat[1][1] = cos(fAlpha); mat[1][2] = -sin(fAlpha);
        mat[2][1] = sin(fAlpha); mat[2][2] =  cos(fAlpha);
    } else if (iAxis == 1) {
        mat[1][1] = mat[3][3] = 1.0;
        mat[0][0] = cos(fAlpha); mat[0][2] = -sin(fAlpha);
        mat[2][0] = sin(fAlpha); mat[2][2] =  cos(fAlpha);
    } else {
        mat[2][2] = mat[3][3] = 1.0;
        mat[0][0] = cos(fAlpha); mat[0][1] = -sin(fAlpha);
        mat[1][0] = sin(fAlpha); mat[1][1] =  cos(fAlpha);
    }
}

/**
 * set matrix to a translation matrix.
 * Attention: The complete matrix will get overwritten!
 * \param fX x translation
 * \param fY y translation
 * \param fZ z translation
 */
void live_4x4_matrix::setTranslation(float fX, float fY, float fZ)
{
    setId();
    mat[3][0] = fX;
    mat[3][1] = fY;
    mat[3][2] = fZ;
}

/**
 * set matrix to scaling matrix.
 * Attention: The complete matrix will get overwritten!
 * \param fS scaling.
 */
void live_4x4_matrix::setScale(float fS)
{
    setId();
    mat[0][0] = fS;
    mat[1][1] = fS;
    mat[2][2] = fS;
}

/**
 * post multiply another matrix
 * \param m m[4][4] array with matrix to post multiply
 */
void live_4x4_matrix::postMult(float m[4][4])
{
    register int i, j, k;
    float temp[4][4];
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            temp[i][j] = 0.0;
            for (k=0; k<4; k++)
                temp[i][j] += mat[i][k] * m[k][j];
        }
    }
    memcpy(mat, temp, 16*sizeof(float));
}

/**
 * pre multiply another matrix
 * \param m m[4][4] array with matrix to pre multiply
 */
void live_4x4_matrix::preMult(float m[4][4])
{
    register int i, j, k;
    float temp[4][4];
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            temp[i][j] = 0.0;
            for (k=0; k<4; k++)
                temp[i][j] += m[i][k] * mat[k][j];
        }
    }
    memcpy(mat, temp, 16*sizeof(float));
}

/**
 * post multiply another matrix
 * \param pM pointer to other live_4x4_matrix
 */
void live_4x4_matrix::postMult(live_4x4_matrix *pM)
{
  postMult(pM->mat);
}

/**
 * pre multiply another matrix
 * \param pM pointer to other live_4x4_matrix
 */
void live_4x4_matrix::preMult(live_4x4_matrix *pM)
{
  preMult(pM->mat);
}

/**
 * add an rotation to this matrix.
 * \param iAxis the axis (0 - x axis, 1 - y axis, 2 - z axis)
 * \param fAlpha rotation value (radian value. If you have an angle value and want to calculate the radian: (angle*PI)/180.0f)
 */
void live_4x4_matrix::addRotation(int iAxis, float fAlpha)
{
    int i,j;
    float tmp[4][4];
    for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j] = (float) 0.0;
    tmp[3][3] = 1.0;
    if (iAxis == 0) {
        tmp[0][0] = tmp[3][3] = 1.0;
        tmp[1][1] = cos(fAlpha); tmp[1][2] = -sin(fAlpha);
        tmp[2][1] = sin(fAlpha); tmp[2][2] =  cos(fAlpha);
    } else if (iAxis == 1) {
        tmp[1][1] = tmp[3][3] = 1.0;
        tmp[0][0] = cos(fAlpha); tmp[0][2] = -sin(fAlpha);
        tmp[2][0] = sin(fAlpha); tmp[2][2] =  cos(fAlpha);
    } else {
        tmp[2][2] = tmp[3][3] = 1.0;
        tmp[0][0] = cos(fAlpha); tmp[0][1] = -sin(fAlpha);
        tmp[1][0] = sin(fAlpha); tmp[1][1] =  cos(fAlpha);
    }
    preMult(tmp);
}

/**
 * add a translation to the matrix.
 * \param fX x translation
 * \param fY y translation
 * \param fZ z translation
 */
void live_4x4_matrix::addTranslation(float fX, float fY, float fZ)
{
  mat[3][0] += fX;
  mat[3][1] += fY;
  mat[3][2] += fZ;
}

/**
 * add scaling to the matrix.
 * \param fS scaling.
 */
void live_4x4_matrix::addScale(float fS)
{
  int i,j;
  float tmp[4][4];
  for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j] = (float) 0.0;
  tmp[0][0] = fS;
  tmp[1][1] = fS;
  tmp[2][2] = fS;
  tmp[3][3] = 1.0;
  postMult(tmp);
}

/**
 * invert the matrix
 */
void live_4x4_matrix::invert()
{
  float inv[4][4];
  compute_inverse(mat, inv);
  memcpy(mat, inv, 16*sizeof(float));
}

/**
 * create a quaternion of the current matrix content.
 * \param pX pointer to the float variable where quaternion x rotation should be stored.
 * \param pY pointer to the float variable where quaternion y rotation should be stored.
 * \param pZ pointer to the float variable where quaternion z rotation should be stored.
 * \param pW pointer to the float variable where quaternion w rotation should be stored.
 */
void live_4x4_matrix::toQuat(float *x, float *y, float *z, float *w)
{
float  tr, s, q[4];
int    i, j, k;
int nxt[3] = {1, 2, 0};
  tr = mat[0][0] + mat[1][1] + mat[2][2];
  // check the diagonal
  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    *w = s / 2.0;
    s = 0.5 / s;
    *x = (mat[1][2] - mat[2][1]) * s;
    *y = (mat[2][0] - mat[0][2]) * s;
    *z = (mat[0][1] - mat[1][0]) * s;
  } else {                
    // diagonal is negative
    i = 0;
    if (mat[1][1] > mat[0][0]) i = 1;
    if (mat[2][2] > mat[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];
    s = sqrt ((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0);
    q[i] = s * 0.5;
    if (s != 0.0) s = 0.5 / s;
    q[3] = (mat[j][k] - mat[k][j]) * s;
    q[j] = (mat[i][j] + mat[j][i]) * s;
    q[k] = (mat[i][k] + mat[k][i]) * s;
    *x = q[0];
    *y = q[1];
    *z = q[2];
    *w = q[3];
  }
}

/**
 * parse a string of 16 float values and store it in the matrix
 * \param pStr pointer to the string.
 */
void live_4x4_matrix::parseString(char *str)
{
    unsigned int i = 0;
    char tmpstr[256];
    int tmpptr = 0;
    int valcnt = 0;
    float m[] = { 1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0 };

    while(i<strlen(str) && valcnt < 16)
    {
        if (str[i] != ' ' && i != strlen(str)-1)
        {
            tmpstr[tmpptr++] = str[i];
        } else {
            if (tmpptr != 0)
            {
                tmpstr[tmpptr] = 0;
                m[valcnt] = atof(tmpstr);
                valcnt++;
            }
            tmpptr = 0;
        }
        i++;
    }

    setToMatrix(m);
}

///////////////////////////////////////////////////////////////////////////////
// protected member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Performs the Lu decomposition for Gaussian elimination with partial
 * pivoting.  On entry:
 * \param xform3 array to be decomposed and after the call it contains the LU decomposition
 * \param pivot contains the pivot history...the original position numbers of the pivoted rows.
 */
int live_4x4_matrix::decomposition(float xform3[4][4], int pivot[4])
{
    register int i, j, step;
    register float temp, mfact;            /*temp & row multiplication factor*/
    register float abs_istep, abs_maxstep; /*used to get absolute values*/
    register int itemp, max;               /*temp & maximum coefficient*/

    for (i = 0; i <= 3; i++)
	pivot[i] = i;
    for (step = 0; step <= 3; step++) {
	max = step;
	for (i = step + 1; i <= 3; i++) {
	    abs_istep = xform3[i][step];
	    if (abs_istep < 0.0)
		abs_istep = -abs_istep;
	    abs_maxstep = xform3[max][step];
	    if (abs_maxstep < 0.0)
		abs_maxstep = -abs_maxstep;
	    if (abs_istep > abs_maxstep)
		max = i;
	}
	if (xform3[max][step] == 0.0) {
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

void live_4x4_matrix::solve(float xform3[4][4], float col[4], float result[4], int pivot[4])
{
    register int solv, i;
    float sum;              /*used for computing a sum while backsolving*/
    float y[4];             /*used for computing Ly=Pb*/
    /*Solve for y in Ly=Pb*/
    for (solv = 0; solv <= 3; solv++) {
	sum = 0.0;
	for (i = 0; i <= solv - 1; i++)
	    sum = sum + y[i] * xform3[solv][i];
	y[solv] = col[pivot[solv]] - sum;
    }
    /*Solve for x in Ux=y*/
    for (solv = 3; solv >= 0; solv--) {
	sum = 0.0;
	for (i = solv + 1; i <= 3; i++)
	    sum += result[i] * xform3[solv][i];
	result[solv] = (y[solv] - sum) / xform3[solv][solv];
    }
}

int live_4x4_matrix::compute_inverse(float xform1[4][4], float xform2[4][4])
{
    int pivot[4];       /*pivot history - used to remember original*/
			/*row numbers of the pivoted rows*/
    float idcol[4];     /*columns of the identity matrix*/
    float result[4];    /*inverse is solved one column at a time*/
    register int i, j;
    float txform[4][4];    /*temp copy of xform1*/

    memcpy(txform, xform1, 16*sizeof(float));
    if (!decomposition(txform, pivot)) return 0;
    /*Solve system with columns of identity matrix as solutions*/
    for (i = 0; i <= 3; i++) {
	for (j = 0; j <= 3; j++)
	    idcol[j] = 0.0;
	idcol[i] = 1.0;
	solve(txform, idcol, result, pivot);
	/* Resultant columns are the columns of inverse matrix*/
	for (j = 0; j <= 3; j++)
	    xform2[j][i] = result[j];
    }
    return 1;
}
