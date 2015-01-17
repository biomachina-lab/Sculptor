/***************************************************************************
                          live_fastrak.h
			  -------------
    begin                : 11/19/2003
    authors              : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <live_fastrak.h>

#include <svt_basics.h>
#include <svt_init.h>
#include <svt_iostream.h>
#include <svt_swap.h>
#include <svt_threads.h>
#include <svt_time.h>

extern "C"{
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
}
namespace termios
{
#include <termios.h>
}

svt_matrix4<Real32> g_oUpdateMat;

/**
 * Constructor
 */
live_fastrak::live_fastrak()
{
    m_iFD = 0;
    m_bReady = false;

    svt_createThread(&updateThread, (void*)this, SVT_THREAD_PRIORITY_NORMAL );

    //Real32 fOriginX = svt_getConfig()->getValue("TrackerOriginX", 0.0f);
    //Real32 fOriginY = svt_getConfig()->getValue("TrackerOriginY", 0.0f);
    //Real32 fOriginZ = svt_getConfig()->getValue("TrackerOriginZ", 0.0f);
    const char* pStr = svt_getConfig()->getValue("TrackerMatrix",(char*)NULL);
    int iRet;

    if (pStr != NULL)
    {
	iRet = sscanf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
	       &m_oTracker2World[0][0],
	       &m_oTracker2World[0][1],
	       &m_oTracker2World[0][2],
	       &m_oTracker2World[0][3],
	       &m_oTracker2World[1][0],
	       &m_oTracker2World[1][1],
	       &m_oTracker2World[1][2],
	       &m_oTracker2World[1][3],
	       &m_oTracker2World[2][0],
	       &m_oTracker2World[2][1],
	       &m_oTracker2World[2][2],
	       &m_oTracker2World[2][3],
	       &m_oTracker2World[3][0],
	       &m_oTracker2World[3][1],
	       &m_oTracker2World[3][2],
	       &m_oTracker2World[3][3]);

	cout << "m_oTracker2World: " << m_oTracker2World << endl;
    }

    pStr = svt_getConfig()->getValue("Sensor1Matrix",(char*)NULL);
    if (pStr != NULL)
    {
	iRet = sscanf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
	       &m_oSensor2Tracker[0][0][0],
	       &m_oSensor2Tracker[0][0][1],
	       &m_oSensor2Tracker[0][0][2],
	       &m_oSensor2Tracker[0][0][3],
	       &m_oSensor2Tracker[0][1][0],
	       &m_oSensor2Tracker[0][1][1],
	       &m_oSensor2Tracker[0][1][2],
	       &m_oSensor2Tracker[0][1][3],
	       &m_oSensor2Tracker[0][2][0],
	       &m_oSensor2Tracker[0][2][1],
	       &m_oSensor2Tracker[0][2][2],
	       &m_oSensor2Tracker[0][2][3],
	       &m_oSensor2Tracker[0][3][0],
	       &m_oSensor2Tracker[0][3][1],
	       &m_oSensor2Tracker[0][3][2],
	       &m_oSensor2Tracker[0][3][3]);

	cout << "m_oSensor2Tracker: " << m_oSensor2Tracker[0] << endl;
    }

    pStr = svt_getConfig()->getValue("Sensor2Matrix",(char*)NULL);
    if (pStr != NULL)
    {
	iRet = sscanf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
	       &m_oSensor2Tracker[1][0][0],
	       &m_oSensor2Tracker[1][0][1],
	       &m_oSensor2Tracker[1][0][2],
	       &m_oSensor2Tracker[1][0][3],
	       &m_oSensor2Tracker[1][1][0],
	       &m_oSensor2Tracker[1][1][1],
	       &m_oSensor2Tracker[1][1][2],
	       &m_oSensor2Tracker[1][1][3],
	       &m_oSensor2Tracker[1][2][0],
	       &m_oSensor2Tracker[1][2][1],
	       &m_oSensor2Tracker[1][2][2],
	       &m_oSensor2Tracker[1][2][3],
	       &m_oSensor2Tracker[1][3][0],
	       &m_oSensor2Tracker[1][3][1],
	       &m_oSensor2Tracker[1][3][2],
	       &m_oSensor2Tracker[1][3][3]);

	cout << "m_oSensor2Tracker: " << m_oSensor2Tracker[1] << endl;
    }

    pStr = svt_getConfig()->getValue("Sensor3Matrix",(char*)NULL);
    if (pStr != NULL)
    {
	iRet = sscanf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
	       &m_oSensor2Tracker[2][0][0],
	       &m_oSensor2Tracker[2][0][1],
	       &m_oSensor2Tracker[2][0][2],
	       &m_oSensor2Tracker[2][0][3],
	       &m_oSensor2Tracker[2][1][0],
	       &m_oSensor2Tracker[2][1][1],
	       &m_oSensor2Tracker[2][1][2],
	       &m_oSensor2Tracker[2][1][3],
	       &m_oSensor2Tracker[2][2][0],
	       &m_oSensor2Tracker[2][2][1],
	       &m_oSensor2Tracker[2][2][2],
	       &m_oSensor2Tracker[2][2][3],
	       &m_oSensor2Tracker[2][3][0],
	       &m_oSensor2Tracker[2][3][1],
	       &m_oSensor2Tracker[2][3][2],
	       &m_oSensor2Tracker[2][3][3]);

	cout << "m_oSensor2Tracker: " << m_oSensor2Tracker[2] << endl;
    }

    pStr = svt_getConfig()->getValue("Sensor4Matrix",(char*)NULL);
    if (pStr != NULL)
    {
	iRet = sscanf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
	       &m_oSensor2Tracker[3][0][0],
	       &m_oSensor2Tracker[3][0][1],
	       &m_oSensor2Tracker[3][0][2],
	       &m_oSensor2Tracker[3][0][3],
	       &m_oSensor2Tracker[3][1][0],
	       &m_oSensor2Tracker[3][1][1],
	       &m_oSensor2Tracker[3][1][2],
	       &m_oSensor2Tracker[3][1][3],
	       &m_oSensor2Tracker[3][2][0],
	       &m_oSensor2Tracker[3][2][1],
	       &m_oSensor2Tracker[2][2][2],
	       &m_oSensor2Tracker[3][2][3],
	       &m_oSensor2Tracker[3][3][0],
	       &m_oSensor2Tracker[3][3][1],
	       &m_oSensor2Tracker[3][3][2],
	       &m_oSensor2Tracker[3][3][3]);

	cout << "m_oSensor2Tracker: " << m_oSensor2Tracker[3] << endl;
    }
}
/**
 * Destructor
 */
