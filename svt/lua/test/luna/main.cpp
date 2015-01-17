/***************************************************************************
                          loadScript (test program)
                          -------------------------
    begin                : 10/16/2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_lua.h>
#include <svt_iostream.h>

#define LUA cout << svt_trimName("lua_test")

///////////////////////////////////////////////////////////////////////////////

/**
 * Hello world-like example for a C++ binding to Lua
 */
class svt_helloWorld
{
public:
    svt_helloWorld(lua_State *)
    {
        printf("Constructor of svt_helloWorld is called!\n");
    }

    int hello(lua_State *)
    {
        printf("Hello function is called in svt_helloWorld!\n");
        return 0;
    }

    ~svt_helloWorld() {
        printf("Destructor of svt_helloWorld is called!\n");
    }

    static const char className[];
    static const svt_luna<svt_helloWorld>::RegType methods[];
};

const char svt_helloWorld::className[] = "svt_helloWorld";
const svt_luna<svt_helloWorld>::RegType svt_helloWorld::methods[] =
{
    { "hello", &svt_helloWorld::hello },
    { 0 }
};

///////////////////////////////////////////////////////////////////////////////

int main()
{
    LUA << "Opening new lua interpreter" << endl;
    svt_lua oLua;

    // register our fancy hello world class with the lua interpreter
    svt_luna<svt_helloWorld>::Register( oLua.getState() );

    // and now lets test it by executing some lua code...
    const char* pScript = "     \n\
        \n\
        print('Instanciating svt_helloWorld object:!')   \n\
        \n\
        local helloWorld = svt_helloWorld()\n\
        \n\
        print('Now a function call:')   \n\
        \n\
        helloWorld:hello()\n\
    ";

    oLua.runScript( pScript );

    return 0;
};
