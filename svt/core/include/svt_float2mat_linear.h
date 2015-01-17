/***************************************************************************
                          svt_float2mat_linear
                          --------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2MAT_LINEAR_H
#define SVT_FLOAT2MAT_LINEAR_H

#include <svt_stlVector.h>
#include <svt_basics.h>
#include <svt_vector3.h>
#include <svt_float2mat.h>

/**
 * This class represents a simple linear transfer function that linearly increases the opacity and color values from the minimum voxel value to the maximum.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2mat_linear : public svt_float2mat
{
private:

    Real32 m_fFact;

public:

    /**
     * Constructor
     */
    svt_float2mat_linear( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2mat_linear() { };

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
     * update the object, if the minimum or the maximum value have changed
     */
    virtual void update();
};

#endif
