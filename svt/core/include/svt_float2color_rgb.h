/***************************************************************************
                          svt_float2color_rgb.h
                          ---------------------
    begin                : 16.06.2005
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2COLOR_RGB_H
#define SVT_FLOAT2COLOR_RGB_H

/**
 * This is a color transfer function to whole color spectrum.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_float2color_rgb : public svt_float2color
{
public:

    /**
     * Constructor
     */
    svt_float2color_rgb(float fMin=0.0f, float fMax=0.0f);
    virtual ~svt_float2color_rgb(){};

    /**
     * get the red color value of the float
     * \param fX the float value
     */
    virtual char getR(float fX);
    /**
     * get the green color value of the float
     * \param fX the float value
     */
    virtual char getG(float fX);
    /**
     * get the blue color value of the float
     * \param fX the float value
     */
    virtual char getB(float fX);

protected:

    virtual void calcFact(void);
};

#endif
