/***************************************************************************
                          svt_float2opacity_linear.cpp
                          ----------------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// clib includes
#include <math.h>
// svt includes
#include <svt_core.h>
#include <svt_float2opacity_linear.h>

#include <stdio.h>

/**
 * Constructor
 */
svt_float2opacity_linear::svt_float2opacity_linear( float fMin, float fMax) : svt_float2opacity( fMin, fMax )
{
    calcFact();
};

/**
 * get the opacity value of the float
 * \param fX the float value
 */
char svt_float2opacity_linear::getOpacity(float fX)
{
    /**if (fX > 0)
        return (char)floor(fabs(m_fFactP * fX));
    else
    return (char)floor(fabs(m_fFactN * fX));*/

    //printf("%f*%f=%x\n", m_fFactP, fX,(unsigned char)(fabs(m_fFactP * fX) + 0.5f) );

    return (unsigned char)(fabs(m_fFactP * fX) + 0.5f);
}

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2opacity_linear::calcFact( void )
{
    m_fFactP = 255.0f / (getMax()-getMin());

    /**if (getMax())
        m_fFactP = (256.0f)/(getMax());
    if (getMin())
        m_fFactN = (256.0f)/(getMin());*/
}
