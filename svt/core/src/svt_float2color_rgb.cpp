/***************************************************************************
                          svt_float2color_rgb.cpp
                          -----------------------
    begin                : 16.06.2005
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

// clib includes
#include <math.h>
// svt includes
#include <svt_core.h>
#include <svt_float2color.h>
#include <svt_float2color_rgb.h>

// not 360 because of purple and circle
#define MAX_HUE (240.)
#define SATURATION (1.)
#define VALUE (1.)

/**
 * converts color space hsv to rgb 
 * \param h hue 0..360, s saturation 0..1, v value 0..1, 
 *        for return: r red 0..1, g green 0..1, b blue 0..1
 */
void hsv2rgb(float h, float s, float v, float &r, float &g, float &b)
{
    int i;
	float f, p, q, t;

    // achromatic (grey)
    if(s == 0) 
	{
        r = g = b = v;
        return;
    }

    h /= 60;                        // sector 0 to 5
    i = (int)h;
    f = h - i;                      // factorial part of h
    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));

    switch(i) 
	{
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:                // case 5:
            r = v;
            g = p;
            b = q;
            break;
    }
}

/**
 * Constructor
 */
svt_float2color_rgb::svt_float2color_rgb(float fMin, float fMax) : svt_float2color(fMin, fMax)
{
    setFloat2ColorName(RGB);
};

/**
 * get the red color value of the float
 * \param fX the float value
 */
char svt_float2color_rgb::getR(float fX)
{
    float r, g, b;

    hsv2rgb(MAX_HUE-((fX + ((-1)*getMin()))*m_fFact), SATURATION, VALUE, r, g, b);
    return (char) (255.0*r);
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
char svt_float2color_rgb::getG(float fX)
{
    float r, g, b;

    hsv2rgb(MAX_HUE-((fX + ((-1)*getMin()))*m_fFact), SATURATION, VALUE, r, g, b);
    return (char) (255.0*g);
}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
char svt_float2color_rgb::getB(float fX)
{
    float r, g, b;

    hsv2rgb(MAX_HUE-((fX + ((-1)*getMin()))*m_fFact), SATURATION, VALUE, r, g, b);
    return (char) (255.0*b);
}

///////////////////////////////////////////////////////////////////////////////
// protected functions
///////////////////////////////////////////////////////////////////////////////

void svt_float2color_rgb::calcFact(void)
{
    m_fFact = (MAX_HUE)/(getMax() - getMin());
}
