/***************************************************************************
                          live_4x4_matrix.h
                          -------------------
    begin                : 19.04.2001
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_4X4_MATRIX_H
#define LIVE_4X4_MATRIX_H

#include <live_win.h>

#include <stdio.h>
#include <math.h>

/** A 4x4 matrix
  *@author Herwig Zilken
  */
class DLLEXPORT live_4x4_matrix
{
protected:
    /**
     * Performs the Lu decomposition for Gaussian elimination with partial
     * pivoting.  On entry:
     * \param xform3 array to be decomposed and after the call it contains the LU decomposition
     * \param pivot contains the pivot history...the original position numbers of the pivoted rows.
     */
    int decomposition(float xform3[4][4], int pivot[4]);
    void solve(float xform3[4][4], float col[4], float result[4], int pivot[4]);
    int compute_inverse(float xform1[4][4], float xform2[4][4]);
    char *name;

public:
    float mat[4][4];

    /**
     * Constructor.
     * \param pMatName pointer to the name of the matrix.
     */
    live_4x4_matrix(char *pMatName =NULL);
    /**
     * Constructor.
     * \param pM pointer to a float array with 16 values (the initial values are copied from there)
     * \param pMatName pointer to the name of the matrix.
     */
    live_4x4_matrix(float *pM, char *pMatName =NULL);
    /**
     * Constructor.
     * \param pMatrix pointer to another live_4x4_matrix object (the initial values are copied from there)
     * \param pMatName pointer to the name of the matrix.
     */
    live_4x4_matrix(live_4x4_matrix *pMatrix, char *pMatName =NULL);

    /**
     * print the content of the matrix to stdout.
     */
    void print();
    /**
     * set matrix to identity matrix
     */
    void setId();
    /**
     * set matrix to another matrix
     * \param pMat pointer to 16 float values from where the new data will be copied.
     */
    void setToMatrix(float *m);
    /**
     * set matrix to a quaternian rotation.
     * Attention: The complete matrix will get overwritten!
     * \param fX the x rotation
     * \param fY the y rotation
     * \param fZ the z rotation
     * \param fW the w rotation
     */
    void setToQuat(float fX, float fY, float fZ, float fW);
    /**
     * set rotational part (upper 3x3 matrix) of the matrix to a quaternian rotation.
     * \param fX the x rotation
     * \param fY the y rotation
     * \param fZ the z rotation
     * \param fW the w rotation
     */
    void setRotationToQuat(float x, float y, float z, float w);
    /**
     * set matrix to a rotation matrix.
     * Attention: The complete matrix will get overwritten!
     * \param iAxis the axis (0 - x axis, 1 - y axis, 2 - z axis)
     * \param fAlpha rotation value (radian value. If you have an angle value and want to calculate the radian: (angle*PI)/180.0f)
     */
    void setRotation(int iAxis, float fAlpha);
    /**
     * set matrix to a translation matrix.
     * Attention: The complete matrix will get overwritten!
     * \param fX x translation
     * \param fY y translation
     * \param fZ z translation
     */
    void setTranslation(float fX, float fY, float fZ);
    /**
     * set matrix to scaling matrix.
     * Attention: The complete matrix will get overwritten!
     * \param fS scaling.
     */
    void setScale(float fS);
    /**
     * post multiply another matrix
     * \param m m[4][4] array with matrix to post multiply
     */
    void postMult(float m[4][4]);
    /**
     * pre multiply another matrix
     * \param m m[4][4] array with matrix to pre multiply
     */
    void preMult(float m[4][4]);
    /**
     * post multiply another matrix
     * \param pM pointer to other live_4x4_matrix
     */
    void postMult(live_4x4_matrix *pM);
    /**
     * pre multiply another matrix
     * \param pM pointer to other live_4x4_matrix
     */
    void preMult(live_4x4_matrix *pM);
    /**
     * add an rotation to this matrix.
     * \param iAxis the axis (0 - x axis, 1 - y axis, 2 - z axis)
     * \param fAlpha rotation value (radian value. If you have an angle value and want to calculate the radian: (angle*PI)/180.0f)
     */
    void addRotation(int axis, float alpha);
    /**
     * add a translation to the matrix.
     * \param fX x translation
     * \param fY y translation
     * \param fZ z translation
     */
    void addTranslation(float x, float y, float z);
    /**
     * add scaling to the matrix.
     * \param fS scaling.
     */
    void addScale(float s);
    /**
     * invert the matrix
     */
    void invert();
    /**
     * create a quaternion of the current matrix content.
     * \param pX pointer to the float variable where quaternion x rotation should be stored.
     * \param pY pointer to the float variable where quaternion y rotation should be stored.
     * \param pZ pointer to the float variable where quaternion z rotation should be stored.
     * \param pW pointer to the float variable where quaternion w rotation should be stored.
     */
    void toQuat(float *pX, float *pY, float *pZ, float *pW);
    /**
     * apply the matrix to the current modelview matrix.
     */
    void applyToGL();
    /**
     * parse a string of 16 float values and store it in the matrix
     * \param pStr pointer to the string.
     */
    void parseString(char *pStr);
};

#endif
