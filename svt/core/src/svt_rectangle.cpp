/***************************************************************************
                          svt_rectangle.cpp  -  description
                             -------------------
    begin                : Jul 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_rectangle.h>
// clib includes
#include <svt_iostream.h>
#include <svt_opengl.h>

/**
 * Constructor
 * You need four points to construct a rectangle
 * \param pA pointer to svt_pos object
 * \param pB pointer to svt_pos object
 * \param pC pointer to svt_pos object
 * \param pD pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_rectangle::svt_rectangle(svt_pos* pPos, svt_properties* pProp, Real32 fWidth, Real32 fHeight) : svt_node( pPos, pProp ),
    m_fWidth( fWidth ),
    m_fHeight( fHeight )
{
}

/**
 * draw the rectangle
 */
void svt_rectangle::drawGL()
{
    if (getTexture() != NULL)
    {
        glEnable(GL_TEXTURE_2D);
        getTexture()->BindTexture();
        glBegin(GL_QUADS);
        
        glNormal3f( 0.0f, 1.0f, 0.0f );
        glTexCoord2f(0.0f, 0.0f); glVertex3f( -m_fWidth*0.5f, -m_fHeight*0.5f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( +m_fWidth*0.5f, -m_fHeight*0.5f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( +m_fWidth*0.5f, +m_fHeight*0.5f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( -m_fWidth*0.5f, +m_fHeight*0.5f, 0.0f);
        
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        glBegin(GL_QUADS);
        
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( -m_fWidth*0.5f, -m_fHeight*0.5f, 0.0f);
        glVertex3f( +m_fWidth*0.5f, -m_fHeight*0.5f, 0.0f);
        glVertex3f( +m_fWidth*0.5f, +m_fHeight*0.5f, 0.0f);
        glVertex3f( -m_fWidth*0.5f, +m_fHeight*0.5f, 0.0f);
        
        glEnd();
    }
};

/**
 * print "Rectangle" to stdout
 */
void svt_rectangle::printName()
{
    cout << "Rectangle" << endl;
};
