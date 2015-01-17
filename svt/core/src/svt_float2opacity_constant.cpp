/***************************************************************************
                          svt_float2opacity_constant.cpp
                          ----------------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// clib includes
#include <math.h>
// svt includes
#include <svt_core.h>
#include <svt_float2opacity_constant.h>

/**
 * Constructor
 */
svt_float2opacity_constant::svt_float2opacity_constant( float fMin, float fMax) : svt_float2opacity( fMin, fMax )
{
    m_cConstant = (char)(0xFF);
};

/**
 * get the opacity value of the float
 * \param fX the float value
 */
char svt_float2opacity_constant::getOpacity(float)
{
    return m_cConstant;
}

/**
 * set the constant
 * \param cConstant char value with the constant opacity value
 */
void svt_float2opacity_constant::setConstant(char cConstant)
{
    m_cConstant = cConstant;
}
/**
 * get the constant
 * \return char value with the constant opacity value
 */
char svt_float2opacity_constant::getConstant()
{
    return m_cConstant;
}
