/***************************************************************************
                          svt_basic_point.h
                          ------------------
    begin                : 7/6/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BASIC_POINT_H
#define SVT_BASIC_POINT_H

#include <svt_basic_object.h>
#include <svt_vector4.h>

/**
 * \brief The basic point class
 * \author Stefan Birmanns
 */
class DLLEXPORT svt_basic_point : public svt_basic_object
{
protected:
    svt_vec4real32 m_oPoint;

public:
    /**
     * Constructor
     * \param fX x position
     * \param fY y position
     * \param fZ z position
     * \param fRed red color component
     * \param fGreen green color component
     * \param fBlue blue color component
     */
    svt_basic_point( Real32 fX, Real32 fY, Real32 fZ, Real32 fRed=1.0f, Real32 fGreen=1.0f, Real32 fBlue=1.0f );
    /**
     * Constructor
     * \param oPoint position
     * \param fRed red color component
     * \param fGreen green color component
     * \param fBlue blue color component
     */
    svt_basic_point( svt_vec4real32 oPoint, Real32 fRed=1.0f, Real32 fGreen=1.0f, Real32 fBlue=1.0f );

    virtual ~svt_basic_point(){};

    /**
     * set the position of the point
     * \param fX x position
     * \param fY y position
     * \param fZ z position
     */
    inline void setPosition(Real32 fX, Real32 fY, Real32 fZ)
    {
        m_oPoint.x( fX );
        m_oPoint.y( fY );
        m_oPoint.z( fZ );
    };
    /**
     * set the position of the point
     * \param oPoint position
     */
    inline void setPosition(svt_vec4real32 oPoint)
    {
        m_oPoint = oPoint;
    };

    /**
     * draw the point (PovRay)
     */
    virtual void drawPOV();

    /**
     * draw the object (OpenGL)
     * \param iWin index of window (opengl context)
     * \param rCont reference to parent container object
     */
    virtual void drawGL(int iWin, svt_basic_container& rCont);
};

#endif
