/***************************************************************************
                          svt_displayList.h
                          -----------------
    begin                : 13.02.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_DISPLAY_H
#define SVT_DISPLAY_H

#include <svt_core.h>
#include <svt_contextData.h>

/**
 */
class DLLEXPORT svt_displayList : public svt_contextData
{

protected:

    int* m_pDisplayLists;

public:

    /**
     * Constructor
     */
    svt_displayList();
    /**
     * Destructor
     */
    virtual ~svt_displayList();

    /**
     * destroy
     * \param iWin context/window number
     */
    virtual void destroy(int iWin);

    /**
     * set display list
     * \param iDisplayList integer with handle of display list object
     */
    void setDisplayList(int iDisplayList);
    /**
     * get display list
     * \return integer with handle of display list object
     */
    int getDisplayList();
};

#endif
