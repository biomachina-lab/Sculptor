/***************************************************************************
                          svt_basic_point.cpp
                          --------------------
    begin                : 6/14/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basic_point.h>
#include <GL/gl.h>

/**
 * Constructor
 * \param fX x position
 * \param fY y position
 * \param fZ z position
 * \param fRed red color component
 * \param fGreen green color component
 * \param fBlue blue color component
 */
svt_basic_point::svt_basic_point( Real32 fX, Real32 fY, Real32 fZ, Real32 fRed, Real32 fGreen, Real32 fBlue ) :
    svt_basic_object( fRed, fGreen, fBlue)
{
    setPosition( fX, fY, fZ );
}

/**
 * Constructor
 * \param oPoint position
 * \param fRed red color component
 * \param fGreen green color component
 * \param fBlue blue color component
 */
svt_basic_point::svt_basic_point( svt_vec4real32 oPoint, Real32 fRed, Real32 fGreen, Real32 fBlue ) :
    svt_basic_object( fRed, fGreen, fBlue)
{
    setPosition( oPoint );
}

/**
 * draw the point (PovRay)
 */
void svt_basic_point::drawPOV()
{
    cout << "    sphere {" << endl;
    cout << "      < " << m_oPoint.x() << ", " << m_oPoint.y() << ", " << m_oPoint.z() << " >, 0.001 " << endl;

    svt_basic_object::drawPOV();

    cout << "    }" << endl;
}

/**
 * draw the point (OpenGL)
 * \param iWin index of window (opengl context)
 * \param rCont reference to parent container object
 */
void svt_basic_point::drawGL(int iWin, svt_basic_container& rCont)
{
    // apply colors
    svt_basic_object::drawGL(iWin, rCont);

    // draw point
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);

    glBegin(GL_POINTS);
    glVertex3f( m_oPoint.x(), m_oPoint.y(), m_oPoint.z() );
    glEnd();

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
};
