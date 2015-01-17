/***************************************************************************
                          svt_coord_system
			  --------
    begin                : 04/25/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_coord_system.h>
#include <svt_simple_text.h>
#include <svt_init.h>

/**
 * Constructor
 */
svt_coord_system::svt_coord_system()
    : svt_node(),
      m_pQuadric( gluNewQuadric() ),
      m_aModelviewMatrix( new GLdouble[16] ),
      m_aProjectionMatrix( new GLdouble[16] ),
      m_aViewport( new GLint[4] )
{
    setAutoDL(false);
    setIgnoreClipPlanes(true);
}

/**
 * Destructor
 */
svt_coord_system::~svt_coord_system()
{
    gluDeleteQuadric(m_pQuadric);
}

/**
 * draw the coordinate system
 */
void svt_coord_system::drawGL()
{
    if (svt_getScene()->getSelection())
        return;

    m_fLength = 18.0f / svt_getPhysicalDisplayHeight(svt_getCurrentCanvas());

    //
    // Manipulate MV Matrix so that the coordinate system is always drawn at the same position,
    // bottom left corner
    //

    m_oMatrix.loadModelviewMatrix();

    Real32 fScaleX = sqrt(m_oMatrix[0][0]*m_oMatrix[0][0] + m_oMatrix[1][0]*m_oMatrix[1][0] + m_oMatrix[2][0]*m_oMatrix[2][0]);
    Real32 fScaleY = sqrt(m_oMatrix[0][1]*m_oMatrix[0][1] + m_oMatrix[1][1]*m_oMatrix[1][1] + m_oMatrix[2][1]*m_oMatrix[2][1]);
    Real32 fScaleZ = sqrt(m_oMatrix[0][2]*m_oMatrix[0][2] + m_oMatrix[1][2]*m_oMatrix[1][2] + m_oMatrix[2][2]*m_oMatrix[2][2]);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    m_oMatrix.setTranslation(0.0f, 0.0f, 0.0f);
    m_oMatrix.scale(1.0f / fScaleX, 1.0f / fScaleY, 1.0f / fScaleZ);
    m_oMatrix.applyToGL();

    glGetDoublev(GL_MODELVIEW_MATRIX,  m_aModelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, m_aProjectionMatrix);
    glGetIntegerv(GL_VIEWPORT, m_aViewport);

    //check if fog is enabled. coord sys should not be foggy
    GLboolean b;
    glGetBooleanv(GL_FOG, &b);
    if (b)
	glDisable(GL_FOG);

    if (gluUnProject((GLdouble)80,
                     (GLdouble)64,
                     0.83f,                  // z == 0 returns object coords at near clipping plane, z == 1 at far clipping plane
                     m_aModelviewMatrix,
                     m_aProjectionMatrix,
                     m_aViewport,
                     &m_dX,
                     &m_dY,
                     &m_dZ)
	== GLU_FALSE)
    {
	setVisible(false);
    }


    //
    // Now draw the Coordinate System
    //
    m_oMatrix.loadIdentity();
    m_oMatrix.setTranslation(m_dX, m_dY, m_dZ);
    m_oMatrix.applyToGL();



    // make some OpenGL settings for the coordinate system
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glDepthRange(0.0f, 0.04f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

 

    // legend
    svt_drawText( m_fLength + 0.03f, 0.0f,              0.0f,              1.0f, 0.0f, 0.0f, "x");
    svt_drawText( 0.0f,              m_fLength + 0.03f, 0.0f,              0.0f, 1.0f, 0.0f, "y");
    svt_drawText( 0.0f,              0.0f,              m_fLength + 0.03f, 0.0f, 0.0f, 1.0f, "z");



    // z-axis:
    glColor3f(0.25f, 0.25f, 1.0f);
    gluCylinder(m_pQuadric, 0.0625f*m_fLength, 0.0625f*m_fLength, m_fLength, 24, 1);

    // cone on z-axis:
    m_oMatrix.setTranslation(0.0f, 0.0f, m_fLength);
    m_oMatrix.applyToGL();
    gluCylinder(m_pQuadric, 0.18f*m_fLength, 0.0f, 0.25f*m_fLength, 24, 1);



    // x-axis:
    glColor3f(1.0f, 0.2f, 0.2f);
    m_oMatrix.setTranslation(0.0f, 0.0f, -m_fLength);
    m_oMatrix.applyToGL();
    m_oMatrix.loadIdentity();
    m_oMatrix.rotate(0.0f, 1.0f, 0.0f, PI/2);
    m_oMatrix.applyToGL();
    gluCylinder(m_pQuadric, 0.0625f*m_fLength, 0.0625f*m_fLength, m_fLength, 24, 1);

    // cone on x-axis:
    m_oMatrix.loadIdentity();
    m_oMatrix.setTranslation(0.0f, 0.0f, m_fLength);
    m_oMatrix.applyToGL();
    gluCylinder(m_pQuadric, 0.18f*m_fLength, 0.0f, 0.25f*m_fLength, 24, 1);



    // y-axis:
    glColor3f(0.2f, 1.0f, 0.2f);
    m_oMatrix.setTranslation(0.0f, 0.0f, -m_fLength);
    m_oMatrix.applyToGL();
    m_oMatrix.loadIdentity();
    m_oMatrix.rotate(1.0f, 0.0f, 0.0f, -PI/2);
    m_oMatrix.applyToGL();
    gluCylinder(m_pQuadric, 0.0625f*m_fLength, 0.0625f*m_fLength, m_fLength, 24, 1);

    // cone on y-axis:
    m_oMatrix.loadIdentity();
    m_oMatrix.setTranslation(0.0f, 0.0f, m_fLength);
    m_oMatrix.applyToGL();
    gluCylinder(m_pQuadric, 0.18f*m_fLength, 0.0f, 0.25f*m_fLength, 24, 1);



    // restore OpenGL settings
    glDepthRange(0.0f, 1.0f);
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
}
