/***************************************************************************
                          svt_fps.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_fps.h>
#include <svt_node.h>
#include <svt_pos.h>
#include <svt_properties.h>
#include <svt_init.h>
// clib includes
#include <svt_iostream.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param pFont pointer to svt_font object
 */
svt_fps::svt_fps(svt_pos* pPos, svt_properties* pProp) : svt_node(pPos, pProp)
{
};
svt_fps::~svt_fps()
{
}

/**
 * recalc function
 */
bool svt_fps::recalc()
{
    char text[256];
    sprintf(text,"FPS: %i", svt_getFPS());

    return true;
}

/**
 * print "FPS" to stdout
 */
void svt_fps::printName()
{
    cout << "FPS" << endl;
};
