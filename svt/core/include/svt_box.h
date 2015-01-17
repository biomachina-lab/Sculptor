/***************************************************************************
                          svt_box.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BOX_H
#define SVT_BOX_H

// svt includes
#include <svt_node.h>
// clib includes
#include <svt_iostream.h>

/** A box
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_box : public svt_node  {

protected:

    // we need a second coordinate for a box to describe the size
    svt_pos* m_pSize;

public:
    /**
     * Constructor
     * \param pPos pointer to a svt_pos object for the position of the new object
     * \param pSize pointer to a svt_pos object for the second coordinate for the svt_box object (the lower right vertice of the box)
     * \param pProp pointer to a svt_properties object for the properties
     */
    svt_box(svt_pos* pPos=NULL, svt_pos* pSize=NULL, svt_properties* pProp=NULL);
    virtual ~svt_box();

    /**
     * set the second coordinate of the box
     * \param pSize pointer to a svt_pos object with the second coord of the box (lower right vertice of the box)
     */
    void setSize(svt_pos* pSize);
    /**
     * get the second coordinate of the box
     * \return pointer to a svt_pos object with the second coord
     */
    svt_pos* getSize();

    /**
     * draw the box
     */
    void drawGL();

    /**
     * print the name of the node to stdout
     */
    void printName();
};

#endif
