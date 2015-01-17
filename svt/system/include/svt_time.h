/***************************************************************************
                          svt_time.h  -  description
                             -------------------
    begin                : 12.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

/// return the time of day in millisecond (milliseconds since Jan. 1, 1970)
DLLEXPORT unsigned long svt_getToD( void );

/// return the elapsed time since starting the program in milliseconds
DLLEXPORT int svt_getElapsedTime( void );

/// sleep for the given amount of milliseconds
DLLEXPORT void svt_sleep(unsigned uiMilliSeconds);

/**
 * The the current day of the month (1-31).
 */
DLLEXPORT int svt_getDay();

/**
 * The the current month (1-12).
 */
DLLEXPORT int svt_getMonth();

/**
 * The the current year.
 */
DLLEXPORT int svt_getYear();
