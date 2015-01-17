/***************************************************************************
                          runScript (test program)
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

    const char* pScript = "     \n\
        print('Hello World!')   \n\
        print('This is super!') \n\
    ";

    LUA << "Executing script " << endl;
    LUA << pScript << endl;
    oLua.runScript( pScript );

    return 0;
};
