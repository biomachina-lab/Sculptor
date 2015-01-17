/***************************************************************************
                          svt_threads
                          -----------
    begin                : 07.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_system.h>
#include <svt_threads.h>
// clib includes
#include <svt_iostream.h>

#ifdef SVT_SYSTEM_QT

#include <qthread.h>
#include <qapplication.h>

#ifdef LINUX
#include <unistd.h>
#endif

class svt_thread : public QThread
{
    void* (*m_pFunc)(void*);
    void* m_pArg;

public:

    void setFunc( void* (*pFunc)(void*), void* pArg );

    virtual void run();
};

void svt_thread::run()
{
    (m_pFunc)(m_pArg);
};

void svt_thread::setFunc( void* (*pFunc)(void*), void* pArg )
{
    m_pFunc = pFunc;
    m_pArg = pArg;
};

DLLEXPORT void svt_createThread(void* (*func)(void*), void* arg, int)
{
    svt_thread* pThread = new svt_thread;

    pThread->setFunc( func, arg );
    pThread->start();
}

/**
 * Terminate thread. Has to be called by the thread itself!
 */
DLLEXPORT void svt_terminateThread()
{
    //QT3FIXME QThread::exit();
};

#else

#ifdef WIN32
/**
 * Create a seperate thread with the function func
 * \param pFunc pointer to the function which should be executed in the seperate thread
 * \param pArg  pointer to the arguments for the function
 * \param iPriority priority of the thread (e.g. SVT_THREAD_PRIORITY_HIGH)
 */
DLLEXPORT void svt_createThread(void* (*func)(void*), void* arg, int iPriority)
{
    unsigned long nThreadID;

    HANDLE handle = CreateThread(0,0,(LPTHREAD_START_ROUTINE)func, (LPVOID)arg, 0, &nThreadID);

    if (handle)
        switch(iPriority)
        {
        case SVT_THREAD_PRIORITY_NORMAL:
            break;
        case SVT_THREAD_PRIORITY_HIGH:
            //SetPriorityClass(handle, HIGH_PRIORITY_CLASS);
            SetThreadPriority(handle, THREAD_PRIORITY_ABOVE_NORMAL);
            break;
        case SVT_THREAD_PRIORITY_LOW:
            SetThreadPriority(handle, THREAD_PRIORITY_BELOW_NORMAL);
            break;
        default:
            break;
        }
};

/**
 * Terminate thread. Has to be called by the thread itself!
 */
DLLEXPORT void svt_terminateThread()
{

};

#endif

#ifdef LINUX

#include <svt_threads.h>
#include <pthread.h>
#include <unistd.h>

pthread_t recalc_it;

/**
 * Create a seperate thread with the function func
 * \param pFunc pointer to the function which should be executed in the seperate thread
 * \param pArg  pointer to the arguments for the function
 * \param iPriority priority of the thread (e.g. SVT_THREAD_PRIORITY_HIGH)
 */
void svt_createThread(void* (*func)(void*), void* arg, int)
{
    pthread_create(&recalc_it, NULL, func, arg);
    pthread_detach(recalc_it);
};

/**
 * Terminate thread. Has to be called by the thread itself!
 */
DLLEXPORT void svt_terminateThread()
{
    pthread_exit(NULL);
};

#endif

#endif
