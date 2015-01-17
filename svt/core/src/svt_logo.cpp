/***************************************************************************
                          svt_logo.cpp  -  description
                             -------------------
    begin                : Mon May 28 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt_includes
#include <svt_core.h>
#include <svt_logo.h>
#include <svt_init.h>
#include <svt_time.h>
#include <svt_color.h>
#include <svt_texture.h>
#include <svt_opengl.h>
// clib includes
#include <math.h>
#include <svt_iostream.h>

#define ZEIT 40

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_logo::svt_logo(svt_pos* pPos, svt_properties* pProp) : svt_node(pPos, pProp)
{
    if (pPos == NULL)
        setPos(new svt_pos(0.22f,-0.17f,-0.5f));

    m_iDrawAngle = 0;
    initTextures();
    m_pTempCol = new svt_color(0.0f,0.15f,0.2f);
    m_pTemp2Col = new svt_color(1.0f,1.0f,1.0f);
    m_iStartZeit = svt_getElapsedTime();
};
svt_logo::~svt_logo()
{
    delete m_pTempCol;
    delete m_pTemp2Col;
};

/**
 * draw the logo
 */
void svt_logo::drawGL()
{
    // Drehung alle ZEIT Millisekunden
    m_iAktZeit = svt_getElapsedTime();
    while (m_iAktZeit - m_iStartZeit > ZEIT)
    {
        m_iDrawAngle = (m_iDrawAngle+2) % 360;
        m_iStartZeit += ZEIT;
    }
    //m_iStartZeit = glutGet((ENUM_GLENUM)GLUT_ELAPSED_TIME);

    // Positionierung unabh„ngig von vorangegangen Transformationen
    glPushMatrix();
    glLoadIdentity();

    getPos()->applyToGL();

    // Farbe fuer die Seiten. Diese Farbe wird mit der Texturfarbe gemischt
    m_pTemp2Col->applyGL();

    // Drehung
    glRotatef(-15, 0.0, 1.0, 0.0);
    glRotatef(m_iDrawAngle, 1.0, 0.0, 0.0);
    glScalef(0.02f, 0.02f, 0.02f);

    // Textur Code des Logos
    glEnable(GL_TEXTURE_2D);
    m_pLogo1->BindTexture();
    // Vorderseite
    glBegin(GL_POLYGON);
    glNormal3f( 0.0f,	 1.0f,	 0.0f);
    glTexCoord2f(0.00f, 1.0f-0.50f);	glVertex3f(-1.0f,	0.2f,	 0.0f);
    glTexCoord2f(0.05f, 1.0f-0.70f);	glVertex3f(-0.9f,	0.2f,	 0.4f);
    glTexCoord2f(0.15f, 1.0f-0.85f);	glVertex3f(-0.7f,	0.2f,	 0.7f);
    glTexCoord2f(0.30f, 1.0f-0.95f);	glVertex3f(-0.4f,	0.2f,	 0.9f);
    glTexCoord2f(0.50f, 1.0f-1.00f);	glVertex3f( 0.0f,	0.2f,	 1.0f);
    glTexCoord2f(0.70f, 1.0f-0.95f);	glVertex3f( 0.4f,	0.2f,	 0.9f);
    glTexCoord2f(0.85f, 1.0f-0.85f);	glVertex3f( 0.7f,	0.2f,	 0.7f);
    glTexCoord2f(0.95f, 1.0f-0.70f);	glVertex3f( 0.9f,	0.2f,	 0.4f);
    glTexCoord2f(1.00f, 1.0f-0.50f);	glVertex3f( 1.0f,	0.2f,	 0.0f);
    glTexCoord2f(0.95f, 1.0f-0.30f);	glVertex3f( 0.9f,	0.2f,	-0.4f);
    glTexCoord2f(0.85f, 1.0f-0.15f);	glVertex3f( 0.7f,	0.2f,	-0.7f);
    glTexCoord2f(0.70f, 1.0f-0.05f);	glVertex3f( 0.4f,	0.2f,	-0.9f);
    glTexCoord2f(0.50f, 1.0f-0.00f);	glVertex3f( 0.0f,	0.2f,	-1.0f);
    glTexCoord2f(0.30f, 1.0f-0.05f);	glVertex3f(-0.4f,	0.2f,	-0.9f);
    glTexCoord2f(0.15f, 1.0f-0.15f);	glVertex3f(-0.7f,	0.2f,	-0.7f);
    glTexCoord2f(0.05f, 1.0f-0.30f);	glVertex3f(-0.9f,	0.2f,	-0.4f);
    glEnd();
    m_pLogo2->BindTexture();
    //Rueckseite
    glBegin(GL_POLYGON);
    glNormal3f( 0.0f,	 -1.0f,	 0.0f);
    glTexCoord2f(0.00f, 0.50f);	glVertex3f(-1.0f,	-0.2f,	 0.0f);
    glTexCoord2f(0.05f, 0.70f);	glVertex3f(-0.9f,	-0.2f,	 0.4f);
    glTexCoord2f(0.15f, 0.85f);	glVertex3f(-0.7f,	-0.2f,	 0.7f);
    glTexCoord2f(0.30f, 0.95f);	glVertex3f(-0.4f,	-0.2f,	 0.9f);
    glTexCoord2f(0.50f, 1.00f);	glVertex3f( 0.0f,	-0.2f,	 1.0f);
    glTexCoord2f(0.70f, 0.95f);	glVertex3f( 0.4f,	-0.2f,	 0.9f);
    glTexCoord2f(0.85f, 0.85f);	glVertex3f( 0.7f,	-0.2f,	 0.7f);
    glTexCoord2f(0.95f, 0.70f);	glVertex3f( 0.9f,	-0.2f,	 0.4f);
    glTexCoord2f(1.00f, 0.50f);	glVertex3f( 1.0f,	-0.2f,	 0.0f);
    glTexCoord2f(0.95f, 0.30f);	glVertex3f( 0.9f,	-0.2f,	-0.4f);
    glTexCoord2f(0.85f, 0.15f);	glVertex3f( 0.7f,	-0.2f,	-0.7f);
    glTexCoord2f(0.70f, 0.05f);	glVertex3f( 0.4f,	-0.2f,	-0.9f);
    glTexCoord2f(0.50f, 0.00f);	glVertex3f( 0.0f,	-0.2f,	-1.0f);
    glTexCoord2f(0.30f, 0.05f);	glVertex3f(-0.4f,	-0.2f,	-0.9f);
    glTexCoord2f(0.15f, 0.15f);	glVertex3f(-0.7f,	-0.2f,	-0.7f);
    glTexCoord2f(0.05f, 0.30f);	glVertex3f(-0.9f,	-0.2f,	-0.4f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Rand
    // Farbe fuer den Rand
    m_pTempCol->applyGL();
    glBegin(GL_QUAD_STRIP);
    glNormal3f(-1.0f,	 0.0f,	 0.0f);
    glVertex3f(-1.0f,	 0.2f,	 0.0f);
    glVertex3f(-1.0f,	-0.2f,	 0.0f);
    glNormal3f(-1.0f,	 0.0f,	 0.5f);
    glVertex3f(-0.9f,	 0.2f,	 0.4f);
    glVertex3f(-0.9f,	-0.2f,	 0.4f);
    glNormal3f(-1.0f,	 0.0f,	 1.0f);
    glVertex3f(-0.7f,	 0.2f,	 0.7f);
    glVertex3f(-0.7f,	-0.2f,	 0.7f);
    glNormal3f(-0.5f,	 0.0f,	 1.0f);
    glVertex3f(-0.4f,	 0.2f,	 0.9f);
    glVertex3f(-0.4f,	-0.2f,	 0.9f);
    glNormal3f( 0.0f,	 0.0f,	 1.0f);
    glVertex3f( 0.0f,	 0.2f,	 1.0f);
    glVertex3f( 0.0f,	-0.2f,	 1.0f);
    glNormal3f( 0.5f,	 0.0f,	 1.0f);
    glVertex3f( 0.4f,	 0.2f,	 0.9f);
    glVertex3f( 0.4f,	-0.2f,	 0.9f);
    glNormal3f( 1.0f,	 0.0f,	 1.0f);
    glVertex3f( 0.7f,	 0.2f,	 0.7f);
    glVertex3f( 0.7f,	-0.2f,	 0.7f);
    glNormal3f( 1.0f,	 0.0f,	 0.5f);
    glVertex3f( 0.9f,	 0.2f,	 0.4f);
    glVertex3f( 0.9f,	-0.2f,	 0.4f);
    glNormal3f( 1.0f,	 0.0f,	 0.0f);
    glVertex3f( 1.0f,	 0.2f,	 0.0f);
    glVertex3f( 1.0f,	-0.2f,	 0.0f);
    glNormal3f( 1.0f,	 0.0f,	-0.5f);
    glVertex3f( 0.9f,	 0.2f,	-0.4f);
    glVertex3f( 0.9f,	-0.2f,	-0.4f);
    glNormal3f( 1.0f,	 0.0f,	-1.0f);
    glVertex3f( 0.7f,	 0.2f,	-0.7f);
    glVertex3f( 0.7f,	-0.2f,	-0.7f);
    glNormal3f( 0.5f,	 0.0f,	-1.0f);
    glVertex3f( 0.4f,	 0.2f,	-0.9f);
    glVertex3f( 0.4f,	-0.2f,	-0.9f);
    glNormal3f( 0.0f,	 0.0f,	-1.0f);
    glVertex3f( 0.0f,	 0.2f,	-1.0f);
    glVertex3f( 0.0f,	-0.2f,	-1.0f);
    glNormal3f(-0.5f,	 0.0f,	-1.0f);
    glVertex3f(-0.4f,	 0.2f,	-0.9f);
    glVertex3f(-0.4f,	-0.2f,	-0.9f);
    glNormal3f(-1.0f,	 0.0f,	-1.0f);
    glVertex3f(-0.7f,	 0.2f,	-0.7f);
    glVertex3f(-0.7f,	-0.2f,	-0.7f);
    glNormal3f(-1.0f,	 0.0f,	-0.5f);
    glVertex3f(-0.9f,	 0.2f,	-0.4f);
    glVertex3f(-0.9f,	-0.2f,	-0.4f);
    glNormal3f(-1.0f,	 0.0f,	 0.0f);
    glVertex3f(-1.0f,	 0.2f,	 0.0f);
    glVertex3f(-1.0f,	-0.2f,	 0.0f);
    glEnd();
	
    glPopMatrix();
}

/**
 * print "Logo" to stdout
 */
void svt_logo::printName()
{
    cout << "Logo" << endl;
};

/**
 * initialize the textures
 */
void svt_logo::initTextures()
{
  m_pLogo1 = new svt_texture(svt_getConfig()->getValue("LogoFront","../../data/bmp/logo.bmp"));
  m_pLogo2 = new svt_texture(svt_getConfig()->getValue("LogoBack","../../data/bmp/logo.bmp"));
}

