/***************************************************************************
                          svt_displayList.cpp  -  description
                             -------------------
    begin                : 30.01.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_displayList.h>
#include <svt_init.h>
#include <svt_opengl.h>

/**
 * Constructor
 */
svt_displayList::svt_displayList() : svt_contextData(),
    m_pDisplayLists(NULL)
{
    int iWinNum = svt_getWindowsNum();

    m_pDisplayLists = new int[iWinNum+1];

    for(int i=0;i<iWinNum;i++)
        m_pDisplayLists[i] = -1;
}
/**
 * Destructor
 */
svt_displayList::~svt_displayList()
{
    if (m_pDisplayLists)
        delete[] m_pDisplayLists;
};

/**
 * destroy
 */
void svt_displayList::destroy(int iWin)
{
    if (m_pDeleted[iWin-1] == false && iWin > 0 && m_pDisplayLists[iWin-1] != -1)
        glDeleteLists(m_pDisplayLists[iWin-1], 1);

    svt_contextData::destroy(iWin);
}

/**
 * set display list
 * \param iDisplayList integer with handle of display list object
 */
void svt_displayList::setDisplayList(int iDisplayList)
{
    int iWin = svt_getCurrentWindow();
    if (iWin > 0)
        m_pDisplayLists[iWin-1] = iDisplayList;
}
/**
 * get display list
 * \return integer with handle of display list object
 */
int svt_displayList::getDisplayList()
{
    int iWin = svt_getCurrentWindow();

    if (iWin > 0)
        return m_pDisplayLists[iWin-1];
    else
        return -1;
}
