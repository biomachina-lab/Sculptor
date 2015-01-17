/***************************************************************************
                          svt_kb_spline.cpp
                          -----------------
    begin                : 12/21/2006
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_kb_spline.h>

/**
 * Default constructor
 */
svt_kb_spline::svt_kb_spline()
{
    m_fTension=0.0f;
    m_fBias=0.0f;
    m_fContinuity=0.0f;

    init();
}

/**
 * Constructor
 */
svt_kb_spline::svt_kb_spline(Real32 fTension = 0.0f, Real32 fBias = 0.0f, Real32 fContinuity = 0.0f)
{
    setTension   ( fTension );
    setBias      ( fBias );
    setContinuity( fContinuity );

    init();
}

/**
 * compute the basis matrix
 */
void svt_kb_spline::init()
{
    Real32 fA, fB, fC, fD, fDiv;

    fA = (1.0f-m_fTension)*(1.0f+m_fContinuity)*(1.0f+m_fBias);
    fB = (1.0f-m_fTension)*(1.0f-m_fContinuity)*(1.0f-m_fBias);
    fC = (1.0f-m_fTension)*(1.0f-m_fContinuity)*(1.0f+m_fBias);
    fD = (1.0f-m_fTension)*(1.0f+m_fContinuity)*(1.0f-m_fBias);

    fDiv=2.0f;

    m_aSplineKB[0][0] = -fA/fDiv;
    m_aSplineKB[0][1] = (4.0f+fA-fB-fC)/fDiv;
    m_aSplineKB[0][2] = (-4.0f+fB+fC-fD)/fDiv;
    m_aSplineKB[0][3] = fD/fDiv;

    m_aSplineKB[1][0] = 2.0f*fA/fDiv;
    m_aSplineKB[1][1] = (-6.0f -2.0f*fA +2.0f*fB+fC)/fDiv;
    m_aSplineKB[1][2] = (6.0f -2.0f*fB-fC+fD)/fDiv;
    m_aSplineKB[1][3] = -fD/fDiv;

    m_aSplineKB[2][0] = -fA/fDiv;
    m_aSplineKB[2][1] = (fA-fB)/fDiv;
    m_aSplineKB[2][2] = fB/fDiv;
    m_aSplineKB[2][3] = 0.0f;

    m_aSplineKB[3][0] = 0.0f;
    m_aSplineKB[3][1] = 2.0f/fDiv;
    m_aSplineKB[3][2] = 0.0f;
    m_aSplineKB[3][3] = 0.0f;
}

/**
 * calculate the matrix used for the spline interpolation (it depends on the points, therefore it has to be recalculated everytime you start
 * with new points to interpolate a curve through)
 */
void svt_kb_spline::calcSplineQMatrix(svt_vector4<Real32>& rA, svt_vector4<Real32>& rB, svt_vector4<Real32>& rC, svt_vector4<Real32>& rD)
{
    for (int i=0; i<4; i++)
    {
        m_aSplineQ[i][0] = m_aSplineQ[i][1] = m_aSplineQ[i][2] = 0.0f;

        m_aSplineQ[i][0] += m_aSplineKB[i][0] * rA.x();
        m_aSplineQ[i][1] += m_aSplineKB[i][0] * rA.y();
        m_aSplineQ[i][2] += m_aSplineKB[i][0] * rA.z();

        m_aSplineQ[i][0] += m_aSplineKB[i][1] * rB.x();
        m_aSplineQ[i][1] += m_aSplineKB[i][1] * rB.y();
        m_aSplineQ[i][2] += m_aSplineKB[i][1] * rB.z();

        m_aSplineQ[i][0] += m_aSplineKB[i][2] * rC.x();
        m_aSplineQ[i][1] += m_aSplineKB[i][2] * rC.y();
        m_aSplineQ[i][2] += m_aSplineKB[i][2] * rC.z();

        m_aSplineQ[i][0] += m_aSplineKB[i][3] * rD.x();
        m_aSplineQ[i][1] += m_aSplineKB[i][3] * rD.y();
        m_aSplineQ[i][2] += m_aSplineKB[i][3] * rD.z();
    }
}
/**
 * calculate the spline interpolation points
 */
void svt_kb_spline::calcSplinePoints(svt_vector4<Real32> *pOut, unsigned int iSteps)
{
    for (unsigned int i=0; i<iSteps; ++i)
    {
        fW = (Real32) ((Real32)(i)) / ((Real32) (iSteps-1));
        pOut[i].x( fW * (fW * (fW * m_aSplineQ[0][0] + m_aSplineQ[1][0]) + m_aSplineQ[2][0]) + m_aSplineQ[3][0] );
        pOut[i].y( fW * (fW * (fW * m_aSplineQ[0][1] + m_aSplineQ[1][1]) + m_aSplineQ[2][1]) + m_aSplineQ[3][1] );
        pOut[i].z( fW * (fW * (fW * m_aSplineQ[0][2] + m_aSplineQ[1][2]) + m_aSplineQ[2][2]) + m_aSplineQ[3][2] );
    }
}
/**
 * set the tension parameter between -1 and +1
 */
void svt_kb_spline::setTension( Real32 fTension )
{
    if (fTension < -1.0f)
	m_fTension = -1.0f;
    else if (fTension > 1.0f)
	m_fTension = 1.0f;
    else
	m_fTension = fTension;

    init();
}
/**
 * set the bias parameter between -1 and +1
 */
void svt_kb_spline::setBias( Real32 fBias )
{
    if (fBias < -1.0f)
	m_fBias = -1.0f;
    else if (fBias > 1.0f)
	m_fBias = 1.0f;
    else
	m_fBias = fBias;

    init();
}
/**
 * set the continuity parameter between -1 and +1
 */
void svt_kb_spline::setContinuity( Real32 fContinuity )
{
    if (fContinuity < -1.0f)
	m_fContinuity = -1.0f;
    else if (fContinuity > 1.0f)
	m_fContinuity = 1.0f;
    else
	m_fContinuity = fContinuity;

    init();
}
/**
 * set the tension, bias, and continuity parameters between -1 and +1
 */
void svt_kb_spline::setTBC( Real32 fTension, Real32 fBias, Real32 fContinuity )
{
    if (fTension < -1.0f)
	m_fTension = -1.0f;
    else if (fTension > 1.0f)
	m_fTension = 1.0f;
    else
	m_fTension = fTension;


    if (fBias < -1.0f)
	m_fBias = -1.0f;
    else if (fBias > 1.0f)
	m_fBias = 1.0f;
    else
	m_fBias = fBias;


    if (fContinuity < -1.0f)
	m_fContinuity = -1.0f;
    else if (fContinuity > 1.0f)
	m_fContinuity = 1.0f;
    else
	m_fContinuity = fContinuity;


    init();
}
