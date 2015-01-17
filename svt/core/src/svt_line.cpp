/***************************************************************************
                          svt_line.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_line.h>
#include <svt_iostream.h>
#include <svt_opengl.h>

/**
 * Constructor
 * You need four points to construct a line
 * \param pA pointer to svt_pos object
 * \param pB pointer to svt_pod object
 * \param pProp pointer to svt_properties object
 */
svt_line::svt_line(svt_pos* pA, svt_pos* pB, svt_properties* pProp) : svt_node(pA, pProp)
{
    setB(pB);
    setWidth(1.0f);
};
svt_line::~svt_line()
{
};

/**
 * set the second point
 * \param pB pointer to svt_pod object
 */
void svt_line::setB(svt_pos* pB)
{
    m_pB = pB;
};

/**
 * set line width
 * \param fLineWidth line width
 */
void svt_line::setWidth(Real32 fLineWidth)
{
    m_fLineWidth = fLineWidth;
};
/**
 * get line width
 * \return line width
 */
Real32 svt_line::getWidth(void)
{
    return m_fLineWidth;
};

/**
 * draw the line
 */
void svt_line::drawGL()
{
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glLineWidth((GLfloat) m_fLineWidth);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(m_pB->getX(), m_pB->getY(), m_pB->getZ());
    glEnd();
    glLineWidth((GLfloat) m_fLineWidth);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
};

/**
 * print "Line" to stdout
 */
void svt_line::printName()
{
    cout << "Line" << endl;
}
