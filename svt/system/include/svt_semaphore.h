/***************************************************************************
                          svt_semaphore.h  -  description
                             -------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SEMAPHORE_H
#define SVT_SEMAPHORE_H

#ifndef WIN32
#include <pthread.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

/** a class implementing semaphore. Semaphores are kind of mutexes, but they solve the problem of the lost-signals.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_semaphore
{
private:
    int init;
    int s;
    int del;
#ifdef WIN32
    HANDLE mux;
#endif

#ifndef WIN32
    pthread_mutex_t mux;
    pthread_cond_t pos;
#endif
public:
    svt_semaphore(int i = 1);
    ~svt_semaphore();

public:
    void P();
    void V();
    bool tryLock();
};

#endif
