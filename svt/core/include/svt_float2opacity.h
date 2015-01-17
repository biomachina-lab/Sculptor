/***************************************************************************
                          svt_float2opacity.h
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2OPACITY_H
#define SVT_FLOAT2OPACITY_H

/**
 * This class represents a opacity transfer function, i.e. a function calculating opacity values out of a floatingpoint number.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_float2opacity
{
protected:
    float m_fMax;
    float m_fMin;
    float m_fFact;

public:

    /**
     * Constructor
     */
    svt_float2opacity( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2opacity( ) {};

    /**
     * get the opacity value of the float
     * \param fX the float value
     */
    virtual char getOpacity(float fX);

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
