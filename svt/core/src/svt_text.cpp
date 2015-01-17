/***************************************************************************
                          svt_text.cpp  -  description
                             -------------------
    begin                : May 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_text.h>
// clib includes
#include <svt_iostream.h>
#include <string.h>
// opengl includes
#include <glf.h>
#include <GL/gl.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pText pointer to svt_text object
 * \param fSize size of text
 * \param pFont pointer to svt_font object
 */
svt_text::svt_text(svt_pos* pPos, char* pText, Real32 fSize, svt_font* pFont ) : svt_node_noautodl(pPos)
{
    setSize(fSize);
    setText(pText);
    setFont(pFont);
    set3D(true);
};

/**
 * set the text
 * \param pText pointer to array of char with the text
 */
void svt_text::setText(char* pText)
{
    if (pText != NULL)
    {
        strncpy(m_pString, pText, 256);
        rebuildDL();
    }
}
/**
 * get the text
 * \return pointer to array of char with the text
 */
char* svt_text::getText()
{
    return m_pString;
}

/**
 * set the size
 * \param fSize size of the text (scaling)
 */
void svt_text::setSize(float fSize)
{
    m_fSize = fSize;
    rebuildDL();
}
/**
 * get the size
 * \return size of the text
 */
float svt_text::getSize()
{
    return m_fSize;
}

/**
 * set the font
 * \param pFont pointer to svt_font object
 */
void svt_text::setFont(svt_font* pFont)
{
    m_pFont = pFont;
    rebuildDL();
}
/**
 * get the font
 * \return pointer to svt_font object
 */
svt_font* svt_text::getFont()
{
    return m_pFont;
}

/**
 * 3D or only flat 2D text?
 * \param b3D true if 3D text
 */
void svt_text::set3D(bool b3D)
{
    m_b3D = b3D;
    rebuildDL();
}

/**
 * draw the text
 */
void svt_text::drawGL()
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    if (m_pFont == NULL)
        return;

    if (m_fSize == 0)
        return;

    m_pFont->applyGL();

    if (getTexture() != NULL){
        glfEnable(GLF_TEXTURING);
        glEnable(GL_TEXTURE_2D);
        getTexture()->BindTexture();
    }

    if (m_pFont->getBitmapMode() == false)
    {
        glScalef(m_fSize/50,m_fSize/50,m_fSize/50);

        if (m_b3D)
            glfDraw3DSolidString(m_pString);
        else
            glfDrawSolidString(m_pString);

        glScalef(50*m_fSize, 50*m_fSize, 50*m_fSize);

    } else {
        glShadeModel(GL_FLAT);								// Enables Smooth Color Shading
        glDisable(GL_LIGHTING);
        glfStartBitmapDrawing();
        glScalef(m_fSize/2, m_fSize/2, 0);
        glfDrawBString(m_pString);
        glScalef(2/m_fSize, 2/m_fSize, 0);
        glfStopBitmapDrawing();
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);								// Enables Smooth Color Shading
    }

    if (getTexture() != NULL)
    {
        glfDisable(GLF_TEXTURING);
        glDisable(GL_TEXTURE_2D);
    }

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
};

/**
 * print "Text" to stdout
 */
void svt_text::printName()
{
    cout << "Text" << endl;
}
