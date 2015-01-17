/***************************************************************************
                          svt_palette
			  -----------
    begin                : 10/09/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_palette.h>

/**
 * Constructor
 */
svt_palette::svt_palette()
{
}

/**
 * Return the size of the palette
 * \return int with the number of color items in the palette
 */
unsigned int svt_palette::size()
{
    return m_aPalette.size();
}

/**
 * Returns a color from the palette
 * \param iIndex index of the color
 * \return pointer to a svt_color object
 */
svt_color* svt_palette::getColor(unsigned int iIndex)
{
    if (m_aPalette.size() > 0)
        return &m_aPalette[iIndex % m_aPalette.size()];

    svt_color oColor;
    addColor( oColor );

    return &m_aPalette[0];
};

/**
 * Add a new color to the palette
 * \param oColor svt_color object
 */
void svt_palette::addColor( svt_color oColor )
{
    m_aPalette.push_back( oColor );
};

/**
 * Change a color
 * \param iIndex index of the color that one wants to change
 * \param oColor the new color object
 */
void svt_palette::changeColor( unsigned int iIndex, svt_color oColor )
{
    m_aPalette[iIndex] = oColor;
};

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_palette::setState(TiXmlElement*)
{
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_palette::getState(TiXmlElement*)
{
}
