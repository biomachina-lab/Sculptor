/***************************************************************************
                          svt_atom_colmap.h
			  -----------------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ATOM_COLMAP_H
#define SVT_ATOM_COLMAP_H

#include <svt_core.h>
#include <svt_array.h>
#include <svt_types.h>
#include <svt_color.h>
#include <svt_palette.h>

class svt_atom;
class TiXmlElement;

typedef struct colmap
{
    char m_aType[5];
    unsigned int m_iColor;
} atom_colmap;

/** This class maps a color to an atom of a certain type. This is just the base class for all specific colormappings. E.g. there is a class svt_atom_type_colmap which maps the color according to the atom type.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_atom_colmap
{

protected:

    vector<atom_colmap> m_aMapping;
    svt_color m_cColor;

    svt_palette* m_pPalette;

public:
    /**
     * Constructor
     */
    svt_atom_colmap();
    virtual ~svt_atom_colmap();

    /**
     * returns the color object for an atom
     * \param pAtom pointer to an svt_atom
     * \return pointer to a svt_color object
     */
    virtual svt_color* getColor(svt_atom* pAtom);

    /**
     * returns the color object for an atom
     * \param pAtom pointer to an svt_atom
     * \return index into palette
     */
    virtual unsigned int getColorIndex(svt_atom* pAtom);

    /**
     * Returns a color object from the palette
     * \param iIndex index
     * \return svt_color object
     */
    svt_color getColor( unsigned int iIndex );

    /**
     * returns the color object for the bonds
     * \return pointer to a svt_color object
     */
    virtual svt_color* getColorBonds();

    /**
     * change the color mapping for a certain type of atoms
     * \param pType pointer to an array of char with the type/name of the atoms
     * \param iIndex index into the color palette
     */
    virtual void changeMapping(const char* pType, unsigned int iIndex);

    /**
     * get the mapping for a certain atom type
     * \param pType pointer to an array of char with the type/name of the atoms
     * \return index into the palette
     */
    virtual unsigned int getMapping(const char* pType);

    /**
     * get the mapping for a certain atom type
     * \param pType pointer to an array of char with the type/name of the atoms
     * \return red component
     */
    virtual Real32 getMappingRed(const char* pType);
    /**
     * get the mapping for a certain atom type
     * \param pType pointer to an array of char with the type/name of the atoms
     * \return green component
     */
    virtual Real32 getMappingGreen(const char* pType);
    /**
     * get the mapping for a certain atom type
     * \param pType pointer to an array of char with the type/name of the atoms
     * \return blue component
     */
    virtual Real32 getMappingBlue(const char* pType);

    /**
     * restores the state of document from xml datastructure
     * \param xml element with stored state of document
     */
    virtual void setState(TiXmlElement* pXml);
    /**
     * stores the state of document in xml datastructure
     * \param xml element to store state of document
     */
    virtual void getState(TiXmlElement* pXml);

    /**
     * Set the palette
     * \param pPalette pointer to svt_palette object
     */
    void setPalette( svt_palette* pPalette );
    /**
     * Get the palette
     * \return pointer to svt_palette object
     */
    svt_palette* getPalette( );
};

#endif
