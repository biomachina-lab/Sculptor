/***************************************************************************
                          svt_semaphore.cpp  -  description
                             -------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_system.h>
#include <svt_semaphore.h>

#include <stdlib.h>
#include <stdio.h>
#include <svt_iostream.h>

extern void __svt_fatalError(const char*);

#ifndef WIN32

svt_semaphore::svt_semaphore(int)
{
    if (pthread_mutex_init(&mux, NULL) < 0)
        __svt_fatalError("svt_system: cannot initialize svt_semaphore.mux!");
}
svt_semaphore::~svt_semaphore()
{
}

void svt_semaphore::P()
{
    if (pthread_mutex_lock(&mux) < 0)
        __svt_fatalError("svt_system: cannot lock svt_semaphore.mux!");
}

void svt_semaphore::V()
{
    if (pthread_mutex_unlock(&mux) < 0)
        __svt_fatalError("svt_system: cannot unlock svt_semaphore.mux!");
}

bool svt_semaphore::tryLock()
{
    if (pthread_mutex_trylock(&mux) == 0)
        return true;
    else
        return false;
}

#else

svt_semaphore::svt_semaphore(int i)
{
    mux = CreateSemaphore(NULL, i, i, NULL);
    if (mux == NULL)
        __svt_fatalError("svt_system: cannot initialize svt_semaphore.mux!");
}
svt_semaphore::~svt_semaphore()
{
    CloseHandle(mux);
}

void svt_semaphore::P()
{
    if (WaitForSingleObject(mux, INFINITE) != WAIT_OBJECT_0)
        __svt_fatalError("svt_system: cannot lock svt_semaphore.mux!");
}

void svt_semaphore::V()
{
    if (!ReleaseSemaphore(mux,1,NULL))
        __svt_fatalError("svt_system: cannot unlock svt_semaphore.mux!");
}

bool svt_semaphore::tryLock()
{
    return (WaitForSingleObject(mux, 0) == WAIT_OBJECT_0);
}

#endif
