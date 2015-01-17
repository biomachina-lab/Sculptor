/***************************************************************************
                          svt_float2color.h
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2COLOR_H
#define SVT_FLOAT2COLOR_H

enum Float2ColorName {BW, RWB, RYG, RGB}; // type of color transfer function

/**
 * This class represents a color transfer function, i.e. a function calculating the color values for a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2color
{
protected:
    float m_fMax;
    float m_fMin;
    float m_fFact;
    enum Float2ColorName m_eFloat2ColorName;

public:
    /**
     * Constructor
     */
    svt_float2color( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2color( ) {};

    /**
     * get the name of the color transfer function
     * \return name of the color transfer function
     */
    enum Float2ColorName getFloat2ColorName();
    /**
     * set the name of the color transfer function
     * \param name of the color transfer function
     */
    void setFloat2ColorName(enum Float2ColorName);
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

    /**
     * set the maximum of the possible float values
     */
    void setMax(float fMax);
    /**
     * set the minimum of the possible float values
     */
    void setMin(float fMin);
    /**
     * get the maximum of the possible float values
     */
    float getMax( void );
    /**
     * get the minimum of the possible float values
     */
    float getMin( void );

    /**
     * update the object, if the minimum or the maximum value have changed
     */
    void update();

protected:
    virtual void calcFact( void );
};

#endif
