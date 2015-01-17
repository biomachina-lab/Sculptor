/***************************************************************************
                          svt_float2opacity_constant.h
                          --------------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2OPACITY_CONSTANT_H
#define SVT_FLOAT2OPACITY_CONSTANT_H

#include <svt_float2opacity.h>

/**
 * This class represents a opacity transfer function, i.e. a function calculating opacity values out of a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2opacity_constant : public svt_float2opacity
{
protected:
    char m_cConstant;

public:

    /**
     * Constructor
     */
    svt_float2opacity_constant( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2opacity_constant( ) {};

    /**
     * get the opacity value of the float
     * \param fX the float value
     */
    virtual char getOpacity(float fX);

    /**
     * set the constant
     * \param cConstant char value with the constant opacity value
     */
    void setConstant(char cConstant);
    /**
     * get the constant
     * \return char value with the constant opacity value
     */
    char getConstant();
};

#endif
