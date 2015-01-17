/***************************************************************************
                          svt_float2opacity_points
                          ------------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2OPACITY_POINTS_H
#define SVT_FLOAT2OPACITY_POINTS_H

#include <svt_stlVector.h>
#include <svt_basics.h>
#include <svt_vector3.h>
#include <svt_float2opacity.h>

/**
 * This class represents a opacity transfer function, i.e. a function calculating opacity values out of a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2opacity_points : public svt_float2opacity
{
protected:

    vector< svt_vector3<double> > m_aPointsO;

public:

    /**
     * Constructor
     */
    svt_float2opacity_points( float fMin=0.0f, float fMax=0.0f );
    virtual ~svt_float2opacity_points() { };

    /**
     * get the opacity value of the float
     * \param fX the float value
     */
    virtual char getOpacity(float fX);

    /**
     * delete all points
     */
    inline void deletePoints()
    {
	m_aPointsO.clear();
    };

    /**
     * add a point
     * \param oPoint svt_vector3<double> object - z is always 0!
     */
    void addPointO(svt_vector3<double>oPoint);
};

#endif
