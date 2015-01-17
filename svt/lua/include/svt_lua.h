/***************************************************************************
                          svt_lua.h
                          ---------
    begin                : 10/16/2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_LUA_H
#define __SVT_LUA_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <svt_stlString.h>

// QT3FIXME #define method(class, name) {#name, &class::name}

/**
 * Lua interpreter class.
 * This class encapsulates the lua interpreter. It will create a new interpreter object and will execute code. One can query the content of Lua variables from C++.
 *
 * \author Stefan Birmanns
 */
class svt_lua
{
protected:

    lua_State* m_pState;
    bool m_bRunning;

public:

    /**
     * Constructor. Initializes the lua interpreter.
     */
    svt_lua();

    /**
     * Destructor. Will close lua interpreter.
     */
    virtual ~svt_lua();

    /**
     * Initialize interpreter and register all classes and functions. Typically needs to get overloaded.
     */
    virtual void init();

    /**
     * Load and execute a lua script file.
     * \param pFilename pointer to array of char with the lua script filename
     */
    void loadScript( const char* pFilename );

    /**
     * Run a script given as string
     * \param pScript pointer to array of char with the lua code
     */
    void runScript( const char* pScript );

    /**
     * Stop the interpreter.
     */
    void stopScript();

    /**
     * Is the interpreter currently running?
     * \return true if the interpreter is running
     */
    bool isRunning();

    /**
     * Error message routine.
     * \param pFunction pointer to array of char with the function name where the error has occured
     */
    void error( const char* pFunction );

    /**
     * Get the content of a Lua variable
     * \param pVar pointer to array of char with the variable name
     */
    double getNumber( const char* pVar );

    /**
     * Get the content of a Lua variable
     * \param pVar pointer to array of char with the variable name
     */
    int getInteger( const char* pVar );

    /**
     * Get the content of a Lua variable
     * \param pVar pointer to array of char with the variable name
     */
    string getString( const char* pVar );

    /**
     * Get the lua state
     * \return pointer to the lua state of the interpreter
     */
    lua_State* getState();
};

/**
 * Template class with which one can register a C++ class in Lua
 */
template<class T> class svt_luna
{
    typedef struct { T *pT; } userdataType;

public:

    typedef int (T::*mfp)(lua_State *L);
    typedef struct { const char *name; mfp mfunc; } RegType;

    /**
     * Register a class T with the lua interpreter
     */
    static void Register(lua_State *L)
    {
        lua_newtable(L);
        int methods = lua_gettop(L);

        luaL_newmetatable(L, T::className);
        int metatable = lua_gettop(L);

        // store method table in globals so that
        // scripts can add functions written in Lua.
        lua_pushstring(L, T::className);
        lua_pushvalue(L, methods);
        lua_settable(L, LUA_GLOBALSINDEX);

        lua_pushliteral(L, "__metatable");
        lua_pushvalue(L, methods);
        lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

        lua_pushliteral(L, "__index");
        lua_pushvalue(L, methods);
        lua_settable(L, metatable);

        lua_pushliteral(L, "__tostring");
        lua_pushcfunction(L, tostring_T);
        lua_settable(L, metatable);

        lua_pushliteral(L, "__gc");
        lua_pushcfunction(L, gc_T);
        lua_settable(L, metatable);

        lua_newtable(L);                // mt for method table
        int mt = lua_gettop(L);
        lua_pushliteral(L, "__call");
        lua_pushcfunction(L, new_T);
        lua_pushliteral(L, "new");
        lua_pushvalue(L, -2);           // dup new_T function
        lua_settable(L, methods);       // add new_T to method table
        lua_settable(L, mt);            // mt.__call = new_T
        lua_setmetatable(L, methods);

        // fill method table with methods from class T
        for (const RegType *l = T::methods; l->name; l++)
        {
            lua_pushstring(L, l->name);
            lua_pushlightuserdata(L, (void*)l);
            lua_pushcclosure(L, thunk, 1);
            lua_settable(L, methods);
        }

        lua_pop(L, 2);  // drop metatable and method table
    }

    /**
     * get userdata from Lua stack and return pointer to T object
     */
    static T *luaCheck(lua_State *L, int narg)
    {
        userdataType *ud = static_cast<userdataType*>(luaL_checkudata(L, narg, T::className));
        if(!ud)
            luaL_typerror(L, narg, T::className);

        return ud->pT;  // pointer to T object
    }

    /**
     * check if there is really a userdata element of the given type on the stack
     */
    static bool luaIsType(lua_State *L, int narg)
    {
        void* p = lua_touserdata( L, narg );

        if (p != NULL) /* value is a userdata? */
        {
            if (lua_getmetatable(L, narg)) /* does it have a metatable? */
            {
                lua_getfield(L, LUA_REGISTRYINDEX, T::className);  /* get correct metatable */
                if (lua_rawequal(L, -1, -2)) /* does it have the correct mt? */
                {
                    lua_pop(L, 2);  /* remove both metatables */
                    return true;
                }
                lua_pop(L, 1);  /* remove both metatables */
            }
            lua_pop(L, 1);  /* remove both metatables */
        }

        return false;
    };

    /**
     * Push a new T object onto the Lua stack a userdata containing a pointer to T object
     */
    static int pushToLua(T* obj, lua_State *L)
    {
        userdataType *ud = static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
        ud->pT = obj;  // store pointer to object in userdata
        luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
        lua_setmetatable(L, -2);
        return 1;  // userdata containing pointer to T object
    }

private:

    // hide default constructor
    svt_luna();

    /**
     * Member function dispatcher
     */
    static int thunk(lua_State *L)
    {
        // stack has userdata, followed by method args
        T *obj = luaCheck(L, 1);  // get 'self', or if you prefer, 'this'
        lua_remove(L, 1);  // remove self so member function args start at index 1
        // get member function from upvalue
        RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
        return (obj->*(l->mfunc))(L);  // call member function
    }

    /**
     * Create a new T object and
     * push onto the Lua stack a userdata containing a pointer to T object
     */
    static int new_T(lua_State *L)
    {
        lua_remove(L, 1);   // use classname:new(), instead of classname.new()
        T *obj = new T(L);  // call constructor for T objects
        userdataType *ud =
            static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
        ud->pT = obj;  // store pointer to object in userdata
        luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
        lua_setmetatable(L, -2);
        return 1;  // userdata containing pointer to T object
    }

    /**
     * garbage collection metamethod
     */
    static int gc_T(lua_State *L)
    {
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        delete obj;  // call destructor for T objects
        return 0;
    }

    static int tostring_T (lua_State *L)
    {
        char buff[32];
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        sprintf(buff, "%p", obj);
        lua_pushfstring(L, "%s (%s)", T::className, buff);
        return 1;
    }
};

#endif
