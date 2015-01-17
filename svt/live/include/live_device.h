/***************************************************************************
                          live_device.h
                          ------------------
    begin                : 11.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_DEVICE_H
#define LIVE_DEVICE_H

/** the device structure */
typedef struct
{
    /** ptr to the channels of this device */
    live_channel* chn;

    /** open function */
    UInt16 (*devOpen)();
    /** close function */
    UInt16 (*devClose)();
    /** update the channel data */
    UInt16 (*devUpdate)();

    /** get number of channels function */
    UInt16 (*chnNum)();
    /** get the type of the channel */
    UInt8 (*chnType)(UInt16);
    /** get direction of the channel */
    UInt8 (*chnDir)(UInt16);
    /** get the integer data of the channel */
    Int16 (*chnGetDataINT)(UInt16);
    /** get the float data of the channel */
    Real32 (*chnGetDataFLOAT)(UInt16);
    /** get the char data of the channel */
    char (*chnGetDataCHAR)(UInt16);
    /** get the mat data of the channel */
    Real32* (*chnGetDataMAT)(UInt16);
    /** set the integer data of the channel */
    void (*chnSetDataINT)(UInt16, Int16);
    /** set the float data of the channel */
    void (*chnSetDataFLOAT)(UInt16, Real32);
    /** set the char data of the channel */
    void (*chnSetDataCHAR)(UInt16, char);
    /** set the mat data of the channel */
    void (*chnSetDataMAT)(UInt16, Real32*);

} live_device;

#endif
