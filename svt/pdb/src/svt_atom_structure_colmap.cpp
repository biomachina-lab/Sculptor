/***************************************************************************
                          svt_atom_structure_colmap.cpp
			  -----------------------------
    begin                : 08/20/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_structure_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_structure_colmap::svt_atom_structure_colmap() : svt_atom_colmap()
{
    atom_colmap T = { "T", 10 };
    m_aMapping.push_back(T);
    atom_colmap C = { "C", 10 };
    m_aMapping.push_back(C);
    atom_colmap H = { "H", 3 };
    m_aMapping.push_back(H);
    atom_colmap G = { "G", 3 };
    m_aMapping.push_back(G);
    atom_colmap I = { "I", 3 };
    m_aMapping.push_back(I);
    atom_colmap E = { "E", 5 };
    m_aMapping.push_back(E);
    atom_colmap B = { "B", 5 };
    m_aMapping.push_back(B);

    m_iMinResiduesPerHelix = 2;
    m_iMinResiduesPerSheet = 2;
}


/**
 * sets the minimum number of residues per helix
 * \param iMinResiduesPerHelix
 */
void svt_atom_structure_colmap::setMinResiduesPerHelix(int iMinResiduesPerHelix)
{
    m_iMinResiduesPerHelix = iMinResiduesPerHelix;
}


/**
 * returns the minimum number of residues per helix
 * \param iMinResiduesPerHelix
 */
int svt_atom_structure_colmap::getMinResiduesPerHelix()
{
    return m_iMinResiduesPerHelix;
}


/**
 * sets the minimum number of residues per sheet
 * \param iMinResiduesPerSheet
 */
void svt_atom_structure_colmap::setMinResiduesPerSheet(int iMinResiduesPerSheet)
{
    m_iMinResiduesPerSheet = iMinResiduesPerSheet;
}


/**
 * returns the minimum number of residues per sheet
 * \param iMinResiduesPerSheet
 */
int svt_atom_structure_colmap::getMinResiduesPerSheet()
{
    return m_iMinResiduesPerSheet;
}


/**
 * returns the color object for an atom
 * \param pAtom
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_structure_colmap::getColor(svt_atom* pAtom)
{
    m_cColor.setR(1.0f);
    m_cColor.setG(1.0f);
    m_cColor.setB(1.0f);

    switch (pAtom->getSecStruct())
    {
        case 'H': case 'G': case 'I':
	{

	    if (pAtom->getSecStructNumResidues() >= m_iMinResiduesPerHelix)
	        m_cColor = svt_atom_colmap::getColor( m_aMapping[2].m_iColor );
	    else
	        m_cColor = svt_atom_colmap::getColor( m_aMapping[0].m_iColor );

	    break;

	}
        case 'E': case 'B':
	{

	    if (pAtom->getSecStructNumResidues() >= m_iMinResiduesPerSheet)
		m_cColor = svt_atom_colmap::getColor( m_aMapping[5].m_iColor );
	    else
	        m_cColor = svt_atom_colmap::getColor( m_aMapping[0].m_iColor );

	    break;

	}
        default:
	{

	    m_cColor = svt_atom_colmap::getColor( m_aMapping[0].m_iColor );
	    break;

	}
    }

//     or comment the above and uncomment the lines below if different types of helix or sheet are to be colored differently
//     above version is probably somewhat faster...
//     unsigned int i;
//     char cSecStruc = pAtom->getSecStruct();
//     for(i=0; i<m_aMapping.size(); i++)
//         if (cSecStruc == m_aMapping[i].m_aType[0])
//         {
// 	    if (cSecStruc == 'H' || cSecStruc == 'G' || cSecStruc == 'I')
// 	    {
// 		if (pAtom->getSecStructNumResidues() >= m_iMinResiduesPerHelix)
// 		    m_cColor = svt_atom_colmap::getColor( m_aMapping[i].m_iColor );
// 		else
// 		    m_cColor = svt_atom_colmap::getColor( m_aMapping[0].m_iColor );
// 		break;
// 	    }
// 	    if (cSecStruc == 'E' || cSecStruc == 'B')
// 	    {
// 		if (pAtom->getSecStructNumResidues() >= m_iMinResiduesPerSheet)
// 		    m_cColor = svt_atom_colmap::getColor( m_aMapping[i].m_iColor );
// 		else
// 		    m_cColor = svt_atom_colmap::getColor( m_aMapping[0].m_iColor );
// 		break;
// 	    }
// 	    m_cColor = svt_atom_colmap::getColor( m_aMapping[i].m_iColor );
//             break;
//         }

    return &m_cColor;
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_structure_colmap::getColorIndex(svt_atom* pAtom)
{
    unsigned int i;
    char cSecStruc = pAtom->getSecStruct();

    for(i=0; i<m_aMapping.size(); i++)
        if (cSecStruc == m_aMapping[i].m_aType[0])
        {
	    if (cSecStruc == 'H' || cSecStruc == 'G' || cSecStruc == 'I')
	    {
		if (pAtom->getSecStructNumResidues() >= m_iMinResiduesPerHelix)
		    return i;
		else
		    return 0;
	    }
	    if (cSecStruc == 'E' || cSecStruc == 'B')
	    {
		if (pAtom->getSecStructNumResidues() >= m_iMinResiduesPerSheet)
		    return i;
		else
		    return 0;
	    }
	    return i;
        }
    return 0;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_structure_colmap::setState(TiXmlElement* pXml)
{
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pXml->FirstChild( "COLMAPSTRUCTURE" )) != NULL)
	svt_atom_colmap::setState( pChild );
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_structure_colmap::getState(TiXmlElement* pXml)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPSTRUCTURE" );
    pXml->LinkEndChild(pMode);

    svt_atom_colmap::getState( pMode );
}
