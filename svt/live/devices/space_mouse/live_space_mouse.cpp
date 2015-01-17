/***************************************************************************
                          live_space_mouse.cpp
                          --------------------
    begin                : 11.04.2001 (linux), 13.09.2005 (win)
    author               : Stefan Birmanns, Maik Boltes
    email                : sculptor@biomachina.org, sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <svt_matrix4.h>
#include <svt_init.h>

#include <stdio.h>

/******************************************************************************
** variables
******************************************************************************/

/* Program Specific Includes */

Real32 xpos =0.;
Real32 ypos =0.;
Real32 zpos =0.;
Real32 xrot =0.;
Real32 yrot =0.;
Real32 zrot =0.;

Int16 buttons[10];

Int16 no_trans = 0;
Int16 no_rot   = 0;
Int16 abs_mode = 0;

svt_matrix4<Real32> mat;
svt_matrix4<Real32> rotx_mat;
svt_matrix4<Real32> roty_mat;
svt_matrix4<Real32> rotz_mat;

Real32 g_aSMMat[4][4];

#ifndef WIN32

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

extern "C" {
#include "xdrvlib.h"
}

/******************************************************************************
** variables
******************************************************************************/

Display* display;
Window win, root;

MagellanFloatEvent MagellanEvent;
char MagellanBuffer[ 256 ];
XEvent report;
double MagellanSensitivity = 1.0f;

#define EVENT_MASK ExposureMask | StructureNotifyMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask

/******************************************************************************
** device functions
******************************************************************************/

/* open the device */
extern "C" Int16 devOpen()
{
    if ((display = XOpenDisplay(":0.0")) == NULL)
    {
        printf("live_space_mouse: failed to connect to X server!\n");
        return 0;
    }

    root = XRootWindow(display, DefaultScreen(display));

    win = XCreateSimpleWindow( display, root, 0,0, 10,10, 20,
			       BlackPixel(display,DefaultScreen(display)),
			       WhitePixel(display,DefaultScreen(display)));

    if ( !MagellanInit( display, win ) )
    {
        fprintf( stderr, "live_space_mouse: no space mouse driver is running. Exit ... \n" );
        return 0;
    }

    XSelectInput( display, win, KeyPressMask | KeyReleaseMask );

    mat.setTranslation( 0.0f, 0.0f, 0.0f );
    rotx_mat.setTranslation( 0.0f, 0.0f, 0.0f );
    roty_mat.setTranslation( 0.0f, 0.0f, 0.0f );
    rotz_mat.setTranslation( 0.0f, 0.0f, 0.0f );

    return 1;
}

/* close the device */
extern "C" Int16 devClose()
{
    return 1;
}

