/***************************************************************************
                          live_net_server.cpp
                          ------------------
    begin                : 11.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <live_net_server.h>
#include <svt_config.h>
#include <svt_init.h>

#include <stdlib.h>

int main()
{
    svt_config oConfig(".svtrc");
    svt_setConfig(&oConfig);

    while(1)
    {
        wait_for_connection();
        parseFunctions();
    }
}
