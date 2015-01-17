/***************************************************************************
                          svt_system.cpp
			  --------------
    begin                : 12.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_system.h>
#include <svt_time.h>
#include <svt_semaphore.h>
// clib includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// c++ abi
#ifdef WIN32_MSVC
  #include <imagehlp.h>
#else
  #include <cxxabi.h>
#endif

#ifdef WIN32
#include <svt_init.h>
#include <svt_iostream.h>
#endif

char g_pName[256];

#ifdef WIN32
  #ifndef STATICQT
  extern "C" DLLEXPORT BOOL WINAPI DllMain( HANDLE hModule, DWORD dwFunction, LPVOID lpNot )
  {
    svt_registerCout( &std::cout );
    return true;
  };
  #endif
#endif

// forward declarations

extern void __svt_initTime( void );
extern void __svt_initWindow( void );
extern bool __svt_mainloopWindow( void );
extern void __svt_idleTest( void );

svt_semaphore g_cDrawingSema(1);


/**
 * initialize the svt_system module.
 */
DLLEXPORT void svt_system_init( void )
{
    __svt_initTime();
    __svt_initWindow();
};

/**
 * The main loop for the svt_system.
 * Attention: This function will only return if the program terminates!
 */
DLLEXPORT int svt_system_mainloop( void )
{
    while(true)
    {
        if (!__svt_mainloopWindow())
            return 1;

        __svt_idleTest();
    }
};

/**
 * Initiates a single screen and event update
 */
DLLEXPORT int svt_system_update( void )
{
    if (!__svt_mainloopWindow())
	return 1;

    __svt_idleTest();

    return 0;
}

/**
 * Get the main drawing semaphore. You can lock this semaphore if you want to execute code, which could change parameters of the drawing loop in svt.
 */
DLLEXPORT svt_semaphore* svt_getDrawingSemaphore()
{
    return &g_cDrawingSema;
};

/**
 * is the redraw thread running right now
 */
DLLEXPORT bool svt_getRunning()
{
    bool bNotRunning = svt_getDrawingSemaphore()->tryLock();

    if (bNotRunning)
        svt_getDrawingSemaphore()->V();

    return !bNotRunning;
}

/**
 * Execute a system command.
 */
DLLEXPORT int svt_system(char* pCommand)
{
    return system( pCommand );
};

/**
 * Get type name of object
 * \param oType type_info object usually coming from a typeid( pointer to object ) call.
 * \return const char*
 */
DLLEXPORT char* svt_getName(const char* pName)
{
    size_t iLength = 30;

#ifdef WIN32_MSVC
    char* pBuffer = (char*)malloc(iLength);
    UnDecorateSymbolName( pName, pBuffer, 30, UNDNAME_COMPLETE);
#else
    int iStatus;
    char* pBuffer = abi::__cxa_demangle( pName, NULL, &iLength, &iStatus);
#endif

    // copy only the first 20 bytes into an internal buffer
    unsigned int i;
    for(i=0; i<strlen(pBuffer) && i<20; i++)
        g_pName[i] = pBuffer[i];
    for(;i<=20;i++)
        g_pName[i] = ' ';
    g_pName[i++] = '>';
    g_pName[i++] = ' ';
    g_pName[i] = 0;

    free(pBuffer);

    return g_pName;
};

/**
 * Trim string to 30 chars
 */
DLLEXPORT char* svt_trimName(const char* pName)
{
    // copy only the first 20 bytes into an internal buffer
    char pBuf[21];
    unsigned int i;
    for(i=0; i<strlen(pName) && i<20; i++)
        pBuf[i] = pName[i];
    pBuf[i] = 0;

    sprintf( g_pName, "%-20s > ", pName );

    return g_pName;
};

/**
 * Get the content of an environment variable
 */
DLLEXPORT string svt_getEnv(string oEnv)
{
    return string( getenv( oEnv.c_str() ) );
};

/**
 * Get the homedirectory
 */
DLLEXPORT string svt_getHomeDir()
{

#ifdef WIN32

    char tmp[1024], homeEnv[2048];
    tmp[0] = '\0';
    string home;

    if ((GetEnvironmentVariable("HOME", tmp, sizeof(tmp)) == 0) )
        strcpy(tmp, "%HOMEDRIVE%%HOMEPATH%");

    if (tmp[0] != '\0' && ExpandEnvironmentStrings(tmp, homeEnv, sizeof(homeEnv)) != 0)
        home = homeEnv;

    return home;

#else

    return svt_getEnv("HOME");

#endif
};

#ifdef WIN32
vector< std::ostream* > g_aCoutStreams;
/**
 * Register a Cout object with SVT
 */
DLLEXPORT void svt_registerCout( std::ostream* pCoutStream )
{
    g_aCoutStreams.push_back( pCoutStream );
}
/**
 * Redirect Cout
 */
DLLEXPORT void svt_redirectCout( std::basic_streambuf<char>* pCout )
{
    unsigned int i;
    for(i=0; i<g_aCoutStreams.size(); i++)
        g_aCoutStreams[i]->rdbuf( pCout );
};
#endif
