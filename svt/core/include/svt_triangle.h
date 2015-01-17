/***************************************************************************
                          svt_triangle.h  -  description
                             -------------------
    begin                : Jul 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TRIANGLE_H
#define SVT_TRIANGLE_H

#include <svt_node.h>

/** A triangle
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_triangle : public svt_node
{
protected:

    svt_pos* m_pB;
    svt_pos* m_pC;
    svt_pos* m_pNormA;
    svt_pos* m_pNormB;
    svt_pos* m_pNormC;
    float m_aTexX[3];
    float m_aTexY[3];

public:

    /**
     * Constructor
     * You need four points to construct a line
     * \param pA pointer to svt_pos object
     * \param pB pointer to svt_pos object
     * \param pC pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_triangle(svt_pos* pA, svt_pos* pB, svt_pos* pC, svt_properties* pProp=NULL);
    virtual ~svt_triangle();

public:

    /**
     * set second point of triangle
     */
    void setB(svt_pos* pB);
    /**
     * get second point of triangle
     */
    svt_pos* getB();

    /**
     * set third point of triangle
     */
    void setC(svt_pos* pC);
    /**
     * get third point of triangle
     */
    svt_pos* getC();

    /**
     * set texture coord
     */
    void setTextureCoord(int i, float x, float y);
    /**
     * get texture coord
     */
    float getTextureCoordX(int i) const;
    /**
     * get texture coord
     */
    float getTextureCoordY(int i) const;

    /**
     * draw triangle
     */
    void drawGL();

    /**
     * print "Triangle" to stdout
     */
    void printName();
};

#endif
