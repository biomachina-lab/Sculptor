/***************************************************************************
                          svt_properties.h
			  ----------------
    begin                : Tue Mar 30 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PROPERTIES_H
#define SVT_PROPERTIES_H

#include <svt_color.h>
#include <svt_opengl.h>

/**
 * Represents the (material) properties of a svt object
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_properties
{

protected:

    svt_color* m_pColor;
    svt_color* m_pWireframeColor;
    svt_color* m_pUnderTextureColor;

    bool m_bWireframe;
    bool m_bSolid;
    bool m_bLighting;
    bool m_bSmooth;
    bool m_bTexture;
    bool m_bNormals;
    bool m_bCullBackfaces;
    bool m_bTwoSided;

    static svt_color sm_oDefWireframeColor;
    static svt_color sm_oDefUnderTextureColor;
    static svt_color sm_oDefColor;

    bool m_bDeleteColors;

public:

    /**
     * Constructor.
     * \param pColor pointer to a svt_color object
     */
    svt_properties(svt_color* pColor =NULL);
    virtual ~svt_properties();

public:

    /**
     * set the color of this properties object
     * \param pColor pointer to the svt_color object. Set theis pointer to
     *               NULL to restore return to the default color
     */
    void setColor(svt_color* pColor);
    void setWireframeColor(svt_color* pColor);
    void setUnderTextureColor(svt_color* pColor);

    /**
     * get the color
     * \return pointer to the svt_color object or NULL, if the global
     *         default color is currently used.
     */
    svt_color* getColor() const;
    svt_color* getWireframeColor() const;
    svt_color* getUnderTextureColor() const;

    /**
     * set the wireframe mode
     * \param bWireframe if true if the object is visualized in wireframe mode
     */
    void setWireframe(bool b);
    void setSolid(bool b);
    void setSmooth(bool b);
    void setLighting(bool b);
    void setTexture(bool b);
    void setNormals(bool b);
    void setBackfaceCulling(bool b);
    void setTwoSided(bool b);

    /**
     * get the wireframe mode
     * \return if true if the object is visualized in wireframe mode
     */
    bool getWireframe() const;
    bool getSolid() const;
    bool getSmooth() const;
    bool getLighting() const;
    bool getTexture() const;
    bool getNormals() const;
    bool getBackfaceCulling() const;
    bool getTwoSided() const;

    /**
     * toggle settings
     */
    bool toggleWireframe();
    bool toggleSolid();
    bool toggleSmooth();
    bool toggleLighting();
    bool toggleTexture();
    bool toggleNormals();
    bool toggleBackfaceCulling();

    /**
     * apply the properties to the current opengl state
     */
    void applyGLsolid(bool bTextureAvailable);
    void applyGLwireframe();
    void applyGLnormals();

    /**
     * print the vrml code for the properties object
     */
    void applyVrml();

    /**
     * set the default color
     */
    static void setDefColor(Real32 r, Real32 g, Real32 b);
    static void setDefWireframeColor(Real32 r, Real32 g, Real32 b);
    static void setDefUnderTextureColor(Real32 r, Real32 g, Real32 b);

    static const svt_color& defColor();
    static const svt_color& defWireframeColor();
    static const svt_color& defUnderTextureColor();

    /**
     * Set if the object should delete the color objects by itself or wait for somebody else to do it. It will delete all color objects - color, wireframe color, color under texture objects!
     * \param bDeleteColors if true, the object will delete the color objects in the destructor
     */
    void setDeleteColors( bool bDeleteColors );
    /**
     * Get if the object should delete the color objects by itself or wait for somebody else to do it. It will delete all color objects - color, wireframe color, color under texture objects!
     * \return if true, the object will delete the color objects in the destructor
     */
    bool getDeleteColors( );
};

#endif
