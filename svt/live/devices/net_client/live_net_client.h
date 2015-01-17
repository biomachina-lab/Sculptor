/***************************************************************************
                          live_net_client.h
                          ------------------
    begin                : 10.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

/** This is a special device which translates all function calls into data, transmitted
 over a network to a live_net_server program. This program has access to a "real" live device driver
 and makes the according functions calls there. The results are transmitted over the network to the
 client again, who gives the results to the application program. With this mechanism the application can have
 access to devices connected to another computer. */

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

#include <svt_basics.h>

extern "C"
{

/* open the device */
DLLEXPORT UInt16 devOpen();
/* close the device */
DLLEXPORT UInt16 devClose();
/* update the device data */
DLLEXPORT void devUpdate();

/* get the number of channels */
DLLEXPORT UInt16 chnNum();
/* get the type of a channel */
DLLEXPORT UInt8 chnType(UInt16);
/* direction is input for all channels */
DLLEXPORT UInt8 chnDir(UInt16);

/* return the integer data */
DLLEXPORT Int16 chnGetDataINT(UInt16);
/* return the char data */
DLLEXPORT char chnGetDataCHAR(UInt16);
/* return the float data */
DLLEXPORT float chnGetDataFLOAT(UInt16);
/* return the matrix data */
DLLEXPORT float* chnGetDataMAT(UInt16);

/* write a int to the device */
DLLEXPORT void chnSetDataINT(UInt16, Int16);
/* write a char to the device */
DLLEXPORT void chnSetDataCHAR(UInt16, char);
/* write a float to the device */
DLLEXPORT void chnSetDataFLOAT(UInt16, Real32);
/* write a matrix to the device */
DLLEXPORT void chnSetDataMAT(UInt16, Real32*);

}
