/***************************************************************************
                          svt_kb_spline.h
                          ---------------
    begin                : 12/21/2006
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_KB_SPLINE_H
#define SVT_KB_SPLINE_H

#include <svt_basics.h>
#include <svt_structs.h>
#include <svt_types.h>
#include <svt_vector4.h>

/**
 * Kochanek-Bartels Spline (also known as TCB Splines) class
 * Setting T = B = C = 0 results in a Catmull-Rom spline
 * see Kochanek, Bartels: Interpolating Splines with Local Tension, Continuity and Bias Control, SIGGRAPH 84, 33-41.
 */
class svt_kb_spline
{
private:
    Real32 m_aSplineQ[4][3];
    Real32 m_aSplineKB[4][4];
    Real32 m_fTension;
    Real32 m_fBias;
    Real32 m_fContinuity;

    Real32 fW;

    /**
     * compute the basis matrix
     */
    void init();

public:

    /**
     * Default constructor
     */
    svt_kb_spline();
    /**
     * Constructor
     */
    svt_kb_spline(Real32 fTension, Real32 fBias, Real32 fContinuity);
    /**
     * calculate the matrix used for the spline interpolation (it depends on the points, therefore it has to be recalculated everytime you start
     * with new points to interpolate a curve through)
     * \param rA first base point
     * \param rB second base point
     * \param rC third base point
     * \param rD fourth base point
     */
    void calcSplineQMatrix(svt_vector4<Real32>& rA, svt_vector4<Real32>& rB, svt_vector4<Real32>& rC, svt_vector4<Real32>& rD);
    /**
     * calculate the spline interpolation points
     */
    void calcSplinePoints(svt_vector4<Real32> *pOut, unsigned int iSteps =7);
    /**
     * set the tension parameter between -1 and +1
     */
    void setTension( Real32 fTension );
    /**
     * set the bias parameter between -1 and +1
     */
    void setBias( Real32 fBias );
    /**
     * set the continuity parameter between -1 and +1
     */
    void setContinuity( Real32 fContinuity );
    /**
     * set the tension, bias, and continuity parameters between -1 and +1
     */
    void setTBC( Real32 fTension, Real32 fBias, Real32 fContinuity );
};

#endif
