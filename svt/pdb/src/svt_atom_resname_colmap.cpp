/***************************************************************************
                          svt_atom_resname_colmap.cpp  -  description
                             -------------------
    begin                : 01/08/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_resname_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_resname_colmap::svt_atom_resname_colmap() : svt_atom_colmap()
{
    atom_colmap ALA = { "ALA", 0 };
    m_aMapping.push_back(ALA);
    atom_colmap ARG = { "ARG", 1 };
    m_aMapping.push_back(ARG);
    atom_colmap ASN = { "ASN", 2 };
    m_aMapping.push_back(ASN);
    atom_colmap ASP = { "ASP", 3 };
    m_aMapping.push_back(ASP);
    atom_colmap CYS = { "CYS", 4 };
    m_aMapping.push_back(CYS);
    atom_colmap GLY = { "GLY", 5 };
    m_aMapping.push_back(GLY);
    atom_colmap GLU = { "GLU", 6 };
    m_aMapping.push_back(GLU);
    atom_colmap GLN = { "GLN", 7 };
    m_aMapping.push_back(GLN);
    atom_colmap HIS = { "HIS", 8 };
    m_aMapping.push_back(HIS);
    atom_colmap ILE = { "ILE", 9 };
    m_aMapping.push_back(ILE);
    atom_colmap LEU = { "LEU", 10 };
    m_aMapping.push_back(LEU);
    atom_colmap LYS = { "LYS", 11 };
    m_aMapping.push_back(LYS);
    atom_colmap MET = { "MET", 12 };
    m_aMapping.push_back(MET);
    atom_colmap PHE = { "PHE", 13 };
    m_aMapping.push_back(PHE);
    atom_colmap PRO = { "PRO", 14 };
    m_aMapping.push_back(PRO);
    atom_colmap SER = { "SER", 15 };
    m_aMapping.push_back(SER);
    atom_colmap THR = { "THR", 16 };
    m_aMapping.push_back(THR);
    atom_colmap TRP = { "TRP", 17 };
    m_aMapping.push_back(TRP);
    atom_colmap TYR = { "TYR", 18 };
    m_aMapping.push_back(TYR);
    atom_colmap VAL = { "VAL", 19 };
    m_aMapping.push_back(VAL);
    atom_colmap ADE = { "ADE", 20 };
    m_aMapping.push_back(ADE);
    atom_colmap CYT = { "CYT", 21 };
    m_aMapping.push_back(CYT);
    atom_colmap GUA = { "GUA", 22 };
    m_aMapping.push_back(GUA);
    atom_colmap THY = { "THY", 23 };
    m_aMapping.push_back(THY);
    atom_colmap TIP = { "TIP", 24 };
    m_aMapping.push_back(TIP);
    atom_colmap WAT = { "WAT", 25 };
    m_aMapping.push_back(WAT);
    atom_colmap SOL = { "SOL", 26 };
    m_aMapping.push_back(SOL);
    atom_colmap H2O = { "H2O", 27 };
    m_aMapping.push_back(H2O);
    atom_colmap LYR = { "LYR", 28 };
    m_aMapping.push_back(LYR);
    atom_colmap EOH = { "EOH", 29 };
    m_aMapping.push_back(EOH);
}

/**
 * returns the color object for an atom
 * \param pAtom
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_resname_colmap::getColor(svt_atom* pAtom)
{
    m_cColor.setR(1.0f);
    m_cColor.setG(1.0f);
    m_cColor.setB(1.0f);

    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (pAtom->getResname()[0] == m_aMapping[i].m_aType[0] &&
            pAtom->getResname()[1] == m_aMapping[i].m_aType[1] &&
            pAtom->getResname()[2] == m_aMapping[i].m_aType[2])
        {
            m_cColor = svt_atom_colmap::getColor( m_aMapping[i].m_iColor );
            break;
        }

    return &m_cColor;
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_resname_colmap::getColorIndex(svt_atom* pAtom)
{
    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (pAtom->getResname()[0] == m_aMapping[i].m_aType[0] &&
            pAtom->getResname()[1] == m_aMapping[i].m_aType[1] &&
            pAtom->getResname()[2] == m_aMapping[i].m_aType[2])
        {
            return i;
        }

    return 0;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_resname_colmap::setState(TiXmlElement* pXml)
{
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pXml->FirstChild( "COLMAPRESNAME" )) != NULL)
	svt_atom_colmap::setState( pChild );
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_resname_colmap::getState(TiXmlElement* pXml)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPRESNAME" );
    pXml->LinkEndChild(pMode);

    svt_atom_colmap::getState( pMode );
}
