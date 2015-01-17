/***************************************************************************
                          sculptor_node.h  -  description
                             -------------------
    begin                : 11.08.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_NODE_H
#define SCULPTOR_NODE_H

//svt includes
#include <svt_node.h>
class sculptor_document;



/** This class stores the sculptor version of a svt_node
 * @author Mirabela Rusu
 */
class sculptor_node : public svt_node
{

    //the sculptor document that contains node
    sculptor_document * m_pDoc;
public:
    /**
     * Constructor
     */ 
    sculptor_node(sculptor_document *pDoc, svt_pos* pPos =NULL, svt_properties* pProp =NULL):svt_node(pPos, pProp),
    m_pDoc(pDoc){};
    
    /**
     * Destructor
     */ 
    virtual ~sculptor_node(){};

    /**
    * recalc
    */
    virtual bool recalc();
};

#endif
