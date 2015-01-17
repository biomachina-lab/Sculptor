/***************************************************************************
                          svt_light.h  -  description
                             -------------------
    begin                : Jul 20 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_LIGHT_H
#define SVT_LIGHT_H

#define MAX_LIGHT_SOURCES 8
#define SVT_UNBOUND 0
#define SVT_BOUND   1

#include <svt_node.h>
#include <svt_lightsource.h>
#include <svt_opengl.h>

/**
 * Container for the lightsources - for use with svt_scene. Just add objects of the type svt_lightsource to it and call setLight() of your svt_scene object.
 * See the example Lighting.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_light : public svt_node_noautodl  {

protected:

    int* m_pLightFlags;
    static GLint m_iMaxLights;

public:
    /**
     * Constructor
     */
    svt_light();
    virtual ~svt_light();

    /**
     * draw all attached lightsources
     * \param iMode mode for the drawing (e.g. SVT_MODE_GL, SVT_MODE_VRML)
     */
    void draw(int iMode);

    /**
     * draw the light
     */
    virtual void drawGL();

    /**
     * add a lightsource to the container
     * \param pLight pointer to svt_lightsource
     */
    void add(svt_lightsource* pLight);
    /**
     * del a lightsource from the container
     * \param pLight pointer to svt_lightsource
     */
    void del(svt_lightsource* pLight);

    /**
     * find a free light and register it (called by svt_lightsource)
     */
    int registerLightsource();

protected:
    /**
     * is this light free or bound to a lightsource
     * \param iIndex number of the lightsource
     * \param iFlag new status for the lightsource (e.g. SVT_BOUND)
     */
    void setBound(int iIndex, int iFlag);
    /**
     * is this light free or bound to a lightsource?
     * \param iIndex number of the lightsource
     * \return status for the lightsource (e.g. SVT_BOUND)
     */
    int getBound(int iIndex);
};

#endif // SVT_LIGHT_H
