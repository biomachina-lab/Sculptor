/***************************************************************************
                          svt_disc.cpp  -  description
                             -------------------
    begin                : May 10 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_disc.h>
#include <svt_pos.h>
#include <svt_properties.h>
// clib includes
#include <svt_iostream.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fRadius radius of the disc
 * \param iSlices number of slices of the disc
 * \param iLoops number of loops of the disc
 */
svt_disc::svt_disc(svt_pos* pPos, svt_properties* pProp, Real32 fRadius, int iSlices, int iLoops) : svt_node(pPos, pProp)
{
    prepareQuad();
    setRadius(fRadius);
    setSlices(iSlices);
    setLoops(iLoops);
    setName("svt_disc");
};
svt_disc::~svt_disc()
{
    gluDeleteQuadric(m_pQuadric);
};

/**
 * set the radius of the disc
 * \param fRadius radius of the disc
 */
void svt_disc::setRadius(Real32 fRadius)
{
    m_fRadius = fRadius;
}

/**
 * set number of slices of the disc
 * \param iSlices number of slices
 */
void svt_disc::setSlices(int iSlices)
{
    m_iSlices = iSlices;
};
/**
 * get number of slices of the disc
 * \return number of slices of the disc
 */
int svt_disc::getSlices()
{
    return m_iSlices;
};
/**
 * set number of loops of the disc
 * \param iLoops number of loops
 */
void svt_disc::setLoops(int iLoops)
{
    m_iLoops = iLoops;
};
/**
 * get number of loops of the disc
 * \return number of loops of the disc
 */
int svt_disc::getLoops()
{
    return m_iLoops;
};

/**
 * draw the disc
 */
void svt_disc::drawGL(){

    if (getTexture() != NULL){
        glEnable(GL_TEXTURE_2D);
        gluQuadricTexture(m_pQuadric, GL_TRUE);
        getTexture()->BindTexture();
    }

    gluDisk(m_pQuadric, 0, m_fRadius, m_iSlices, m_iLoops);
    glDisable(GL_TEXTURE_2D);
}

/**
 * prepare the quadrics object
 */
void svt_disc::prepareQuad()
{
    m_pQuadric = gluNewQuadric();
}
