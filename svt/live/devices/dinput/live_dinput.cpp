/***************************************************************************
                          live_dinput.cpp
                          ------------------
    begin                : 19.06.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <live_4x4_matrix.h>

#include "live_dinput.h"

#include <windows.h>
#include <windowsx.h>
#include <dinput.h>

#include <svt_iostream.h>

///////////////////////////////////////////////////////////////////////////////
// variables
///////////////////////////////////////////////////////////////////////////////

HINSTANCE              g_sHinst;           // Main hinstance of app
HWND                   g_sHwnd;            // Handle of window

LPDIRECTINPUT          g_pDi;              // IDirectInput Interface
LPDIRECTINPUTDEVICE    g_pDev;               // IDirectInputDevice Interface
LPDIRECTINPUTDEVICE2   g_pJoystick = NULL;        // IDirectInputDevice2 Interface
LPDIRECTINPUTEFFECT    g_pEffect;          // IDirectInputEffect Interface
DIEFFECT               g_sEff;                // DIEffect Structure

DWORD                  rgdwAxes[3]     = {DIJOFS_X, DIJOFS_Y, DIJOFS_Z};
LONG                   rglDirection[3] = { 0, 0, 0 };

DICONSTANTFORCE        g_sCf = { DI_FFNOMINALMAX };

LONG                   g_lConst = 1000;
DICONDITION            g_sCn = { 1000, 10000, 10000, 10000, 10000, 0 };

live_4x4_matrix        g_cMatrix("position");

Int16                  g_iButton = 0;
Real32                 g_fForceX = 0.0f;
Real32                 g_fForceY = 0.0f;
Real32                 g_fForceZ = 0.0f;

Real32                 g_fScaleX = 0.005f;
Real32                 g_fScaleY = 0.005f;
Real32                 g_fScaleZ = 0.005f;

int                    g_iFFAxes = 2;
bool                   g_bSwitchYZ = true;

///////////////////////////////////////////////////////////////////////////////
// helper functions
///////////////////////////////////////////////////////////////////////////////

void __acquireDevice()
{
    HRESULT hr;

    // acquire the joystick
    hr = g_pJoystick->Acquire();

    if (!FAILED(hr))
        return;

    switch(hr)
    {
    case S_FALSE:
        cout << "live_dinput: cannot acquire the di device, because some other program has acquired the device. Fatal Error!" << endl;
        exit(1);

    case DIERR_INVALIDPARAM:
        cout << "live_dinput: DIERR_INVALIDPARAM." << endl;
        exit(1);

    case DIERR_NOTINITIALIZED:
        cout << "live_dinput: DIERR_NOTINITIALIZED." << endl;
        exit(1);

    case DIERR_OTHERAPPHASPRIO:
        cout << "live_dinput: DIERR_OTHERAPPHASPRIO." << endl;
        exit(1);

    case DI_OK:
        cout << "live_dinput: ok, device acquired." << endl;
        break;

    case DIERR_NOINTERFACE:
        cout << "live_dinput: DIERR_NOINTERFACE" << endl;
        exit(1);

    default:
        cout.setf(ios::hex, ios::basefield);
        cout << "live_dinput: Acquire result: " << hr << endl;

        cout << "DIERR_OLDDIRECTINPUTVERSION:" << DIERR_OLDDIRECTINPUTVERSION << endl;
        cout << "DIERR_BETADIRECTINPUTVERSION:" << DIERR_BETADIRECTINPUTVERSION << endl;
        cout << "DIERR_BADDRIVERVER:" << DIERR_BADDRIVERVER << endl;
        cout << "DIERR_DEVICENOTREG:" << DIERR_DEVICENOTREG << endl;
        cout << "DIERR_NOTFOUND:" << DIERR_NOTFOUND << endl;
        cout << "DIERR_OBJECTNOTFOUND:" << DIERR_OBJECTNOTFOUND << endl;
        cout << "DIERR_NOINTERFACE: " << DIERR_NOINTERFACE << endl;
        cout << "DIERR_GENERIC:" << DIERR_GENERIC << endl;
        cout << "DIERR_OUTOFMEMORY:" << DIERR_OUTOFMEMORY << endl;
        cout << "DIERR_UNSUPPORTED:" << DIERR_UNSUPPORTED << endl;
        cout << "DIERR_NOTINITIALIZED:" << DIERR_NOTINITIALIZED << endl;
        cout << "DIERR_ALREADYINITIALIZED:" << DIERR_ALREADYINITIALIZED << endl;
        cout << "DIERR_NOAGGREGATION:" << DIERR_NOAGGREGATION << endl;
        cout << "DIERR_INPUTLOST:" << DIERR_INPUTLOST << endl;
        cout << "DIERR_ACQUIRED:" << DIERR_ACQUIRED << endl;
        cout << "DIERR_NOTACQUIRED:" << DIERR_NOTACQUIRED << endl;
        cout << "DIERR_READONLY:" << DIERR_READONLY << endl;
        cout << "DIERR_HANDLEEXISTS:" << DIERR_HANDLEEXISTS << endl;
        exit(1);
        break;
    }
}

void enumEffects()
{
}

// set the forces
void __setForces(float x, float y, float z)
{
    float _tx = x * (0.01f * DI_FFNOMINALMAX);
    float _ty = y * (0.01f * DI_FFNOMINALMAX);
    float _tz = z * (0.01f * DI_FFNOMINALMAX);

    if(g_bSwitchYZ)
    {
        float tmp = _tz;
        _tz = _ty;
        _ty = tmp;
    }

    g_sCf.lMagnitude = sqrt(_tx*_tx + _ty*_ty + _tz*_tz);

    float len = sqrt(_tx*_tx + _ty*_ty + _tz*_tz);
    g_sCn.lOffset = (LONG)(len * 500);
    _tx /= len;
    _ty /= len;
    _tz /= len;
    LONG rglDirection[3] = { (LONG)_tx, (LONG)_ty, (LONG)_tz };

    g_sEff.dwSize = sizeof(DIEFFECT);
    g_sEff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    g_sEff.cAxes = g_iFFAxes;
    g_sEff.rglDirection = rglDirection;
    g_sEff.lpEnvelope = 0;
    g_sEff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    g_sEff.lpvTypeSpecificParams = &g_sCf;

    HRESULT hr = g_pEffect->SetParameters(&g_sEff, DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS);
    //g_pEffect->Start(1, 0);

    if (FAILED(hr))
    {
        cout << "live_dinput: failed to set the forces. Fatal Error!" << endl;
        exit(1);
    }
}


// this function is the filter enumeration function for DI. It stops with the first joystick.
BOOL CALLBACK DInput_Enum_Joysticks( LPCDIDEVICEINSTANCE lpDDI, LPVOID guid_ptr)
{
    // create device com object
    if (FAILED(g_pDi->CreateDevice(lpDDI->guidInstance, &g_pDev, NULL)))
    {
        cout << "live_dinput: cannot create DirectInput device. Fatal Error!" << endl;
        exit(1);
    }

    // now we have a direct input device, but we need a DI2 Device (new version of the interface).
    if (FAILED(g_pDev->QueryInterface(IID_IDirectInputDevice2, (LPVOID *)&g_pJoystick)))
    {
        cout << "live_dinput: failed to create a di2 device. Fatal Error!" << endl;
        exit(1);
    }

    // print the joystick name
    cout << "live_dinput: found a joystick:" << lpDDI->tszProductName << endl;

    // stop with the first joystick
    return (DIENUM_STOP);
}

void __initDI()
{
    // create main com object
    if (FAILED(DirectInputCreate(g_sHinst, DIRECTINPUT_VERSION, &g_pDi, NULL)))
    {
        cout << "live_dinput: cannot create DirectInput main COM interface. Fatal Error!" << endl;
        exit(1);
    }

    // enumerate the joysticks
    if (FAILED(g_pDi->EnumDevices(
                                  DIDEVTYPE_JOYSTICK,
                                  DInput_Enum_Joysticks,
                                  NULL,
                                  DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK
                                 )))
    {
        cout << "live_dinput: cannot find a joystick device in the system. Fatal Error!" << endl;
        exit(1);
    }

    // release old device
    if (g_pDev)
        g_pDev->Release();

    // really got a joystick?
    if (g_pJoystick == NULL)
    {
        cout << "live_dinput: cannot find a joystick device in the system. Fatal Error!" << endl;
        exit(1);
    }


    // set the cooperative level
    if (FAILED(g_pJoystick->SetCooperativeLevel(g_sHwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND)))
    {
        cout << "live_dinput: cannot set the cooperative level for the device. FatalError!" << endl;
        exit(1);
    }

    // set the joystick data format
    if (FAILED(g_pJoystick->SetDataFormat(&c_dfDIJoystick2)))
    {
        cout << "live_dinput: cannot set the data format for the device. FatalError!" << endl;
        exit(1);
    }

    // set the properties for the device
    /*DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = FALSE;
    if (FAILED(g_pJoystick->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph)))
    {
        cout << "live_dinput: cannot set the properties for the device. Fatal Error!" << endl;
        exit(1);
    }*/

    DIPROPRANGE joy_range;
    joy_range.lMin = -100;
    joy_range.lMax = 100;
    joy_range.diph.dwSize = sizeof(DIPROPRANGE);
    joy_range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    joy_range.diph.dwObj = DIJOFS_X;
    joy_range.diph.dwHow = DIPH_BYOFFSET;
    if (FAILED(g_pJoystick->SetProperty(DIPROP_RANGE, &joy_range.diph)))
    {
        cout << "live_dinput: cannot set the properties for the device. Fatal Error!" << endl;
        exit(1);
    }

    joy_range.lMin = -100;
    joy_range.lMax = 100;
    joy_range.diph.dwSize = sizeof(DIPROPRANGE);
    joy_range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    joy_range.diph.dwObj = DIJOFS_Y;
    joy_range.diph.dwHow = DIPH_BYOFFSET;
    if (FAILED(g_pJoystick->SetProperty(DIPROP_RANGE, &joy_range.diph)))
    {
        cout << "live_dinput: cannot set the properties for the device. Fatal Error!" << endl;
        exit(1);
    }

    // create effect
    g_sEff.dwSize =          sizeof(DIEFFECT);
    g_sEff.dwFlags =         DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    g_sEff.dwDuration =      INFINITE;
    g_sEff.dwSamplePeriod =  0;
    g_sEff.dwGain =          DI_FFNOMINALMAX;
    g_sEff.dwTriggerButton = DIEB_NOTRIGGER;
    g_sEff.cAxes =           g_iFFAxes;
    g_sEff.rgdwAxes =        rgdwAxes;
    g_sEff.rglDirection =    rglDirection;
    g_sEff.lpEnvelope =      0;
    g_sEff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    g_sEff.lpvTypeSpecificParams = &g_sCf;

    if (FAILED(g_pJoystick->CreateEffect(GUID_ConstantForce, &g_sEff, &g_pEffect, NULL)))
    {
        cout << "live_dinput: failed to create the constant force effect. Fatal Error!" << endl;
        exit(1);
    }

    __acquireDevice();

    g_pEffect->Start(1, 0);

    __setForces(0.0f, 0.0f, 0.0f);

    // check status of the device
    DWORD res;
    if (FAILED(g_pJoystick->GetForceFeedbackState((LPDWORD)&res)))
    {
        cout << "live_dinput: cannot ask for ff state." << endl;
    } else {
        if (res & DIGFFS_POWERON)
            cout << "live_dinput: device is on!" << endl;
        if (res & DIGFFS_SAFETYSWITCHON)
            cout << "live_dinput: safetyswitch ok. " << endl;
        if (res & DIGFFS_DEVICELOST)
            cout << "live_dinput: huuups, device lost!" << endl;
        if (res & DIGFFS_EMPTY)
            cout << "live_dinput: no effects loaded." << endl;
        if (res & DIGFFS_ACTUATORSON)
            cout << "live_dinput: ok, actuators are on" << endl;
        if (res & DIGFFS_PAUSED)
            cout << "live_dinput: effects are paused???!!" << endl;
        if (res & DIGFFS_STOPPED)
            cout << "live_dinput: device stopped!" << endl;
        if (res & DIGFFS_USERFFSWITCHON)
            cout << "live_dinput: user ff switch on!" << endl;
    }
}

