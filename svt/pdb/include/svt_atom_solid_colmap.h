/***************************************************************************
                          svt_atom_solid_colmap.h  -  description
                             -------------------
    begin                : 01/08/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ATOM_SOLID_COLMAP_H
#define SVT_ATOM_SOLID_COLMAP_H

#include <svt_core.h>
#include <svt_color.h>
#include <svt_atom_colmap.h>

/** This class maps the same color to all atoms
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_atom_solid_colmap : public svt_atom_colmap
{
    unsigned int m_iIndex;

public:
    /**
     * Constructor
     */
    svt_atom_solid_colmap();

    /**
     * returns the color object for an atom
     * \param pAtom pointer to a svt_atom object
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
    virtual Real32 getMappingRed(char* pType);
    /**
     * get the mapping for a certain atom type
     * \param pType pointer to an array of char with the type/name of the atoms
     * \return green component
     */
    virtual Real32 getMappingGreen(char* type);
    /**
     * get the mapping for a certain atom type
     * \param pType pointer to an array of char with the type/name of the atoms
     * \return blue component
     */
    virtual Real32 getMappingBlue(char* type);

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
};

#endif