live_fastrak::~live_fastrak()
{
    writeString("c\r");
    writeString("W\r"),

    close();
}

/**
 * open the serial port
 * \param pTtyName name of the port to open
 * \param iBaudrate baudrate speed
 */
bool live_fastrak::open(const char *pTtyName, int iBaudrate)
{
    struct termios::termios oldtio, newtio;
  
    // close filediscriptor if already open */
    if (m_iFD > 0)
	close();

    cout << "live_fastrak: open file descriptor" << endl;

    // open the specified serial port in for read and write O_RDWR */
    // NO_CTTY makes it a non-controlling TTY. */
    m_iFD = ::open(pTtyName, O_RDWR | O_NOCTTY);
    if (m_iFD <= 0)
    {
	perror(pTtyName);
	return false;
    }

    // Get current port config.
    tcgetattr(m_iFD, &(oldtio) );
    // reset new config
    memset(&newtio, 0, sizeof(newtio));

    switch(iBaudrate)
    {
    case 2400:
	cfsetispeed(&newtio,B2400);
	cfsetospeed(&newtio,B2400);
	break;
    case 4800:
	cfsetispeed(&newtio,B4800);
	cfsetospeed(&newtio,B4800);
	break;
    case 9600:
	cfsetispeed(&newtio,B9600);
	cfsetospeed(&newtio,B9600);
	break;
    case 19200:
	cfsetispeed(&newtio,B19200);
	cfsetospeed(&newtio,B19200);
	break;
    case 38400:
	cfsetispeed(&newtio,B38400);
	cfsetospeed(&newtio,B38400);
	break;
#ifdef B57600
    case 57600:
	cfsetispeed(&newtio,B57600);
	cfsetospeed(&newtio,B57600);
	break;
#endif
#ifdef B115200
    case 115200:
	cfsetispeed(&newtio,B115200);
	cfsetospeed(&newtio,B115200);
	break;
#endif
    default:
	printf("live_fastrak::open(): Unsupported baud rate %d\n", iBaudrate);
	return false;
    }

    // We've got baudrate. Add other options.
    newtio.c_cflag |= CS8 | CLOCAL | CREAD;
    newtio.c_iflag= IGNPAR;
    // output mode: raw output
    newtio.c_oflag = 0;
    // local mode: canonical input
    newtio.c_lflag = ICANON;

    // control chars:
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;    /* inter-character timer */
    newtio.c_cc[VMIN]     = 0;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */

    // try and configure the port appropriately...
    termios::tcflush(m_iFD, TCIFLUSH);
    termios::tcsetattr(m_iFD, TCSANOW, &newtio);

    return true;
}

