/***************************************************************************
                          svt_atom_colmap.cpp
			  -------------------
    begin                : 11/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_atom_colmap.h>
#include <svt_atom.h>
#include <string.h>
#include <svt_tinyxml.h>
#include <svt_palette.h>

svt_color g_oColorWhite(1.0f,1.0f,1.0f);

svt_palette* g_pDefaultPalette = NULL;

/**
 * Constructor
 */
svt_atom_colmap::svt_atom_colmap()
{
    m_cColor.setShininess( 50.0f );
    m_cColor.setSpecular( 1.0f, 1.0f, 1.0f, 1.0f );

    if (g_pDefaultPalette == NULL)
    {
        g_pDefaultPalette = new svt_palette();
        g_pDefaultPalette->addColor( svt_color(0.90f, 0.90f, 0.90f ) );
        g_pDefaultPalette->addColor( svt_color(0.80f, 0.20f, 0.20f ) );
        g_pDefaultPalette->addColor( svt_color(0.25f, 0.25f, 1.00f ) );
        g_pDefaultPalette->addColor( svt_color(0.25f, 0.75f, 0.75f ) );
        g_pDefaultPalette->addColor( svt_color(0.80f, 0.80f, 0.00f ) );
        g_pDefaultPalette->addColor( svt_color(0.50f, 0.50f, 0.20f ) );
        g_pDefaultPalette->addColor( svt_color(0.60f, 0.60f, 0.60f ) );
    };

    setPalette( g_pDefaultPalette );
}
svt_atom_colmap::~svt_atom_colmap()
{
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_colmap::getColor(svt_atom*)
{
    return &g_oColorWhite;
}

/**
 * returns the color object for an atom
 * \param pAtom pointer to an svt_atom
 * \return index into palette
 */
unsigned int svt_atom_colmap::getColorIndex(svt_atom*)
{
    return 0;
}

/**
 * Returns a color object from the palette
 * \param iIndex index
 * \return svt_color object
 */
svt_color svt_atom_colmap::getColor( unsigned int iIndex )
{
    if (m_pPalette != NULL)
        return *(m_pPalette->getColor( iIndex ) );

    svt_color oColor;
    return oColor;
}

/**
 * returns the color object for the bonds
 * \return pointer to a svt_color object
 */
svt_color* svt_atom_colmap::getColorBonds()
{
    return &g_oColorWhite;
}

/**
 * change the color mapping for a certain type of atoms
 * \param pType pointer to an array of char with the type/name of the atoms
 * \param iIndex index into the color palette
 */
void svt_atom_colmap::changeMapping(const char* pType, unsigned int iIndex)
{
    if (pType == NULL)
        return;

    unsigned int i;

    for(i=0;i<m_aMapping.size();i++)
    {
        if (strcmp(m_aMapping[i].m_aType, pType) == 0)
	{
	    m_aMapping[i].m_iColor = iIndex;
            return;
        }
    }

    atom_colmap col;
    strcpy(col.m_aType, pType);
    col.m_iColor = iIndex;
    m_aMapping.push_back( col );
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return index into the palette
 */
unsigned int svt_atom_colmap::getMapping(const char* pType)
{
    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (strcmp(pType, m_aMapping[i].m_aType)==0)
            return m_aMapping[i].m_iColor;

    return 0;
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return red component
 */
Real32 svt_atom_colmap::getMappingRed(const char* pType)
{
    if (m_pPalette == NULL)
        return 0.0f;

    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (strcmp(pType, m_aMapping[i].m_aType)==0)
            return m_pPalette->getColor(m_aMapping[i].m_iColor)->getR();

    return 1.0f;
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return green component
 */
Real32 svt_atom_colmap::getMappingGreen(const char* pType)
{
    if (m_pPalette == NULL)
        return 0.0f;

    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (strcmp(pType, m_aMapping[i].m_aType)==0)
            return m_pPalette->getColor(m_aMapping[i].m_iColor)->getG();

    return 1.0f;
}

/**
 * get the mapping for a certain atom type
 * \param pType pointer to an array of char with the type/name of the atoms
 * \return blue component
 */
Real32 svt_atom_colmap::getMappingBlue(const char* pType)
{
    if (m_pPalette == NULL)
        return 0.0f;

    for(unsigned int i=0;i<m_aMapping.size(); i++)
        if (strcmp(pType, m_aMapping[i].m_aType)==0)
            return m_pPalette->getColor(m_aMapping[i].m_iColor)->getB();

    return 1.0f;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void svt_atom_colmap::setState(TiXmlElement* pElem)
{
    int iMappings;
    char pStr[256];
    const char* pPointer;
    TiXmlElement* pChild;
    int iIndex;

    m_aMapping.clear();

    if (pElem->Attribute("MAPPINGS", &iMappings))
    {
	for(int i=0;i<iMappings; i++)
	{
	    atom_colmap oColmap;
            oColmap.m_aType[0] = 0;
            oColmap.m_aType[1] = 0;
            oColmap.m_aType[2] = 0;
            oColmap.m_aType[3] = 0;
            oColmap.m_aType[4] = 0;

	    sprintf( pStr, "MAPPING%i", i );

	    if ((pChild = (TiXmlElement*) pElem->FirstChild( pStr )) != NULL)
	    {
		if (pChild->Attribute("INDEX", &iIndex))
                    oColmap.m_iColor = iIndex;
		if ((pPointer = pChild->Attribute("TYPE")) != NULL)
		    strncpy( oColmap.m_aType, pPointer, 5 );
	    }

            m_aMapping.push_back( oColmap );
	}
    }
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void svt_atom_colmap::getState(TiXmlElement* pElem)
{
    pElem->SetAttribute("MAPPINGS", m_aMapping.size());
    char pStr[256];

    for(unsigned int i=0;i<m_aMapping.size(); i++)
    {
	sprintf( pStr, "MAPPING%i", i );
	TiXmlElement* pMap = new TiXmlElement( pStr );
	pElem->LinkEndChild(pMap);

        pMap->SetAttribute("INDEX", m_aMapping[i].m_iColor );
        pMap->SetAttribute("TYPE",  m_aMapping[i].m_aType );
    }
}

/**
 * Set the palette
 * \param pPalette pointer to svt_palette object
 */
void svt_atom_colmap::setPalette( svt_palette* pPalette )
{
    m_pPalette = pPalette;
};

/**
 * Get the palette
 * \return pointer to svt_palette object
 */
svt_palette* svt_atom_colmap::getPalette( )
{
    return m_pPalette;
};

