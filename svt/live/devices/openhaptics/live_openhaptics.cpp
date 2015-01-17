/***************************************************************************
                          live_openhaptics
                          ----------------
    begin                : 05/31/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <svt_matrix4f.h>

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////

#define LOH svt_trimName("live_openhaptics")

// openhaptics variables
HDdouble g_pForce[3];
HDdouble g_pTorque[3];
HDfloat g_pTrans[16];
HDint g_iButtons;
HHD g_hHD;
bool g_b6DOF = false;
HDenum g_iCalibrationStyle;

// live variables
int g_iButton = 0;
svt_matrix4f g_cMatrix("position");
Real32 g_oOutputMatrix[16];

///////////////////////////////////////////////////////////////////////////////
// OpenHaptics functions
///////////////////////////////////////////////////////////////////////////////

HDboolean CheckCalibration(HDenum calibrationStyle)
{
    HDenum status = hdCheckCalibration();
    
    if (status == HD_CALIBRATION_OK)
    {
        return TRUE;
    }
    else if (status == HD_CALIBRATION_NEEDS_MANUAL_INPUT)
    {
        cout << LOH << "Calibration requires manual input..." << endl;
        return FALSE;
    }
    else if (status == HD_CALIBRATION_NEEDS_UPDATE)
    {
	hdUpdateCalibration(g_iCalibrationStyle);

        if (HD_DEVICE_ERROR(hdGetError()))
	{
            cout << endl;
            cout << LOH << "Failed to update calibration." << endl;
            return FALSE;
        }
        else
	{
            cout << LOH << "Calibration updated successfully." << endl;
            return TRUE;
        }
    }
    else
    {
        cout << LOH << "Unknown calibration status." << endl;
        return FALSE;
    }
}

/**
 * calibrate device
 */
void initCalibration()
{
    HDErrorInfo error;
    int supportedCalibrationStyles;

    /* Choose a calibration style.  Some devices may support multiple types of 
       calibration.  In that case, prefer auto calibration over inkwell 
       calibration, and prefer inkwell calibration over reset encoders. */
    hdGetIntegerv(HD_CALIBRATION_STYLE, &supportedCalibrationStyles);
    if (supportedCalibrationStyles & HD_CALIBRATION_ENCODER_RESET)
    {
        g_iCalibrationStyle = HD_CALIBRATION_ENCODER_RESET;
    }
    if (supportedCalibrationStyles & HD_CALIBRATION_INKWELL)
    {
        g_iCalibrationStyle = HD_CALIBRATION_INKWELL;
    }
    if (supportedCalibrationStyles & HD_CALIBRATION_AUTO)
    {
        g_iCalibrationStyle = HD_CALIBRATION_AUTO;
    }

    /* Some haptic devices only support manual encoder calibration via a
       hardware reset. This requires that the endpoint be placed at a known
       physical location when the reset is commanded. For the PHANTOM haptic
       devices, this means positioning the device so that all links are
       orthogonal. Also, this reset is typically performed before the servoloop
       is running, and only technically needs to be performed once after each
       time the device is plugged in. */

    if (g_iCalibrationStyle == HD_CALIBRATION_ENCODER_RESET)
    {
        cout << LOH << "Please prepare for manual calibration by placing the device" << endl;
        cout << LOH << "at its reset position" << endl;
        cout << LOH << "Press any key to continue..." << endl;

        getchar();
        hdUpdateCalibration(g_iCalibrationStyle);
        if (hdCheckCalibration() == HD_CALIBRATION_OK)
        {
            cout << "Calibration complete." << endl << endl;
        }
        if (HD_DEVICE_ERROR(error = hdGetError()))
        {
            //hduPrintError(stderr, &error, "live_openhaptics> Reset encoders reset failed.");
            exit(1);
        }
    }
};

/**
 * initialize openhaptics
 */
void initOH()
{
    // Initialize the device
    g_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        //hduPrintError(stderr, &error, "live_openhaptics> Failed to initialize haptic device");
	exit(1);
    }
    cout << LOH << "Found device: " << hdGetString(HD_DEVICE_MODEL_TYPE) << endl;

    initCalibration();

    hdEnable(HD_FORCE_OUTPUT);
    hdEnable(HD_MAX_FORCE_CLAMPING);

    // do we have a 6DOF?
    HDint iDOF;
    hdGetIntegerv(HD_OUTPUT_DOF, &iDOF);
    if (iDOF == 6)
    {
	g_b6DOF = true;

        cout << LOH << "This is a 6DOF output device!" << endl;

    } else {
	g_b6DOF = false;

	cout << LOH << "This is a 3DOF output device!" << endl;
    }

    // determine workspace
    HDdouble maxWorkspace[6];
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);
    cout << LOH << "Workspace: " << endl;
    cout << LOH << "   Low: " << maxWorkspace[0] << " Left: " << maxWorkspace[1] << " Back: " << maxWorkspace[2] << endl;
    cout << LOH << "   Top: " << maxWorkspace[3] << " Right:" << maxWorkspace[4] << " Front:" << maxWorkspace[5] << endl;

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        //hduPrintError(stderr, &error, "live_openhaptics> Failed to start scheduler");
        exit(1);
    }
    cout << LOH << "Scheduler started..." << endl;
};

/**
 * stop openhaptics
 */
void stopOH()
{

};

/**
 * openhaptics callback for setting the force
 */
