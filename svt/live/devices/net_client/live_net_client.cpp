/***************************************************************************
                          live_net_client.cpp
                          ------------------
    begin                : 10.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <live_net_client.h>
#include <svt_init.h>

#ifndef WIN32
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <winsock.h>
#endif

#include <svt_iostream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
char m_aData[MAX_DATA];

typedef struct {
    UInt16 ret;          // return value of devOpen
    UInt16 chn;          // number of channels
} initanswer;

///////////////////////////////////////////////////////////////////////////////
// variables
///////////////////////////////////////////////////////////////////////////////

const char* m_pServerAddr;
const char* m_pDevice;
#ifdef WIN32
SOCKET m_iSocketFd;
#else
int m_iSocketFd;
#endif
int m_iClientPort;
int m_iServerPort;
UInt32 m_iPid; // packet id

live_channel m_pChannels[MAX_CHN];
Int16 m_iNumChn = -1;

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

void __initChannels()
{
    for (int i=0;i< MAX_CHN; i++)
    {
        m_pChannels[i].type = LIVE_CHN_UNKNOWN;
        m_pChannels[i].direction = LIVE_CHN_INPUT;
    }
}

void __connect()
{
    struct sockaddr_in servAddr;

#ifdef WIN32
    WSADATA wsadata;
    if (WSAStartup( MAKEWORD( 1,1 ), &wsadata ) != 0)
    {
        cout << "live_net_client: couldnt initialize winsockets" << endl;
        exit(1);
    }
#endif

    m_iSocketFd = socket(PF_INET, SOCK_STREAM, 0);
#ifdef WIN32
    if (m_iSocketFd == INVALID_SOCKET)
    {
        cout << "WSAGetLastError(): "  << WSAGetLastError() << endl;
        cout << "live_net_client: error during socket() call." << endl;
        exit(1);
    }
#else
    if (m_iSocketFd < 0)
    {
        cout << "live_net_client: error during socket() call." << endl;
        exit(1);
    }
#endif

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(m_iServerPort);
    servAddr.sin_addr.s_addr = inet_addr(m_pServerAddr);
    memset(&(servAddr.sin_zero),0, 8);

    if (connect(m_iSocketFd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr)))
    {
        cout << "live_net_client: error during connect() call. perhaps the live_net_server is not running..." << endl;
        exit(1);
    }

    if (__bigEndianMachine())
        cout << "live_net_client: I am working on a big endian machine, so no changing of the endianess is necessary." << endl;
    else
        cout << "live_net_client: I am working on a little endian machine, so changing of the endianess is necessary." << endl;
}

void __disconnect()
{
#ifdef WIN32
    WSACleanup();
#endif
}

UInt32 __callFunc(UInt8 id, UInt16 chn, void* data, UInt16 size)
{
    m_sPackage.pid = m_iPid;
    m_sPackage.marker = (UInt8)LIVE_NET_FUNC;
    m_sPackage.fid = id;
    m_sPackage.chn = chn;
    m_sPackage.size = size;

    memcpy(m_sPackage.data, data, size);

    if (!__bigEndianMachine())
        __swapHeader();

    send(m_iSocketFd, (const char*)&m_sPackage, sizeof(package), 0);

    return m_iPid++;
}

void __receiveAnswer(UInt32 pid, void* buf, UInt16 size)
{
    recv(m_iSocketFd, (char*)&m_sPackage, sizeof(package), 0);

    if (!__bigEndianMachine())
        __swapHeader();

    if (m_sPackage.marker != (UInt8)LIVE_NET_ANSWER)
    {
        cout << "live_net_client: received " << m_sPackage.marker << " instead of answer marker! Trying again..." << endl;
        exit(1);
    }

    if (m_sPackage.pid != pid)
    {
        cout << "live_net_client: received answer for another func call (" << m_sPackage.pid << "). Fatal error." << endl;
        exit(1);
    }

    memcpy(buf, m_sPackage.data, size);
}

void __receiveChannels(UInt32 pid)
{
    recv(m_iSocketFd, (char*)&m_sPackage, sizeof(package),0);

    if (!__bigEndianMachine())
        __swapHeader();

    if (m_sPackage.marker != (UInt8)LIVE_NET_ANSWER)
    {
        cout << "live_net_client (receive channels): received " << m_sPackage.marker << " instead of answer marker! Trying again..." << endl;
        exit(1);
    }

    if (m_sPackage.pid != pid)
    {
        cout << "live_net_client: received answer for another func call (" << m_sPackage.pid << ")." << endl;
        //exit(1);
    }

    for(int i=0;i<m_iNumChn;i++)
    {
        if (m_pChannels[i].direction == LIVE_CHN_INPUT)
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
        }
    }
}

void __sendChannels()
{
    live_channel  swappedChn;

    for(int i=0;i<m_iNumChn;i++)
    {
        memcpy(&swappedChn, &m_pChannels[i], sizeof(live_channel));

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

        memcpy(m_aData+(i*sizeof(live_channel)), &swappedChn, sizeof(live_channel));
    }
}

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

/* open the device */
extern "C" UInt16 devOpen()
{
    // read configuration
    svt_config* config = svt_getConfig();

    if (config == NULL)
        return 0;

    m_pServerAddr = svt_getConfig()->getValue("LiveNetClient_ServerAddr", (char*)"127.0.0.1");
    m_pDevice     = svt_getConfig()->getValue("LiveNetClient_Device", (char*)"mouse");
    m_iClientPort = svt_getConfig()->getValue("LiveNetClient_ClientPort", (int)8000);
    m_iServerPort = svt_getConfig()->getValue("LiveNetClient_ServerPort", (int)8000);

    cout << "live_net_client: connecting to remote computer: " << m_pServerAddr << endl;
    cout << "live_net_client: connecting to remote port: " << m_iServerPort << endl;

    // connect to server

    __connect();

    // receive answer

    initanswer ans;
    __receiveAnswer(__callFunc(devOpen_ID, 0, (void*)m_pDevice, strlen(m_pDevice)+1), &ans, sizeof(initanswer));

    if (!__bigEndianMachine())
    {
        SwapUInt16(&ans.ret);
        SwapUInt16(&ans.chn);
    }

    if (ans.ret)
    {
        m_iNumChn = ans.chn;
        __initChannels();
    }

    return ans.ret;
}

