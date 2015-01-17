/***************************************************************************
                          svt_float2mat_linear
                          --------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_float2mat_linear.h>

/**
 * Constructor
 */
svt_float2mat_linear::svt_float2mat_linear( float fMin, float fMax ) : svt_float2mat( fMin, fMax )
{
    m_fFact = 255.0f;
};

/**
 * get the red color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_linear::getR(float fX)
{
    return (unsigned char) ((fX - m_fMin) * m_fFact);
};
/**
 * get the green color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_linear::getG(float fX)
{
    return (unsigned char) ((fX - m_fMin) * m_fFact);
};
/**
 * get the blue color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_linear::getB(float fX)
{
    return (unsigned char) ((fX - m_fMin) * m_fFact);
};
/**
 * get the opacity value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_linear::getOpacity(float fX)
{
    if (fX == 0.0 && m_bZero == true)
	return 0;

    return (unsigned char) ((fX - m_fMin) * m_fFact);
};

/**
 * update the object, if the minimum or the maximum value have changed
 */
void svt_float2mat_linear::update()
{
    m_fFact = 255.0f / (m_fMax - m_fMin);
};
