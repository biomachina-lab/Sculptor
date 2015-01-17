/***************************************************************************
                          svt_cone.cpp  -  description
                             -------------------
    begin                : May 10 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_cone.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fHeight height of the cone
 * \param fRadius radius of the cone
 */
svt_cone::svt_cone(svt_pos* pPos, svt_properties* pProp, Real32 fHeight, Real32 fRadius) : svt_node(pPos, pProp),
    m_pQuadric( NULL )
{
    prepareQuad();
    setRadius(fRadius);
    setHeight(fHeight);
    setName("svt_cone");
    m_fRotAngle=0.0f;
};
/**
 * Constructor
 * Create a cone between two positions
 * \param pPosStart pointer to svt_pos object
 * \param pPosEnd pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_cone::svt_cone(svt_pos* pPosStart, svt_pos* pPosEnd, svt_properties* pProp, Real32 fRadius) : svt_node(pPosStart, pProp)
{
    Real32 fPx, fPy, fPz;

    prepareQuad();

    setRadius(fRadius);
    fPx = pPosEnd->getX() - pPosStart->getX();
    fPy = pPosEnd->getY() - pPosStart->getY();
    fPz = pPosEnd->getZ() - pPosStart->getZ();
    Real32 fLength = (Real32)sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
    setHeight(fLength);
    pPosEnd->setX(-fPy/fLength);
    pPosEnd->setY(fPx/fLength);
    pPosEnd->setZ(0.0f);
    m_pRotationVector = pPosEnd;
    m_fRotAngle = (Real32)acos(fPz/fLength)*180.0f/3.1415f;

}
svt_cone::~svt_cone()
{
    if (m_pQuadric)
        gluDeleteQuadric( m_pQuadric );
};

/**
 * set the height of the cone
 * \param fHeight the height of the cone
 */
void svt_cone::setHeight(Real32 fHeight)
{
    m_fHeight = fHeight;
}

/**
 * set the radius of the cone
 * \param fRadius of the cone
 */
void svt_cone::setRadius(Real32 fRadius)
{
    m_fRadius = fRadius;
}

/**
 * draw the cone
 */
void svt_cone::drawGL()
{
    if (m_fRotAngle != 0.0f)
    {
        glRotatef((GLfloat) m_fRotAngle, (GLfloat) m_pRotationVector->getX(), (GLfloat) m_pRotationVector->getY(), (GLfloat) m_pRotationVector->getZ());
    }

    if (getTexture() != NULL){
        gluQuadricTexture(m_pQuadric, GL_TRUE);
        getTexture()->BindTexture();
    }
    gluCylinder(m_pQuadric, m_fRadius, 0.0f, m_fHeight, 8, 4);
}

/**
 * prepare the quadrics object
 */
void svt_cone::prepareQuad()
{
    m_pQuadric = gluNewQuadric();
}
