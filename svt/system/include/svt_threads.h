/***************************************************************************
                          svt_threads.h
			  -------------
    begin                : 07.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_THREADS_H
#define SVT_THREADS_H

enum
{
    SVT_THREAD_PRIORITY_NORMAL,
    SVT_THREAD_PRIORITY_HIGH,
    SVT_THREAD_PRIORITY_LOW
};

/**
 * Create a seperate thread with the function func
 * \param pFunc pointer to the function which should be executed in the seperate thread
 * \param pArg  pointer to the arguments for the function
 * \param iPriority priority of the thread (e.g. SVT_THREAD_PRIORITY_HIGH)
 */
DLLEXPORT void svt_createThread(void* (*pFunc)(void*), void* pArg, int iPriority =SVT_THREAD_PRIORITY_NORMAL);

/**
 * Terminate thread. Has to be called by the thread itself!
 */
DLLEXPORT void svt_terminateThread();

#endif