void __exitDI()
{
/*    // release effect
    if (g_pEffect)
        g_pEffect->Release();

    // release joystick
    if (g_pJoystick)
    {
        g_pJoystick->Unacquire();
        g_pJoystick->Release();
    }

    // release main COM object
    if (g_pDi)
        g_pDi->Release();*/
}

void __updateForces()
{
    __setForces(g_fForceX, g_fForceY, g_fForceZ);
}

void __updateCoords()
{
    if (FAILED(g_pJoystick->Poll()))
    {
        cout << "live_dinput: failed to poll of the di device. Fatal Error!" << endl;
        exit(1);
    }

    DIJOYSTATE2 joystate;
    HRESULT hr;
    hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), (LPVOID)&joystate);

    if (hr != DI_OK)
    {
        if (hr == DIERR_NOTACQUIRED)
        {
            __acquireDevice();
            cout << "live_dinput: Must reacquire the device" << endl;
            g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&joystate);
        } else {
            cout << "live_dinput: failed to get the device state of the di device. Fatal Error!" << endl;
            exit(1);
        }
    }

    g_iButton = (Int16)(joystate.rgbButtons[0]);
    if (g_iButton > 0)
        g_iButton = 1;

    if (g_bSwitchYZ)
        g_cMatrix.setTranslation(((Real32)joystate.lX)*g_fScaleX,((Real32)joystate.lZ)*g_fScaleZ, ((Real32)joystate.lY)*g_fScaleY);
    else
        g_cMatrix.setTranslation(((Real32)joystate.lX)*g_fScaleX,((Real32)joystate.lY)*g_fScaleY, ((Real32)joystate.lZ)*g_fScaleZ);
}

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

