/***************************************************************************
                          svt_text.h  -  description
                             -------------------
    begin                : May 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TEXT_H
#define SVT_TEXT_H

#include <svt_node_noautodl.h>
#include <svt_font.h>

/**
 * Textured text (supporting glf font format)
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_text : public svt_node_noautodl
{
protected:
    char m_pString[256];
    float m_fSize;
    svt_font* m_pFont;
    bool m_b3D;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pText pointer to array of char with the text
     * \param fSize size of text
     * \param pFont pointer to svt_font object
     */
    svt_text(svt_pos* pPos =NULL, char* pText =NULL, Real32 fSize =10.0f, svt_font* pFont =NULL);
    virtual ~svt_text(){};

    /**
     * set the text
     * \param pText pointer to array of char with the text
     */
    void setText(char* pText);
    /**
     * get the text
     * \return pointer to array of char with the text
     */
    char* getText();

    /**
     * set the size
     * \param fSize size of the text (scaling)
     */
    void setSize(Real32 fSize);
    /**
     * get the size
     * \return size of the text
     */
    float getSize();

    /**
     * set the font
     * \param pFont pointer to svt_font object
     */
    void setFont(svt_font* pFont);
    /**
     * get the font
     * \return pointer to svt_font object
     */
    svt_font* getFont();

    /**
     * 3D or only flat 2D text?
     * \param b3D true if 3D text
     */
    void set3D(bool b3D);

    /**
     * draw the text
     */
    virtual void drawGL();

    /**
     * print "Text" to stdout
     */
    virtual void printName();
};

#endif
