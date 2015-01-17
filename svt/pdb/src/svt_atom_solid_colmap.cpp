/***************************************************************************
                          svt_atom_solid_colmap.cpp  -  description
                             -------------------
    begin                : 01/08/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_solid_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_solid_colmap::svt_atom_solid_colmap() : svt_atom_colmap(),
    m_iIndex( 0 )
{
    m_cColor.setR(1.0f);
    m_cColor.setG(1.0f);
    m_cColor.setB(1.0f);
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to a svt_atom object
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_solid_colmap::getColor(svt_atom*)
{
    m_cColor = svt_atom_colmap::getColor( m_iIndex );

    return &m_cColor;
}
/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_solid_colmap::getColorIndex(svt_atom*)
{
    return m_iIndex;
}


/**
 * returns the color object for the bonds
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_solid_colmap::getColorBonds()
{
    m_cColor = svt_atom_colmap::getColor( m_iIndex );

    return &m_cColor;
}

/**
 * change the color mapping for a certain type of atoms
 * \param pType pointer to an array of char with the type/name of the atoms
 * \param iIndex index into the color palette
 */
void svt_atom_solid_colmap::changeMapping(const char*, unsigned int iIndex)
{
    m_iIndex = iIndex;
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return index into the palette
 */
unsigned int svt_atom_solid_colmap::getMapping(const char*)
{
    return m_iIndex;
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return red component
 */
Real32 svt_atom_solid_colmap::getMappingRed(char*)
{
    m_cColor = svt_atom_colmap::getColor( m_iIndex );

    return m_cColor.getR();
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return green component
 */
Real32 svt_atom_solid_colmap::getMappingGreen(char*)
{
    m_cColor = svt_atom_colmap::getColor( m_iIndex );

    return m_cColor.getG();
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return blue component
 */
Real32 svt_atom_solid_colmap::getMappingBlue(char*)
{
    m_cColor = svt_atom_colmap::getColor( m_iIndex );

    return m_cColor.getB();
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_solid_colmap::setState(TiXmlElement* pElem)
{
    int i;
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pElem->FirstChild( "COLMAPSOLID" )) != NULL)
    {
	if (pChild->Attribute("INDEX", &i))
	    m_iIndex = i;
    }
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_solid_colmap::getState(TiXmlElement* pElem)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPSOLID" );
    pElem->LinkEndChild(pMode);
 
    pMode->SetAttribute("INDEX", m_iIndex );
}

