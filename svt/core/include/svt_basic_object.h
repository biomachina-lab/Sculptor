/***************************************************************************
                          svt_basic_object.h
                          -------------------
    begin                : 6/14/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BASIC_OBJECT_H
#define SVT_BASIC_OBJECT_H

#include <svt_matrix4.h>
#include <svt_types.h>
#include <svt_color.h>

class svt_basic_container;

/**
  * \brief The basic object class (base class for simple nodes like svt_basic_sphere)
  * \author Stefan Birmanns
  *
  * This is, compared to the standard svt_node, the base class for only very basic primitives. The derived classes like svt_basic_sphere,
  * only hold the very basic information about the objects, like position and orientation, radius and color. The objects don't have
  * more fancy properties like texturing, but they can be drawn very efficiently using a container class (svt_basic_container).
  * Attention: This is not a scenegraph-node! Only the svt_basic_container can get attached to a scenegraph!
  */
class DLLEXPORT svt_basic_object
{


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
    svt_basic_object( svt_basic_container& rCont, Real32 fRed, Real32 fGreen, Real32 fBlue );
    /**
     * Destructor
     */
    virtual ~svt_basic_object();

    /**
     * set the color
     * \param fRed red color component
     * \param fGreen green color component
     * \param fBlue blue color component
     */
    inline void setColor( Real32 fRed, Real32 fGreen, Real32 fBlue )
    {
        m_aMatAmbient[0] = fRed;
        m_aMatAmbient[1] = fGreen;
        m_aMatAmbient[2] = fBlue;
        m_aMatAmbient[3] = 1.0f;
    };
    /**
     * set the color
     * \param rColor reference to svt_color object
     */
    inline void setColor( svt_color& rColor )
    {
        m_aMatAmbient[0] = rColor.getR();
        m_aMatAmbient[1] = rColor.getG();
        m_aMatAmbient[2] = rColor.getB();
        m_aMatAmbient[3] = 1.0f;
    };
    /**
     * get the red color component
     * \return red color component
     */
    inline Real32 getR() { return m_aMatAmbient[0]; };
    /**
     * get the green color component
     * \return green color component
     */
    inline Real32 getG() { return m_aMatAmbient[1]; };
    /**
     * get the blue color component
     * \return blue color component
     */
    inline Real32 getB() { return m_aMatAmbient[2]; };

    /**
     * draw the object (OpenGL)
     * \param iWin index of window (opengl context)
     * \param rCont reference to parent container object
     */
    virtual void drawGL(int iWin, svt_basic_container& rCont);

    /**
     * draw the object (OpenInventor)
     */
    virtual void drawIV();

    /**
     * draw the object (PovRay)
     */
    virtual void drawPOV();

    /**
     * draw the object (Yafray)
     */
    virtual void drawYafray();

    /**
     * draw the object (Alias OBJ format)
     */
    virtual void drawOBJ();


};

#endif
