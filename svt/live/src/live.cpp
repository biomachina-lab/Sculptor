/***************************************************************************
                          live.cpp
                          --------
    begin                : 11.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// includes
///////////////////////////////////////////////////////////////////////////////

#include <live.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#ifdef WIN32
  #ifndef STATICQT
  extern "C" DLLEXPORT BOOL WINAPI DllMain( HANDLE hModule, DWORD dwFunction, LPVOID lpNot )
  {
    svt_registerCout( &std::cout );
    return true;
  };
  #endif
#endif

///////////////////////////////////////////////////////////////////////////////
// global functions
///////////////////////////////////////////////////////////////////////////////

/**
 * open a device.
 * This function will load the shared lib (Unix: liblive_name.so Win32: live_name.DLL)
 * and will try to find the live functions in the lib. If it cannot find the functions, it will return NULL.
 * \param pName name of the device
 * \return pointer to the live_device struct or NULL, if the function cannot find the device lib, or if the device lib is no live lib.
 */
live_device* live_openDevice(const char* pName)
{
    UInt16 cn;
    char buf[256];

    live_device* dev = (live_device*)malloc(sizeof(live_device));
    if (dev == NULL)
    {
        printf("live: memory management error!\n");
        return NULL;
    }
    memset(dev, 0, sizeof(live_device));

#ifndef WIN32
    int i=0;
    sprintf(buf, "liblive_");
    while(pName[i] !=0 && pName[i] != '\n' && pName[i] != '\r')
    {
	buf[i+8] = pName[i];
        i++;
    }
    buf[i+8] = '.';
    buf[i+9] = 's';
    buf[i+10] = 'o';
    buf[i+11] = 0;

    //sprintf(buf,"/data/local/svt/lib/liblive_openhaptics.so");
    void* lib = dlopen(buf, RTLD_LAZY);

    if (lib == NULL)
    {
        printf("live: sorry, couldnt open the device lib (%s)!\n", buf);
        return NULL;
    }

    dev->devOpen  = ( UInt16(*)() ) dlsym(lib,"devOpen"); if (dev->devOpen == NULL) printf("devOpen\n");
    dev->devClose = ( UInt16(*)() ) dlsym(lib, "devClose"); if (dev->devClose == NULL) printf("devClose\n");
    dev->devUpdate = ( UInt16(*)() ) dlsym(lib, "devUpdate"); if (dev->devUpdate == NULL) printf("devUpdate\n");

    dev->chnNum = ( UInt16(*)() ) dlsym(lib, "chnNum"); if (dev->chnNum == NULL) printf("chnNum\n");
    dev->chnType = ( UInt8(*)(UInt16) ) dlsym(lib, "chnType"); if (dev->chnType == NULL) printf("chnType\n");
    dev->chnDir = ( UInt8(*)(UInt16) ) dlsym(lib, "chnDir"); if (dev->chnDir == NULL) printf("chnDir\n");
    dev->chnGetDataINT = ( Int16(*)(UInt16) ) dlsym(lib, "chnGetDataINT"); if (dev->chnGetDataINT == NULL) printf("chnGetDataINT\n");
    dev->chnGetDataCHAR = ( char(*)(UInt16) ) dlsym(lib, "chnGetDataCHAR");  if (dev->chnGetDataCHAR == NULL) printf("chnGetDataCHAR\n");
    dev->chnGetDataFLOAT = ( Real32(*)(UInt16) ) dlsym(lib, "chnGetDataFLOAT");  if (dev->chnGetDataFLOAT == NULL) printf("chnGetDataFLOAT\n");
    dev->chnGetDataMAT = ( Real32*(*)(UInt16) ) dlsym(lib, "chnGetDataMAT");  if (dev->chnGetDataMAT == NULL) printf("chnGetDataMAT\n");
    dev->chnSetDataINT = ( void(*)(UInt16, Int16) ) dlsym(lib, "chnSetDataINT"); if (dev->chnSetDataINT == NULL) printf("chnSetDataINT\n");
    dev->chnSetDataCHAR = ( void(*)(UInt16, char) ) dlsym(lib, "chnSetDataCHAR"); if (dev->chnSetDataCHAR == NULL) printf("chnSetDataCHAR\n");
    dev->chnSetDataFLOAT = ( void(*)(UInt16, Real32) ) dlsym(lib, "chnSetDataFLOAT"); if (dev->chnSetDataFLOAT == NULL) printf("chnGetDataFLOAT\n");
    dev->chnSetDataMAT = ( void(*)(UInt16, Real32*) ) dlsym(lib, "chnSetDataMAT"); if (dev->chnSetDataMAT == NULL) printf("chnSetDataMAT\n");
#else
    HINSTANCE lib;

    sprintf(buf,"live_%s.DLL", pName);
    lib= LoadLibrary(buf);

    if (lib == NULL)
    {
        printf("live: sorry, couldnt open the device lib (%s)!\n", buf);
        return NULL;
    }

    dev->devOpen  = ( UInt16(*)() ) GetProcAddress(lib,"devOpen");
    dev->devClose = ( UInt16(*)() ) GetProcAddress(lib, "devClose");
    dev->devUpdate = ( UInt16(*)() ) GetProcAddress(lib, "devUpdate");

    dev->chnNum = ( UInt16(*)() ) GetProcAddress(lib, "chnNum");
    dev->chnType = ( UInt8(*)(UInt16) ) GetProcAddress(lib, "chnType");
    dev->chnDir = ( UInt8(*)(UInt16) ) GetProcAddress(lib, "chnDir");
    dev->chnGetDataINT = ( Int16(*)(UInt16) ) GetProcAddress(lib, "chnGetDataINT");
    dev->chnGetDataCHAR = ( char(*)(UInt16) ) GetProcAddress(lib, "chnGetDataCHAR");
    dev->chnGetDataFLOAT = ( Real32(*)(UInt16) ) GetProcAddress(lib, "chnGetDataFLOAT");
    dev->chnGetDataMAT = ( Real32*(*)(UInt16) ) GetProcAddress(lib, "chnGetDataMAT");
    dev->chnSetDataINT = ( void(*)(UInt16, Int16) ) GetProcAddress(lib, "chnSetDataINT");
    dev->chnSetDataCHAR = ( void(*)(UInt16, char) ) GetProcAddress(lib, "chnSetDataCHAR");
    dev->chnSetDataFLOAT = ( void(*)(UInt16, Real32) ) GetProcAddress(lib, "chnSetDataFLOAT");
    dev->chnSetDataMAT = ( void(*)(UInt16, Real32*) ) GetProcAddress(lib, "chnSetDataMAT");
#endif

    if (dev->devOpen == NULL ||
        dev->devClose == NULL ||
        dev->devUpdate == NULL ||
        dev->chnNum == NULL ||
        dev->chnType == NULL ||
        dev->chnDir == NULL ||
        dev->chnGetDataINT == NULL ||
        dev->chnGetDataFLOAT == NULL ||
        dev->chnGetDataCHAR == NULL ||
        dev->chnGetDataMAT == NULL ||
        dev->chnSetDataINT == NULL ||
        dev->chnSetDataFLOAT == NULL ||
        dev->chnSetDataCHAR == NULL ||
        dev->chnSetDataMAT == NULL
       )
    {
        printf("live: sorry, the requested device is not a valid live device!\n");
        return NULL;
    }

    if (!dev->devOpen())
    {
        printf("live: device responds error!\n");
        return NULL;
    }

    cn = live_getNumChannels(dev);
    if (cn > 0)
    {
        dev->chn = (live_channel*)malloc(sizeof(live_channel)*cn);
        memset(dev->chn, 0, sizeof(live_channel)*cn);
        int i;
        for(i=1;i<=cn;i++)
        {
            dev->chn[i-1].type = dev->chnType(i);
            dev->chn[i-1].direction  = dev->chnDir(i);
        }
    } else {
        printf("live: device responds <= 0 channels. Obscure...");
        return NULL;
    }

    return dev;
}

