/***************************************************************************
                          svt_palette
			  -----------------------
    begin                : 10/09/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PALETTE_H
#define SVT_PALETTE_H

#include <svt_core.h>
#include <svt_types.h>
#include <svt_color.h>
#include <svt_stlVector.h>
#include <svt_tinyxml.h>

/**
 * This class provides a color palette.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_palette
{

protected:

    vector<svt_color> m_aPalette;

public:
    /**
     * Constructor
     */
    svt_palette();
    virtual ~svt_palette(){};

    /**
     * Return the size of the palette
     * \return int with the number of color items in the palette
     */
    unsigned int size();

    /**
     * Returns a color from the palette
     * \param iIndex index of the color
     * \return pointer to a svt_color object
     */
    svt_color* getColor(unsigned int iIndex);

    /**
     * Add a new color to the palette
     * \param oColor svt_color object
     */
    void addColor( svt_color oColor );

    /**
     * Change a color
     * \param iIndex index of the color that one wants to change
     * \param oColor the new color object
     */
    void changeColor( unsigned int iIndex, svt_color oColor );

    /**
     * restores the state of document from xml datastructure
     * \param xml element with stored state of document
     */
    void setState(TiXmlElement* pXml);
    /**
     * stores the state of document in xml datastructure
     * \param xml element to store state of document
     */
    void getState(TiXmlElement* pXml);
};

#endif
