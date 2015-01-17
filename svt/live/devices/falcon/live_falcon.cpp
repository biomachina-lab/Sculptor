/***************************************************************************
                          live_falcon
                          ----------------
    begin                : 12/18/2007
    author               : Jochen Heyd
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <svt_matrix4f.h>
#include <svt_time.h>

#include "hdl/hdl.h"

///////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////

// Scaling of Falcon input and output
#define FALCON_INPUT_SCALE_LOW     3.0 
#define FALCON_INPUT_SCALE_HIGH   30.0
#define FALCON_FORCE_SCALE         2.0
#define FALCON_Z_AXIS_FORCE_SCALE  1.5

#define LOH svt_trimName("live_falcon")

// Falcon variables
HDLDeviceHandle    g_hHD;                     // Handle to device
HDLServoOpExitCode g_servoOp;                 // Handle to Contact Callback 
double             g_workspaceDims[6];        // Device workspace dimensions
double             g_positionServo[3];        // Device position (in)
int                g_buttonsServo;            // Device buttons (in)
double             g_forceServo[3];           // Device forces (out)
bool               g_scale_button_last;       // Remembers last state
bool               g_input_scale_high = true; // Low or high amount of scaling

// live variables
int g_iButton1 = 0;
int g_iButton2 = 0;
int g_iButton3 = 0;
svt_matrix4f g_cMatrix("position");
Real32 g_oOutputMatrix[16];

///////////////////////////////////////////////////////////////////////////////
// OpenHaptics functions
///////////////////////////////////////////////////////////////////////////////

bool CheckCalibration()
{
    unsigned int state = hdlGetState();

    return ((state & HDAL_NOT_CALIBRATED) == 0);
}

/**
 * calibrate device
 */
