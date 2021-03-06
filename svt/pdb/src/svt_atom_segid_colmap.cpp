/***************************************************************************
                          svt_atom_segid_colmap.cpp  -  description
                             -------------------
    begin                : 02.05.02
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_segid_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_segid_colmap::svt_atom_segid_colmap() : svt_atom_colmap()
{
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
}

/**
 * returns the color object for an atom
 * \param pAtom
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_segid_colmap::getColor(svt_atom* pAtom)
{
    if (pAtom == NULL)
        return &m_cColor;

    int iSegID = 0;

    if (pAtom->getSegmentID() != NULL)
        iSegID = pAtom->getSegmentID()[0] + pAtom->getSegmentID()[1] + pAtom->getSegmentID()[2] + pAtom->getSegmentID()[3];

    m_cColor = svt_atom_colmap::getColor( iSegID % 30 );

    return &m_cColor;
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_segid_colmap::getColorIndex(svt_atom* pAtom)
{
    if (pAtom == NULL)
        return 0;

    int iSegID = 0;

    if (pAtom->getSegmentID() != NULL)
        iSegID = pAtom->getSegmentID()[0] + pAtom->getSegmentID()[1] + pAtom->getSegmentID()[2] + pAtom->getSegmentID()[3];

    return iSegID % 30;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_segid_colmap::setState(TiXmlElement* pXml)
{
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pXml->FirstChild( "COLMAPSEGID" )) != NULL)
	svt_atom_colmap::setState( pChild );
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_segid_colmap::getState(TiXmlElement* pXml)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPSEGID" );
    pXml->LinkEndChild(pMode);

    svt_atom_colmap::getState( pMode );
}
