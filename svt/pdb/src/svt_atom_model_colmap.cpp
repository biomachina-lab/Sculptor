/***************************************************************************
                          svt_atom_model_colmap.cpp
			  ---------------------------
    begin                : 05/31/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_model_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_model_colmap::svt_atom_model_colmap() : svt_atom_colmap()
{
    char oStr[10];

    for(unsigned int i=0; i<99;i++)
    {
        sprintf( oStr, "%i", i );
        atom_colmap A;
        strcpy( A.m_aType, oStr );
        A.m_iColor = i;
        m_aMapping.push_back(A);
    }
}

/**
 * returns the color object for an atom
 * \param pAtom
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_model_colmap::getColor(svt_atom* pAtom)
{
    unsigned int iModel = pAtom->getModel() % 99;

    m_cColor = svt_atom_colmap::getColor(m_aMapping[iModel].m_iColor);

    return &m_cColor;
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_model_colmap::getColorIndex(svt_atom* pAtom)
{
    return m_aMapping[pAtom->getModel() % 99].m_iColor;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_model_colmap::setState(TiXmlElement* pXml)
{
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pXml->FirstChild( "COLMAPMODEL" )) != NULL)
        svt_atom_colmap::setState( pChild );
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_model_colmap::getState(TiXmlElement* pXml)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPMODEL" );
    pXml->LinkEndChild(pMode);

    svt_atom_colmap::getState( pMode );
}
