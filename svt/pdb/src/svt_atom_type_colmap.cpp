/***************************************************************************
                          svt_atom_type_colmap.cpp
			  ------------------------
    begin                : 01/08/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_type_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_type_colmap::svt_atom_type_colmap() : svt_atom_colmap()
{
    atom_colmap H = { "H", 0 };
    m_aMapping.push_back(H);
    atom_colmap O = { "O", 1 };
    m_aMapping.push_back(O);
    atom_colmap N = { "N", 2 };
    m_aMapping.push_back(N);
    atom_colmap C = { "C", 3 };
    m_aMapping.push_back(C);
    atom_colmap S = { "S", 4 };
    m_aMapping.push_back(S);
    atom_colmap P = { "P", 5 };
    m_aMapping.push_back(P);
    atom_colmap Z = { "Z", 6 };
    m_aMapping.push_back(Z);
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to a svt_atom object
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_type_colmap::getColor(svt_atom* pAtom)
{
    m_cColor.setR(1.0f);
    m_cColor.setG(1.0f);
    m_cColor.setB(1.0f);

    for(unsigned int i=0;i<m_aMapping.size(); i++)
	if (strcmp(pAtom->getName(), m_aMapping[i].m_aType)==0 || pAtom->getName()[0] == m_aMapping[i].m_aType[0])
	{
            m_cColor = svt_atom_colmap::getColor(m_aMapping[i].m_iColor);
            break;
	}

    return &m_cColor;
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_type_colmap::getColorIndex(svt_atom* pAtom)
{
    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (strcmp(pAtom->getName(), m_aMapping[i].m_aType)==0 || pAtom->getName()[0] == m_aMapping[i].m_aType[0])
            return m_aMapping[i].m_iColor;

    return 0;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_type_colmap::setState(TiXmlElement* pXml)
{
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pXml->FirstChild( "COLMAPTYPE" )) != NULL)
	svt_atom_colmap::setState( pChild );
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_type_colmap::getState(TiXmlElement* pXml)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPTYPE" );
    pXml->LinkEndChild(pMode);

    svt_atom_colmap::getState( pMode );
}
