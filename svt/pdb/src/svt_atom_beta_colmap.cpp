/***************************************************************************
                          svt_atom_beta_colmap
			  --------------------
    begin                : 06/21/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_beta_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>

/**
 * Constructor
 */
svt_atom_beta_colmap::svt_atom_beta_colmap() : svt_atom_colmap(),
    m_fMaximum( 1000.0 ),
    m_fMinimum( 0.0 )
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
svt_color* svt_atom_beta_colmap::getColor(svt_atom* pAtom)
{
    if (m_pPalette != NULL)
    {
	unsigned int iIndex = (unsigned int)(((pAtom->getTempFact() - m_fMinimum) / m_fMaximum) * (Real64)(m_pPalette->size()));

	m_cColor = svt_atom_colmap::getColor( iIndex );
    }

    return &m_cColor;
}
/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_beta_colmap::getColorIndex(svt_atom* pAtom)
{
    if (m_pPalette != NULL)
    {
	unsigned int iIndex = (unsigned int)(((pAtom->getTempFact() - m_fMinimum) / m_fMaximum) * (Real64)(m_pPalette->size()));
	return iIndex;
    }

    return 0;
}

/**
 * Set maximum b-factor value
 * \param fMaximum the maximum b-factor in the structure
 */
void svt_atom_beta_colmap::setMaxBeta( Real64 fMaximum )
{
    m_fMaximum = fMaximum;
};
/**
 * Set minimum b-factor value
 * \param fMinimum the minimum b-factor in the structure
 */
void svt_atom_beta_colmap::setMinBeta( Real64 fMinimum )
{
    m_fMinimum = fMinimum;
};

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_beta_colmap::setState(TiXmlElement* pElem)
{
    Real64 fValue;
    TiXmlElement* pChild;

    if ((pChild = (TiXmlElement*) pElem->FirstChild( "COLMAPBETA" )) != NULL)
    {
	if (pChild->Attribute("MAXIMUM", &fValue))
	    m_fMaximum = fValue;
	if (pChild->Attribute("MINIMUM", &fValue))
	    m_fMinimum = fValue;
    }
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_beta_colmap::getState(TiXmlElement* pElem)
{
    TiXmlElement* pMode = new TiXmlElement( "COLMAPBETA" );
    pElem->LinkEndChild(pMode);
 
    pMode->SetDoubleAttribute("MAXIMUM", m_fMaximum );
    pMode->SetDoubleAttribute("MINIMUM", m_fMinimum );
}

