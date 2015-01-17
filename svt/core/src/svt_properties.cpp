/***************************************************************************
                          svt_properties.cpp
			  ------------------
    begin                : Tue Mar 30 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_init.h>
#include <svt_properties.h>
// clib includes
#include <svt_iostream.h>

svt_color svt_properties::sm_oDefColor(0.875, 0.625, 0.5);
svt_color svt_properties::sm_oDefWireframeColor(0,0,0);
svt_color svt_properties::sm_oDefUnderTextureColor(1,1,1);

/**
 * Constructor.
 * \param pColor pointer to a svt_color object
 */
svt_properties::svt_properties(svt_color* pColor) 
  : m_pWireframeColor(&sm_oDefWireframeColor),
    m_pUnderTextureColor(&sm_oDefUnderTextureColor),
    m_bWireframe(false),
    m_bSolid(true),
    m_bLighting(true),
    m_bSmooth(true),
    m_bTexture(true),
    m_bNormals(false),
    m_bCullBackfaces(false),
    m_bTwoSided(false),
    m_bDeleteColors(true)
{
    if (pColor)
	m_pColor = pColor;
    else
	m_pColor = &sm_oDefColor;
}


svt_properties::~svt_properties()
{
    if (m_bDeleteColors && m_pColor && m_pColor != &sm_oDefColor)
        delete m_pColor;
    if (m_bDeleteColors && m_pWireframeColor && m_pWireframeColor != &sm_oDefWireframeColor)
        delete m_pWireframeColor;
    if (m_bDeleteColors && m_pUnderTextureColor && m_pUnderTextureColor != &sm_oDefUnderTextureColor)
        delete m_pWireframeColor;
}

/**
 * set the color of this properties object
 * \param pColor pointer to the svt_color object
 */
void svt_properties::setColor(svt_color* pColor)
{
    m_pColor = pColor ? pColor : &sm_oDefColor;

    svt_redraw();
}

void svt_properties::setWireframeColor(svt_color* pColor)
{
    m_pWireframeColor = pColor ? pColor : &sm_oDefWireframeColor;

    svt_redraw();
}

void svt_properties::setUnderTextureColor(svt_color* pColor)
{
    m_pColor = pColor ? pColor : &sm_oDefColor;

    svt_redraw();
}

/**
 * get the color
 * \return pointer to the svt_color object
 */
svt_color* svt_properties::getColor() const
{
    return (m_pColor==&sm_oDefColor) ? NULL : m_pColor;
}

svt_color* svt_properties::getWireframeColor() const
{
    return (m_pWireframeColor==&sm_oDefWireframeColor) ? NULL : m_pWireframeColor;
}

svt_color* svt_properties::getUnderTextureColor() const
{
    return (m_pUnderTextureColor==&sm_oDefUnderTextureColor) ? NULL : m_pUnderTextureColor;
}

/**
 * apply the properties to the current opengl state
 */
void svt_properties::applyGLsolid(bool bTextureAvailable)
{
  if (m_bSolid)
  {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glShadeModel( getSmooth() ? GL_SMOOTH : GL_FLAT );

      //if (m_bCullBackfaces)
      //    glEnable(GL_CULL_FACE);
      //else
      //    glDisable(GL_CULL_FACE);

      if (getLighting())
	  glEnable(GL_LIGHTING);
      else
	  glDisable(GL_LIGHTING);

      if (getTexture() && bTextureAvailable )
      {
	  glEnable(GL_TEXTURE_2D);
	  m_pUnderTextureColor->applyGL();
      }
      else
      {
	  m_pColor->applyGL();
	  glDisable(GL_TEXTURE_2D);
      }

      if (getWireframe())
      {
          glLineWidth( 1.0 );
	  glEnable(GL_POLYGON_OFFSET_FILL);
	  glPolygonOffset(1.0f, 4.0f);
      }
      else
	  glDisable(GL_POLYGON_OFFSET_FILL);

      if (getTwoSided())
	  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      else
	  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

      //if ( svt_getScene()->getPass() == SVT_PASS_CLIP )
      //{
      //    glDisable( GL_LIGHTING );
      //}
  }
}

void svt_properties::applyGLwireframe()
{
  if (getWireframe())
  {
      glLineWidth( 1.0 );
      glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
      m_pWireframeColor->applyGL();
      glDisable(GL_TEXTURE_2D);

      if (getTwoSided())
	  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      else
	  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

      if (getLighting())
	  glEnable(GL_LIGHTING);
      else
	  glDisable(GL_LIGHTING);
  }
}

void svt_properties::applyGLnormals()
{
  if (getNormals())
  {
      m_pWireframeColor->applyGL();
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
  }
}

/**
 * print the vrml code for the properties object
 */
void svt_properties::applyVrml()
{
    cout << "appearance Appearance { material Material {";

    if (m_pColor)
        m_pColor->applyVrml();
    else
        cout << "diffuseColor 0.60 0.43 0.0";

    cout << "} }" << endl;
}


