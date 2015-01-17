/***************************************************************************
                          svt_float2color_bw.cpp
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// clib includes
#include <math.h>
// svt includes
#include <svt_core.h>
#include <svt_float2color.h>
#include <svt_float2color_bw.h>

/**
 * Constructor
 */
svt_float2color_bw::svt_float2color_bw( float fMin, float fMax ) : svt_float2color(fMin, fMax)
{
    setFloat2ColorName(BW);
};

/**
 * get the red color value of the float
 * \param fX the float value
 */
char svt_float2color_bw::getR(float y)
{
    return (char)((y + ((-1)*getMin()))*m_fFact);
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
char svt_float2color_bw::getG(float y)
{
    return (char)((y + ((-1)*getMin()))*m_fFact);
}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
char svt_float2color_bw::getB(float y)
{
    return (char)((y + ((-1)*getMin()))*m_fFact);
}

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2color_bw::calcFact( void )
{
    m_fFact = (255.0)/(getMax() - getMin());
}
