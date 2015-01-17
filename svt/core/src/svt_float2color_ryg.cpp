/***************************************************************************
                          svt_float2color_ryg.cpp
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_float2color.h>
#include <svt_float2color_ryg.h>
// clib includes
#include <math.h>

/**
 * Constructor
 */
svt_float2color_ryg::svt_float2color_ryg( float fMin, float fMax ) : svt_float2color(fMin, fMax)
{
    setFloat2ColorName(RYG);
};

/**
 * get the red color value of the float
 * \param fX the float value
 */
char svt_float2color_ryg::getR(float fX)
{
    if (fX < 0)
        return (char)(0xFF);
    else
        return (char)(0xFF) - (char)(m_fFactR*fX);
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
char svt_float2color_ryg::getG(float fX)
{
    if (fX > 0)
        return (char)(0xFF);
    else
        return (char)(0x00) + (char)(m_fFactG*fX);
}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
char svt_float2color_ryg::getB(float)
{
    return (char)(0x00);
}

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2color_ryg::calcFact( void )
{
    m_fFactG = (255.0)/(0 - getMin());
    m_fFactR = (255.0)/(getMax());
}