/* update the device data */
extern "C" void devUpdate()
{
    if (XPending(display))
    {
        XNextEvent( display, &report );
        switch( report.type )
        {
        case ClientMessage :
            switch( MagellanTranslateEvent( display, &report, &MagellanEvent, 1.0, 1.0 ) )
            {
            case MagellanInputMotionEvent :
                MagellanRemoveMotionEvents( display );

                if (!abs_mode)
                {
                    if (!no_trans)
                    {
                        xpos = MagellanEvent.MagellanData[ MagellanX ] / 10000;
                        ypos = MagellanEvent.MagellanData[ MagellanY ] / 10000;
                        zpos = -MagellanEvent.MagellanData[ MagellanZ ] / 10000;
                    }
                    if (!no_rot)
                    {
                        xrot = MagellanEvent.MagellanData[ MagellanA ] / 100.0f;
                        yrot = MagellanEvent.MagellanData[ MagellanB ] / 100.0f;
                        zrot = -MagellanEvent.MagellanData[ MagellanC ] / 100.0f;
                    }
                } else {
                    mat.loadIdentity();
                    if (!no_trans)
                    {
                        xpos = MagellanEvent.MagellanData[ MagellanX ] / 100;
                        ypos = MagellanEvent.MagellanData[ MagellanY ] / 100;
                        zpos = -MagellanEvent.MagellanData[ MagellanZ ] / 100;
                    }
                    if (!no_rot)
                    {
                        xrot = MagellanEvent.MagellanData[ MagellanA ];
                        yrot = MagellanEvent.MagellanData[ MagellanB ];
                        zrot = -MagellanEvent.MagellanData[ MagellanC ];
                    }
                }

                // init rotation matrices
                rotx_mat.loadIdentity();
                roty_mat.loadIdentity();
                rotz_mat.loadIdentity();
                rotx_mat.rotate( 0, (xrot*PI)/180.0f );
                roty_mat.rotate( 1, (yrot*PI)/180.0f );
                rotz_mat.rotate( 2, (zrot*PI)/180.0f );
                // multiply them
                mat *= rotx_mat;
                mat *= roty_mat;
                mat *= rotz_mat;
                // and add translation
                mat.translate( xpos, ypos, zpos );

                break;

            case MagellanInputButtonPressEvent :
                buttons[ MagellanEvent.MagellanButton-1 ] = 1;
                switch ( MagellanEvent.MagellanButton )
                {
                case 2:
                    buttons[0] = (buttons[0]==0) ? 1 : 0;
                    printf("live_space_mouse: Toggle locking picker\n");
                    break;
                case 5: 
                    MagellanApplicationSensitivity( display, MagellanSensitivity / 2.0 ); 
                    MagellanSensitivity = MagellanSensitivity <= 1.0/32.0 ? 1.0/32.0 : MagellanSensitivity/2.0;
                    break;
                case 6: 
                    MagellanApplicationSensitivity( display, MagellanSensitivity * 2.0 ); 
                    MagellanSensitivity = MagellanSensitivity >= 32.0 ? 32.0 : MagellanSensitivity*2.0;
                    break;
                case 7: 
                    MagellanApplicationSensitivity( display, 1.0 ); 
                    MagellanSensitivity = 1.0;
                    break;
                };

                switch( MagellanEvent.MagellanButton )
                {
                case 5:
                case 6:
                case 7:
                    printf("live_space_mouse: Application Sensitivity = %lf \n", MagellanSensitivity );
                    break;
                };
                break;

            case MagellanInputButtonReleaseEvent :
                buttons[ MagellanEvent.MagellanButton-1 ] = 0;
                switch( MagellanEvent.MagellanButton )
                {
                case 3:
                    if (no_trans)
                    {
                        printf("live_space_mouse: translation enabled!\n");
                        no_trans = 0;
                    } else {
                        printf("live_space_mouse: translation disabled!\n");
                        no_trans = 1;
                    }
                    break;
                case 4:
                    if (no_rot)
                    {
                        printf("live_space_mouse: rotation enabled!\n");
                        no_rot = 0;
                    } else {
                        printf("live_space_mouse: rotation disabled!\n");
                        no_rot = 1;
                    }
                    break;
                case 8:
                    if (abs_mode)
                    {
                        printf("live_space_mouse: relative mode!\n");
                        abs_mode = 0;
                    } else {
                        printf("live_space_mouse: absolute mode!\n");
                        abs_mode = 1;
                    }
                    break;
                }
                break;

            default : /* another ClientMessage event */
                break;
            }
            break;
        }
    }
}

#else

char g_aClassName[] = "Live_SPACE_MOUSE";
HINSTANCE              g_sHinst;           // Main hinstance of app
HWND                   g_sHwnd;            // Handle of window

//#include <svt_window.h>
//extern HWND svt_getHWNDFromWin(svt_win_handle); //system: svt_windows_win32.cpp

// SpaceWare Specific Includes
extern "C" {
#include "si.h"        /* Required for any SpaceWare support within an app.*/
#include "siapp.h"     /* Required for siapp.lib symbols */
}

/******************************************************************************
** variables
******************************************************************************/

/* Program Specific Includes */

SiHdl       devHdl = NULL;       /* Handle to Spaceball Device */

double sensitivity = 1.0f;

/******************************************************************************
** device functions
******************************************************************************/

/* open the device */
extern "C" DLLEXPORT Int16 devOpen()
{
    SiOpenData oData;                    /* OS Independent data to open ball  */ 
  
    /* init the SpaceWare input library */
    if (SiInitialize() == SPW_DLL_LOAD_ERROR)  
    {
        fprintf( stderr, "live_space_mouse: Could not load SiAppDll dll files. Exit ... \n" );
        return 0;
	 }

    SiOpenWinInit(&oData, g_sHwnd);    /* svt_getHWNDFromWin(svt_getCurrentWindow()); init Win. platform specific data  */

    /* open data, which will check for device type and return the device handle
       to be used by this function */ 
    if ((devHdl = SiOpen("live_space_mouse", SI_ANY_DEVICE, SI_NO_MASK,  
                          SI_EVENT, &oData)) == NULL) 
    {
        SiTerminate();  /* called to shut down the SpaceWare input library */
        fprintf( stderr, "live_space_mouse: no space mouse driver is running. Exit ... \n" );
        return 0;
    }

    mat.setTranslation( 0.0f, 0.0f, 0.0f );
    rotx_mat.setTranslation( 0.0f, 0.0f, 0.0f );
    roty_mat.setTranslation( 0.0f, 0.0f, 0.0f );
    rotz_mat.setTranslation( 0.0f, 0.0f, 0.0f );

    printf("live_space_mouse: init space mouse successful\n");
    return 1; /* opened device succesfully */ 
}

