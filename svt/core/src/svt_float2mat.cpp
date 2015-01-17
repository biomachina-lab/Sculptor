/***************************************************************************
                          svt_float2mat
                          -------------
    begin                : 03/08/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_float2mat.h>

/**
 * Constructor
 */
svt_float2mat::svt_float2mat( float fMin, float fMax ) :
    m_fMin(fMin),
    m_fMax(fMax),
    m_bZero(true)
{

};


/**
 * get the red color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat::getR(float)
{
    return 0;
};
/**
 * get the green color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat::getG(float)
{
    return 0;
};
/**
 * get the blue color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat::getB(float)
{
    return 0;
};
/**
 * get the opacity value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat::getOpacity(float)
{
    return 0;
};


/**
 * set the maximum of the possible float values
 */
void svt_float2mat::setMax(float fMax)
{
    m_fMax = fMax;
};
/**
 * set the minimum of the possible float values
 */
void svt_float2mat::setMin(float fMin)
{
    m_fMin = fMin;
};
/**
 * get the maximum of the possible float values
 */
float svt_float2mat::getMax( void ) const
{
    return m_fMax;
};
/**
 * get the minimum of the possible float values
 */
float svt_float2mat::getMin( void ) const
{
    return m_fMin;
};

/**
 * update the object, if the minimum or the maximum value have changed
 */
void svt_float2mat::update()
{
};
