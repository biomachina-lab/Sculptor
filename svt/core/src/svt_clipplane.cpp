/***************************************************************************
                           svt_clipplane.cpp
                           -------------------
    begin                : 02/02/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_iostream.h>
#include <svt_init.h>
#include <svt_clipplane.h>
#include <svt_opengl.h>

///////////////////////////////////////////////////////////////////////////////
// global variables
///////////////////////////////////////////////////////////////////////////////

GLdouble svt_clipplane::m_iClipEqn[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
int      svt_clipplane::m_iClipCount = 0;
bool*    svt_clipplane::m_bClipOcc = NULL;
GLint    svt_clipplane::m_iMaxClipPlanes = -1;

///////////////////////////////////////////////////////////////////////////////
// member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * Create a new svt_clipplane object
 */
svt_clipplane::svt_clipplane() : svt_node()
{
    m_iClipPln = -1;
    m_iClipCount++;
    m_bRectVisible = true;

    setAutoDL( false );

	// FIXME would prefer to use m_pProp, but this produces GL errors (see comment in drawGL() )
//     setProperties(new svt_properties(new svt_color(255.0f,0.0f,0.0f)));
//     getProperties()->getColor()->setTransparency( 0.2f );
//     getProperties()->setTwoSided( true );

    if (m_iMaxClipPlanes == -1)
    {
	glGetIntegerv(GL_MAX_CLIP_PLANES, &m_iMaxClipPlanes);

	if (m_iMaxClipPlanes <= 0)
	{
	    //svt_fatalError("svt_clipplane: your opengl implementation doesnt support clipping planes!");
	    svtout << "Your opengl implementation does not support clipping planes!";
	    setVisible(false);
	}
    }

    // allocate a new occupation array
    if (m_bClipOcc == NULL)
    {
	m_bClipOcc = new bool[m_iMaxClipPlanes];
	int i;
	for(i=0;i<m_iMaxClipPlanes;i++)
	    m_bClipOcc[i] = false;
    }
}

svt_clipplane::~svt_clipplane()
{
    // are we the last clipplane object -> then delete the occupation array
    if (m_iClipCount == 1 && m_bClipOcc)
    {
        delete m_bClipOcc;
        m_bClipOcc = NULL;
        return;
    }

    // unoccupy the clipplane
    if (m_iClipPln >= 0 && m_iClipPln < m_iMaxClipPlanes && m_bClipOcc != NULL)
        m_bClipOcc[m_iClipPln] = false;

    m_iClipCount--;
}

/**
 * get the number of clipplanes your opengl implementation supports
 * \return number of available clipplanes
 */
int svt_clipplane::getNumClipPlanes()
{
    return m_iMaxClipPlanes;
}

/**
 * get the number of the currently used clipplanes
 */
int svt_clipplane::getNumUsedClipPlanes()
{
    return m_iClipCount;
}

/**
 * return a pointer to the clipplane array
 */
bool* svt_clipplane::getClipOcc()
{
    return m_bClipOcc;
}

/**
 * draw the clipplane
 */
void svt_clipplane::drawGL()
{
    if ( svt_getScene()->getPass() == SVT_PASS_OPAQUE )
    {
        if (m_iClipPln == -1)
        {
            m_iClipPln = findFree();
            if (m_iClipPln == -1)
            {
                cout << "svt_clipplane: sorry, your opengl implementation supports only up to " << m_iMaxClipPlanes << " clipplanes!" << endl;
                return;
            }
            else
                m_bClipOcc[m_iClipPln] = true;
        }

        glClipPlane((ENUM_GLENUM)(GL_CLIP_PLANE0+m_iClipPln), m_iClipEqn);
        glEnable((ENUM_GLENUM)(GL_CLIP_PLANE0+m_iClipPln));

        return;

    }
    else if ( svt_getScene()->getPass() == SVT_PASS_CLIPPLANE_RECTANGLE && m_bRectVisible )
    {
	GLint i;
        glDisable((ENUM_GLENUM)(GL_CLIP_PLANE0+m_iClipPln));

	// FIXME would prefer to use m_pProp, but this produces GL errors
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// clipping plane rectangle should not receive fog
	glGetIntegerv(GL_FOG, &i);
 	if (i == GL_TRUE)
 	    glDisable(GL_FOG);

        glBegin(GL_QUADS);
            glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
            glNormal3f( 1.0f, 0.0f, 0.0f );
            glVertex3f( 0.0f, -0.5f, -0.5f);
            glVertex3f( 0.0f, +0.5f, -0.5f);
            glVertex3f( 0.0f, +0.5f, +0.5f);
            glVertex3f( 0.0f, -0.5f, +0.5f);
        glEnd();

	if (i == GL_TRUE)
	    glEnable(GL_FOG);

 	glDisable(GL_COLOR_MATERIAL);
 	glDisable(GL_BLEND);

        glEnable((ENUM_GLENUM)(GL_CLIP_PLANE0+m_iClipPln));
    }
}

/**
 * find a free clipplane, not used by another svt_clipplane object
 */
int svt_clipplane::findFree()
{
    int i;
    for(i=0;i<m_iMaxClipPlanes;i++)
	if (!m_bClipOcc[i])
	    return i;

    return -1;
}

/**
 * get the type of the node - e.g. SVT_NODE_RENDERING or SVT_NODE_CLIPPLANE. Default is SVT_NODE_RENDERING.
 */
int svt_clipplane::getType()
{
    return SVT_NODE_CLIPPLANE;
}

/**
 *
 */
bool svt_clipplane::getRectVisible()
{
    return m_bRectVisible;
}

/**
 *
 */
void svt_clipplane::setRectVisible(bool bVisible)
{
    m_bRectVisible = bVisible;
    svt_redraw();
}