/**
 * close the fastrak port
 */
bool live_fastrak::close()
{
    if (::close(m_iFD)<0)
    {
	cerr << "live_fastrak: close: "<< strerror(errno) << endl;
	return false;
    }
    m_iFD = 0;
    return true;
}

/**
 * read character from serial line
 * \param pC pointer char variable
 * \return true if successful
 */
bool live_fastrak::readChar(char* pC)
{
    int iRet = ::read(m_iFD, pC, 1);

    if (iRet == -1)
    {
	cerr << "live_fastrak: read: " << strerror(errno) << endl;
	return false;
    }

    return true;
}

/**
 * write string to serial line
 * \param pString pointer to array of chars
 * \return true if successful
 */
bool live_fastrak::writeString(char* pString)
{
    if (pString == NULL)
	return false;

    int iRet = ::write(m_iFD, pString, strlen(pString));

    if (iRet != (int)strlen(pString))
    {
	cerr << "live_fastrak: write: "<< strerror(errno) << endl;
	return false;
    }

    return true;
}

/**
 * read a float from the serial line (IEEE format)
 * \return the float value
 */
float live_fastrak::readFloat()
{
    Real32 i = 0.0f;
    ssize_t iT = ::read(m_iFD, &i, 4);

    if (iT != 0)
	if(svt_bigEndianMachine())
	    svt_swapReal32(&i);

    return i;
}

/**
 * initialize the fastrak
 * \param iStylus number of the station where the stylus is connected (default: 1)
 */
void live_fastrak::init(const char* pDev, int iBaudrate, int iStylus)
{
    cout << "live_fastrak> device: " << pDev << " baudrate: " << iBaudrate << " stylus station: " << iStylus << endl;

    m_iStylus = iStylus;

    // open serial device
    if (!open(pDev, iBaudrate))
    {
        cout << "Cannot open port..." << endl;
	exit(1);
    }

    cout << "live_fastrak> Port successully opened" << endl;
    cout << "live_fastrak> Reset device: " << endl;

    // do a reset
    if (!writeString("W"))
    {
	cout << "Cannot write to port..." << endl;
        exit(1);
    }
    svt_sleep(5000);
    cout << "live_fastrak> Done. " << endl;

    // disable continous printing
    writeString("c");
    // set output to centimeters
    writeString("u");
    // binary mode output
    writeString("f");

    // configure output
    writeString("O1,2,0,5,0,6,0,7,0,16,0,1\r");
    writeString("O2,2,0,5,0,6,0,7,0,16,0,1\r");
    writeString("O3,2,0,5,0,6,0,7,0,16,0,1\r");
    writeString("O4,2,0,5,0,6,0,7,0,16,0,1\r");

    // reset alignment reference frame AND boresight
    writeString("R1\rb1\r");
    writeString("R2\rb2\r");
    writeString("R3\rb3\r");
    writeString("R4\rb4\r");

    // set hemisphere
    writeString("H1,1,0,0\r");
    writeString("H2,1,0,0\r");
    writeString("H3,1,0,0\r");
    writeString("H4,1,0,0\r");

    // adjust filter
    //writeString("x5\r");
    //writeString("v5\r");

    //writeString("I1,0.5\r");

    // enable stylus button
    char oTmp[80];
    sprintf(oTmp,"e%1d,0\r", iStylus);
    writeString(oTmp);

    //termios::tcflush(m_iFD, TCIFLUSH);

    // enable continous printing
    writeString("C\r");
    printf("live_fastrak> Polhemus fastrak successfully initialized\n");
    m_bReady = true;
}

/**
 * wait for the next header of a station
 * \return int with the number of the next station
 */
