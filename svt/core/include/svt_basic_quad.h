/***************************************************************************
                          svt_basic_quad.h
                          ----------------
    begin                : 6/14/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BASIC_QUAD_H
#define SVT_BASIC_QUAD_H

#include <svt_basic_object.h>

/**
 * \brief The basic quad class
 * \author Stefan Birmanns
 */
class DLLEXPORT svt_basic_quad : public svt_basic_object
{
protected:
    Real32 m_fX1;
    Real32 m_fY1;
    Real32 m_fZ1;

    Real32 m_fX2;
    Real32 m_fY2;
    Real32 m_fZ2;

public:
    /**
     * Constructor
     * A quad between start and end-position with a certain color.
     * \param fX1 x start-position
     * \param fY1 y start-position
     * \param fZ1 z start-position
     * \param fX2 x end-position
     * \param fY2 y end-position
     * \param fZ2 z end-position
     * \param fRed red color component
     * \param fGreen green color component
     * \param fBlue blue color component
     */
    svt_basic_quad( Real32 fX1, Real32 fY1, Real32 fZ1, Real32 fX2, Real32 fY2, Real32 fZ2, Real32 fRed, Real32 fGreen, Real32 fBlue );
    virtual ~svt_basic_quad() {};

    /**
     * set the start position of the quad
     * \param fX x position
     * \param fY y position
     * \param fZ z position
     */
    inline void setStartPosition(Real32 fX, Real32 fY, Real32 fZ)
    {
        m_fX1 = fX;
        m_fY1 = fY;
        m_fZ1 = fZ;
    };
    /**
     * set the end position of the quad
     * \param fX x position
     * \param fY y position
     * \param fZ z position
     */
    inline void setEndPosition(Real32 fX, Real32 fY, Real32 fZ)
    {
        m_fX2 = fX;
        m_fY2 = fY;
        m_fZ2 = fZ;
    };

    /**
     * draw the quad (PovRay)
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
