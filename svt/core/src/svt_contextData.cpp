/***************************************************************************
                          svt_contextData.cpp  -  description
                             -------------------
    begin                : 30.01.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_contextData.h>
#include <svt_init.h>
#include <svt_opengl.h>

/**
 * Constructor
 */
svt_contextData::svt_contextData() :
    m_pCreated( NULL ),
    m_pDeleted( NULL )
{
    m_bDestroy = false;

    int iWinNum = svt_getWindowsNum();

    m_pCreated = new bool[iWinNum];
    m_pDeleted = new bool[iWinNum];

    for(int i=0;i<iWinNum;i++)
    {
        m_pCreated[i] = false;
        m_pDeleted[i] = false;
    }

    svt_addContextData(this);
}
svt_contextData::~svt_contextData()
{
    if (m_pCreated)
        delete[] m_pCreated;
    if (m_pDeleted)
        delete[] m_pDeleted;
}

/**
 * create
 */
void svt_contextData::create(int iWin)
{
    m_pCreated[iWin-1] = true;
}

/**
 * destroy
 */
void svt_contextData::destroy(int iWin)
{
    m_pDeleted[iWin-1] = true;
}

/**
 * created in all contexts?
 */
bool svt_contextData::isCreated()
{
    int iWinNum = svt_getWindowsNum();
    for(int i=0;i<iWinNum;i++)
        if (!m_pCreated[i])
            return false;

    return true;
}
/**
 * deleted in all contexts?
 */
bool svt_contextData::isDestroyed()
{
    int iWinNum = svt_getWindowsNum();
    for(int i=0;i<iWinNum;i++)
        if (!m_pDeleted[i])
            return false;

    return true;
}

/**
 * the context data should get desotryed
 */
void svt_contextData::destroyCD()
{
    svt_getContextSema().P();
    m_bDestroy = true;
    svt_getContextSema().V();
}

/**
 * should the contextData get destroyed?
 * \return true if the system should destory the cd
 */
bool svt_contextData::shouldDestroy()
{
    return m_bDestroy;
}
