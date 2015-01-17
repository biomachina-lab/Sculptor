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

int main()
{
    LUA << "Opening new lua interpreter" << endl;
    svt_lua oLua;
    LUA << "Loading and executing script file" << endl;
    oLua.loadScript("loadScript.lua");

    LUA << "Querying global variables: " << endl;
    LUA << "  test_var_a = " << oLua.getNumber("test_var_a") << endl;
    LUA << "  test_var_b = " << oLua.getNumber("test_var_b") << endl;

    return 0;
};
