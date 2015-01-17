/***************************************************************************
                          svt_lightsource.cpp  -  description
                             -------------------
    begin                : May 17 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_defs.h>
#include <svt_lightsource.h>
#include <svt_light.h>
#include <svt_opengl.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 */
svt_lightsource::svt_lightsource(svt_pos* pPos, bool bInfinite) 
  : svt_node_noautodl(pPos), m_bInfinite(bInfinite)
{
    setAmbient(0.1f, 0.1f, 0.1f, 1.0f);
    setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
    setSpecular(0.4f, 0.4f, 0.4f, 1.0f);
    setAttenuation(0.01f);
    setLightNumber(GL_LIGHT0);
    setSpotlight(false);
};
svt_lightsource::~svt_lightsource()
{
};

/**
 * draw the lightsource
 */
void svt_lightsource::drawGL()
{
    if (m_iLight == -1)
        m_iLight = m_pLight->registerLightsource();

    GLfloat pLightPos[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (getTransformation() != NULL)
    {
        pLightPos[0] = getTransformation()->getTranslationX();
        pLightPos[1] = getTransformation()->getTranslationY();
        pLightPos[2] = getTransformation()->getTranslationZ();
    }

    if (!m_bInfinite)
        pLightPos[3] = 1.0f;
    else
        pLightPos[3] = 0.0f;

    if (m_pLightAmbient[0] >= 0)
        glLightfv( (ENUM_GLENUM)m_iLight, GL_AMBIENT, m_pLightAmbient );
    if (m_pLightDiffuse[0] >= 0)
        glLightfv( (ENUM_GLENUM)m_iLight, GL_DIFFUSE, m_pLightDiffuse );
    if (m_pLightSpecular[0] >= 0)
        glLightfv( (ENUM_GLENUM)m_iLight, GL_SPECULAR, m_pLightSpecular );
    if (m_fAttenuation >= 0)
        glLightf( (ENUM_GLENUM)m_iLight, GL_LINEAR_ATTENUATION, m_fAttenuation);

    glLightfv((ENUM_GLENUM)m_iLight, GL_POSITION, pLightPos);
    glEnable( (ENUM_GLENUM)m_iLight );
    glEnable( GL_LIGHTING );

    if (isSpotlight())
    {
        GLfloat pSpotDir[] = { 0.0f, 0.0f, 0.0f, 1.0f };

        svt_vector4f oVec = getSpotDir();
        if (getTransformation() != NULL)
            oVec.postMult(*getTransformation());

        pSpotDir[0] = oVec.x();
        pSpotDir[1] = oVec.y();
        pSpotDir[2] = oVec.z();

        glLightfv((ENUM_GLENUM)m_iLight, GL_SPOT_DIRECTION, pSpotDir);
    }
}

/**
 * set ambient
 */
void svt_lightsource::setAmbient(Real32 fA1, Real32 fA2, Real32 fA3, Real32 fA4)
{
    m_pLightAmbient[0] = (GLfloat)fA1;
    m_pLightAmbient[1] = (GLfloat)fA2;
    m_pLightAmbient[2] = (GLfloat)fA3;
    m_pLightAmbient[3] = (GLfloat)fA4;
};
/**
 * get ambient
 */
const GLfloat* svt_lightsource::getAmbient()
{
    return m_pLightAmbient;
};

/**
 * set diffuse
 */
void svt_lightsource::setDiffuse(Real32 fD1, Real32 fD2, Real32 fD3, Real32 fD4)
{
    m_pLightDiffuse[0] = (GLfloat)fD1;
    m_pLightDiffuse[1] = (GLfloat)fD2;
    m_pLightDiffuse[2] = (GLfloat)fD3;
    m_pLightDiffuse[3] = (GLfloat)fD4;
};
/**
 * get diffuse
 */
const GLfloat* svt_lightsource::getDiffuse()
{
    return m_pLightDiffuse;
};

/**
 * set specular
 */
void svt_lightsource::setSpecular(Real32 fS1, Real32 fS2, Real32 fS3, Real32 fS4)
{
    m_pLightSpecular[0] = (GLfloat)fS1;
    m_pLightSpecular[1] = (GLfloat)fS2;
    m_pLightSpecular[2] = (GLfloat)fS3;
    m_pLightSpecular[3] = (GLfloat)fS4;
};
/**
 * get specular */
const GLfloat* svt_lightsource::getSpecular()
{
    return m_pLightSpecular;
};

/**
 * set the m_fAttenuation
 */
void svt_lightsource::setAttenuation(Real32 fAttenuation)
{
    m_fAttenuation = (GLfloat)fAttenuation;
};
/**
 * get the m_fAttenuation
 */
Real32 svt_lightsource::getAttenuation()
{
    return (Real32)m_fAttenuation;
};

/**
 * set the number of the light - just internal use!
 */
void svt_lightsource::setLightNumber(int iLight)
{
    m_iLight = iLight;
};
/**
 * get the number of the light - just internal use!
 */
int svt_lightsource::getLightNumber()
{
    return m_iLight;
};

/**
 * set the svt_light object (internally used by svt_light)
 * \param pLight pointer to svt_light object
 */
void svt_lightsource::setLight(svt_light* pLight)
{
    m_pLight = pLight;
}

/**
 * infinte light or local light. The infinite lights are much faster than the local lights!
 */
void svt_lightsource::setInfinite(bool bInfinite)
{
    m_bInfinite = bInfinite;
};
/**
 * get the infinite mode status
 */
bool svt_lightsource::getInfinite()
{
    return m_bInfinite;
};

/**
 * is it a spotlight?
 * \return true if the lightsource is a spotlight
 */
bool svt_lightsource::isSpotlight()
{
    return m_bSpotlight;
}
/**
 * is it a spotlight?
 * \param bSpotlight true if the lightsource is a spotlight
 */
void svt_lightsource::setSpotlight(bool bSpotlight)
{
    m_bSpotlight = bSpotlight;
}
/**
 * get direction of spotlight (vector relative to the lightsource transformation matrix)
 * \return direction vector
 */
svt_vector4f svt_lightsource::getSpotDir()
{
    return m_oSpotDir;
}
/**
 * set direction of spotlight (vector relative to the lightsource transformation matrix)
 * \param oSpotDir direction vector
 */
void svt_lightsource::setSpotDir(svt_vector4f &oSpotDir)
{
    m_oSpotDir = oSpotDir;
}

/**
 * print "Lightsource" to stdout
 */
void svt_lightsource::printName()
{
    cout << "Lightsource" << endl;
};
