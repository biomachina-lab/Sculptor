/***************************************************************************
                          svt_float2color.cpp
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

/**
 * Constructor
 */
svt_float2color::svt_float2color( float fMin, float fMax)
{
    setMin(fMin);
    setMax(fMax);
    calcFact();
};

/**
 * get the name of the color transfer function
 * \return name of the color transfer function
 */
enum Float2ColorName svt_float2color::getFloat2ColorName()
{
    return m_eFloat2ColorName;
}
/**
 * set the name of the color transfer function
 * \param name of the color transfer function
 */
void svt_float2color::setFloat2ColorName(enum Float2ColorName FCN)
{
    m_eFloat2ColorName = FCN;
}

/**
 * get the red color value of the float
 * \param fX the float value
 */
char svt_float2color::getR(float y)
{
    return (char)floor(fabs(m_fFact * y));
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
char svt_float2color::getG(float y)
{
    return (char)floor(fabs(m_fFact * y));
}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
char svt_float2color::getB(float y)
{
    return (char)floor(fabs(m_fFact * y));
}

/**
 * set the maximum of the possible float values
 */
void svt_float2color::setMax(float fMax)
{
    m_fMax = fMax;
};
/**
 * set the minimum of the possible float values
 */
void svt_float2color::setMin(float fMin)
{
    m_fMin = fMin;
};
/**
 * get the maximum of the possible float values
 */
float svt_float2color::getMax( void )
{
    return m_fMax;
};
/**
 * get the minimum of the possible float values
 */
float svt_float2color::getMin( void )
{
    return m_fMin;
};

/**
 * update the object, if the minimum or the maximum value have changed
 */
void svt_float2color::update()
{
    calcFact();
};

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2color::calcFact( void )
{
    m_fFact = (255.0)/(getMax() - getMin());
}
