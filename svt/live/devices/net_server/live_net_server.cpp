/***************************************************************************
                          live_net_server.cpp
                          ------------------
    begin                : 11.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <live_net_server.h>

#include <svt_iostream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <winsock.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// package
///////////////////////////////////////////////////////////////////////////////

#define MAX_CHN 256
#define MAX_DATA 1024
#define HEADER_SIZE 8

// the package data struct
typedef struct {
    UInt32 pid;          // package id
    UInt8 marker;        // package type (function call or answer)
    UInt8 fid;           // function id
    UInt16 chn;          // channel nr
    UInt16 size;         // size of data
    char data[MAX_DATA]; // data
} package;

package m_sPackage;

typedef struct {
    UInt16 ret;          // return value of devOpen
    UInt16 chn;          // number of channels
} initanswer;

///////////////////////////////////////////////////////////////////////////////
// variables
///////////////////////////////////////////////////////////////////////////////

// tcp/ip stuff

int m_iServerPort = 8000;
struct sockaddr_in m_sServerAddr;
#ifndef linux
int m_iServerAddrLen = sizeof(m_sServerAddr);
#else
socklen_t m_iServerAddrLen = sizeof(m_sServerAddr);
#endif
int m_iSocketFd;
int m_iSocketConnFd;

// internal vars

UInt32 m_iPid;        // packet id
UInt8 m_iFunc;
UInt16 m_iChannel;
UInt16 m_iNumChn = 0; // number of channels of the device
live_device* m_pLiveDev;
live_channel m_pChannels[MAX_CHN];

// defines

#define LIVE_NET_FUNC 1000
#define LIVE_NET_ANSWER 1001

///////////////////////////////////////////////////////////////////////////////
// helper functions - endian
///////////////////////////////////////////////////////////////////////////////

#define swap(a, b) (temp)=*(a);\
                   *(a)=*(b);\
                   *(b)=(temp);

// returns true of bigendian
bool __bigEndianMachine()
{
    short int swaptest=0x0102;
    short int p = htons(swaptest);

    return (p == swaptest);
}

void SwapInt16(Int16 *value)
{
    char *cp, temp;

    cp=(char *)value;
    swap(cp, cp+1);
}

void SwapInt32(Int32 *value)
{
    char *cp, temp;

    cp=(char *)value;
    swap(cp, cp+3);
    cp++;
    swap(cp, cp+1);
}

void SwapUInt16(UInt16 *value)
{
    char *cp, temp;

    cp=(char *)value;
    swap(cp, cp+1);
}

void SwapUInt32(UInt32 *value)
{
    char *cp, temp;

    cp=(char *)value;
    swap(cp, cp+3);
    cp++;
    swap(cp, cp+1);
}

void SwapReal32(Real32 *value)
{
    char *cp, temp;

    cp=(char *)value;
    swap(cp, cp+3);
    cp++;
    swap(cp, cp+1);
}

void SwapMat(Real32 *value)
{
    for(int i=0;i<16;i++)
        SwapReal32(&value[i]);
}

void __swapHeader()
{
    SwapUInt32(&m_sPackage.pid);
    SwapUInt16(&m_sPackage.chn);
    SwapUInt16(&m_sPackage.size);
}

///////////////////////////////////////////////////////////////////////////////
// helper functions
///////////////////////////////////////////////////////////////////////////////

void __sendAnswer(void* data, UInt16 size)
{
    m_sPackage.marker = (UInt8)LIVE_NET_ANSWER;
    m_sPackage.pid = m_iPid;
    m_sPackage.size = size;

    memcpy(m_sPackage.data, data, size);

    if (!__bigEndianMachine())
        __swapHeader();

    send(m_iSocketConnFd, (char*)&m_sPackage, sizeof(package),0);
}

void __sendChannels()
{
    m_sPackage.marker = (UInt8)LIVE_NET_ANSWER;
    m_sPackage.pid = m_iPid;
    m_sPackage.size = m_iNumChn*sizeof(live_channel);

    live_channel* chn;
    live_channel  swappedChn;

    for(int i=0;i<m_iNumChn;i++)
    {
        chn = live_getChannel(m_pLiveDev, i+1);
        memcpy(&swappedChn, chn, sizeof(live_channel));

        if (!__bigEndianMachine())
            switch(swappedChn.type)
            {
            case LIVE_CHN_INT:
                SwapInt16(&swappedChn.data.ival);
                break;
            case LIVE_CHN_FLOAT:
                SwapReal32(&swappedChn.data.fval);
                break;
            case LIVE_CHN_MATRIX:
                SwapMat(swappedChn.data.mval);
                break;
            }

        memcpy(m_sPackage.data+(i*sizeof(live_channel)), &swappedChn, sizeof(live_channel));
    }

    if (!__bigEndianMachine())
        __swapHeader();

    send(m_iSocketConnFd, (char*)&m_sPackage, sizeof(package), 0);
}

void __receiveChannels()
{
    live_channel* chn;

    for(int i=0;i<m_iNumChn;i++)
    {
        memcpy(&m_pChannels[i], m_sPackage.data+(i*sizeof(live_channel)), sizeof(live_channel));

        if (!__bigEndianMachine())
        {
            switch(m_pChannels[i].type)
            {
            case LIVE_CHN_INT:
                SwapInt16(&m_pChannels[i].data.ival);
                break;
            case LIVE_CHN_FLOAT:
                SwapReal32(&m_pChannels[i].data.fval);
                break;
            case LIVE_CHN_MATRIX:
                SwapMat(m_pChannels[i].data.mval);
                break;
            }
        }

        if (m_pChannels[i].direction == LIVE_CHN_OUTPUT && m_pChannels[i].type != LIVE_CHN_UNKNOWN)
        {
            chn = live_getChannel(m_pLiveDev, i+1);
            memcpy(chn, &m_pChannels[i], sizeof(live_channel));
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// device function calls
///////////////////////////////////////////////////////////////////////////////

void devOpen()
{
    cout << "live_net_server: devOpen called!" << endl;

    char devName[512];

    memcpy(devName, &m_sPackage.data, m_sPackage.size);

    cout << "live_net_server: opening live device: " << devName << endl;

    m_pLiveDev = live_openDevice(devName);

    if (m_pLiveDev == NULL)
    {
        printf("live_net_server: couldnt open device!\n");
        exit(1);
    }

    cout << "live_net_server: opened live device:" << devName << endl;

    // prepare and send answer

    initanswer ans;

    ans.ret = 1;
    ans.chn = live_getNumChannels(m_pLiveDev);
    m_iNumChn = ans.chn;

    if (!__bigEndianMachine())
    {
        SwapUInt16(&ans.ret);
        SwapUInt16(&ans.chn);
    }

    __sendAnswer(&ans, sizeof(initanswer));
}

void devUpdate()
{
    if (m_pLiveDev != NULL)
        live_updateData(m_pLiveDev);
    else
        exit(1);

    __receiveChannels();

    __sendChannels();
}

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

void wait_for_connection()
{
#ifdef WIN32
    WSADATA wsadata;
    if (WSAStartup( MAKEWORD( 1,1 ), &wsadata ) != 0)
    {
        cout << "live_net_client: couldnt initialize winsockets" << endl;
        exit(1);
    }
#endif


    m_iSocketFd = socket(PF_INET, SOCK_STREAM, 0);

    int i=1;
    setsockopt(m_iSocketFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&i ,sizeof(i));

    m_sServerAddr.sin_family = AF_INET;
    m_sServerAddr.sin_port = htons(m_iServerPort);
    memset(&m_sServerAddr.sin_addr, 0, sizeof(m_sServerAddr.sin_addr));

    if (bind(m_iSocketFd, (struct sockaddr *)&m_sServerAddr, sizeof(m_sServerAddr)))
    {
        printf("live_net_server: error in bind function.\n");
        exit(1);
    }

    if (listen(m_iSocketFd, 5))
    {
        printf("live_net_server: error in listen function.\n");
        exit(1);
    }

    if (__bigEndianMachine())
        cout << "live_net_server: I am working on a big endian machine, so no changing of the endianess is necessary." << endl;
    else
        cout << "live_net_server: I am working on a little endian machine, so changing of the endianess is necessary." << endl;

    printf("live_net_server: waiting for incoming call...\n");

    if ( (m_iSocketConnFd = accept(m_iSocketFd, (struct sockaddr *)&m_sServerAddr, &m_iServerAddrLen)) >=0)
    {
        printf("live_net_server: ok, incoming call detected.\n");
    }
}

void closeConnection()
{
    cout << "live_net_server: closing connection." << endl;

#ifndef WIN32
    close(m_iSocketFd);
    close(m_iSocketConnFd);
#else
    closesocket(m_iSocketFd);
    closesocket(m_iSocketConnFd);
#endif

#ifdef WIN32
    WSACleanup();
#endif

}

void parseFunctions()
{
    while(m_iFunc != devClose_ID)
    {
        // read package
        recv(m_iSocketConnFd, (char*)&m_sPackage, sizeof(package), 0);

        if (!__bigEndianMachine())
            __swapHeader();

        //cout << "pid: " << m_sPackage.pid << endl;

        // check if this is a function call
        if (m_sPackage.marker != (UInt8)LIVE_NET_FUNC)
        {
            cout << "live_net_server: no function marker found (" << m_sPackage.marker << "). exiting. " << endl;
            exit(1);
        }

        // set some global vars
        m_iPid = m_sPackage.pid;
        m_iFunc = m_sPackage.fid;
        m_iChannel = m_sPackage.chn;

        // ok, now lets make the function calls
        switch (m_iFunc)
        {
        case devOpen_ID:
            devOpen();
            break;
        case devClose_ID:
            closeConnection();
            break;
        case devUpdate_ID:
            devUpdate();
            break;
        }
    }

    cout << "exiting." << endl;
}
