/***************************************************************************
                          svt_float2color_bw.h
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2COLOR_BW_H
#define SVT_FLOAT2COLOR_BW_H

/**
 * This is a simple black-and-white color transfer function.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_float2color_bw : public svt_float2color
{
public:

    /**
     * Constructor
     */
    svt_float2color_bw( float fMin=0.0f, float fMax=0.0f );
    virtual ~svt_float2color_bw(){};

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

    virtual void calcFact( void );
};

#endif
