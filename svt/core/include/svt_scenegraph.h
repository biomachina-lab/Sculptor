/***************************************************************************
                          svt_scenegraph.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SCENEGRAPH_H
#define SVT_SCENEGRAPH_H

#include <svt_core.h>
#include <svt_node.h>
#include <svt_array.h>

/** Represents the scenegraph concept in svt
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_scenegraph : public svt_node {

public:
    svt_scenegraph();
    virtual ~svt_scenegraph();

public:
    /** print a text to stdout showing the hierarchy of the scenegraph */
    void printGraph();

    /**
     * vrml drawing routine
     */
    virtual void drawVrml();

protected:
    void printGraph(svt_node* start, int hier);
};

#endif