int live_fastrak::matchNextStationHeader()
{
    int iStation = 0;
    bool bHeaderFound = false;

    int iTries = 0;

    while (!bHeaderFound)
    {
	/* find a cr */
	char cC;
	while (1)
	{
	    readChar(&cC);
	    if (cC == 13)
		break;

	    iTries ++;
	    if (iTries > 65536)
	    {
		fprintf(stderr, "live_fastrak: matchNextStationHeader():  can't find header start after %d tries.\n", iTries);
		exit(1);
	    }
        }

        readChar(&cC);
	if (cC != 10)
            continue;

        readChar(&cC);
	if (cC != '0')
            continue;

        readChar(&cC);
        iStation = cC - '0';
	if ((iStation < 1) || (iStation > MAX_STATIONS) )
	{
	    printf("live_fastrak: station: %i\n", iStation);
	    continue;
	}

	readChar(&cC);

        bHeaderFound = true;
    }

    return iStation;
}

/**
 * update fastrak in thread
 * \param args pointer to fastrak object (this is a static function)
 */
void* updateThread(void* args)
{
    live_fastrak *pFastrak = (live_fastrak *) args;

    cout << "live_fastrak: thread started!" << endl;

    while(!pFastrak->isReady());

    cout << "live_fastrak: measurement starts..." << endl;

    while(1)
    {
	pFastrak->update();
    }
}

/**
 * update fastrak
 */
void live_fastrak::update()
{
    int iStation;
    char cC;

    iStation = matchNextStationHeader();

    // translation
    g_oUpdateMat[0][3] = readFloat()/100.0f;
    g_oUpdateMat[1][3] = readFloat()/100.0f;
    g_oUpdateMat[2][3] = readFloat()/100.0f;

    //cout << "translation: " << g_oUpdateMat[0][3] << "," << g_oUpdateMat[1][3] << "," << g_oUpdateMat[2][3] << endl;

    g_oUpdateMat[3][3] = (float)1.0;
    g_oUpdateMat[3][2] = (float)0.0;
    g_oUpdateMat[3][1] = (float)0.0;
    g_oUpdateMat[3][0] = (float)0.0;

    readChar(&cC);
    if (cC != ' ')
        return;

    // cos of rotation, x-values
    g_oUpdateMat[0][0] = readFloat();
    g_oUpdateMat[0][1] = readFloat();
    g_oUpdateMat[0][2] = readFloat();

    readChar(&cC);
    if (cC != ' ')
        return;

    // cos of rotation, y-values
    g_oUpdateMat[1][0] = readFloat();
    g_oUpdateMat[1][1] = readFloat();
    g_oUpdateMat[1][2] = readFloat();

    readChar(&cC);
    if (cC != ' ')
        return;

    // cos of rotation, z-values
    g_oUpdateMat[2][0] = readFloat();
    g_oUpdateMat[2][1] = readFloat();
    g_oUpdateMat[2][2] = readFloat();

    readChar(&cC);
    if (cC != ' ')
        return;

    // stylus button
    readChar(&cC);
    //cout << cC << endl;
    readChar(&cC);
    if (cC == '1' && iStation == m_iStylus)
	m_bButton = true;
    if (cC == '0' && iStation == m_iStylus)
	m_bButton = false;

    readChar(&cC);
    if (cC == ' ')
    {
	m_oSemaphore.P();
        m_oSensorMat[iStation-1] = g_oUpdateMat;
	m_oSemaphore.V();
    }
}

/**
 * copies the 4x4 transformation matrix into dest
 * \param aDest 4x4 array of floats
 * \param iStation station number
 */
void live_fastrak::getTransMat(float* pDest, int iStation)
{
    svt_matrix4<Real32> oTempMat;

    m_oSemaphore.P();
    oTempMat = m_oTracker2World * m_oSensorMat[iStation] * m_oSensor2Tracker[iStation];
    m_oSemaphore.V();

    for(int x=0;x<4;x++)
	for(int y=0;y<4;y++)
	    pDest[y*4+x] = oTempMat[x][y];
}

/**
 * get the status of the stylus button
 * \return 1 if the button is pressed, 0 if not
 */
int live_fastrak::getStylusButton()
{
    if (m_bButton)
	return 1;
    else
        return 0;
}

/**
 * is the device ready (got the serial port sucessfully opened, the device initialized...)
 * \return false if the device is not ready
 */
bool live_fastrak::isReady()
{
    return m_bReady;
}
