/***************************************************************************
                          svt_billboard.h
                          -------------------
    begin                : 06/06/2001
    author               : Herwig Zilken, Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BILLBOARD_H
#define SVT_BILLBOARD_H

#include <svt_core.h>
#include <svt_node_noautodl.h>

class DLLEXPORT svt_billboard : public svt_node_noautodl
{
public:
    /**
     * Constructor
     * After this node the next nodes in the scenegraph are aligned towards the viewer
     */
    svt_billboard();
    virtual ~svt_billboard(){};

    /**
     * drawGL is here responsible for the alignement to the viewer
     */
    virtual void drawGL();
};

#endif