/* close the device */
extern "C" UInt16 devClose()
{
    __callFunc(devClose_ID, 0, 0, 0);
    __disconnect();

    return 0;
}

/* update the device data */
extern "C" void devUpdate()
{
    __sendChannels();
    __receiveChannels(__callFunc((UInt8)devUpdate_ID, 0, m_aData, MAX_DATA));
}

/* get the number of channels */
extern "C" UInt16 chnNum()
{
    return m_iNumChn;
}

/* get the type of a channel */
extern "C" UInt8 chnType(UInt16 cn)
{
    return m_pChannels[cn-1].type;
}

/* direction is input for all channels */
extern "C" UInt8 chnDir(UInt16 cn)
{
    return m_pChannels[cn-1].direction;
}

/* return the integer data */
extern "C" Int16 chnGetDataINT(UInt16 cn)
{
    return m_pChannels[cn-1].data.ival;
};
/* return the char data */
extern "C" char chnGetDataCHAR(UInt16 cn)
{
    return m_pChannels[cn-1].data.cval;
};
/* return the float data */
extern "C" Real32 chnGetDataFLOAT(UInt16 cn)
{
    return m_pChannels[cn-1].data.fval;
};
/* return the matrix data */
extern "C" Real32* chnGetDataMAT(UInt16 cn)
{
    return m_pChannels[cn-1].data.mval;
};

/* write a int to the device */
extern "C" void chnSetDataINT(UInt16 cn, Int16 i)
{
    m_pChannels[cn-1].data.ival = i;
};
/* write a char to the device */
extern "C" void chnSetDataCHAR(UInt16 cn, char c)
{
    m_pChannels[cn-1].data.cval = c;
};
/* write a float to the device */
extern "C" void chnSetDataFLOAT(UInt16 cn, Real32 f)
{
    m_pChannels[cn-1].data.fval = f;
};
/* write a matrix to the device */
extern "C" void chnSetDataMAT(UInt16 cn, Real32* mat)
{
    memcpy(m_pChannels[cn-1].data.mval, mat, 16*sizeof(Real32));
};
