/***************************************************************************
                          svt_system.h  -  description
                             -------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SYSTEM_H
#define SVT_SYSTEM_H

#include <svt_basics.h>
#include <svt_semaphore.h>
#include <svt_stlString.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <svt_iostream.h>

#define svtout cout << svt_getName( typeid(this).name() )

/**
 * initialize the svt_system module.
 */
DLLEXPORT void svt_system_init( void );

/**
 * The main loop for the svt_system.
 * Attention: This function will only return if the program terminates!
 */
DLLEXPORT int svt_system_mainloop( void );
/**
 * Initiates a single screen and event update
 */
DLLEXPORT int svt_system_update( void );

/**
 * Get the main drawing semaphore. You can lock this semaphore if you want to execute code, which could change parameters of the drawing loop in svt.
 */
DLLEXPORT svt_semaphore* svt_getDrawingSemaphore();
/**
 * is the redraw thread running right now
 */
DLLEXPORT bool svt_getRunning();

/**
 * Execute a system command.
 */
DLLEXPORT int svt_system(char* pCommand);

/**
 * Get type name of object
 * \param oType type_info object usually coming from a typeid( pointer to object ) call.
 * \return const char*
 */
DLLEXPORT char* svt_getName(const char* pName);

/**
 * Trim string to 30 chars
 */
DLLEXPORT char* svt_trimName(const char* pName);

/**
 * Get the content of an environment variable
 */
DLLEXPORT string svt_getEnv(string oEnv);

/**
 * Get the homedirectory
 */
DLLEXPORT string svt_getHomeDir();

#ifdef WIN32
/**
 * Register a Cout object with SVT
 */
DLLEXPORT void svt_registerCout( std::ostream* pCoutStream );
/**
 * Redirect Cout
 */
DLLEXPORT void svt_redirectCout( std::basic_streambuf<char>* pCout );
#endif

#endif