/**
 * close a device (and free the memory for the live_device structure!)
 * \param pDev pointer to the live_device struct of the device
 */
void live_closeDevice(live_device* pDev)
{
    pDev->devClose()
        ;
    free(pDev->chn);
    free(pDev);
}

/**
 * get the number of channels a device provides
 * \param pDev pointer to the live_device struct of the device
 * \return number of channels of the device
 */
UInt16 live_getNumChannels(live_device* pDev)
{
    return pDev->chnNum();
}

/**
 * get the ith channel of a device
 * \param pDev pointer to the live_device struct of the device
 * \param iChn number of the channel
 * \return pointer to the live_channel struct
 */
live_channel* live_getChannel(live_device* pDev, UInt16 iChn)
{
    return &pDev->chn[iChn-1];
}

/**
 * update a device (the device handler will try to get new data from the physical device, or will post the current data to the device - depends on the direction of the channels). All channels will get updated!
 * \param pDev pointer to the live_device struct of the device
 */
void live_updateData(live_device* pDev)
{
    UInt16 cn, i;

    if (pDev)
    {
        cn = live_getNumChannels(pDev);

        /* the device should update its data */
        pDev->devUpdate();

        /* now copy the data into the live_channel structure */
        for(i=1;i<=cn;i++)
        {
            pDev->chn[i-1].type = pDev->chnType(i);
            pDev->chn[i-1].direction  = pDev->chnDir(i);

            switch(pDev->chnDir(i))
            {
            case LIVE_CHN_INPUT:

                switch(pDev->chnType(i))
                {
                case LIVE_CHN_INT:
                    pDev->chn[i-1].data.ival = pDev->chnGetDataINT(i);
                    break;
                case LIVE_CHN_CHAR:
                    pDev->chn[i-1].data.cval = pDev->chnGetDataCHAR(i);
                    break;
                case LIVE_CHN_FLOAT:
                    pDev->chn[i-1].data.fval = pDev->chnGetDataFLOAT(i);
                    break;
                case LIVE_CHN_MATRIX:
                    memcpy(pDev->chn[i-1].data.mval, pDev->chnGetDataMAT(i), 16*sizeof(Real32));
                    break;
                }

                break;

            case LIVE_CHN_OUTPUT:

                switch(pDev->chnType(i))
                {
                case LIVE_CHN_INT:
                    pDev->chnSetDataINT(i, pDev->chn[i-1].data.ival);
                    break;
                case LIVE_CHN_CHAR:
                    pDev->chnSetDataCHAR(i, pDev->chn[i-1].data.cval);
                    break;
                case LIVE_CHN_FLOAT:
                    pDev->chnSetDataFLOAT(i, pDev->chn[i-1].data.fval);
                    break;
                case LIVE_CHN_MATRIX:
                    pDev->chnSetDataMAT(i, pDev->chn[i-1].data.mval);
                    break;
                }

            }
        }
    }
}
