/***************************************************************************
                          svt_disc.h
			  ----------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_DISK_H
#define SVT_DISK_H

// svt includes
#include <svt_node.h>
#include <svt_opengl.h>

/**
 * A disc
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_disc : public svt_node  {

protected:
    Real32 m_fRadius;
    int m_iSlices;
    int m_iLoops;
    GLUquadricObj* m_pQuadric;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     * \param fRadius radius of the disc
     * \param iSlices number of slices of the disc
     * \param iLoops number of loops of the disc
     */
    svt_disc(svt_pos* pPos =NULL, svt_properties* pProp =NULL, Real32 fRadius =0.1f, int iSlices =16, int iLoops =16);
    virtual ~svt_disc();

    /**
     * set the radius of the disc
     * \param fRadius radius of the disc
     */
    void setRadius(Real32 fRadius);
    /**
     * set number of slices of the disc
     * \param iSlices number of slices
     */
    void setSlices(int iSlices);
    /**
     * get number of slices of the disc
     * \return number of slices of the disc
     */
    int getSlices();
    /**
     * set number of loops of the disc
     * \param iLoops number of loops
     */
    void setLoops(int iLoops);
    /**
     * get number of loops of the disc
     * \return number of loops of the disc
     */
    int getLoops();

    /**
     * draw the disc
     */
    void drawGL();

protected:
    /**
     * prepare the quadric object for glu
     */
    void prepareQuad();
};

#endif
