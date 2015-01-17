/***************************************************************************
                          svt_float2mat
                          -------------
    begin                : 03/08/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_FLOAT2MAT_H
#define __SVT_FLOAT2MAT_H

#include <svt_basics.h>

/**
 * This class represents a transfer function, i.e. a function calculating the material properties out of a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2mat
{

protected:

    float m_fMin;
    float m_fMax;
    bool m_bZero; // if true than opacity is 0 at a voxel value of 0 regardless of what transfer function is used

public:

    /**
     * Constructor
     */
    svt_float2mat( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2mat() { };

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
     * set the maximum of the possible float values
     */
    void setMax(float fMax);
    /**
     * set the minimum of the possible float values
     */
    void setMin(float fMin);
    /**
     * get the maximum of the possible float values
     */
    float getMax( void ) const;
    /**
     * get the minimum of the possible float values
     */
    float getMin( void ) const;

    /**
     * set zero treatment
     * \param bZero if true than opacity is 0 at a voxel value of 0 regardless of what transfer function is used
     */
    inline void setZero(bool bZero)
    {
	m_bZero = bZero;
    };
    /**
     * get zero treatment
     * \return if true than opacity is 0 at a voxel value of 0 regardless of what transfer function is used
     */
    inline bool getZero()
    {
	return m_bZero;
    };

    /**
     * update the object, if the minimum or the maximum value have changed
     */
    virtual void update();

};

#endif
