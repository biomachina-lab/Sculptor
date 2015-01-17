/***************************************************************************
                          svt_spline_section.h  -  description
                             -------------------
    begin                : 12/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SPLINE_SECTION_H
#define SVT_SPLINE_SECTION_H

#include <svt_core.h>
#include <svt_node_noautodl.h>
#include <svt_node.h>
#include <svt_structs.h>
#include <svt_atom.h>
#include <svt_atom_colmap.h>

#include <GL/gl.h>
#include <GL/glu.h>

class svt_basic_container;

enum {
    SVT_SPLINE_OFF,
    SVT_SPLINE_LINE,
    SVT_SPLINE_CYLINDER
};

/** A spline section class (a spline section between two atoms). This class is inherited from svt_node, but you should not use it directly. Instead it is used internally by the svt_pdb node.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_spline_section : public svt_node
{
protected:
    // spline interpolation points
    svt_p3d m_aSplinePoints[7];
    // length of cylinders
    Real32 m_aCylLength[6];
    // rotation vectors for all cylinders
    svt_p3d m_aCylRotVec[6];
    // rotation angle for all cylinders
    Real32 m_aCylRotAngle[6];
    // bond between atom a and b
    svt_atom* m_pAtomA;
    svt_atom* m_pAtomB;
    // how should this bond be displayed
    Int16  m_iDisplayMode;
    // the color mapping
    svt_atom_colmap* m_pMapping;
    // diameter of the cylinders
    float m_fCylDia;

public:
    /**
     * Constructor
     * create a spline section  between atom a and b
     * \param pA pointer to the first svt_atom object
     * \param pB pointer to the second svt_atom object
     * \param pMapping pointer to a svt_atom_colmap object
     */
    svt_spline_section(svt_atom* pA, svt_atom* pB, svt_atom_colmap* pMapping=NULL);
    ~svt_spline_section();

    /**
     * set the display mode for this spline section
     * \param iDisplayMode display mode (e.g. SVT_SPLINE_CYLINDER)
     */
    void setDisplayMode(Int16 dispmode);
    /**
     * get the display mode for this spline section
     * \return display mode
     */
    Int16 getDisplayMode();

    /**
     * set first atom
     * \param pA pointer to the first atom
     */
    void setAtomA(svt_atom* pA);
    /**
     * get the first atom
     * \return pointer to the first atom
     */
    svt_atom* getAtomA();
    /**
     * set the second atom
     * \param pB pointer to the second atom
     */
    void setAtomB(svt_atom* pB);
    /**
     * get the second atom
     * \return pointer to the second atom
     */
    svt_atom* getAtomB();

    /**
     * set the spline interpolation points
     * \param pP pointer to a svt_p3d object
     */
    void setSplinePoints(svt_p3d* pP);

    /**
     * prepare container
     * \param m_rCont reference to the svt_basic_container
     * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
     */
    void prepareContainer(svt_basic_container& m_rCont);

    /**
     * set the cylinder diameter (default is 2.0f)
     * \param fCylDia diameter of the cylinders
     */
    void setCylinderDia(float fCylDia);
    /**
     * get the cylinder diameter
     * \return diameter of the cylinders
     */
    float getCylinderDia();

    /**
     * set the colormapping
     * \param pMapping pointer to a svt_atom_colmap object
     */
    void setAtomColmap(svt_atom_colmap* pMapping);

protected:
    /**
     * calculate the data for the cylinders mode
     */
    void calcCylinders();
};

#endif
