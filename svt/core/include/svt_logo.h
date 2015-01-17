/***************************************************************************
                          svt_logo.h  -  description
                             -------------------
    begin                : Mon May 28 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_LOGO_H
#define SVT_LOGO_H

#include <svt_core.h>
#include <svt_node.h>

class svt_texture;
class svt_color;

/**
 * A rotating logo of the research center Juelich
 *@author Jan Deiterding, Herwig Zilken
 */
class DLLEXPORT svt_logo : public svt_node  {

protected:
    // two logos, one for the front and one for the back side
    svt_texture *m_pLogo1, *m_pLogo2;
    unsigned int m_pTexture[1];
    int m_iDrawAngle;
    int m_iStartZeit;
    int m_iAktZeit;
    svt_color* m_pTempCol;
    svt_color* m_pTemp2Col;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_logo(svt_pos* pPos =NULL, svt_properties* pProp =NULL);
    virtual ~svt_logo();

    /**
     * draw the Logo
     */
    virtual void drawGL();

    /**
     * print "Logo" to stdout
     */
    void printName();

protected:
    /**
     * Initialize the textures
     */
    void initTextures();
};

#endif
