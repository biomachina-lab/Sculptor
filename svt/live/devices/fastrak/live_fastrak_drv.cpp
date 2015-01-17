/***************************************************************************
                          live_fastrak_drv.cpp
                          ------------------
    begin                : 19.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <live_fastrak.h>
#include <stdio.h>

#include <svt_init.h>
#include <svt_config.h>

/******************************************************************************
** variables
******************************************************************************/

live_fastrak* g_pFastrak;
int g_iStylus;

Real32 mat1[16];
Real32 mat2[16];
Real32 mat3[16];
Real32 mat4[16];

/******************************************************************************
** device functions
******************************************************************************/

/* open the device */
extern "C" UInt16 devOpen()
{
    svt_config* pConfig = svt_getConfig();

    g_pFastrak = new live_fastrak();
    g_pFastrak->init(pConfig->getValue("TrackerSerial", "/dev/ttyS0"), pConfig->getValue("TrackerBaudrate", 38400), pConfig->getValue("TrackerStylus", 1));

    return 1;
}

/* close the device */
extern "C" UInt16 devClose()
{
    delete g_pFastrak;

    return 1;
}

/* update the device data */
extern "C" void devUpdate()
{
    //g_pFastrak->update();

    g_pFastrak->getTransMat(mat1, 0);
    g_pFastrak->getTransMat(mat2, 1);
    g_pFastrak->getTransMat(mat3, 2);
    g_pFastrak->getTransMat(mat4, 3);

    g_iStylus = g_pFastrak->getStylusButton();
}

/******************************************************************************
** channel functions
******************************************************************************/

/* chn1 = 4x4 matrix station 1 */
/* chn2 = 4x4 matrix station 2 */
/* chn3 = 4x4 matrix station 3 */
/* chn4 = 4x4 matrix station 4 */
/* chn5 = stylus button */
extern "C" UInt16 chnNum()
{
    return 5;
}

extern "C" UInt8 chnType(UInt16 chn)
{
    switch(chn)
    {
    case 1:
        return LIVE_CHN_MATRIX;
    case 2:
        return LIVE_CHN_MATRIX;
    case 3:
        return LIVE_CHN_MATRIX;
    case 4:
        return LIVE_CHN_MATRIX;
    case 5:
        return LIVE_CHN_INT;
    default:
        return LIVE_CHN_UNKNOWN;
    }
}

/* direction is input for all channels */
extern "C" UInt8 chnDir(UInt16)
{
    return LIVE_CHN_INPUT;
}

/* return the integer data (only valid for the buttons) */
extern "C" Int16 chnGetDataINT(UInt16 chn)
{
    if (chn != 5)
        return 0;

    return g_iStylus;
}

/* return the char data (not valid here) */
extern "C" char chnGetDataCHAR(UInt16)
{
    return ' ';
}

/* return the float data (not valid here) */
extern "C" Real32 chnGetDataFLOAT(UInt16)
{
    return -1;
}

/* return the matrix data (only valid for chn1) */
extern "C" Real32* chnGetDataMAT(UInt16 chn)
{
    switch(chn)
    {
    case 1:
        return (float*)mat1;
    case 2:
        return (float*)mat2;
    case 3:
        return (float*)mat3;
    case 4:
        return (float*)mat4;
    default:
        return NULL;
    }
}

/* the functions for writing data to the device (not valid for a mouse) */
extern "C" void chnSetDataINT(UInt16, Int16 ){};
extern "C" void chnSetDataCHAR(UInt16, char){};
extern "C" void chnSetDataFLOAT(UInt16, Real32){};
extern "C" void chnSetDataMAT(UInt16, Real32*){};
