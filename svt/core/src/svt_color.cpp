/***************************************************************************
                          svt_color.cpp  -  description
                             -------------------
    begin                : Fri Feb 25 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_color.h>
#include <svt_iostream.h>
#include <svt_opengl.h>

GLubyte g_aStipple00[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

GLubyte g_aStipple25[] =
{
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44,
    0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22
};

GLubyte g_aStipple50[] =
{
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55
};

GLubyte g_aStipple75[] =
{
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB,
    0xEE, 0xEE, 0xEE, 0xEE, 0xDD, 0xDD, 0xDD, 0xDD
};

/**
 * Constructor
 * Create a new color.
 * \param fR red color component
 * \param fG green color component
 * \param fB blue color component
 */
svt_color::svt_color(Real32 fR, Real32 fG, Real32 fB)
{
    m_aMatAmbient[0] = 0.2f;
    m_aMatAmbient[1] = 0.2f;
    m_aMatAmbient[2] = 0.2f;
    m_aMatAmbient[3] = 1.0f;
    m_aMatDiffuse[0] = fR;
    m_aMatDiffuse[1] = fG;
    m_aMatDiffuse[2] = fB;
    m_aMatDiffuse[3] = 1.0f;
    m_aMatSpecular[0] = 1.0f;
    m_aMatSpecular[1] = 1.0f;
    m_aMatSpecular[2] = 1.0f;
    m_aMatSpecular[3] = 1.0f;
    m_aMatShininess[0] = 128.0f;

    setStipple(false);
};
svt_color::~svt_color()
{
};

/**
 * Copy Constructor
 * \param rColor reference to color object
 */
svt_color::svt_color(const svt_color &rColor)
{
    int i;

    const Real32* pMatAmbient = rColor.m_aMatAmbient;
    for(i=0; i<4; i++)
        m_aMatAmbient[i] = pMatAmbient[i];

    const Real32* pMatDiffuse = rColor.m_aMatDiffuse;
    for(i=0; i<4; i++)
        m_aMatDiffuse[i] = pMatDiffuse[i];

    const Real32* pMatSpecular = rColor.m_aMatSpecular;
    for(i=0; i<4; i++)
	m_aMatSpecular[i] = pMatSpecular[i];

    m_aMatShininess[0] = rColor.m_aMatShininess[0];

    m_bStipple = rColor.m_bStipple;
}

/**
 * get the red component
 * \return red component
 */
Real32 svt_color::getR() const
{
    return m_aMatDiffuse[0];
};
/**
 * get the green component
 * \return green component
 */
Real32 svt_color::getG() const
{
    return m_aMatDiffuse[1];
};
/**
 * get the blue component
 * \return blue component
 */
Real32 svt_color::getB() const
{
    return m_aMatDiffuse[2];
};

/**
 * set the red component
 * \param fR the red component
 */
void svt_color::setR(float fR)
{
    m_aMatDiffuse[0] = fR;
};
/**
 * set the green component
 * \param fG the green component
 */
void svt_color::setG(float fG)
{
    m_aMatDiffuse[1] = fG;
};
/**
 * set the blue component
 * \param fB the blue component
 */
void svt_color::setB(float fB)
{
    m_aMatDiffuse[2] = fB;
};

void svt_color::set(float r, float g, float b)
{
  m_aMatDiffuse[0]=r;
  m_aMatDiffuse[1]=g;
  m_aMatDiffuse[2]=b;
}

/**
 * set the transparency
 * \param fTransparency if this is set to a value not equal to 1.0f (opaque), the transparency mode is switched on.
 */
void svt_color::setTransparency(Real32 fTransparency)
{
    m_aMatDiffuse[3] = fTransparency;
}
/**
 * get the transparency
 * \return transparency value (1.0f is opaque)
 */
Real32 svt_color::getTransparency() const
{
    return m_aMatDiffuse[3];
}

/**
 * set the material property: ambient
 */
void svt_color::setAmbient(Real32 fA1, Real32 fA2, Real32 fA3, Real32 fA4)
{
    m_aMatAmbient[0] = fA1;
    m_aMatAmbient[1] = fA2;
    m_aMatAmbient[2] = fA3;
    m_aMatAmbient[3] = fA4;
};
/**
 * returns a pointer to a float array with 4 float, describing the ambient property
 * \return pointer to a four element Real32 array
 */
Real32* svt_color::getAmbient()
{
    return m_aMatAmbient;
};

/**
 * set the material property: diffuse
 */
void svt_color::setDiffuse(Real32 fA1, Real32 fA2, Real32 fA3, Real32 fA4)
{
    m_aMatDiffuse[0] = fA1;
    m_aMatDiffuse[1] = fA2;
    m_aMatDiffuse[2] = fA3;
    m_aMatDiffuse[3] = fA4;
};
/**
 * returns a pointer to a float array with 4 float, describing the diffuse property
 * \return pointer to a four element Real32 array
 */
