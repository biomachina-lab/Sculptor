/***************************************************************************
                          svt_fps
                          -------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FPS_H
#define SVT_FPS_H

#include <svt_node.h>
#include <svt_iostream.h>

/** A frame per second counter
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_fps : public svt_node
{
public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param pFont pointer to svt_font object
     */
    svt_fps(svt_pos* pPos =NULL, svt_properties* pProp =NULL);
    virtual ~svt_fps();

    /**
     * recalc function
     */
    bool recalc();

    /**
     * print "FPS" to stdout
     */
    void printName();
};

#endif
