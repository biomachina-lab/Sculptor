/***************************************************************************
                          svt_cr_spline.h
                          ---------------
    begin                : 7/9/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CR_SPLINE_H
#define SVT_CR_SPLINE_H

#include <svt_basics.h>
#include <svt_structs.h>
#include <svt_types.h>
#include <svt_vector4.h>

/**
 * Catmull-Rom Spline class
 * see Foley, van Dam, et. al. "Computer Graphics: Principles and Practice", Addison Wesley, 1990. p. 505 Catmull-Rom Spline function. Q = [CR][Pi]
 */
class svt_cr_spline
{
protected:
    Real32 m_aSplineQ[4][3];
    static const Real32 m_aSplineCR[4][4];

public:

    /**
     * Constructor
     */
    svt_cr_spline();

    /**
     * calculate the matrix used for the spline interpolation (it depends on the points, therefor it has to be recalculated everytime you start
     * with new points to interpolate a curve through)
     * \param rA first base point
     * \param rB second base point
     * \param rC third base point
     * \param rD forth base point
     */
    void calcSplineQMatrix(svt_vector4<Real32>& rA, svt_vector4<Real32>& rB, svt_vector4<Real32>& rC, svt_vector4<Real32>& rD);

    /**
     * calculate the spline interpolation points
     */
    void calcSplinePoints(svt_vector4<Real32> *pOut, int iSteps =7);
};

#endif
