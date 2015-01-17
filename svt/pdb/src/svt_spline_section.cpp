/***************************************************************************
                          svt_spline_section.cpp  -  description
                             -------------------
    begin                : 12/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_spline_section.h>
#include <svt_atom_colmap.h>

#include <svt_basic_container.h>
#include <svt_basic_cylinder.h>
#include <svt_basic_line.h>

///////////////////////////////////////////////////////////////////////////////
// member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * create a spline section  between atom a and b
 * \param pA pointer to the first svt_atom object
 * \param pB pointer to the second svt_atom object
 * \param pMapping pointer to a svt_atom_colmap object
 */
svt_spline_section::svt_spline_section(svt_atom* pA, svt_atom* pB, svt_atom_colmap* pMapping)
{
    setAtomColmap(pMapping);
    setAtomA(pA);
    setAtomB(pB);
    setDisplayMode(SVT_SPLINE_LINE);
    setCylinderDia(2.0f);
};
svt_spline_section::~svt_spline_section()
{
};

/**
 * set the display mode for this spline section
 * \param iDisplayMode display mode (e.g. SVT_SPLINE_CYLINDER)
 */
void svt_spline_section::setDisplayMode(Int16 iDisplayMode)
{
    m_iDisplayMode = iDisplayMode;
}
/**
 * get the display mode for this spline section
 * \return display mode
 */
Int16 svt_spline_section::getDisplayMode()
{
    return m_iDisplayMode;
}

/**
 * set first atom
 * \param pA pointer to the first atom
 */
void svt_spline_section::setAtomA(svt_atom* pA)
{
    m_pAtomA = pA;
}
/**
 * get the first atom
 * \return pointer to the first atom
 */
svt_atom* svt_spline_section::getAtomA()
{
    return m_pAtomA;
}

/**
 * set the second atom
 * \param pB pointer to the second atom
 */
void svt_spline_section::setAtomB(svt_atom* pB)
{
    m_pAtomB = pB;
}
/**
 * get the second atom
 * \return pointer to the second atom
 */
svt_atom* svt_spline_section::getAtomB()
{
    return m_pAtomB;
}

/**
 * set the spline interpolation points
 * \param pP pointer to a svt_p3d object
 */
void svt_spline_section::setSplinePoints(svt_p3d* pP)
{
    memcpy(m_aSplinePoints, pP, sizeof(svt_p3d)*7);
    calcCylinders();
}

/**
 * prepare container
 * \param m_rCont reference to the svt_basic_container
 * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc) instead of directly outputting opengl commands.
 */
void svt_spline_section::prepareContainer(svt_basic_container& m_rCont)
{
    svt_basic_line* pLine;
    svt_basic_cylinder* pCyl;
    int i;
    int iDisplayMode = m_iDisplayMode;

    if (m_fCylDia < 0.5f)
        iDisplayMode = SVT_SPLINE_LINE;

    switch(iDisplayMode)
    {
    case SVT_SPLINE_CYLINDER:
        for(i=0;i<3;i++)
        {
            pCyl = new svt_basic_cylinder(
                                          m_aSplinePoints[i+1].x, m_aSplinePoints[i+1].y, m_aSplinePoints[i+1].z,
                                          m_aCylRotVec[i].x, m_aCylRotVec[i].y, m_aCylRotVec[i].z, m_aCylRotAngle[i],
                                          m_aCylLength[i],
                                          m_fCylDia * 0.001f,
                                          m_pMapping->getColor(m_pAtomA)->getR(),
                                          m_pMapping->getColor(m_pAtomA)->getG(),
                                          m_pMapping->getColor(m_pAtomA)->getB()
                                         );
            m_rCont << pCyl;
        }
        for(i=3;i<6;i++)
        {
            pCyl = new svt_basic_cylinder(
                                          m_aSplinePoints[i+1].x,m_aSplinePoints[i+1].y,m_aSplinePoints[i+1].z,
                                          m_aCylRotVec[i].x, m_aCylRotVec[i].y, m_aCylRotVec[i].z, m_aCylRotAngle[i],
                                          m_aCylLength[i],
                                          m_fCylDia * 0.001f,
                                          m_pMapping->getColor(m_pAtomB)->getR(),
                                          m_pMapping->getColor(m_pAtomB)->getG(),
                                          m_pMapping->getColor(m_pAtomB)->getB()
                                         );
            m_rCont << pCyl;
        }
        break;

    case SVT_SPLINE_LINE:
        for(i=0;i<3;i++)
        {
            pLine = new svt_basic_line(
                                       m_aSplinePoints[i].x, m_aSplinePoints[i].y, m_aSplinePoints[i].z,
                                       m_aSplinePoints[i+1].x, m_aSplinePoints[i+1].y, m_aSplinePoints[i+1].z,
                                       m_pMapping->getColor(m_pAtomA)->getR(),
                                       m_pMapping->getColor(m_pAtomA)->getG(),
                                       m_pMapping->getColor(m_pAtomA)->getB()
                                      );
            m_rCont << pLine;
        }
        for(i=3;i<6;i++)
        {
            pLine = new svt_basic_line(
                                       m_aSplinePoints[i].x, m_aSplinePoints[i].y, m_aSplinePoints[i].z,
                                       m_aSplinePoints[i+1].x, m_aSplinePoints[i+1].y, m_aSplinePoints[i+1].z,
                                       m_pMapping->getColor(m_pAtomB)->getR(),
                                       m_pMapping->getColor(m_pAtomB)->getG(),
                                       m_pMapping->getColor(m_pAtomB)->getB()
                                      );
            m_rCont << pLine;
        }
        break;

    default:
        break;
    }
}

/**
 * set the cylinder diameter
 * \param fCylDia diameter of the cylinders
 */
void svt_spline_section::setCylinderDia(float fCylDia)
{
    m_fCylDia = fCylDia;
}
/**
 * get the cylinder diameter
 * \return diameter of the cylinders
 */
float svt_spline_section::getCylinderDia()
{
    return m_fCylDia;
}

/**
 * set the colormapping
 * \param pMapping pointer to a svt_atom_colmap object
 */
void svt_spline_section::setAtomColmap(svt_atom_colmap* pMapping)
{
    m_pMapping = pMapping;
}

/**
 * calculate the data for the cylinders mode
 */
void svt_spline_section::calcCylinders()
{
    int i;
    Real32 fX, fY, fZ;
    for(i=0;i<6;i++)
    {
        fX = m_aSplinePoints[i].x - m_aSplinePoints[i+1].x;
        fY = m_aSplinePoints[i].y - m_aSplinePoints[i+1].y;
        fZ = m_aSplinePoints[i].z - m_aSplinePoints[i+1].z;

        m_aCylLength[i] = sqrt(fX*fX + fY*fY + fZ*fZ);

        if (m_aCylLength[i] != 0)
        {
            m_aCylRotVec[i].x =-fY/m_aCylLength[i];
            m_aCylRotVec[i].y = fX/m_aCylLength[i];
            m_aCylRotVec[i].z = 0.0f;

            m_aCylRotAngle[i] = acos(fZ/m_aCylLength[i])*180.0/3.1415;
        }

        m_aCylLength[i] += 0.0007f + ((m_fCylDia-2.0f)*0.3f);
    }
}