/** open the device */
UInt16 devOpen()
{
    g_cMatrix.setId();

    __initDI();

    return 1;
}

/** close the device */
UInt16 devClose()
{
    return 1;
}

/** update the device data */
void devUpdate()
{
    __updateForces();
    __updateCoords();
}


/** chn1 = 4x4 matrix transformation
 chn2 = int button
 chn3 = float force x axis
 chn4 = float force y axis
 chn5 = float force z axis
 chn6 = float torque force x axis
 chn7 = float torque force y axis
 chn8 = float torque force z axis
 */
UInt16 chnNum()
{
    return 8;
}


UInt8 chnType(UInt16 chn)
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
    default:
        return LIVE_CHN_UNKNOWN;
        break;
    }
}

/** direction is input for all channels */
DLLEXPORT UInt8 chnDir(UInt16 chn)
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
    default:
        return LIVE_CHN_INPUT;
    }
}

/* return the integer data (only valid for the button, chn2) */
DLLEXPORT Int16 chnGetDataINT(UInt16 chn)
{
    if (chn == 2)
        return g_iButton;
    else
        return 0;
}

/* return the char data (not valid here) */
DLLEXPORT char chnGetDataCHAR(UInt16 chn)
{
    return 0;
}

/* return the float data (not valid here) */
DLLEXPORT Real32 chnGetDataFLOAT(UInt16 chn)
{
    return 0;
}
/* return the matrix data (only valid for chn1,2,3,4) */
DLLEXPORT Real32* chnGetDataMAT(UInt16 chn)
{
    return (Real32*)&g_cMatrix.mat;
}

