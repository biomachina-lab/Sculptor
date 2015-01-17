/***************************************************************************
                          svt_time.cpp  -  description
                             -------------------
    begin                : 12.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_system.h>
#include <svt_time.h>

// system depend includes
#ifdef TRUE_WIN32
#  include <sys/timeb.h>
#else
#  include <sys/time.h>
#  include <unistd.h>
#  include <math.h>
#endif

#include <svt_iostream.h>

// correct definition of gettimeofday
#if defined(SVR4) && !defined(sun)
#define GETTIMEOFDAY(x) gettimeofday(x)
#else
#define GETTIMEOFDAY(x) gettimeofday(x,NULL)
#endif

static long beginning;
static int beenhere = 0;

///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////

DLLEXPORT void __svt_initTime( void )
{
    if (!beenhere)
    {
        beenhere = 1;
	beginning = svt_getToD();
    }
};

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

#ifdef TRUE_WIN32
DLLEXPORT unsigned long svt_getToD( void )
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    long msec = st.wMilliseconds;

    return GetTickCount();
};
#else
unsigned long svt_getToD( void )
{
    struct timeval now;
    GETTIMEOFDAY( &now );

    return (now.tv_sec * 1000) + (now.tv_usec / 1000);
};
#endif

DLLEXPORT int svt_getElapsedTime( void )
{ 
    int elap_time = svt_getToD() - beginning;

    return elap_time;
};

DLLEXPORT void svt_sleep(unsigned uiMilliSeconds)
{
#ifdef TRUE_WIN32
  Sleep(uiMilliSeconds);
#else
  usleep(uiMilliSeconds*1000);
#endif
  
  return;
}

/**
 * The the current day of the week (Monday = 0)
 */
DLLEXPORT int svt_getDay()
{
    time_t my_time;
    struct tm *ts;

    my_time = time(NULL);
    ts = localtime(&my_time);

    return ts->tm_mday;
}

/**
 * The the current month
 */
DLLEXPORT int svt_getMonth()
{
    time_t my_time;
    struct tm *ts;

    my_time = time(NULL);
    ts = localtime(&my_time);

    return ts->tm_mon + 1;
}

/**
 * The the current year
 */
DLLEXPORT int svt_getYear()
{
    time_t my_time;
    struct tm *ts;

    my_time = time(NULL);
    ts = localtime(&my_time);

    return 1900 + ts->tm_year;
}