void svt_properties::setDefColor(Real32 r, Real32 g, Real32 b)
{ 
    sm_oDefColor.setR(r);
    sm_oDefColor.setG(g);
    sm_oDefColor.setB(b);

    svt_redraw();
}

void svt_properties::setDefWireframeColor(Real32 r, Real32 g, Real32 b)
{
    sm_oDefWireframeColor.setR(r);
    sm_oDefWireframeColor.setG(g);
    sm_oDefWireframeColor.setB(b);

    svt_redraw();
}


void svt_properties::setDefUnderTextureColor(Real32 r, Real32 g, Real32 b)
{
    sm_oDefUnderTextureColor.setR(r);
    sm_oDefUnderTextureColor.setG(g);
    sm_oDefUnderTextureColor.setB(b);

    svt_redraw();
}


const svt_color& svt_properties::defColor()
{
    return sm_oDefColor;
}

const svt_color& svt_properties::defWireframeColor()
{
    return sm_oDefWireframeColor;
}

const svt_color& svt_properties::defUnderTextureColor()
{
    return sm_oDefUnderTextureColor;
}

/**
 * set the wireframe mode
 * \param bWireframe if true if the object is visualized in wireframe mode
 */
void svt_properties::setWireframe(bool b)
{
    if (m_bWireframe != b)
    {
	m_bWireframe = b;
	svt_redraw();
    }
};
void svt_properties::setSolid(bool b)
{
    if (m_bSolid != b)
    {
	m_bSolid = b;
	svt_redraw();
    }
};
void svt_properties::setSmooth(bool b)
{
    if (m_bSmooth != b)
    {
	m_bSmooth = b;
	svt_redraw();
    }
};
void svt_properties::setLighting(bool b)
{
    if (m_bLighting != b)
    {
	m_bLighting = b;
	svt_redraw();
    }
};
void svt_properties::setTexture(bool b)
{
    if (m_bTexture != b)
    {
	m_bTexture=b;
	svt_redraw();
    }
};
void svt_properties::setNormals(bool b)
{
    if (m_bNormals != b)
    {
	m_bNormals=b;
	svt_redraw();
    }
};
void svt_properties::setBackfaceCulling(bool b)
{
    if (m_bCullBackfaces != b)
    {
	m_bCullBackfaces=b;
	svt_redraw();
    }
};
void svt_properties::setTwoSided(bool b)
{
    if (m_bTwoSided != b)
    {
	m_bTwoSided = b;
	svt_redraw();
    }
}

/**
 * get the wireframe mode
 * \return if true if the object is visualized in wireframe mode
 */
bool svt_properties::getWireframe() const
{
    return m_bWireframe;
}
bool svt_properties::getSolid() const
{
    return m_bSolid;
}
bool svt_properties::getSmooth() const
{
    return m_bSmooth;
}
bool svt_properties::getLighting() const
{
    return m_bLighting;
};
bool svt_properties::getTexture() const
{
    return m_bTexture;
};
bool svt_properties::getNormals() const
{
    return m_bNormals;
};
bool svt_properties::getBackfaceCulling() const
{
    return m_bCullBackfaces;
};
bool svt_properties::getTwoSided() const
{
    return m_bTwoSided;
};

/**
 * toggle settings
 */
bool svt_properties::toggleWireframe()
{
    m_bWireframe=!m_bWireframe;
    svt_redraw();
    return m_bWireframe;
};
bool svt_properties::toggleSolid()
{
    m_bSolid=!m_bSolid;
    svt_redraw();
    return m_bSolid;
};
bool svt_properties::toggleSmooth()
{
    m_bSmooth=!m_bSmooth;
    svt_redraw();
    return m_bSmooth;
};
bool svt_properties::toggleLighting()
{
    m_bLighting=!m_bLighting;
    svt_redraw();
    return m_bLighting;
};
bool svt_properties::toggleTexture()
{
    m_bTexture=!m_bTexture;
    svt_redraw();
    return m_bTexture;
};
bool svt_properties::toggleNormals()
{
    m_bNormals=!m_bNormals;
    svt_redraw();
    return m_bNormals;
};
bool svt_properties::toggleBackfaceCulling()
{
    m_bCullBackfaces=!m_bCullBackfaces;
    svt_redraw();
    return m_bCullBackfaces;
};

/**
 * Set if the object should delete the color objects by itself or wait for somebody else to do it. It will delete all color objects - color, wireframe color, color under texture objects!
 * \param bDeleteColors if true, the object will delete the color objects in the destructor
 */
void svt_properties::setDeleteColors( bool bDeleteColors )
{
    m_bDeleteColors = bDeleteColors;
};
/**
 * Get if the object should delete the color objects by itself or wait for somebody else to do it. It will delete all color objects - color, wireframe color, color under texture objects!
 * \return if true, the object will delete the color objects in the destructor
 */
bool svt_properties::getDeleteColors( )
{
    return m_bDeleteColors;
};