/* the functions for writing data to the device (not valid for a mouse) */
extern "C" void chnSetDataINT(UInt16, Int16){};
extern "C" void chnSetDataCHAR(UInt16, char){};

extern "C" void chnSetDataFLOAT(UInt16 chn , Real32 val)
{
    switch(chn)
    {
    case 3:
        g_fForceX = val;
        break;
    case 4:
        g_fForceY = val;
        break;
    case 5:
        g_fForceZ = val;
        break;
    default:
        break;
    }
};

extern "C" void chnSetDataMAT(UInt16, Real32*){};

///////////////////////////////////////////////////////////////////////////////
// window
///////////////////////////////////////////////////////////////////////////////

char g_aClassName[] = "Live_DINPUT";

LRESULT CALLBACK __wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void __createWindow()
{
    WNDCLASS wc;

    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon   = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_aClassName;
    wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
    wc.hInstance = g_sHinst;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = __wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    if (!RegisterClass(&wc))
    {
        cout << "live_dinput: cannot register window class. Fatal Error!" << endl;
        exit(1);
    }

    g_sHwnd = CreateWindow(g_aClassName,
                           "live_dinput",
                           WS_MINIMIZE | WS_DISABLED,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL,
                           NULL,
                           g_sHinst,
                           0
                          );

    ShowWindow(g_sHwnd, SW_SHOW);
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    g_sHinst = hInstance;

    if (dwReason == DLL_PROCESS_ATTACH)
        __createWindow();
    else if (dwReason == DLL_PROCESS_DETACH)
        __exitDI();

    return 1;
}
