/***************************************************************************
                          svt_color.h
			  -----------
    begin                : Fri Feb 25 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_COLOR_H
#define SVT_COLOR_H

#include <svt_types.h>

/**
 * Represents the color of a svt object
 */
class DLLEXPORT svt_color {

protected:
    Real32 m_aMatAmbient[4];
    Real32 m_aMatDiffuse[4];
    Real32 m_aMatSpecular[4];
    Real32 m_aMatShininess[1];
    bool m_bStipple;

public:
    /**
     * Constructor
     * Create a new color.
     * \param fR red color component
     * \param fG green color component
     * \param fB blue color component
     */
    svt_color(Real32 fR=1.0f, Real32 fG=1.0f, Real32 fB=1.0f);
    /**
     * Copy Constructor
     * \param rColor reference to color object
     */
    svt_color(const svt_color &rColor);
    virtual ~svt_color();

    /**
     * get the red component
     * \return red component
     */
    Real32 getR() const;
    /**
     * get the green component
     * \return green component
     */
    Real32 getG() const;
    /**
     * get the blue component
     * \return blue component
     */
    Real32 getB() const;

    /**
     * set the red component
     * \param fR the red component
     */
    void setR(float fR);
    /**
     * set the green component
     * \param fG the green component
     */
    void setG(float fG);
    /**
     * set the blue component
     * \param fB the blue component
     */
    void setB(float fB);

    /**
     * set all color components
     * \param r red
     * \param g green
     * \param b blue
     */
    void set(float r, float g, float b);

    /**
     * set the transparency
     * \param fTransparency if this is set to a value not equal to 1.0f (opaque), the transparency mode is switched on.
     */
    void setTransparency(Real32 fTransparency);
    /**
     * get the transparency
     * \return transparency value (1.0f is opaque)
     */
    Real32 getTransparency() const;

    /**
     * set the material property: ambient
     */
    void setAmbient(Real32 fA1, Real32 fA2, Real32 fA3, Real32 fA4);
    /**
     * returns a pointer to a float array with 4 float, describing the ambient property
     * \return pointer to a four element Real32 array
     */
    Real32* getAmbient();

    /**
     * set the material property: diffuse
     */
    void setDiffuse(Real32 fA1, Real32 fA2, Real32 fA3, Real32 fA4);
    /**
     * returns a pointer to a float array with 4 float, describing the diffuse property
     * \return pointer to a four element Real32 array
     */
    Real32* getDiffuse();

    /**
     * set the material property: specular
     */
    void setSpecular(Real32 fS1, Real32 fS2, Real32 fS3, Real32 fS4=1.0f);
    /**
     * returns a pointer to a float array with 4 float, describing the specular property.
     * \return pointer to a four element Real32 array
     */
    Real32* getSpecular();

    /**
     * set the material property: shininess
     * \param fS1 shininess value
     */
    void setShininess(Real32 fS1);
    /**
     * get the material property: shininess
     * \return shininess value
     */
    Real32 getShininess() const;

    /**
     * get stipple mode status
     * \return true if the stipple mode is active
     */
    bool getStipple() const;
    /**
     * set stipple mode status.
     * The stipple mode is a fake transparent mode (you must also set transparency < 1.0f). All polygons are filled with 0101 patterns to achieve a transparency-like look.
     * \param bStipple true if the stipple mode is active
     */
    void setStipple(bool bStipple);

    /**
     * adjust the current open gl color
     */
    void applyGL();
    /**
     * adjust the current vrml color
     */
    void applyVrml();
    /**
     * output the ascii code for an alias/wavefront material description
     * \param pName pointer to an array of char with the name of the material
     */
    void applyOBJ(char* pName);

    /**
     * Are two color equal?
     */
    bool operator==(const svt_color& rOther);
};

#endif
