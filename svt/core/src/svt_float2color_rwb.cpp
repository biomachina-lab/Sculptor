/***************************************************************************
                          svt_float2color_rwb.cpp
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_float2color.h>
#include <svt_float2color_rwb.h>
// clib includes
#include <math.h>

/**
 * Constructor
 */
svt_float2color_rwb::svt_float2color_rwb( float fMin, float fMax ) : svt_float2color(fMin, fMax)
{
    setFloat2ColorName(RWB);
};

/**
 * get the red color value of the float
 * \param fX the float value
 */
char svt_float2color_rwb::getR(float fX)
{
    if (fX < 0)
        return (char)(0xFF);
    else
        return (char)(0xFF) - (char)floor(fabs(m_fFactP*fX));
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
char svt_float2color_rwb::getG(float fX)
{
    if (fX < 0)
        return (char)(0xFF) - (char)floor(fabs(m_fFactN*fX));
    else
        return (char)(0xFF) - (char)floor(fabs(m_fFactP*fX));
}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
char svt_float2color_rwb::getB(float fX)
{
    if (fX < 0)
        return (char)(0xFF) - (char)floor(fabs(m_fFactN*fX));
    else
        return (char)(0xFF);
}

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2color_rwb::calcFact( void )
{
    m_fFactP = (255.0)/(getMax());
    m_fFactN = (255.0)/(getMin());
}
