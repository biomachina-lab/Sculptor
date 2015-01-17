/***************************************************************************
                          svt_contextData.h
                          -----------------
    begin                : 30.01.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONTEXTDATA_H
#define SVT_CONTEXTDATA_H

#include <svt_core.h>

/**
 */
class DLLEXPORT svt_contextData
{

protected:

    bool* m_pCreated;
    bool* m_pDeleted;
    bool m_bDestroy;

public:

    /**
     * Constructor
     */
    svt_contextData();
    /**
     * Destructor
     */
    virtual ~svt_contextData();

    /**
     * create
     * \param iWin context/window number
     */
    virtual void create(int iWin);

    /**
     * destroy
     * \param iWin context/window number
     */
    virtual void destroy(int iWin);

    /**
     * created in all contexts?
     */
    bool isCreated();
    /**
     * deleted in all contexts?
     */
    bool isDestroyed();

    /**
     * the context data should get destroyed
     */
    void destroyCD();
    /**
     * should the contextData get destroyed?
     * \return true if the system should destory the cd
     */
    bool shouldDestroy();
};

#endif
