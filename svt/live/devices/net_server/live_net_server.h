/***************************************************************************
                          live_net_server.h
                          ------------------
    begin                : 11.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#define devOpen_ID 1
#define devClose_ID 2
#define devUpdate_ID 3

#define chnNum_ID 50
#define chnType_ID 51
#define chnDir_ID 52

#define chnGetDataINT_ID 100
#define chnGetDataCHAR_ID 101
#define chnGetDataFLOAT_ID 102
#define chnGetDataMAT_ID 103

#define chnSetDataINT_ID 200
#define chnSetDataCHAR_ID 201
#define chnSetDataFLOAT_ID 202
#define chnSetDataMAT_ID 203

void wait_for_connection();
void parseFunctions();
void close_connection();
