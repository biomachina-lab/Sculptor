/***************************************************************************
                          svt_float2color_ryg.h
                          -------------------
    begin                : 25.01.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FLOAT2COLOR_RYG
#define SVT_FLOAT2COLOR_RYG

/**
 * This is a red-yellow-green color transfer function.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_float2color_ryg : public svt_float2color
{
protected:
    float m_fFactG;
    float m_fFactR;

public:

    /**
     * Constructor
     */
    svt_float2color_ryg( float fMin=0.0f, float fMax=0.0f );
    /**
     * Destructor
     */
    virtual ~svt_float2color_ryg( ) {};

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
