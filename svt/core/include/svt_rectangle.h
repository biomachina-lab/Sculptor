/***************************************************************************
                          svt_rectangle.h
			  ---------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_RECTANGLE_H
#define SVT_RECTANGLE_H

#include <svt_node.h>

/**
 * A simple rectangle
 */
class DLLEXPORT svt_rectangle : public svt_node
{

protected:

    Real32 m_fWidth;
    Real32 m_fHeight;

public:
    /**
     * Constructor
     * You need four points to construct a rectangle
     * \param pA pointer to svt_pos object
     * \param pB pointer to svt_pos object
     * \param pC pointer to svt_pos object
     * \param pD pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_rectangle(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fWidth=0.5, Real32 fHeight=0.5);
    virtual ~svt_rectangle(){};

    /**
     * set width
     * \param fWidth width of rectangle
     */
    inline void setWidth(Real32 fWidth)
    {
	m_fWidth = fWidth;
    };
    /**
     * get width
     * \return width of rectangle
     */
    inline Real32 getWidth() const
    {
	return m_fWidth;
    };

    /**
     * set height
     * \param fHeight height of rectangle
     */
    inline void setHeight(Real32 fHeight)
    {
	m_fHeight = fHeight;
    };
    /**
     * get height
     * \return height of rectangle
     */
    inline Real32 getHeight() const
    {
	return m_fHeight;
    };

    /**
     * draw the rectangle
     */
    void drawGL();

    /**
     * print "Rectangle" to stdout
     */
    void printName();
};

#endif