/* close the device */
extern "C" DLLEXPORT Int16 devClose()
{
//     if (devHdl)
//         SiClose(devHdl);
    return 1;
}

/* update the device data */
extern "C" DLLEXPORT void devUpdate()
{
    int            num;      /* number of button pressed */
    MSG            msg;      /* incoming message to be evaluated */
    BOOL           handled;  /* is message handled yet */ 
    SiSpwEvent     Event;    /* SpaceWare Event */ 
    SiGetEventData EData;    /* SpaceWare Event Data */
    handled = SPW_FALSE;     /* init handled */
    bool svtScene = false;   /* native svt scene or not */
    bool msgAvailable;       /* is message available */

    // to check, if a native svt scene or a device server uses the space mouse device
    if (svt_getScene())
    {
        svtScene = true;
        // client-server would hang by waiting for event
        msgAvailable = GetMessage(&msg, NULL, 0, 0);
    }
    else
    {
        msgAvailable = PeekMessage(&msg, NULL, 0, 0 , PM_REMOVE); //g_sHwnd PM_REMOVE PM_NOREMOVE
    }

    if (msgAvailable)
    {
        /* init Window platform specific data for a call to SiGetEvent */
        SiGetEventWinInit(&EData, msg.message, msg.wParam, msg.lParam);
       
        /* check whether msg was a Spaceball event and process it */
        if (SiGetEvent (devHdl, 0, &EData, &Event) == SI_IS_EVENT)
        {
            if (Event.type == SI_MOTION_EVENT)
            {
                if (!abs_mode)
                {
                    if (!no_trans)
                    {
                        xpos = Event.u.spwData.mData[SI_TX] / 10000.0f;
                        ypos = Event.u.spwData.mData[SI_TY] / 10000.0f;
                        zpos = -Event.u.spwData.mData[SI_TZ] / 10000.0f;
                    }
                    if (!no_rot)
                    {
                        xrot = Event.u.spwData.mData[SI_RX] / 100.0f;
                        yrot = Event.u.spwData.mData[SI_RY] / 100.0f;
                        zrot = -Event.u.spwData.mData[SI_RZ] / 100.0f;
                    }
                } else {
                    mat.loadIdentity();
                    if (!no_trans)
                    {
                        xpos = 2*Event.u.spwData.mData[SI_TX] / 1000.0f;
                        ypos = 2*Event.u.spwData.mData[SI_TY] / 1000.0f;
                        zpos = -2*Event.u.spwData.mData[SI_TZ] / 1000.0f;
                    }
                    if (!no_rot)
                    {
                        xrot = 2*Event.u.spwData.mData[SI_RX] / 10.0f;
                        yrot = 2*Event.u.spwData.mData[SI_RY] / 10.0f;
                        zrot = -2*Event.u.spwData.mData[SI_RZ] / 10.0f;
                    }
                }

                // init rotation matrices
                rotx_mat.loadIdentity();
                roty_mat.loadIdentity();
                rotz_mat.loadIdentity();
                // init rotation matrices
                rotx_mat.rotate( 0, (sensitivity*xrot*PI)/180.0f );
                roty_mat.rotate( 1, (sensitivity*yrot*PI)/180.0f );
                rotz_mat.rotate( 2, (sensitivity*zrot*PI)/180.0f );
                // multiply them
                mat *= rotx_mat;
                mat *= roty_mat;
                mat *= rotz_mat;
                // and add translation
                mat.translate( sensitivity*xpos, sensitivity*ypos, sensitivity*zpos );

            }
            //            if (Event.type == SI_ZERO_EVENT)
            //            {
            //                SbZeroEvent();                /* process Spaceball zero event */     
            //            }
            if (Event.type == SI_BUTTON_EVENT)
            {
                /* process Spaceball button press event */
                if ((num = SiButtonPressed (&Event)) != SI_NO_BUTTON)	
                {
                    buttons[ num-1 ] = 1;
                    switch ( num )
                    {
                    //case SI_PICK_BUTTON, SI_RESET_BUTTON, SI_DIALOG_BUTTON:
                    case 2:
                        buttons[0] = (buttons[0]==0) ? 1 : 0;
                        printf("live_space_mouse: Toggle locking picker\n");
                        break;
                    case 5: 
                        sensitivity = sensitivity <= 1.0/32.0 ? 1.0/32.0 : sensitivity/2.0;
                        break;
                    case 6: 
                        sensitivity = sensitivity >= 32.0 ? 32.0 : sensitivity*2.0;
                        break;
                    case 7: 
                        sensitivity = 1.0;
                        break;
                    };
                    switch ( num )
                    {
                    case 5:
                    case 6:
                    case 7:
                        printf("live_space_mouse: Application Sensitivity = %lf \n", sensitivity );
                        break;
                    }
                }
                /* process Spaceball button release event */
                if ((num = SiButtonReleased (&Event)) != SI_NO_BUTTON)	
                {
                    buttons[ num-1 ] = 0;
                    switch( num )
                    {
                    case 3:
                        if (no_trans)
                        {
                            printf("live_space_mouse: translation enabled!\n");
                            no_trans = 0;
                        } else {
                            printf("live_space_mouse: translation disabled!\n");
                            no_trans = 1;
                        }
                        break;
                    case 4:
                        if (no_rot)
                        {
                            printf("live_space_mouse: rotation enabled!\n");
                            no_rot = 0;
                        } else {
                            printf("live_space_mouse: rotation disabled!\n");
                            no_rot = 1;
                        }
                        break;
                    case 8:
                        if (abs_mode)
                        {
                            printf("live_space_mouse: relative mode!\n");
                            abs_mode = 0;
                        } else {
                            printf("live_space_mouse: absolute mode!\n");
                            abs_mode = 1;
                        }
                        break;
                    }
                }
            }
           
            handled = SPW_TRUE;              /* spaceball event handled */ 
        }
       
        /* not a Spaceball event, let windows handle it */
        if (svtScene && (handled == SPW_FALSE))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
}

#endif

/******************************************************************************
** channel functions
******************************************************************************/

/* chn1 = position of the mouse as a matrix (4x4) */
// nr:button meanings: (channel = nr+1 = button[nr-1])
// 1: pick button from application
// 2: lock pick button
// 3: toggle enable/disable translation
// 4: toggle enable/disable rotation
// 5: decrease sensitivity
// 6: increase sensitivity
// 7: reset sensitivity
// 8: toggle absolute mode
extern "C" DLLEXPORT UInt16 chnNum()
{
    return 11;
}

extern "C" DLLEXPORT UInt8 chnType(int chn)
{
    if (chn == 1)
        return LIVE_CHN_MATRIX;
    if (chn < 11)
        return LIVE_CHN_INT;

    return LIVE_CHN_UNKNOWN;
}

/* direction is input for all channels */
extern "C" DLLEXPORT UInt8 chnDir(UInt16)
{
    return LIVE_CHN_INPUT;
}

/* return the integer data (only valid for the buttons) */
extern "C" DLLEXPORT Int16 chnGetDataINT(UInt16 chn)
{
    if (chn == 1 || chn > 11)
        return 0;

    return buttons[chn-2];
}

/* return the char data (not valid here) */
extern "C" DLLEXPORT char chnGetDataCHAR(UInt16)
{
    return ' ';
}

/* return the Real32 data (not valid here) */
extern "C" DLLEXPORT Real32 chnGetDataFLOAT(UInt16)
{
    return -1.0f;
}

/* return the matrix data (only valid for chn1) */
extern "C" DLLEXPORT Real32* chnGetDataMAT(UInt16 chn)
{
    if (chn == 1)
    {
        for(int x=0;x<4;x++)
            for(int y=0;y<4;y++)
                g_aSMMat[x][y] = mat[y][x];
        return (Real32*) g_aSMMat;
    } else
        return NULL;
}

/* the functions for writing data to the device (not valid for a mouse) */
extern "C" DLLEXPORT void chnSetDataINT(UInt16, Int16){};
extern "C" DLLEXPORT void chnSetDataCHAR(UInt16, char){};
extern "C" DLLEXPORT void chnSetDataFLOAT(UInt16, Real32){};
extern "C" DLLEXPORT void chnSetDataMAT(UInt16, Real32*){};

#ifdef WIN32

///////////////////////////////////////////////////////////////////////////////
// window
///////////////////////////////////////////////////////////////////////////////

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
    //wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
    wc.hInstance = g_sHinst;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = __wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    if (!RegisterClass(&wc))
    {
        cout << "live_space_mouse: cannot register window class. Fatal Error!" << endl;
        exit(1);
    }

    g_sHwnd = CreateWindow(g_aClassName,
                           "live_space_mouse",
                           WS_MINIMIZE | WS_DISABLED,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL,
                           NULL,
                           g_sHinst,
                           0
                          );

    //    ShowWindow(g_sHwnd, SW_SHOW);
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    g_sHinst = hInstance;
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // cout also needed for some waiting time before calling createWindow()
        cout << "live_space_mouse: init reference window" << endl;
        __createWindow();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        cout << "live_space_mouse: close reference window" << endl;
        devClose(); //__exitDI();
    }

    return 1;
}

#endif
