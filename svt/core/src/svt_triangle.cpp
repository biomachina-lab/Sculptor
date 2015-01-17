/***************************************************************************
                          svt_triangle.cpp  -  description
                             -------------------
    begin                : Jul 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_triangle.h>
#include <svt_opengl.h>
#include <svt_iostream.h>

/**
 * Constructor
 * You need four points to construct a line
 * \param pA pointer to svt_pos object
 * \param pB pointer to svt_pos object
 * \param pC pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_triangle::svt_triangle(svt_pos* pA, svt_pos* pB, svt_pos* pC, svt_properties* pProp) : svt_node(pA, pProp)
{
    svt_pos* pTempB = svt_pos::addPos(pA,pB);
    svt_pos* pTempC = svt_pos::addPos(pA,pC);

    m_pB = pB; m_pC = pC;

    m_pNormA = svt_pos::calcNormalVec(pA, pTempB, pTempC);
    m_pNormB = svt_pos::calcNormalVec(pTempB, pTempC, pA);
    m_pNormC = svt_pos::calcNormalVec(pTempC, pA, pTempB);

    setTextureCoord(0, 0.0f, 0.0f);
    setTextureCoord(1, 0.0f, 1.0f);
    setTextureCoord(2, 1.0f, 1.0f);

    delete pTempB;
    delete pTempC;
};
svt_triangle::~svt_triangle()
{
    if (m_pNormA != NULL)
        delete(m_pNormA);
    if (m_pNormB != NULL)
        delete(m_pNormB);
    if (m_pNormC != NULL)
        delete(m_pNormC);
};

/**
 * set second point of triangle
 */
void svt_triangle::setB(svt_pos* pB)
{
    m_pB = pB;
};
/**
 * get second point of triangle
 */
svt_pos* svt_triangle::getB()
{
    return m_pB;
};

/**
 * set third point of triangle
 */
void svt_triangle::setC(svt_pos* pC)
{
    m_pC = pC;
};
/**
 * get third point of triangle
 */
svt_pos* svt_triangle::getC()
{
    return m_pC;
};

/**
 * set texture coord
 */
void svt_triangle::setTextureCoord(int i, float fX, float fY)
{
    m_aTexX[i] = fX;
    m_aTexY[i] = fY;
};
/**
 * get texture coord
 */
float svt_triangle::getTextureCoordX(int i) const
{
    return m_aTexX[i];
};
/**
 * get texture coord
 */
float svt_triangle::getTextureCoordY(int i) const
{
    return m_aTexY[i];
};

/**
 * draw triangle
 */
void svt_triangle::drawGL()
{
    if (getTexture() != NULL)
    {
        glEnable(GL_TEXTURE_2D);
        getTexture()->BindTexture();
        glBegin(GL_TRIANGLES);
         glNormal3f(m_pNormA->getX(), m_pNormA->getY(), m_pNormA->getZ());
         glTexCoord2f(m_aTexX[0], m_aTexY[0]); glVertex3f(0.0f,0.0f,0.0f);
         glNormal3f(m_pNormB->getX(), m_pNormB->getY(), m_pNormB->getZ());
         glTexCoord2f(m_aTexX[1], m_aTexY[1]); glVertex3f(m_pB->getX(),m_pB->getY(),m_pB->getZ());
         glNormal3f(m_pNormC->getX(), m_pNormC->getY(), m_pNormC->getZ());
         glTexCoord2f(m_aTexX[2], m_aTexY[2]); glVertex3f(m_pC->getX(),m_pC->getY(),m_pC->getZ());
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        glBegin(GL_TRIANGLES);
         glNormal3f(m_pNormA->getX(), m_pNormA->getY(), m_pNormA->getZ());
         glVertex3f(0.0f,0.0f,0.0f);
         glNormal3f(m_pNormB->getX(), m_pNormB->getY(), m_pNormB->getZ());
         glVertex3f(m_pB->getX(),m_pB->getY(),m_pB->getZ());
         glNormal3f(m_pNormC->getX(), m_pNormC->getY(), m_pNormC->getZ());
         glVertex3f(m_pC->getX(),m_pC->getY(),m_pC->getZ());
        glEnd();
    }
};

/**
 * print "Triangle" to stdout
 */
void svt_triangle::printName()
{
    cout << "Triangle" << endl;
};
