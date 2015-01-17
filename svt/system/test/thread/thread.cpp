#include <svt_system.h>
#include <svt_threads.h>
#include <svt_semaphore.h>
#include <svt_time.h>

#include <svt_iostream.h>

#include <stdlib.h>

svt_semaphore oSema;

void* thread(void* pData)
{
    int iClock;
    int iThreadNum = *(int*)(pData);

    while(true)
    {
        iClock = svt_getElapsedTime();
        while(svt_getElapsedTime() - iClock < 500);
        cout << "thread " << iThreadNum << " running..." << endl;
        if ((int)(5.0f*rand()/(RAND_MAX+1.0f)) == 0 )
        {
            oSema.P();
            cout << "thread " << iThreadNum << " blocking!" << endl;
            while(svt_getElapsedTime() - iClock < 500);
            cout << "thread " << iThreadNum << " end blocking." << endl;
            oSema.V();
        }
    }

    return NULL;
};

int main()
{
    for (int i=0;i<20;i++)
    {
        int* iCount = new int;
        *iCount = i;
        if (i % 5 == 0)
            svt_createThread(&thread,iCount,SVT_THREAD_PRIORITY_HIGH);
        else
            svt_createThread(&thread,iCount,SVT_THREAD_PRIORITY_NORMAL);
    }

    while(true);

    return 0;
};
