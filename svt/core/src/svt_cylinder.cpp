/***************************************************************************
                          svt_cylinder.cpp  -  description
                             -------------------
    begin                : May 10 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_cylinder.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fHeight height of the cylinder
 * \param fRadius radius of the cylinder
 */
svt_cylinder::svt_cylinder(svt_pos* pPos, svt_properties* pProp, Real32 fHeight, Real32 fRadius) : svt_node(pPos, pProp)
{
    prepareQuad();
    setRadius(fRadius);
    setHeight(fHeight);
    setName("svt_cylinder");
    m_fRotAngle=0.0f;
};

/**
 * Constructor
 * Create a cylinder between two positions
 * \param pPosStart pointer to svt_pos object
 * \param pPosEnd pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fRadius radius of the cylinder
 */
svt_cylinder::svt_cylinder(svt_pos* pPosStart, svt_pos* pPosEnd, svt_properties* pProp, Real32 fRadius) : svt_node(pPosStart, pProp)
{
    Real32 fPx, fPy, fPz;

    prepareQuad();

    setRadius(fRadius);
    fPx= pPosEnd->getX() - pPosStart->getX();
    fPy = pPosEnd->getY() - pPosStart->getY();
    fPz = pPosEnd->getZ() - pPosStart->getZ();
    Real32 fLength = sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
    setHeight(fLength);
    pPosEnd->setX(-fPy/fLength);
    pPosEnd->setY(fPx/fLength);
    pPosEnd->setZ(0.0);
    m_pRotationVector = pPosEnd;
    m_fRotAngle = acos(fPz/fLength)*180.0/3.1415;

}
svt_cylinder::~svt_cylinder()
{
    gluDeleteQuadric(m_pQuadric);
};

/**
 * set the height of the cylinder
 * \param fHeight height of the cylinder
 */
void svt_cylinder::setHeight(Real32 fHeight)
{
    m_fHeight = fHeight;
}

/**
 * set the radius of the cylinder
 * \param fRadius radius of the cylinder
 */
void svt_cylinder::setRadius(Real32 fRadius)
{
    m_fRadius = fRadius;
}

/**
 * prepare the quadrics object
 */
void svt_cylinder::prepareQuad()
{
    m_pQuadric = gluNewQuadric();
}

/**
 * draw the cylinder
 */
void svt_cylinder::drawGL()
{
    if (m_fRotAngle != 0.0) {
        glRotatef((GLfloat) m_fRotAngle, (GLfloat) m_pRotationVector->getX(), (GLfloat) m_pRotationVector->getY(), (GLfloat) m_pRotationVector->getZ());
    }

    if (getTexture() != NULL){
        glEnable(GL_TEXTURE_2D);
        gluQuadricTexture(m_pQuadric, GL_TRUE);
        getTexture()->BindTexture();
        gluCylinder(m_pQuadric, m_fRadius, m_fRadius, m_fHeight, 8, 4);
        glDisable(GL_TEXTURE_2D);
    } else
        gluCylinder(m_pQuadric, m_fRadius, m_fRadius, m_fHeight, 8, 4);
}
