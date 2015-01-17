/***************************************************************************
                          svt_atom_chainid_colmap
			  -----------------------
    begin                : 01.05.02
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ATOM_CHAINID_COLMAP_H
#define SVT_ATOM_CHAINID_COLMAP_H

#include <svt_core.h>
#include <svt_color.h>
#include <svt_atom_colmap.h>

class TiXmlElement;

/**
 * This class maps a color to an atom according to its chainid.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_atom_chainid_colmap : public svt_atom_colmap
{

public:

    /**
     * Constructor
     */
    svt_atom_chainid_colmap();

    /**
     * returns the color object for an atom
     * \param pAtom
     * \return pointer to a svt_color object
     */
    virtual svt_color* getColor(svt_atom* a);

    /**
     * returns the color object for an atom
     * \param pAtom pointer to an svt_atom
     * \return index into palette
     */
    virtual unsigned int getColorIndex(svt_atom* pAtom);

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
