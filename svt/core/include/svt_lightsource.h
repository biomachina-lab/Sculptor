/***************************************************************************
                          svt_lightsource.h  -  description
                             -------------------
    begin                : May 17 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_LIGHTSOURCE_H
#define SVT_LIGHTSOURCE_H

// svt includes
#include <svt_node_noautodl.h>
#include <svt_types.h>
#include <svt_vector4f.h>
// clib includes
#include <svt_iostream.h>

class svt_light;

/**
 * A lightsource (you can have at most 8 of them - see OpenGL)
 * @author Stefan Birmanns
 */
class DLLEXPORT svt_lightsource : public svt_node_noautodl  {

protected:
    GLfloat m_pLightAmbient[4];
    GLfloat m_pLightDiffuse[4];
    GLfloat m_pLightSpecular[4];
    GLfloat m_fAttenuation;
    int m_iLight;
    bool m_bInfinite;
    svt_light* m_pLight;
    bool m_bSpotlight;
    svt_vector4f m_oSpotDir;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     */
    svt_lightsource(svt_pos* pPos =NULL, bool bInfinite=false);
    virtual ~svt_lightsource();

    /**
     * draw the lightsource
     */
    void drawGL();

    /**
     * set ambient
     */
    void setAmbient(Real32 fA1, Real32 fA2, Real32 fA3, Real32 fA4);
    /**
     * get ambient
     */
    const GLfloat* getAmbient();

    /**
     * set diffuse
     */
    void setDiffuse(Real32 fD1, Real32 fD2, Real32 fD3, Real32 fD4);
    /**
     * get diffuse
     */
    const GLfloat* getDiffuse();

    /**
     * set specular
     */
    void setSpecular(Real32 fS1, Real32 fS2, Real32 fS3, Real32 fS4);
    /**
     * get specular
     */
    const GLfloat* getSpecular();

    /**
     * set the m_fAttenuation
     */
    void setAttenuation(Real32 fAttenuation);
    /**
     * get the m_fAttenuation
     */
    Real32 getAttenuation();

    /**
     * set the number of the light - just internal use!
     */
    void setLightNumber(int iLight);
    /**
     * get the number of the light - just internal use!
     */
    int getLightNumber();

    /**
     * set the svt_light object (internally used by svt_light)
     * \param pLight pointer to svt_light object
     */
    void setLight(svt_light* pLight);

    /**
     * infinte light or local light. The infinite lights are much faster than the local lights!
     */
    void setInfinite(bool bInfinite);
    /**
     * get the infinite mode status
     */
    bool getInfinite();

    /**
     * is it a spotlight?
     * \return true if the lightsource is a spotlight
     */
    bool isSpotlight();
    /**
     * is it a spotlight?
     * \param bSpotlight true if the lightsource is a spotlight
     */
    void setSpotlight(bool bSpotlight);
    /**
     * get direction of spotlight (vector relative to the lightsource transformation matrix)
     * \return direction vector
     */
    svt_vector4f getSpotDir();
    /**
     * set direction of spotlight (vector relative to the lightsource transformation matrix)
     * \param oSpotDir direction vector
     */
    void setSpotDir(svt_vector4f &oSpotDir);

    /**
     * print "Lightsource" to stdout
     */
    void printName();
};

#endif
