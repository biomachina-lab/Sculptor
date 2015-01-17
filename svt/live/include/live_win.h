/***************************************************************************
                          live.h
                          ------------------
    begin                : 11.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_WIN_H
#define LIVE_WIN_H

#ifdef WIN32
	#include <windows.h>
	#ifndef DLLIMPORT
		#define DLLIMPORT   __declspec( dllimport )
	#endif
	#ifndef DLLEXPORT
		#define DLLEXPORT   __declspec( dllexport )
        #endif
#else
        #define DLLEXPORT
#endif

#endif
