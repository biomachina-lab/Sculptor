/***************************************************************************
                          svt_font.h  -  description
                             -------------------
    begin                : May 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FONT_H
#define SVT_FONT_H

#include <svt_core.h>
#include <svt_types.h>

/**
 * A font (for use with the svt_text class)
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_font
{
protected:
    int m_iFontDesc;
    char m_pFontName[256];
    Real32 m_fDepth;
    bool m_bBitmap;
    int* m_pBMFFontDesc;

public:
    /**
     * Constructor
     * \param pFont pointer to array of char with the font filename
     * \param fDepth depth of the characters
     */
    svt_font(char* pFont, float fDepth =1.0f);
    ~svt_font();

    /**
     * set the 3d depth of the font
     * \param fDepth depth of the characters
     */
    void setDepth(Real32 fDepth);
    /**
     * get the 3d depth of the font
     * \return depth of the characters
     */
    Real32 getDepth();

    /**
     * get the name of the font (returns NULL if no font is loaded)
     * \return pointer to array of char with the font name
     */
    char* getName();

    /**
     * is the current font a bitmap or a vector font
     * \return true if in bitmap mode
     */
    bool getBitmapMode();

    /**
     * set the "current font" to this font
     */
    void applyGL( void );

protected:
    /**
     * Load a font
     * \param pFont pointer to array of char with the font filename
     */
    void LoadFont(char* pFont);
    /**
     * unload font from the system
     */
    void UnloadFont();

};

#endif
