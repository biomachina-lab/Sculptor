/***************************************************************************
                          svt_textnode.h  -  description
                             -------------------
    begin                : Feb 28 2008
    author               : Jochen Heyd
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_TEXTNODE_H
#define SVT_TEXTNODE_H

#include <svt_node.h>

/**
 * A text node based on simple_text
 *@author Jochen Heyd
 */
class DLLEXPORT svt_textnode : public svt_node  {

protected:
    char m_pString[256];

    svt_pos m_Position;
    svt_pos m_Offset;
    
    float m_fBackgroundWidth;
    float m_fBackgroundColorR;
    float m_fBackgroundColorG;
    float m_fBackgroundColorB;

    float m_fForegroundWidth;
    float m_fForegroundColorR;
    float m_fForegroundColorG;
    float m_fForegroundColorB;

    bool m_bCentered;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pText pointer to array of char with the text
     */
    svt_textnode(svt_pos* pPos =NULL, char* pText =NULL);
    virtual ~svt_textnode();

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
     * set the text offset from actual position
     * \param fX x coordinate of the offset
     * \param fY y coordinate of the offset
     * \param fZ z coordinate of the offset
     */
    void setOffset( Real64 fX, Real64 fY, Real64 fZ );
    
    /**
     * set foreground color
     * \param fR red value (0..1)
     * \param fG green value (0..1)
     * \param fB blue value (0..1)
     */
    void setFGColor(float fR, float fG, float fB);
    /**
     * set background color
     * \param fR red value (0..1)
     * \param fG green value (0..1)
     * \param fB blue value (0..1)
     */
    void setBGColor(float fR, float fG, float fB);

    /**
     * should the text be centered around the position?
     * \param bCentered
     */
    void setCentered(bool bCentered);

    /**
     * draw textnode
     */
    void drawGL();

protected:
    /**
     * draw actual text
     */
    void drawText(const char pStr[]);
};

#endif