void initCalibration()
{

    bool CalibrationStatus = CheckCalibration();

    if (CalibrationStatus != true)
    {
        cout << LOH << endl;
        cout << LOH << "Please extend the knob all the way" << endl;
        cout << LOH << "then push it straight in" << endl;
	cout << LOH << endl;

	for(int i=0; i<20; i++)
	{
	  if (!CheckCalibration()) {
	    svt_sleep(500);
	  } else {
            cout << LOH << "Calibration complete." << endl;
	    cout << LOH << endl;
	    break;
	  }
        }

	if (!CheckCalibration()) {
	  cout << LOH << "Calibration failed" << endl;
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

    HDLError err = HDL_NO_ERROR;

    // Passing "DEFAULT" or 0 initializes the default device based on the
    // [DEFAULT] section of HDAL.INI.   The names of other sections of HDAL.INI
    // could be passed instead, allowing run-time control of different devices
    // or the same device with different parameters.  See HDAL.INI for details.
    g_hHD = hdlInitNamedDevice("DEFAULT");

    err = hdlGetError();
    if (err != HDL_NO_ERROR)
    {
      cout << LOH << "Error initializing device" << endl;
      exit(1);
    }

    if (g_hHD == HDL_INVALID_HANDLE)
    {
        cout << LOH << "Failed to initialize haptic device" << endl;
	exit(1);
    }

    // Print out model
    cout << LOH << "Found device: " << hdlDeviceModel() << endl;

    // Now that the device is fully initialized, start the servo thread.
    // Failing to do this will result in a non-funtional haptics application.
    hdlStart();
    err = hdlGetError();
    if (err != HDL_NO_ERROR)
    {
      cout << LOH << "Error starting servo thread" << endl;
      exit(1);
    }
    cout << LOH << "Servo thread started" << endl;

    // Make the device current.  All subsequent calls will
    // be directed towards the current device.
    hdlMakeCurrent(g_hHD);
    err = hdlGetError();
    if (err != HDL_NO_ERROR)
    {
      cout << LOH << "Error making device current" << endl;
      exit(1);
    }
    cout << LOH << "Device is current" << endl;

    // Get the extents of the device workspace.
    // Used to create the mapping between device and application coordinates.
    // Returned dimensions in the array are minx, miny, minz, maxx, maxy, maxz
    //                                      left, bottom, far, right, top, near)
    // Right-handed coordinates:
    //   left-right is the x-axis, right is greater than left
    //   bottom-top is the y-axis, top is greater than bottom
    //   near-far is the z-axis, near is greater than far
    // workspace center is (0,0,0)
    double maxWorkspace[6];
    hdlDeviceWorkspace(maxWorkspace);
    err = hdlGetError();
    if (err != HDL_NO_ERROR)
    {
      cout << LOH << "Error retrieving workspace dimensions" << endl;
      exit(1);
    }

    // Translate workspace into same sequence as openhaptics
    g_workspaceDims[0] = maxWorkspace[1];
    g_workspaceDims[1] = maxWorkspace[0];
    g_workspaceDims[2] = maxWorkspace[2];
    g_workspaceDims[3] = maxWorkspace[4];
    g_workspaceDims[4] = maxWorkspace[3];
    g_workspaceDims[5] = maxWorkspace[5];

    cout << LOH << "Workspace: "<< endl;
    cout << LOH << "  Low: " << g_workspaceDims[0] << " Left: " << g_workspaceDims[1] << " Back: " << g_workspaceDims[2] << endl;
    cout << LOH << "  Top: " << g_workspaceDims[3] << " Right:" << g_workspaceDims[4] << " Front:" << g_workspaceDims[5] << endl;

    // Run Calibration
    initCalibration();

};

/**
 * stop openhaptics
 */
void stopOH()
{

    if (g_servoOp != HDL_INVALID_HANDLE)
    {
        cout << LOH << "Stopping servoOp" << endl;
        hdlDestroyServoOp(g_servoOp);
        g_servoOp = HDL_INVALID_HANDLE;
    }
    hdlStop();
    if (g_hHD != HDL_INVALID_HANDLE)
    {
        cout << LOH << "Uninit device" << endl;
        hdlUninitDevice(g_hHD);
        g_hHD = HDL_INVALID_HANDLE;
    }
};

/**
 * openhaptics callback for setting the force
 */
HDLServoOpExitCode updateOH(void *data)
{
    // calibration necessary?
    if (!CheckCalibration()) {
      cout << LOH << "In servo callback and calibration != true" << endl;
      exit(1);
    }

    // Get current state of haptic device
    hdlToolPosition(g_positionServo);
    hdlToolButtons(&g_buttonsServo);

    // Send forces to device
    hdlSetToolForce(g_forceServo);

    return HDL_SERVOOP_EXIT;
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

    g_servoOp = hdlCreateServoOp(updateOH, NULL, /* Blocking */ true);
    if (g_servoOp == HDL_INVALID_HANDLE)
    {
      cout << LOH << "Invalid servo op handle" << endl;
      exit(1);
    }
    HDLError err = hdlGetError();
    if (err != HDL_NO_ERROR)
    {
      cout << LOH << "Error in hdlCreateServoOp" << endl;
      exit(1);
    }

    // copy current information into live data

    {
      int i,j;
      for(i=0; i<4; i++)
	for(j=0; j<4; j++)
	  if (i==j) {
	    g_cMatrix.mat[i][j] = 1;
	  } else {
	    g_cMatrix.mat[i][j] = 0;
	  }
    }

    if (g_input_scale_high == true) {
      g_cMatrix.mat[3][0] = FALCON_INPUT_SCALE_HIGH * g_positionServo[0];
      g_cMatrix.mat[3][1] = FALCON_INPUT_SCALE_HIGH * g_positionServo[1];
      g_cMatrix.mat[3][2] = FALCON_INPUT_SCALE_HIGH * g_positionServo[2];
    } else {
      g_cMatrix.mat[3][0] = FALCON_INPUT_SCALE_LOW * g_positionServo[0];
      g_cMatrix.mat[3][1] = FALCON_INPUT_SCALE_LOW * g_positionServo[1];
      g_cMatrix.mat[3][2] = FALCON_INPUT_SCALE_LOW * g_positionServo[2];
    }

    if (g_buttonsServo & HDL_BUTTON_3) {
      // Button 3 switches input scale
      g_scale_button_last = true;
    } else {
      if(g_scale_button_last == true) {
	// Got release event -> switch resolution
	g_scale_button_last = false;
	if (g_input_scale_high == true)
	  g_input_scale_high = false;
	else
	  g_input_scale_high = true;
      }
    }

    // All other buttons are passed on
    if (g_buttonsServo & HDL_BUTTON_1)
	g_iButton1 = 1;
    else 
	g_iButton1 = 0;

    if (g_buttonsServo & HDL_BUTTON_2)
	g_iButton2 = 1;
    else 
	g_iButton2 = 0;

    if (g_buttonsServo & HDL_BUTTON_4)
	g_iButton3 = 1;
    else 
	g_iButton3 = 0;


}

/**
 * chn1 = 4x4 matrix transformation
 * chn2 = int button 1
 * chn3 = int button 2
 * chn4 = int button 3
 * chn5 = float force x axis
 * chn6 = float force y axis
 * chn7 = float force z axis
 */
extern "C" DLLEXPORT UInt16 chnNum()
{
    return 7;
}
extern "C" DLLEXPORT UInt8 chnType(UInt16 chn)
{
    switch(chn)
    {
    case 1:
        return LIVE_CHN_MATRIX;
        break;
    case 2:
    case 3:
    case 4:
        return LIVE_CHN_INT;
        break;
    case 5:
    case 6:
    case 7:
        return LIVE_CHN_FLOAT;
        break;
    default:
        return LIVE_CHN_UNKNOWN;
        break;
    }
}

/**
 * direction is input for channel 1-4 and output for 5-7
 */
extern "C" DLLEXPORT UInt8 chnDir(UInt16 chn)
{
    switch(chn)
    {
    case 1:
    case 2:
    case 3:
    case 4:
        return LIVE_CHN_INPUT;
        break;
    case 5:
    case 6:
    case 7:
        return LIVE_CHN_OUTPUT;
        break;
    default:
        return LIVE_CHN_INPUT;
    }
}

/**
 * return the integer data (only valid for the buttons, chn 2-4)
 */
extern "C" DLLEXPORT Int16 chnGetDataINT(UInt16 chn)
{
    switch(chn)
    {
    case 2:
        return g_iButton1;
        break;
    case 3:
        return g_iButton2;
        break;
    case 4:
        return g_iButton3;
        break;
    default:
        return 0;
    }
}

/**
 * return the char data (not valid here)
 */
extern "C" DLLEXPORT char chnGetDataCHAR(UInt16 chn)
{
    return 0;
}

/**
 * return the float data (not valid here)
 */
extern "C" DLLEXPORT Real32 chnGetDataFLOAT(UInt16 chn)
{
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

    // Force is in "percent"

    val *= FALCON_FORCE_SCALE;

    switch(chn)
    {
    case 5:
        g_forceServo[0] =  val;
        break;
    case 6:
        g_forceServo[1] =  val;
        break;
    case 7:
        g_forceServo[2] =  FALCON_Z_AXIS_FORCE_SCALE * val;
        break;
    default:
        break;
    }
};

extern "C" DLLEXPORT void chnSetDataMAT(UInt16, Real32*){};
