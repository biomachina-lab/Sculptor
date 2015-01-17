/***************************************************************************
                          svt_node_noautodl.cpp  -  description
                             -------------------
    begin                : 09.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_node_noautodl.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_node_noautodl::svt_node_noautodl(svt_pos* pPos , svt_properties* pProp ) : svt_node(pPos, pProp)
{
    setAutoDL(false);
}
