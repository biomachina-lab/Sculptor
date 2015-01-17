/***************************************************************************
                          svt_window_win32.cpp
                          --------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifdef SVT_SYSTEM_WIN32

#include <svt_system.h>
#include <svt_window.h>
#include <svt_window_int.h>
#include <svt_window_vars.h>
#include <svt_time.h>

#include <windows.h>
#include <stdio.h>

#define __SVT_WIN_CLASS "SVT"

///////////////////////////////////////////////////////////////////////////////
// forward declarations
///////////////////////////////////////////////////////////////////////////////

svt_win_handle __svt_getWinFromHWND(HWND hwnd);
LRESULT CALLBACK svt_checkEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// gui callback function
extern void (*guiCallback)();

///////////////////////////////////////////////////////////////////////////////
// helper functions
///////////////////////////////////////////////////////////////////////////////

void __svt_fatalError(const char *s)
{
    MessageBox(NULL,s,"SVT_System (Window): Fatal Error",MB_OK);
    exit(1);
}

bool __svt_mainloopWindow( void )
{
    MSG msg;
    /*if (PeekMessage(&msg,NULL,0,0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return true;
    } else {
        return true;
    }*/

    int iRet;

    iRet = GetMessage( &msg, NULL, 0, 0 );

    if (iRet > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        int i;
	for (i=0;i < active_windows_count; i++)
		if (active_windows[i].win == msg.hwnd)
			return true;

        if (guiCallback != NULL)
            (guiCallback)();

    }

    return true;
}

void __svt_initWindow( void )
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HINSTANCE hInstanceDLL = GetModuleHandle("svt_system.dll");
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style =		CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.lpfnWndProc =	(WNDPROC)svt_checkEvents;
	wc.cbClsExtra =		0;
	wc.cbWndExtra =		0;
	wc.hInstance =		hInstance;
	wc.hIcon =		LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor =		LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground =	NULL;
	wc.lpszMenuName =	NULL;
	wc.lpszClassName =	__SVT_WIN_CLASS;

	if (!wc.hIcon)
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClass(&wc))
		__svt_fatalError("svt_system: Cannot register SVT window class!");
};

svt_win_handle __svt_getWinFromHWND(HWND hwnd)
{
	int i;
	for (i=0;i < active_windows_count; i++)
		if (active_windows[i].win == hwnd)
			return i+1;

	return 1;
};

DLLEXPORT HWND svt_getHWNDFromWin(svt_win_handle win)
{
	if ((win <= active_windows_count) && (win > 0))
        return active_windows[win-1].win;
    else
        return NULL;
};

void __svt_special_key_pressed(svt_win_handle win, int key)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].special_key_pressed != NULL && key != -1)
    {
        //printf("svt_system: win %i key released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].special_key_pressed)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, key);
    }
};

void __svt_special_key_released(svt_win_handle win, int key)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].special_key_released != NULL && key != -1)
    {
        //printf("svt_system: win %i key released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].special_key_released)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, key);
    }
};

void __svt_swapBuffers(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    SwapBuffers(active_windows[win-1].hdc);

    active_windows[win-1].swap_buf = false;
};

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

svt_win_handle svt_getCurrentWindow()
{
    return 1;
};

bool svt_openWindow(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return false;

	ShowWindow(active_windows[win-1].win, SW_SHOWNORMAL);
	//UpdateWindow(active_windows[win-1].win);

	return true;
};

void svt_switchWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    wglMakeCurrent(active_windows[win-1].hdc,active_windows[win-1].hglrc);
};

/**
 * set mouse position
 * \param iX new x position
 * \param iY new y position
 */
void svt_setMousePos(int iX, int iY)
{
    svt_win_handle handle = svt_getCurrentWindow();

    POINT pt;
    pt.x = iX;
    pt.y = iY;
    ClientToScreen(active_windows[handle-1].win, &pt);
    SetCursorPos(pt.x, pt.y);
};

///////////////////////////////////////////////////////////////////////////////
// window create function
///////////////////////////////////////////////////////////////////////////////

char* __svt_formatErrorMsg()
{
    char* lpMsgBuf;
    FormatMessage(
                  FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) &lpMsgBuf,
                  0,NULL
                 );
    return lpMsgBuf;
};

