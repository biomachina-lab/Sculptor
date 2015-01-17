/***************************************************************************
                          svt_atom_chainid_colmap.cpp  -  description
                             -------------------
    begin                : 01/08/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_chainid_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_chainid_colmap::svt_atom_chainid_colmap() : svt_atom_colmap()
{
    atom_colmap Dash = { "-", 0 };
    m_aMapping.push_back(Dash);
    atom_colmap A = { "A", 0 };
    m_aMapping.push_back(A);
    atom_colmap B = { "B", 1 };
    m_aMapping.push_back(B);
    atom_colmap C = { "C", 2 };
    m_aMapping.push_back(C);
    atom_colmap D = { "D", 3 };
    m_aMapping.push_back(D);
    atom_colmap E = { "E", 4 };
    m_aMapping.push_back(E);
    atom_colmap F = { "F", 5 };
    m_aMapping.push_back(F);
    atom_colmap G = { "G", 6 };
    m_aMapping.push_back(G);
    atom_colmap H = { "H", 7 };
    m_aMapping.push_back(H);
    atom_colmap I = { "I", 8 };
    m_aMapping.push_back(I);
    atom_colmap J = { "J", 9 };
    m_aMapping.push_back(J);
    atom_colmap K = { "K", 10 };
    m_aMapping.push_back(K);
    atom_colmap L = { "L", 11 };
    m_aMapping.push_back(L);
    atom_colmap M = { "M", 12 };
    m_aMapping.push_back(M);
    atom_colmap N = { "N", 13 };
    m_aMapping.push_back(N);
    atom_colmap P = { "P", 14 };
    m_aMapping.push_back(P);
    atom_colmap R = { "R", 15 };
    m_aMapping.push_back(R);
    atom_colmap S = { "S", 16 };
    m_aMapping.push_back(S);
    atom_colmap T = { "T", 17 };
    m_aMapping.push_back(T);
    atom_colmap U = { "U", 18 };
    m_aMapping.push_back(U);
    atom_colmap V = { "V", 19 };
    m_aMapping.push_back(V);
    atom_colmap W = { "W", 20 };
    m_aMapping.push_back(W);
    atom_colmap X = { "X", 21 };
    m_aMapping.push_back(X);
    atom_colmap Y = { "Y", 22 };
    m_aMapping.push_back(Y);
    atom_colmap Z = { "Z", 23 };
    m_aMapping.push_back(Z);
    atom_colmap CI0 = { "0", 24 };
    m_aMapping.push_back(CI0);
    atom_colmap CI1 = { "1", 25 };
    m_aMapping.push_back(CI1);
    atom_colmap CI2 = { "2", 26 };
    m_aMapping.push_back(CI2);
    atom_colmap CI3 = { "3", 27 };
    m_aMapping.push_back(CI3);
    atom_colmap CI4 = { "4", 28 };
    m_aMapping.push_back(CI4);
    atom_colmap CI5 = { "5", 29 };
    m_aMapping.push_back(CI5);
    atom_colmap CI6 = { "6", 30 };
    m_aMapping.push_back(CI6);
    atom_colmap CI7 = { "7", 31 };
    m_aMapping.push_back(CI7);
    atom_colmap CI8 = { "8", 32 };
    m_aMapping.push_back(CI8);
    atom_colmap CI9 = { "9", 33 };
    m_aMapping.push_back(CI9);
}

/**
 * returns the color object for an atom
 * \param pAtom
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_chainid_colmap::getColor(svt_atom* pAtom)
{
    m_cColor.setR(1.0f);
    m_cColor.setG(1.0f);
    m_cColor.setB(1.0f);

    for(unsigned int i=0;i<m_aMapping.size(); i++)
    {
        if (pAtom->getChainID() == m_aMapping[i].m_aType[0])
        {
            m_cColor = svt_atom_colmap::getColor( m_aMapping[i].m_iColor );

            return &m_cColor;
        }
    }

    return &m_cColor;
}

/**
 * returns the color object for an atom
 * \param pAtom
 * \return index into palette
 */
unsigned int svt_atom_chainid_colmap::getColorIndex(svt_atom* pAtom)
{
    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (pAtom->getChainID() == m_aMapping[i].m_aType[0])
            return i;

    return 0;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_chainid_colmap::setState(TiXmlElement* pXml)
{
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pXml->FirstChild( "COLMAPCHAINID" )) != NULL)
	svt_atom_colmap::setState( pChild );
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_chainid_colmap::getState(TiXmlElement* pXml)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPCHAINID" );
    pXml->LinkEndChild(pMode);

    svt_atom_colmap::getState( pMode );
}
