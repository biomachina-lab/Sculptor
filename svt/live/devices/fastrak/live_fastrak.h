/***************************************************************************
                          live_fastrak.h
			  -------------
    begin                : 11/19/2003
    authors              : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_FASTRAK_H
#define LIVE_FASTRAK_H

#define MAX_STATIONS 4

#include <svt_matrix4.h>
#include <svt_semaphore.h>

/**
 * update fastrak in thread
 * \param args pointer to fastrak object (this is a static function)
 */
void* updateThread(void* args);

/**
 * live fastrak driver class
 */
class live_fastrak
{

protected:
    // file descriptor of the serial device
    int m_iFD;

    // sensor matrix
    svt_matrix4<Real32> m_oSensorMat[MAX_STATIONS];
    // where is the origin of the fastrak in the real world
    svt_matrix4<Real32> m_oTracker2World;
    // sensor to tracker
    svt_matrix4<Real32> m_oSensor2Tracker[MAX_STATIONS];

    // which station is the stylus
    int m_iStylus;
    // what is the current status of the button of the stylus
    bool m_bButton;
    // is the device ready to get polled?
    bool m_bReady;
    // semaphore
    svt_semaphore m_oSemaphore;

public:
    /**
     * Constructor
     */
    live_fastrak();
    ~live_fastrak();

    /**
     * open the serial port
     * \param pTtyName name of the port to open
     * \param iBaudrate baudrate speed
     */
    bool open(const char *pTtyName, int iBaudrate);
    /**
     * close the serial port
     */
    bool close();

    /**
     * read character from serial line
     * \param pC pointer char variable
     * \return true if successful
     */
    bool readChar(char* pC);

    /**
     * write string to serial line
     * \param pString pointer to array of chars
     * \return true if successful
     */
    bool writeString(char* pString);

    /**
     * read a float from the serial line (IEEE format)
     * \return the float value
     */
    float readFloat();

    /**
     * initialize the fastrak
     * \param pDev pointer to array of char with the device information
     * \param iBaudrate speed of the serial device
     * \param iStylus number of the station where the stylus is connected (default: 1)
     */
    void init(const char* pDev, int iBaudrate, int iStylus);
    /**
     * wait for the next header of a station
     * \return int with the number of the next station
     */
    int matchNextStationHeader();

    /**
     * update fastrak
     */
    void update();

    /**
     * copies the 4x4 transformation matrix into dest
     * \param aDest 4x4 array of floats
     * \param iStation station number
     */
    void getTransMat(float* pDest, int iStation);

    /**
     * get the status of the stylus button
     * \return 1 if the button is pressed, 0 if not
     */
    int getStylusButton();

    /**
     * is the device ready (got the serial port sucessfully opened, the device initialized...)
     * \return false if the device is not ready
     */
    bool isReady();

};

#endif
