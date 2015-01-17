/***************************************************************************
                          svt_float2opacity.cpp
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// clib includes
#include <math.h>
// svt includes
#include <svt_core.h>
#include <svt_float2opacity.h>

/**
 * Constructor
 */
svt_float2opacity::svt_float2opacity( float fMin, float fMax)
{
    setMin(fMin);
    setMax(fMax);
    calcFact();
};

/**
 * get the opacity value of the float
 * \param fX the float value
 */
char svt_float2opacity::getOpacity(float fX)
{
    return (char)floor(fabs(m_fFact * fX));
}

/**
 * set the maximum of the possible float values
 */
void svt_float2opacity::setMax(float fMax)
{
    m_fMax = fMax;
};
/**
 * set the minimum of the possible float values
 */
void svt_float2opacity::setMin(float fMin)
{
    m_fMin = fMin;
};
/**
 * get the maximum of the possible float values
 */
float svt_float2opacity::getMax( void )
{
    return m_fMax;
};
/**
 * get the minimum of the possible float values
 */
float svt_float2opacity::getMin( void )
{
    return m_fMin;
};

/**
 * update the object, if the minimum or the maximum value have changed
 */
void svt_float2opacity::update()
{
    calcFact();
};

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2opacity::calcFact( void )
{
    m_fFact = (255)/(getMax() - getMin());
}
