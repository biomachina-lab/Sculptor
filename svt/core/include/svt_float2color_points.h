/***************************************************************************
                          svt_float2color_points
                          ----------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2COLOR_POINTS_H
#define SVT_FLOAT2COLOR_POINTS_H

#include <svt_stlVector.h>
#include <svt_basics.h>
#include <svt_vector3.h>
#include <svt_float2color.h>

/**
 * This class represents a color transfer function, i.e. a function calculating the color values for a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2color_points : public svt_float2color
{
protected:

    vector< svt_vector3<double> > m_aPointsR;
    vector< svt_vector3<double> > m_aPointsG;
    vector< svt_vector3<double> > m_aPointsB;

public:

    /**
     * Constructor
     */
    svt_float2color_points( float fMin=0.0f, float fMax=0.0f );
    virtual ~svt_float2color_points() { };

    /**
     * get the red color value of the float
     * \param fX the float value
     */
    virtual char getR(float fX);
    /**
     * get the green color value of the float
     * \param fX the float value
     */
    virtual char getG(float fX);
    /**
     * get the blue color value of the float
     * \param fX the float value
     */
    virtual char getB(float fX);

    /**
     * delete all points
     */
    inline void deletePoints()
    {
	m_aPointsR.clear();
	m_aPointsG.clear();
	m_aPointsB.clear();
    };

    /**
     * add a point
     * \param oPoint svt_vector3<double> object - z is always 0!
     */
    void addPointR(svt_vector3<double>oPoint);
    /**
     * add a point
     * \param oPoint svt_vector3<double> object - z is always 0!
     */
    void addPointG(svt_vector3<double>oPoint);
    /**
     * add a point
     * \param oPoint svt_vector3<double> object - z is always 0!
     */
    void addPointB(svt_vector3<double>oPoint);
};

#endif
