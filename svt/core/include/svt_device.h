/***************************************************************************
                          svt_devices.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_DEVICE_H
#define SVT_DEVICE_H

#include <live.h>
#include <svt_channel.h>

/**
 * This class encapsulates a live device. The default device is the mouse device. Due to some reasons this device is not a live device. The mouse device is programmed inside of svt. For the live devices: See the l.i.v.e. documentation.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_device
{
protected:
    live_device* m_pLiveDev;
    svt_channel* m_pChannels[256];
    int m_iChnNum;

    // builtin mouse support
    bool m_bMouse;
    live_channel* m_pMouseMatChn;
    live_channel* m_pMouseButChn;

    char m_pDevName[256];
    svt_semaphore m_oDeviceSema;

public:
    /**
     * Constructor
     * \param pDevName pointer to array of char with the device name
     */
    svt_device(const char* pDevName);
    virtual ~svt_device();

public:

    /**
     * get the number of channels the device provides
     * \return number of channels
     */
    int getNumChannels();

    /**
     * get a single channel
     * \param iIndex number of the channel
     * \return pointer to a svt_channel object
     */
    svt_channel* getChannel(int iIndex);
    /**
     * update the device data. All channels will get updated!
     */
    void update();

    /**
     * get the name of the device
     * \return pointer to an array of char with the device name
     */
    char* getName();

protected:
    /**
     * init the device (create live device and all channels provided by this device)
     * \param pDevName pointer to array of char with the device name
     */
    bool initDevice(const char* devName);
};

#endif