Real32* svt_color::getDiffuse()
{
    return m_aMatDiffuse;
};

/**
 * set the material property: specular
 */
void svt_color::setSpecular(Real32 fS1, Real32 fS2, Real32 fS3, Real32 fS4)
{
    m_aMatSpecular[0] = fS1;
    m_aMatSpecular[1] = fS2;
    m_aMatSpecular[2] = fS3;
    m_aMatSpecular[3] = fS4;
};
/**
 * returns a pointer to a float array with 4 float, describing the specular property.
 * \return pointer to a four element Real32 array
 */
Real32* svt_color::getSpecular()
{
    return m_aMatSpecular;
};

/**
 * set the material property: shininess
 * \param fS1 shininess value
 */
void svt_color::setShininess(Real32 fS1)
{
    m_aMatShininess[0] = fS1;
};
/**
 * get the material property: shininess
 * \return shininess value
 */
Real32 svt_color::getShininess() const
{
    return m_aMatShininess[0];
};

/**
 * get stipple mode status
 * \return true if the stipple mode is active
 */
bool svt_color::getStipple() const
{
    return m_bStipple;
}
/**
 * set stipple mode status.
 * The stipple mode is a fake transparent mode (you must also set transparency < 1.0f). All polygons are filled with 0101 patterns to achieve a transparency-like look.
 * \param bStipple true if the stipple mode is active
 */
void svt_color::setStipple(bool bStipple)
{
    m_bStipple = bStipple;
}

/**
 * adjust the current open gl color
 */
void svt_color::applyGL(){

    if (m_aMatAmbient[0] != -1.0f)
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_aMatAmbient);
    if (m_aMatDiffuse[0] != -1.0f)
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_aMatDiffuse);
    if (m_aMatSpecular[0] != -1.0f)
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_aMatSpecular);
    if (m_aMatShininess[0] != -1.0f)
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, m_aMatShininess);

    if (m_aMatDiffuse[3] == 1.0f)
    {
        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_STIPPLE);
        glColor3f(m_aMatDiffuse[0], m_aMatDiffuse[1], m_aMatDiffuse[2]);
    } else {
        if (getStipple())
        {
            glColor3f(m_aMatDiffuse[0], m_aMatDiffuse[1], m_aMatDiffuse[2]);
            glEnable(GL_POLYGON_STIPPLE);
            if (m_aMatDiffuse[3] > 0.00f && m_aMatDiffuse[3] <= 0.25f)
                glPolygonStipple(g_aStipple00);
            if (m_aMatDiffuse[3] > 0.25f && m_aMatDiffuse[3] <= 0.50f)
                glPolygonStipple(g_aStipple25);
            if (m_aMatDiffuse[3] > 0.50f && m_aMatDiffuse[3] <= 0.75f)
                glPolygonStipple(g_aStipple50);
            if (m_aMatDiffuse[3] > 0.75f && m_aMatDiffuse[3] < 1.00f)
            glPolygonStipple(g_aStipple75);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(m_aMatDiffuse[0], m_aMatDiffuse[1], m_aMatDiffuse[2], m_aMatDiffuse[3]);
        }
    }
}

/**
 * adjust the current vrml color
 */
void svt_color::applyVrml()
{
    cout << "diffuseColor " << m_aMatDiffuse[0] << " " << m_aMatDiffuse[1] << " " << m_aMatDiffuse[2] << endl;
}

/**
 * output the ascii code for an alias/wavefront material description
 * \param pName pointer to an array of char with the name of the material
 */
void svt_color::applyOBJ(char* pName)
{
    cout << "newmtl " << pName << endl;
    cout << "Ka " << m_aMatAmbient[0] << " " << m_aMatAmbient[1] << " " << m_aMatAmbient[2] << endl;
    cout << "Kd " << m_aMatDiffuse[0] << " " << m_aMatDiffuse[1] << " " << m_aMatDiffuse[2] << endl;
    cout << "Ks " << m_aMatSpecular[0] << " " << m_aMatSpecular[1] << " " << m_aMatSpecular[2] << endl;
    cout << "d " << m_aMatDiffuse[3] << endl;
    cout << "Tr " << m_aMatDiffuse[3] << endl;
    cout << "illum 2" << endl;
    cout << "Ns " << m_aMatShininess[0] << endl;
    cout << endl;
};

/**
 * Are two color equal?
 */
bool svt_color::operator==(const svt_color& rOther)
{
    unsigned int i;

    for(i=0; i<4; i++)
	if (m_aMatAmbient[i] != rOther.m_aMatAmbient[i])
	    return false;

    for(i=0; i<4; i++)
	if (m_aMatDiffuse[i] != rOther.m_aMatDiffuse[i])
	    return false;

    return true;
}
