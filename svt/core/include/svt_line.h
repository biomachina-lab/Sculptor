/***************************************************************************
                          svt_line.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_LINE_H
#define SVT_LINE_H

#include <svt_node.h>

/** A line
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_line : public svt_node  {

protected:
    svt_pos* m_pB;
    Real32 m_fLineWidth;

public:
    /**
     * Constructor
     * You need four points to construct a line
     * \param pA pointer to svt_pos object
     * \param pB pointer to svt_pod object
     * \param pProp pointer to svt_properties object
     */
    svt_line(svt_pos* pA, svt_pos* pB, svt_properties* pProp =NULL);
    virtual ~svt_line();

    /**
     * set the second point
     * \param pB pointer to svt_pod object
     */
    void setB(svt_pos* pB);

    /**
     * set line width
     * \param fLineWidth line width
     */
    void setWidth(Real32 fLineWidth);
    /**
     * get line width
     * \return line width
     */
    Real32 getWidth(void);

    /**
     * draw the line
     */
    void drawGL();

    /**
     * print "Line" to stdout
     */
    void printName();
};

#endif
