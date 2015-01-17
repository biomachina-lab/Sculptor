/***************************************************************************
                          svt_device.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_init.h>
#include <svt_device.h>

/**
 * Constructor
 * \param pDevName pointer to array of char with the device name
 */
svt_device::svt_device(const char* pDevName)
{
    strcpy(m_pDevName, pDevName);

    if (pDevName == NULL || strcmp("mouse", pDevName) == 0)
    {
	// builtin mouse support

	m_pMouseMatChn = new live_channel;
	m_pMouseMatChn->type = (UInt8)LIVE_CHN_MATRIX;
        m_pMouseMatChn->direction = (UInt8)LIVE_CHN_OUTPUT;
        if (svt_getMouseMat() != NULL)
            svt_getMouseMat()->getMatrix(*(Matrix4f *)(m_pMouseMatChn->data.mval));

	m_pMouseButChn = new live_channel;
	m_pMouseButChn->type = (UInt8)LIVE_CHN_INT;
	m_pMouseButChn->direction = (UInt8)LIVE_CHN_OUTPUT;
	m_pMouseButChn->data.ival = 0;

	m_iChnNum      = 2;
	m_pChannels[0] = new svt_channel(m_pMouseMatChn);
	m_pChannels[1] = new svt_channel(m_pMouseButChn);

	m_bMouse = true;

    } else {

        // LIVE devices
	
        initDevice(pDevName);
	m_bMouse = false;
    }

}
svt_device::~svt_device()
{
    live_closeDevice(m_pLiveDev);
}

/**
 * get the number of channels the device provides
 * \return number of channels
 */
int svt_device::getNumChannels()
{
    return m_iChnNum;
}

/**
 * get a single channel
 * \param iIndex number of the channel
 * \return pointer to a svt_channel object
 */
svt_channel* svt_device::getChannel(int i)
{
    if ((i > 0) && (i <= m_iChnNum))
        return m_pChannels[i-1];
    else
        return NULL;
}

/**
 * update the device data. All channels will get updated!
 */
void svt_device::update()
{
    m_oDeviceSema.P();

    if (m_bMouse)
    {
        svt_getMouseMat()->getMatrix(*(Matrix4f *)(m_pMouseMatChn->data.mval));
        m_pMouseButChn->data.ival = svt_getLeftMouseButton();
    } else
        live_updateData(m_pLiveDev);

    m_oDeviceSema.V();
}

/**
 * get the name of the device
 * \return pointer to an array of char with the device name
 */
char* svt_device::getName()
{
    return m_pDevName;
}

/**
 * init the device (create live device and all channels provided by this device)
 * \param pDevName pointer to array of char with the device name
 */
bool svt_device::initDevice(const char* pDevName)
{
    m_pLiveDev = live_openDevice(pDevName);

    if (m_pLiveDev == NULL)
        svt_fatalError("svt_device: cannot open live device!");

    m_iChnNum = live_getNumChannels(m_pLiveDev);

    int i;
    for(i=0;i<m_iChnNum;i++)
        m_pChannels[i] = new svt_channel(live_getChannel(m_pLiveDev, i+1));

    return true;
}
