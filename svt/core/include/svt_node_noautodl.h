/***************************************************************************
                          svt_node_noautodl.h  -  description
                             -------------------
    begin                : 09.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_NODE_NOAUTODL_H
#define SVT_NODE_NOAUTODL_H

#include <svt_node.h>

/**
 * The basic node class for all classes which should not use display lists
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_node_noautodl : public svt_node {

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_node_noautodl(svt_pos* pPos =NULL, svt_properties* pProp =NULL);
    virtual ~svt_node_noautodl(){};
};

#endif
