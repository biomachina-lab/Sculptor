/***************************************************************************
                          svt_cr_spline.cpp
                          -----------------
    begin                : 7/9/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_cr_spline.h>

#define ADJ 1.25f
//#define ADJ 2.00

const Real32 svt_cr_spline::m_aSplineCR[4][4] = {
    {-1.0f/ADJ,  -1.0f/ADJ + 2.0f,  1.0f/ADJ - 2.0f,  1.0f/ADJ},
    { 2.0f/ADJ,   1.0f/ADJ - 3.0f, -2.0f/ADJ+3.0f,   -1.0f/ADJ},
    {-1.0f/ADJ,              0.0f,  1.0f/ADJ,         0.0f},
    { 0.0f,                  1.0f,  0.0f,             0.0f}
};

/**
 * Constructor
 */
svt_cr_spline::svt_cr_spline()
{
}

/**
 * calculate the matrix used for the spline interpolation (it depends on the points, therefor it has to be recalculated everytime you start
 * with new points to interpolate a curve through)
 */
void svt_cr_spline::calcSplineQMatrix(svt_vector4<Real32>& rA, svt_vector4<Real32>& rB, svt_vector4<Real32>& rC, svt_vector4<Real32>& rD)
{
    for (int i=0; i<4; i++)
    {
        m_aSplineQ[i][0] = m_aSplineQ[i][1] = m_aSplineQ[i][2] = 0.0f;

        m_aSplineQ[i][0] += m_aSplineCR[i][0] * rA.x();
        m_aSplineQ[i][1] += m_aSplineCR[i][0] * rA.y();
        m_aSplineQ[i][2] += m_aSplineCR[i][0] * rA.z();

        m_aSplineQ[i][0] += m_aSplineCR[i][1] * rB.x();
        m_aSplineQ[i][1] += m_aSplineCR[i][1] * rB.y();
        m_aSplineQ[i][2] += m_aSplineCR[i][1] * rB.z();

        m_aSplineQ[i][0] += m_aSplineCR[i][2] * rC.x();
        m_aSplineQ[i][1] += m_aSplineCR[i][2] * rC.y();
        m_aSplineQ[i][2] += m_aSplineCR[i][2] * rC.z();

        m_aSplineQ[i][0] += m_aSplineCR[i][3] * rD.x();
        m_aSplineQ[i][1] += m_aSplineCR[i][3] * rD.y();
        m_aSplineQ[i][2] += m_aSplineCR[i][3] * rD.z();
    }
}

/**
 * calculate the spline interpolation points
 */
void svt_cr_spline::calcSplinePoints(svt_vector4<Real32> *pOut, int iSteps)
{
    Real32 fW;

    for (int i=0;i<iSteps;i++)
    {
        fW = float(i)/float(iSteps-1);
        pOut[i].x( fW * (fW * (fW * m_aSplineQ[0][0] + m_aSplineQ[1][0]) + m_aSplineQ[2][0]) + m_aSplineQ[3][0] );
        pOut[i].y( fW * (fW * (fW * m_aSplineQ[0][1] + m_aSplineQ[1][1]) + m_aSplineQ[2][1]) + m_aSplineQ[3][1] );
        pOut[i].z( fW * (fW * (fW * m_aSplineQ[0][2] + m_aSplineQ[1][2]) + m_aSplineQ[2][2]) + m_aSplineQ[3][2] );
    }
}