HDCallbackCode HDCALLBACK updateOH(void *data)
{
    //HHD hHD = hdGetCurrentDevice();

    HDErrorInfo error;
    /*if (HD_DEVICE_ERROR(error = hdGetError()))
    {
	hduPrintError(stderr, &error, "live_openhaptics> Error during scheduler:");
        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }*/



    hdBeginFrame(g_hHD);

    // calibration necessary?
    CheckCalibration(g_iCalibrationStyle);
    // get transformation
    hdGetFloatv(HD_CURRENT_TRANSFORM, g_pTrans);
    // get buttons
    hdGetIntegerv(HD_CURRENT_BUTTONS, &g_iButtons);
    // set force
    hdSetDoublev(HD_CURRENT_FORCE, g_pForce);
    // set torques
    if (g_b6DOF)
	hdSetDoublev(HD_CURRENT_TORQUE, g_pTorque);
        
    hdEndFrame(g_hHD);

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        //hduPrintError(stderr, &error, "live_openhaptics> Error during scheduler:");
        //if (hduIsSchedulerError(&error))
        //{
            return HD_CALLBACK_DONE;
        //}
    }

    return HD_CALLBACK_DONE;
}

///////////////////////////////////////////////////////////////////////////////
// public functions for l.i.v.e.
///////////////////////////////////////////////////////////////////////////////

/**
 * open the device
 */
extern "C" DLLEXPORT UInt16 devOpen()
{
    initOH();

    return 1;
}

/**
 * close the device
 */
extern "C" DLLEXPORT UInt16 devClose()
{
    stopOH();

    return 1;
}

/**
 * update the device data
 */
extern "C" DLLEXPORT void devUpdate()
{
    /*HDSchedulerHandle gSchedulerCallback =*/ hdScheduleSynchronous( updateOH, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

    //HDSchedulerHandle gSchedulerCallback = hdScheduleAsynchronous(
    //    updateOH, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        //hduPrintError(stderr, &error, "live_openhaptics> failed to schedule callback...");
	exit(-1);
    }

    // copy current information into live data

    int x,y;
    for(x=0;x<4;x++)
        for(y=0;y<4;y++)
            g_cMatrix.mat[y][x] = g_pTrans[x+(y*4)];

    g_cMatrix.mat[3][0] *= 0.01f;
    g_cMatrix.mat[3][1] *= 0.01f;
    g_cMatrix.mat[3][2] *= 0.01f;

    if (g_iButtons & HD_DEVICE_BUTTON_1)
        g_iButton = 1;
    else
        g_iButton = 0;
}

/**
 * chn1 = 4x4 matrix transformation
 * chn2 = int button
 * chn3 = float force x axis
 * chn4 = float force y axis
 * chn5 = float force z axis
 * chn6 = float torque force x axis
 * chn7 = float torque force y axis
 * chn8 = float torque force z axis
 * chn9...chn14 temperature of all 6 motors (0.0f .. 1.0f - 1.0f is very hot)
 */
extern "C" DLLEXPORT UInt16 chnNum()
{
    return 14;
}
extern "C" DLLEXPORT UInt8 chnType(UInt16 chn)
{
    switch(chn)
    {
    case 1:
        return LIVE_CHN_MATRIX;
        break;
    case 2:
        return LIVE_CHN_INT;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return LIVE_CHN_FLOAT;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return LIVE_CHN_FLOAT;
        break;
    default:
        return LIVE_CHN_UNKNOWN;
        break;
    }
}

/**
 * direction is input for channel 1,2 and output for 3-8
 */
extern "C" DLLEXPORT UInt8 chnDir(UInt16 chn)
{
    switch(chn)
    {
    case 1:
    case 2:
        return LIVE_CHN_INPUT;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return LIVE_CHN_OUTPUT;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return LIVE_CHN_INPUT;
        break;
    default:
        return LIVE_CHN_INPUT;
    }
}

/**
 * return the integer data (only valid for the button, chn2)
 */
extern "C" DLLEXPORT Int16 chnGetDataINT(UInt16 chn)
{
    if (chn == 2)
        return g_iButton;
    else
        return 0;
}

/**
 * return the char data (not valid here)
 */
extern "C" DLLEXPORT char chnGetDataCHAR(UInt16 chn)
{
    return 0;
}

/**
 * return the float data
 */
extern "C" DLLEXPORT Real32 chnGetDataFLOAT(UInt16 chn)
{
    if (chn > 8 && chn < 15)
        return 0; /*aTemp[chn-9];*/
    else
        return 0;
}
/**
 * return the matrix data
 */
extern "C" DLLEXPORT Real32* chnGetDataMAT(UInt16 chn)
{
    int x,y;
    for(x=0;x<4;x++)
        for(y=0;y<4;y++)
            g_oOutputMatrix[x+(y*4)] = g_cMatrix.mat[y][x];

    return g_oOutputMatrix;
}

/**
 * the functions for writing data to the device
 */
extern "C" DLLEXPORT void chnSetDataINT(UInt16, Int16){};
extern "C" DLLEXPORT void chnSetDataCHAR(UInt16, char){};

extern "C" DLLEXPORT void chnSetDataFLOAT(UInt16 chn , Real32 val)
{
    if (val > 100.0f)
        val = 100.0f;
    if (val < -100.0f)
        val = -100.0f;

    switch(chn)
    {
    case 3:
        g_pForce[0] =  val;
        break;
    case 4:
        g_pForce[1] =  val;
        break;
    case 5:
        g_pForce[2] =  val;
        break;
    case 6:
        g_pTorque[0] =  val ;
        break;
    case 7:
        g_pTorque[1] =  val ;
        break;
    case 8:
        g_pTorque[2] =  val ;
        break;
    default:
        break;
    }
};

extern "C" DLLEXPORT void chnSetDataMAT(UInt16, Real32*){};
