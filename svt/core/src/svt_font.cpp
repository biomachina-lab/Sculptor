/***************************************************************************
                          svt_font.cpp  -  description
                             -------------------
    begin                : May 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_system.h>
#include <svt_window.h>
#include <svt_font.h>
#include <svt_init.h>
// clib includes
#include <string.h>
#include <svt_iostream.h>
// opengl includes
#include <glf.h>

/**
 * Constructor
 * \param pFont pointer to array of char with the font filename
 * \param fDepth depth of the characters
 */
svt_font::svt_font(char* pFont, float fDepth)
{
    m_pBMFFontDesc  = new int[svt_getWindowsNum()];
    int i;
    for(i=0; i<svt_getWindowsNum(); i++)
        m_pBMFFontDesc[i] = -1;

    LoadFont(pFont);
    setDepth(fDepth);
};
svt_font::~svt_font()
{
    UnloadFont();
};

/**
 * set the 3d depth of the font
 * \param fDepth depth of the characters
 */
void svt_font::setDepth(Real32 fDepth)
{
    m_fDepth = fDepth;
}
/**
 * get the 3d depth of the font
 * \return depth of the characters
 */
Real32 svt_font::getDepth()
{
    return m_fDepth;
}


/**
 * get the name of the font (returns NULL if no font is loaded)
 * \return pointer to array of char with the font name
 */
char* svt_font::getName()
{
    if (m_iFontDesc != -1)
        return m_pFontName;
    else
        return NULL;
}

/**
 * is the current font a bitmap or a vector font
 * \return true if in bitmap mode
 */
bool svt_font::getBitmapMode()
{
    return m_bBitmap;
}

/**
 * set the "current font" to this font
 */
void svt_font::applyGL()
{
    if (!m_bBitmap)
    {
        if (m_iFontDesc != -1)
        {
            glfSetCurrentFont(m_iFontDesc);
            glfSetSymbolDepth(m_fDepth);
        }
    } else {
        svt_win_handle iWin = svt_getCurrentWindow();
        if (m_pBMFFontDesc[iWin-1] != -1)
            glfSetCurrentBMFFont(m_pBMFFontDesc[iWin-1]);
        else {
            m_pBMFFontDesc[iWin-1] = glfLoadBMFFont(m_pFontName);
            if (m_pBMFFontDesc[iWin-1] == -1) {
                cout << "svt_font: failed to load bitmap font:" << m_pFontName << endl;
                //svt_exit(1);
            }

            glfSetCurrentBMFFont(m_pBMFFontDesc[iWin-1]);
        }
    }
};

/**
 * Load a font
 * \param pFont pointer to array of char with the font filename
 */
void svt_font::LoadFont(char* pFont)
{
    if (pFont == NULL)
    {
        m_iFontDesc = -1;
        return;
    }

    strncpy(m_pFontName, pFont, 256);

    if (strstr(pFont, ".bmf") == NULL)
    {
        m_iFontDesc = glfLoadFont(pFont); m_bBitmap = false;
        if (m_iFontDesc == -1)
            cout << "svt_font: failed to load vector font:" << pFont << endl;
    } else {
        m_bBitmap = true;
    }
};

/**
 * unload font from the system
 */
void svt_font::UnloadFont()
{
    if (m_iFontDesc != -1)
        glfUnloadFontD(m_iFontDesc);
};
