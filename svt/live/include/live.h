/***************************************************************************
                          live.h
                          ------------------
    begin                : 11.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_H
#define LIVE_H

#include <svt_basics.h>
#include <svt_types.h>
#include <live_channel.h>
#include <live_device.h>

#ifndef PI
#define PI 3.1415926535
#endif

/**
 * open a device.
 * This function will load the shared lib (Unix: liblive_name.so Win32: live_name.DLL)
 * and will try to find the live functions in the lib. If it cannot find the functions, it will return NULL.
 * \param pName name of the device
 * \return pointer to the live_device struct or NULL, if the function cannot find the device lib, or if the device lib is no live lib.
 */
DLLEXPORT live_device* live_openDevice(const char* name);

/**
 * close a device (and free the memory for the live_device structure!)
 * \param pDev pointer to the live_device struct of the device
 */
DLLEXPORT void live_closeDevice(live_device* dev);

/**
 * get the number of channels a device provides
 * \param pDev pointer to the live_device struct of the device
 * \return number of channels of the device
 */
DLLEXPORT UInt16 live_getNumChannels(live_device* dev);

/**
 * get the ith channel of a device
 * \param pDev pointer to the live_device struct of the device
 * \param iChn number of the channel
 * \return pointer to the live_channel struct
 */
DLLEXPORT live_channel* live_getChannel(live_device* dev, UInt16 i);

/**
 * update a device (the device handler will try to get new data from the physical device, or will post the current data to the device - depends on the direction of the channels). All channels will get updated!
 * \param pDev pointer to the live_device struct of the device
 */
DLLEXPORT void live_updateData(live_device* dev);

#endif
