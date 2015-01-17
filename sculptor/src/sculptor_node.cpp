/***************************************************************************
                          sculptor_node.cpp  -  description
                             -------------------
    begin                : 11.08.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

//sculptor includes
#include <sculptor_node.h>
#include <sculptor_window.h>
#include <sculptor_scene.h>
#include <sculptor_doc_marker.h>

extern sculptor_window* g_pWindow;

/**
* recalc
*/
bool sculptor_node::recalc()
{
    if (m_pDoc->getType()==SENSITUS_DOC_MARKER&& g_pWindow->getScene()->getMovedNode()==m_pDoc->getNode())
        ((sculptor_doc_marker*)m_pDoc)->updatePropDlg();

    return true;
};

