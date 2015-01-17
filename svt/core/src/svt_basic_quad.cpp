/***************************************************************************
                          svt_basic_quad.cpp
                          ----------------------
    begin                : 6/15/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basic_quad.h>
#include <svt_window.h>
#include <svt_iostream.h>

/**
 * Constructor
 * A quad between start and end-position with a certain color.
 * \param fX1 x start-position
 * \param fY1 y start-position
 * \param fZ1 z start-position
 * \param fX2 x end-position
 * \param fY2 y end-position
 * \param fZ2 z end-position
 * \param fRed red color component
 * \param fGreen green color component
 * \param fBlue blue color component
 */
svt_basic_quad::svt_basic_quad( Real32 fX1, Real32 fY1, Real32 fZ1, Real32 fX2, Real32 fY2, Real32 fZ2, Real32 fRed, Real32 fGreen, Real32 fBlue ) :
    svt_basic_object( fRed, fGreen, fBlue)
{
    setStartPosition(fX1,fY1,fZ1);
    setEndPosition(fX2,fY2,fZ2);
}

/**
 * draw the quad (PovRay)
 */
void svt_basic_quad::drawPOV()
{
    cout << "    box {" << endl;
    cout << "      < " << m_fX1 << ", " << m_fY1 << ", " << m_fZ1 << " >, " << endl;
    cout << "      < " << m_fX2 << ", " << m_fY2 << ", " << m_fZ2 << " >" << endl;

    // apply colors
    svt_basic_object::drawPOV();

    cout << "    }" << endl;
}

/**
 * draw the quad
 * \param iWin index of window (opengl context)
 * \param rCont reference to parent container object
 */
void svt_basic_quad::drawGL(int iWin, svt_basic_container& rCont)
{
    // apply colors
    svt_basic_object::drawGL(iWin, rCont);

    // draw quad
    glBegin(GL_QUADS);
    // front
    glVertex3f( m_fX1, m_fY1, m_fZ1 );
    glVertex3f( m_fX2, m_fY1, m_fZ1 );
    glVertex3f( m_fX2, m_fY2, m_fZ1 );
    glVertex3f( m_fX1, m_fY2, m_fZ1 );
    // back
    glVertex3f( m_fX1, m_fY1, m_fZ2 );
    glVertex3f( m_fX2, m_fY1, m_fZ2 );
    glVertex3f( m_fX2, m_fY2, m_fZ2 );
    glVertex3f( m_fX1, m_fY2, m_fZ2 );
    // top
    glVertex3f( m_fX1, m_fY1, m_fZ1 );
    glVertex3f( m_fX1, m_fY1, m_fZ2 );
    glVertex3f( m_fX2, m_fY1, m_fZ2 );
    glVertex3f( m_fX2, m_fY1, m_fZ1 );
    // bottom
    glVertex3f( m_fX1, m_fY2, m_fZ1 );
    glVertex3f( m_fX1, m_fY2, m_fZ2 );
    glVertex3f( m_fX2, m_fY2, m_fZ2 );
    glVertex3f( m_fX2, m_fY2, m_fZ1 );
    // right
    glVertex3f( m_fX2, m_fY1, m_fZ1 );
    glVertex3f( m_fX2, m_fY1, m_fZ2 );
    glVertex3f( m_fX2, m_fY2, m_fZ2 );
    glVertex3f( m_fX2, m_fY2, m_fZ1 );
    // left
    glVertex3f( m_fX1, m_fY1, m_fZ1 );
    glVertex3f( m_fX1, m_fY1, m_fZ2 );
    glVertex3f( m_fX1, m_fY2, m_fZ2 );
    glVertex3f( m_fX1, m_fY2, m_fZ1 );
    glEnd();
}
