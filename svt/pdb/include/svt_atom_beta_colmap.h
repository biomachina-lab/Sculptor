/***************************************************************************
                          svt_atom_beta_colmap
			  --------------------
    begin                : 06/21/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ATOM_BETA_COLMAP_H
#define SVT_ATOM_BETA_COLMAP_H

#include <svt_core.h>
#include <svt_color.h>
#include <svt_atom_colmap.h>

/** This class maps the b-factor to a color
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_atom_beta_colmap : public svt_atom_colmap
{
    Real64 m_fMaximum;
    Real64 m_fMinimum;

public:
    /**
     * Constructor
     */
    svt_atom_beta_colmap();

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
     * Set maximum b-factor value
     * \param fMaximum the maximum b-factor in the structure
     */
    void setMaxBeta( Real64 fMaximum );
    /**
     * Set minimum b-factor value
     * \param fMinimum the minimum b-factor in the structure
     */
    void setMinBeta( Real64 fMinimum );

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
