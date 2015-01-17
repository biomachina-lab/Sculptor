/***************************************************************************
                          svt_float2opacity_linear.h
                          --------------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2OPACITY_LINEAR_H
#define SVT_FLOAT2OPACITY_LINEAR_H

#include <svt_float2opacity.h>

/**
 * This class represents a opacity transfer function, i.e. a function calculating opacity values out of a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2opacity_linear : public svt_float2opacity
{
protected:
    float m_fFactP;
    float m_fFactN;

public:

    /**
     * Constructor
     */
    svt_float2opacity_linear( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2opacity_linear( ) {};

    /**
     * get the opacity value of the float
     * \param fX the float value
     */
    virtual char getOpacity(float fX);

protected:

    virtual void calcFact( void );
};

#endif