svt_win_handle __svt_createWindow(int _tx, int _ty, int _tsx, int _tsy, bool _fs, const char* disp)
{

    if (__svt_threads)
        __svt_threads = false;

    if (active_windows != NULL)
        active_windows = (svt_win*)realloc(active_windows, (active_windows_count+1) * sizeof(svt_win));
    else
        active_windows = (svt_win*)malloc(sizeof(svt_win));

    if (active_windows == NULL || active_windows_count < 0)
        __svt_fatalError("svt_system: memory management error!");

    active_windows[active_windows_count].thread = -1;

    active_windows_count++;

    // init additional variables
    active_windows[active_windows_count-1].func = NULL;
    active_windows[active_windows_count-1].mouse_x = 0;
    active_windows[active_windows_count-1].mouse_y = 0;
    active_windows[active_windows_count-1].lmb_up = NULL;
    active_windows[active_windows_count-1].lmb_down = NULL;
    active_windows[active_windows_count-1].rmb_up = NULL;
    active_windows[active_windows_count-1].rmb_down = NULL;
    active_windows[active_windows_count-1].mmb_up = NULL;
    active_windows[active_windows_count-1].mmb_down = NULL;
    active_windows[active_windows_count-1].umb_up = NULL;
    active_windows[active_windows_count-1].umb_down = NULL;
    active_windows[active_windows_count-1].dmb_up = NULL;
    active_windows[active_windows_count-1].dmb_down = NULL;
    active_windows[active_windows_count-1].func = NULL;
    active_windows[active_windows_count-1].resize = NULL;
    active_windows[active_windows_count-1].key_pressed = NULL;
    active_windows[active_windows_count-1].key_released = NULL;
    active_windows[active_windows_count-1].special_key_pressed = NULL;
    active_windows[active_windows_count-1].special_key_released = NULL;
    active_windows[active_windows_count-1].drag = NULL;
    active_windows[active_windows_count-1].move = NULL;
    active_windows[active_windows_count-1].active = true;
    active_windows[active_windows_count-1].button = 0;
    active_windows[active_windows_count-1].redraw = true;
    active_windows[active_windows_count-1].swap_buf = false;
    active_windows[active_windows_count-1].stereo = current_stereo;
	active_windows[active_windows_count-1].current_canvas = 0;
    active_windows[active_windows_count-1].sema = new svt_semaphore(1);

    // fullscreen mode?
    if (_fs)
    {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth	= _tsx;
        dmScreenSettings.dmPelsHeight	= _tsy;
        dmScreenSettings.dmFields=DM_PELSWIDTH|DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
    }

    // fullscreen?
    if (_fs)
    {
        active_windows[active_windows_count-1].pos_x = 0;
        active_windows[active_windows_count-1].pos_y = 0;
        active_windows[active_windows_count-1].size_x = GetSystemMetrics(SM_CXSCREEN);
        active_windows[active_windows_count-1].size_y = GetSystemMetrics(SM_CYSCREEN);
        active_windows[active_windows_count-1].border = false;

    } else {
        active_windows[active_windows_count-1].pos_x = _tx;
        active_windows[active_windows_count-1].pos_y = _ty;
        active_windows[active_windows_count-1].size_x = _tsx;
        active_windows[active_windows_count-1].size_y = _tsy;
        active_windows[active_windows_count-1].border = true;
    }

    // create window
    DWORD dwStyle;
    if (!active_windows[active_windows_count-1].border)
        dwStyle = WS_POPUP;
    else
        dwStyle = WS_OVERLAPPEDWINDOW;

    active_windows[active_windows_count-1].win = CreateWindow(__SVT_WIN_CLASS,
                                                              current_name,
                                                              WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                                                              active_windows[active_windows_count-1].pos_x,
                                                              active_windows[active_windows_count-1].pos_y,
                                                              active_windows[active_windows_count-1].size_x,
                                                              active_windows[active_windows_count-1].size_y,
                                                              NULL,
                                                              NULL,
                                                              GetModuleHandle(NULL),
                                                              NULL);

    if (active_windows[active_windows_count-1].win == NULL)
        __svt_fatalError(__svt_formatErrorMsg());

    // get device context
    active_windows[active_windows_count-1].hdc = GetDC(active_windows[active_windows_count-1].win);

    // pixelformat configuration
    PIXELFORMATDESCRIPTOR pfd;

    memset(&pfd,0,sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    // stereo
    if (current_stereo)
        pfd.dwFlags |= PFD_STEREO;
    // stereo
    if (current_doublebuf)
        pfd.dwFlags |= PFD_DOUBLEBUFFER;
    // rgba
    if (current_rgba)
        pfd.iPixelType = PFD_TYPE_RGBA;
    // color depth
    pfd.cColorBits = 16;
    // depth bits
    pfd.cDepthBits = current_depth;

    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixel = ChoosePixelFormat(active_windows[active_windows_count-1].hdc, &pfd);
    if (pixel == 0)
        __svt_fatalError(__svt_formatErrorMsg());

    DescribePixelFormat( active_windows[active_windows_count-1].hdc, pixel, sizeof(PIXELFORMATDESCRIPTOR), &pfd );
    // check opengl mode
    if ((pfd.dwFlags & PFD_SUPPORT_OPENGL) == 0)
        __svt_fatalError("Sorry, opengl not available!");
    // check draw to window mode
    if ((pfd.dwFlags & PFD_DRAW_TO_WINDOW) == 0)
        __svt_fatalError("Sorry, draw to window mode not available!");
    // check stereo mode
    if (current_stereo && (pfd.dwFlags & PFD_STEREO) == 0)
        __svt_fatalError("Sorry, stereo mode not available!");
    // check depth
    //if (current_depth != pfd.cDepthBits)
    //    __svt_fatalError("Sorry, cannot get sufficient color depth!");
    // check doublebuffer
    if (current_doublebuf && (pfd.dwFlags & PFD_DOUBLEBUFFER) == 0)
        __svt_fatalError("Sorry, double buffering not available!");
    // check rgba
    //if (current_rgba && (pfd.iPixelType & PFD_TYPE_RGBA) == 0)
    //    __svt_fatalError("Sorry, rgba mode not available!");

    BOOL err = SetPixelFormat(active_windows[active_windows_count-1].hdc, pixel, &pfd);
    if (!err)
        __svt_fatalError(__svt_formatErrorMsg());

    // create open gl rendering context
    if (!(active_windows[active_windows_count-1].hglrc = wglCreateContext(active_windows[active_windows_count-1].hdc)))
        __svt_fatalError(__svt_formatErrorMsg());

    return active_windows_count;
};

void __svt_destroyWindow(svt_win_handle win)
{
    // fullscreen mode?
    if (!active_windows[win-1].border)
    {
        ChangeDisplaySettings(NULL,0);
    }

    wglMakeCurrent(NULL,NULL);

    if (active_windows[win-1].hglrc)
        wglDeleteContext(active_windows[win-1].hglrc);

    if (active_windows[win-1].hdc)
        ReleaseDC(active_windows[win-1].win, active_windows[win-1].hdc);

    if (active_windows[win-1].win)
        DestroyWindow(active_windows[win-1].win);
}

///////////////////////////////////////////////////////////////////////////////
// window event function
///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK svt_checkEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

    switch(msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        active_windows[__svt_getWinFromHWND(hwnd)-1].active = false;
        exit(0);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        active_windows[__svt_getWinFromHWND(hwnd)-1].active = false;
        exit(0);
        return 0;
    case WM_MOUSEMOVE:
        active_windows[__svt_getWinFromHWND(hwnd)-1].mouse_x = LOWORD(lParam);
        active_windows[__svt_getWinFromHWND(hwnd)-1].mouse_y = HIWORD(lParam);
        if (active_windows[__svt_getWinFromHWND(hwnd)-1].button)
            __svt_dragMouse(__svt_getWinFromHWND(hwnd));
        else
            __svt_moveMouse(__svt_getWinFromHWND(hwnd));
        break;

    case WM_LBUTTONDOWN:
        active_windows[__svt_getWinFromHWND(hwnd)-1].button = SVT_LEFT_MOUSE_BUTTON;
        __svt_lmb_down(__svt_getWinFromHWND(hwnd));
        break;
    case WM_RBUTTONDOWN:
        active_windows[__svt_getWinFromHWND(hwnd)-1].button = SVT_RIGHT_MOUSE_BUTTON;
        __svt_rmb_down(__svt_getWinFromHWND(hwnd));
        break;
    case WM_MBUTTONDOWN:
        active_windows[__svt_getWinFromHWND(hwnd)-1].button = SVT_MIDDLE_MOUSE_BUTTON;
        __svt_mmb_down(__svt_getWinFromHWND(hwnd));
        break;
    case WM_LBUTTONUP:
        active_windows[__svt_getWinFromHWND(hwnd)-1].button = 0;
        __svt_lmb_up(__svt_getWinFromHWND(hwnd));
        break;
    case WM_RBUTTONUP:
        active_windows[__svt_getWinFromHWND(hwnd)-1].button = 0;
        __svt_rmb_up(__svt_getWinFromHWND(hwnd));
        break;
    case WM_MBUTTONUP:
        active_windows[__svt_getWinFromHWND(hwnd)-1].button = 0;
        __svt_mmb_up(__svt_getWinFromHWND(hwnd));
        break;
    case WM_MOUSEWHEEL:
        if ( (short)(HIWORD(wParam)) < 0 )
            __svt_umb_down( __svt_getWinFromHWND(hwnd) );
        else
            __svt_dmb_down( __svt_getWinFromHWND(hwnd) );
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_F1: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F1); break;
        case VK_F2: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F2); break;
        case VK_F3: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F3); break;
        case VK_F4: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F4); break;
        case VK_F5: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F5); break;
        case VK_F6: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F6); break;
        case VK_F7: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F7); break;
        case VK_F8: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F8); break;
        case VK_F9: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F9); break;
        case VK_F10: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F10); break;
        case VK_F11: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F11); break;
        case VK_F12: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_F12); break;

        case VK_PRIOR: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_PAGE_UP); break;
        case VK_NEXT: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_PAGE_DOWN); break;
        case VK_HOME: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_HOME); break;
        case VK_END: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_END); break;
        case VK_INSERT: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_INSERT); break;

        case VK_RIGHT: __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_RIGHT); break;
        case VK_LEFT:  __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_LEFT); break;
        case VK_UP:    __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_UP); break;
        case VK_DOWN:  __svt_special_key_pressed(__svt_getWinFromHWND(hwnd),SVT_KEY_DOWN); break;
        default:
            __svt_key_pressed(__svt_getWinFromHWND(hwnd), MapVirtualKey(wParam, 2));
            break;
        };
        break;
    case WM_SYSKEYUP:
    case WM_KEYUP:
        switch(wParam)
        {
        case VK_F1: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F1); break;
        case VK_F2: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F2); break;
        case VK_F3: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F3); break;
        case VK_F4: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F4); break;
        case VK_F5: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F5); break;
        case VK_F6: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F6); break;
        case VK_F7: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F7); break;
        case VK_F8: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F8); break;
        case VK_F9: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F9); break;
        case VK_F10: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F10); break;
        case VK_F11: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F11); break;
        case VK_F12: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_F12); break;

        case VK_PRIOR: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_PAGE_UP); break;
        case VK_NEXT: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_PAGE_DOWN); break;
        case VK_HOME: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_HOME); break;
        case VK_END: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_END); break;
        case VK_INSERT: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_INSERT); break;

        case VK_RIGHT: __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_RIGHT); break;
        case VK_LEFT:  __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_LEFT); break;
        case VK_UP:    __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_UP); break;
        case VK_DOWN:  __svt_special_key_released(__svt_getWinFromHWND(hwnd),SVT_KEY_DOWN); break;
        default:
            __svt_key_released(__svt_getWinFromHWND(hwnd), MapVirtualKey(wParam, 2));
            break;
        };
        break;
    case WM_PAINT:
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd,&ps);
        svt_redrawWindow(__svt_getWinFromHWND(hwnd));
        break;
    case WM_SIZE:
        active_windows[__svt_getWinFromHWND(hwnd)-1].size_x = LOWORD(lParam);
        active_windows[__svt_getWinFromHWND(hwnd)-1].size_y = HIWORD(lParam);
        __svt_resizeWindow(__svt_getWinFromHWND(hwnd));
        break;
    default:
        return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;
};

/**
 * Wake-up windows - the windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 * Internal function, please call svt_forceUpdateWindows!
 */
void svt_wakeupWindow(svt_win_handle win)
{
    //InvalidateRect( active_windows[win-1].win, NULL, FALSE );
    //UpdateWindow( active_windows[win-1].win );
    PostMessage(active_windows[win-1].win, WM_PAINT, 0, 0);
};

#endif
