/***************************************************************************
                          live_fastrak_drv.h
                          ------------------
    begin                : 19.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

/******************************************************************************
** device functions
******************************************************************************/

extern "C"
{

/** open the device */
UInt16 devOpen();
/** close the device */
UInt16 devClose();
/** update the device data */
void devUpdate();

/******************************************************************************
** channel functions
******************************************************************************/

/** chn1 = 4x4 matrix station 1
 chn2 = 4x4 matrix station 2
 chn3 = 4x4 matrix station 3
 chn4 = 4x4 matrix station 4
 chn5 = stylus button */
UInt16 chnNum();
UInt8 chnType(UInt16 chn);
/** direction is input for all channels */
UInt8 chnDir(UInt16 chn);
/* return the integer data (only valid for the stylus button, chn5) */
Int16 chnGetDataINT(UInt16 chn);
/* return the char data (not valid here) */
char chnGetDataCHAR(UInt16 chn);
/* return the float data (not valid here) */
Real32 chnGetDataFLOAT(UInt16 chn);
/* return the matrix data (only valid for chn1,2,3,4) */
Real32* chnGetDataMAT(UInt16 chn);

/* the functions for writing data to the device (not valid for a fastrak) */
void chnSetDataINT(UInt16 chn, Int16 data);
void chnSetDataCHAR(UInt16 chn, char data);
void chnSetDataFLOAT(UInt16 chn, Real32 data);
void chnSetDataMAT(UInt16 chn, Real32* data);

}
