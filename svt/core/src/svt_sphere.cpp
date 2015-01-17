/***************************************************************************
                          svt_sphere.cpp  -  description
                             -------------------
    begin                : Mon Feb 28 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_sphere.h>
#include <svt_iostream.h>
#include <svt_container.h>
 
/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_pos object
 * \param fRadius radius of the sphere
 * \param iSlices number of slices of the sphere
 * \param iStacks number of stacks of the sphere
 */
svt_sphere::svt_sphere(svt_pos* pPos, svt_properties* pProp, Real32 fRadius, int iSlices, int iStacks) : svt_node(pPos, pProp),
    m_pQuadric(NULL)
{
    setRadius(fRadius);
    setStacks(iStacks);
    setSlices(iSlices);
    prepareQuad();
    setName( "svt_sphere" );
};
svt_sphere::~svt_sphere()
{
    if (m_pQuadric)
        gluDeleteQuadric( m_pQuadric );
};

/**
 * set radius of the sphere
 */
void svt_sphere::setRadius(Real32 fRadius)
{
    m_fRadius = fRadius;
    rebuildDL();
};
/**
 * get radius of the sphere
 */
Real32 svt_sphere::getRadius() const
{
    return m_fRadius;
}

/**
 * set the stacks
 */
void svt_sphere::setStacks(int iStacks)
{
    m_iStacks = iStacks;
    rebuildDL();
};
/**
 * get the stacks
 */
int svt_sphere::getStacks()
{
    return m_iStacks;
};
/**
 * set the slices
 */
void svt_sphere::setSlices(int iSlices)
{
    m_iSlices = iSlices;
    rebuildDL();
};
/**
 * get the slices
 */
int svt_sphere::getSlices()
{
    return m_iSlices;
};

/**
 * draw the sphere
 */
void svt_sphere::drawGL()
{
    if (getTexture() != NULL){
        glEnable(GL_TEXTURE_2D);
        gluQuadricTexture(m_pQuadric, GL_TRUE);
        getTexture()->BindTexture();
    }

    gluSphere(m_pQuadric,m_fRadius, m_iStacks, m_iSlices);
    glDisable(GL_TEXTURE_2D);
}

/**
 * draw the sphere
 */
void svt_sphere::drawOBJ()
{
    svt_container oCont;

    if (getProperties() != NULL && getProperties()->getColor() != NULL)
        oCont.addSphere( getPosX() * 1.0E-2, getPosY() * 1.0E-2, getPosZ() * 1.0E-2, m_fRadius, *getProperties()->getColor() );
    else
        oCont.addSphere( getPosX() * 1.0E-2, getPosY() * 1.0E-2, getPosZ() * 1.0E-2, m_fRadius, svt_color(1.0, 0.0, 0.0) );

    oCont.drawOBJ();
}

/**
 * print the vrml code
 */
void svt_sphere::drawVrml()
{
    cout << "geometry Sphere { radius " << m_fRadius << " }" << endl;
}

/**
 * prepare the quadrics object
 */
void svt_sphere::prepareQuad()
{
    m_pQuadric = gluNewQuadric();
}
