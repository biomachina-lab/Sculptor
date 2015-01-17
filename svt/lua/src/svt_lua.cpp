/***************************************************************************
                          svt_lua.cpp
                          -----------
    begin                : 10/16/2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_lua.h>
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_stlString.h>
#include <svt_exception.h>

#include <string.h>

#define LUAOUT cout << svt_trimName("svt_lua")

/**
 * Panic function
 */
int svt_luaPanic(lua_State *)
{
    LUAOUT << "Lua panic function called!" << endl;
    throw svt_userInterrupt();
    return 0;
};

/**
 * Constructor. Initializes the lua interpreter.
 */
svt_lua::svt_lua()
{
    m_pState = NULL;
    m_bRunning = false;
};

/**
 * Destructor. Will close lua interpreter.
 */
svt_lua::~svt_lua()
{
    lua_close(m_pState);
};

/**
 * Initialize interpreter and register all classes and functions. Typically needs to get overloaded.
 */
void svt_lua::init()
{
    if (m_pState)
        lua_close(m_pState);
    m_pState = lua_open();
    luaL_openlibs(m_pState);
    lua_atpanic(m_pState, svt_luaPanic);
};

/**
 * Load and execute a lua script file.
 * \param pFilename pointer to array of char with the lua script filename
 */
void svt_lua::loadScript( const char* pFilename )
{
    init();

    m_bRunning = true;
    if (luaL_loadfile(m_pState, pFilename) || lua_pcall(m_pState, 0, 0, 0))
        error("loadScript");
    m_bRunning = false;
};

/**
 * Run a script given as string
 * \param pScript pointer to array of char with the lua code
 */
void svt_lua::runScript( const char* pScript )
{
    try
    {
        init();

        m_bRunning = true;
        int iError = luaL_loadbuffer(m_pState, pScript, strlen(pScript), "line") || lua_pcall(m_pState, 0, 0, 0);
        m_bRunning = false;

        if (iError)
            error("runScript");
        return;

    } catch (svt_userInterrupt&) {

        m_bRunning = false;
        return;
    }
};

/**
 * Stop the interpreter.
 */
void svt_lua::stopScript()
{
    if (m_pState == NULL)
    {
        printf("DEBUG: No state here, cannot stop the interpreter - strange...\n");
        return;
    }

    m_bRunning = false;

    try
    {
        lua_pushstring( m_pState, "User interrupt!" );
        luaL_error(m_pState, "User Interrupt!");

    } catch (svt_userInterrupt&) {

        return;
    }
};

/**
 * Is the interpreter currently running?
 * \return true if the interpreter is running
 */
bool svt_lua::isRunning()
{
    if (m_pState == NULL)
        return false;

    return m_bRunning;
};

/**
 * Error message routine.
 * \param pFunction pointer to array of char with the function name where the error has occured
 */
void svt_lua::error( const char* pFunction )
{
    if (m_pState == NULL)
        return;

    svtout << "Error: " << pFunction << " - " << lua_tostring( m_pState, -1 ) << endl;
    // pop error message from the stack
    lua_pop(m_pState, 1);
};

/**
 * Get the content of a Lua variable
 * \param pVar pointer to array of char with the variable name
 */
double svt_lua::getNumber( const char* pVar )
{
    if (m_pState == NULL)
        return 0.0;

    lua_getglobal( m_pState, pVar );
    if ( lua_isnumber( m_pState, -1 ) )
        return lua_tonumber( m_pState,-1);
    else
        return 0.0;
};

/**
 * Get the content of a Lua variable
 * \param pVar pointer to array of char with the variable name
 */
int svt_lua::getInteger( const char* pVar )
{
    if (m_pState == NULL)
        return 0;

    lua_getglobal( m_pState, pVar );
    if (lua_isnumber( m_pState, -1) )
        return lua_tointeger( m_pState,-1);
    else
        return 0;
};

/**
 * Get the content of a Lua variable
 * \param pVar pointer to array of char with the variable name
 */
string svt_lua::getString( const char* pVar )
{
    if (m_pState == NULL)
        return string("");

    lua_getglobal( m_pState, pVar );
    if (lua_isstring( m_pState, -1) )
        return string(lua_tostring( m_pState,-1));
    else
        return string("");
};

/**
 * Get the lua state
 * \return pointer to the lua state of the interpreter
 */
lua_State* svt_lua::getState()
{
    return m_pState;
};
