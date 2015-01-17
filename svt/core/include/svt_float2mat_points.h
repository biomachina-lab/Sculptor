/***************************************************************************
                          svt_float2mat_points
                          --------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2MAT_POINTS_H
#define SVT_FLOAT2MAT_POINTS_H

#include <svt_stlVector.h>
#include <svt_basics.h>
#include <svt_vector3.h>
#include <svt_float2mat.h>
#include <svt_string.h>

/**
 * This class represents a transfer function based on a couple of points between the class linearly interpolates
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2mat_points : public svt_float2mat
{
protected:

    vector< svt_vector3<double> > m_aPointsR;
    vector< svt_vector3<double> > m_aPointsG;
    vector< svt_vector3<double> > m_aPointsB;
    vector< svt_vector3<double> > m_aPointsO;

    bool m_bUninitialized;

public:

    /**
     * Constructor
     */
    svt_float2mat_points( float fMin=0.0f, float fMax=0.0f );
    virtual ~svt_float2mat_points() { };

    /**
     * get the red color value of the float
     * \param fX the float value
     */
    virtual unsigned char getR(float fX);
    /**
     * get the green color value of the float
     * \param fX the float value
     */
    virtual unsigned char getG(float fX);
    /**
     * get the blue color value of the float
     * \param fX the float value
     */
    virtual unsigned char getB(float fX);
    /**
     * get the opacity value of the float
     * \param fX the float value
     */
    virtual unsigned char getOpacity(float fX);


    /**
     * delete all points
     */
    inline void deletePoints()
    {
	m_aPointsR.clear();
	m_aPointsG.clear();
	m_aPointsB.clear();
        m_aPointsO.clear();

        m_bUninitialized = true;
    };

    /**
     * get points for the red curve
     * \return reference to vector< svt_vector3< double > >
     */
    inline vector< svt_vector3<double> >& getPointsR()
    {
	return m_aPointsR;
    };
    /**
     * get points for the green curve
     * \return reference to vector< svt_vector3< double > >
     */
    inline vector< svt_vector3<double> >& getPointsG()
    {
	return m_aPointsG;
    };
    /**
     * get points for the blue curve
     * \return reference to vector< svt_vector3< double > >
     */
    inline vector< svt_vector3<double> >& getPointsB()
    {
	return m_aPointsB;
    };
    /**
     * get points for the opacity curve
     * \return reference to vector< svt_vector3< double > >
     */
    inline vector< svt_vector3<double> >& getPointsO()
    {
	return m_aPointsO;
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
    /**
     * add a point
     * \param oPoint svt_vector3<double> object - z is always 0!
     */
    void addPointO(svt_vector3<double>oPoint);

    /**
     * update the object, if the minimum or the maximum value have changed
     */
    virtual void update();

    /**
     * save to file
     * \param oFilename std string with filename
     */
    void saveToFile(string oFilename);
    /**
     * load from file
     * \param oFilename std string with filename
     */
    void loadFromFile(string oFilename);

};

#endif
