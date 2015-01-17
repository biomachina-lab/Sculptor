/***************************************************************************
                          svt_box.cpp
			  -----------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_box.h>
#include <svt_opengl.h>

/**
 * Constructor
 * \param pPos pointer to a svt_pos object for the position of the new object
 * \param pSize pointer to a svt_pos object for the second coordinate for the svt_box object (the lower right vertice of the box)
 * \param pProp pointer to a svt_properties object for the properties
 */
svt_box::svt_box(svt_pos* pPos, svt_pos* pSize, svt_properties* pProp) : svt_node(pPos, pProp),
    m_pSize( pSize )
{
};
svt_box::~svt_box()
{
    if (m_pSize)
        delete m_pSize;
};

/**
 * set the second coordinate of the box
 * \param pSize pointer to a svt_pos object with the second coord of the box (lower right vertice of the box)
 */
void svt_box::setSize(svt_pos* pSize)
{
    m_pSize = pSize;
}
/**
 * get the second coordinate of the box
 * \return pointer to a svt_pos object with the second coord
 */
svt_pos* svt_box::getSize()
{
    return m_pSize;
};

/**
 * draw the box
 */
void svt_box::drawGL()
{
    if (getTexture() == NULL)
    {
	glBegin(GL_QUADS);

        //Top
        glNormal3f( 0.0f,                                   1.0f,  0.0f);
        glVertex3f( -m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);

        //Bottom
        glNormal3f(                  0.0f,                 -1.0f,  0.0f);
	glVertex3f( -m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);

        //Front
        glNormal3f(                  0.0f,                  0.0f, +1.0f);
	glVertex3f( +m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);

        //Back
        glNormal3f(                  0.0f,                  0.0f, -1.0f);
	glVertex3f( -m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
	glVertex3f( +m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);

        //Left
        glNormal3f(                 -1.0f,                  0.0f,  0.0f);
        glVertex3f( -m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( -m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);

        //Right
        glNormal3f(                  1.0f,                  0.0f,  0.0f);
	glVertex3f( +m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, -m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
        glVertex3f( +m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, +m_pSize->getZ()/2.0f);
	glVertex3f( +m_pSize->getX()/2.0f, +m_pSize->getY()/2.0f, -m_pSize->getZ()/2.0f);

        glEnd();

    } else {
        glEnable(GL_TEXTURE_2D);
        getTexture()->BindTexture();
        glBegin(GL_QUADS);
        //Top
        glTexCoord2f(0.0f, 0.0f);
        glNormal3f( 0.0f,         1.0f,         0.0f);
        glVertex3f( 0.0f,         0.0f,         0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f( 0.0f,         0.0f,         m_pSize->getZ());
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( m_pSize->getX(), 0.0f,         m_pSize->getZ());
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( m_pSize->getX(), 0.0f,         0.0f);

        //Bottom
        glTexCoord2f(0.0f, 0.0f);
        glNormal3f( 0.0f,         -1.0f,        0.0f);
        glVertex3f( 0.0f,         m_pSize->getY(), 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f( 0.0f,         m_pSize->getY(), m_pSize->getZ());
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( m_pSize->getX(), m_pSize->getY(), m_pSize->getZ());
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( m_pSize->getX(), m_pSize->getY(), 0.0f);

        //Front
        glTexCoord2f(0.0f, 0.0f);
        glNormal3f( 0.0f,         0.0f,         -1.0f);
        glVertex3f( 0.0f,         0.0f,         0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glNormal3f( 0.0f,         0.0f,         -1.0f);
        glVertex3f( 0.0f,         m_pSize->getY(), 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glNormal3f( 0.0f,         0.0f,         -1.0f);
        glVertex3f( m_pSize->getX(), m_pSize->getY(), 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glNormal3f( 0.0f,         0.0f,         -1.0f);
        glVertex3f( m_pSize->getX(), 0.0f,         0.0f);

        //Back
        glTexCoord2f(0.0f, 0.0f);
        glNormal3f( 0.0f,         0.0f,         1.0f);
        glVertex3f( 0.0f,         0.0f,         m_pSize->getZ());
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f( 0.0f,         m_pSize->getY(), m_pSize->getZ());
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( m_pSize->getX(), m_pSize->getY(), m_pSize->getZ());
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( m_pSize->getX(), 0.0f,         m_pSize->getZ());

        //Left
        glTexCoord2f(0.0f, 0.0f);
        glNormal3f( -1.0f,         0.0f,         0.0f);
        glVertex3f( 0.0f,         0.0f,         0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f( 0.0f,         m_pSize->getY(), 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( 0.0f,         m_pSize->getY(), m_pSize->getZ());
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( 0.0f,         0.0f,         m_pSize->getZ());

        //Right
        glTexCoord2f(0.0f, 0.0f);
        glNormal3f( 1.0f,        0.0f,         0.0f);
        glVertex3f( m_pSize->getX(), 0.0f,         0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f( m_pSize->getX(), m_pSize->getY(), 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( m_pSize->getX(), m_pSize->getY(), m_pSize->getZ());
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( m_pSize->getX(), 0.0f,         m_pSize->getZ());
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}

/**
 * print the name of the node to stdout
 */
void svt_box::printName()
{
    cout << "Box" << endl;
};
