/***************************************************************************
                          svt_atom_structure_colmap.h
			  ---------------------------
    begin                : 08/20/03
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ATOM_STRUCTURE_COLMAP_H
#define SVT_ATOM_STRUCTURE_COLMAP_H

#include <svt_core.h>
#include <svt_color.h>
#include <svt_atom_colmap.h>

/** This class maps a color to an atom according to its structure.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_atom_structure_colmap : public svt_atom_colmap
{

public:
    /**
     * Constructor
     */
    svt_atom_structure_colmap();

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

    /**
     * sets the minimum number of residues per helix
     * \param iMinResiduesPerHelix
     */
    void setMinResiduesPerHelix(int iMinResiduesPerHelix);

    /**
     * returns the minimum number of residues per helix
     * \return iMinResiduesPerHelix
     */
    int getMinResiduesPerHelix();

    /**
     * sets the minimum number of residues per sheet
     * \param iMinResiduesPerSheet
     */
    void setMinResiduesPerSheet(int iMinResiduesPerSheet);

    /**
     * returns the minimum number of residues per sheet
     * \return iMinResiduesPerSheet
     */
    int getMinResiduesPerSheet();

private:

    int m_iMinResiduesPerHelix;
    int m_iMinResiduesPerSheet;

};

#endif
