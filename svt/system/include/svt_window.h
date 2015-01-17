/***************************************************************************
                          svt_window.h  -  description
                             -------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_WINDOW_H
#define SVT_WINDOW_H

#include <svt_basics.h>

/* mouse buttons */
#define SVT_LEFT_MOUSE_BUTTON           1
#define SVT_MIDDLE_MOUSE_BUTTON         2
#define SVT_RIGHT_MOUSE_BUTTON          3
#define SVT_MOUSE_WHEEL_UP              4
#define SVT_MOUSE_WHEEL_DOWN            5

/* function keys */
#define SVT_KEY_F1			1
#define SVT_KEY_F2			2
#define SVT_KEY_F3			3
#define SVT_KEY_F4			4
#define SVT_KEY_F5			5
#define SVT_KEY_F6			6
#define SVT_KEY_F7			7
#define SVT_KEY_F8			8
#define SVT_KEY_F9			9
#define SVT_KEY_F10			10
#define SVT_KEY_F11			11
#define SVT_KEY_F12			12
/* directional keys */
#define SVT_KEY_LEFT			100
#define SVT_KEY_UP			101
#define SVT_KEY_RIGHT			102
#define SVT_KEY_DOWN			103
#define SVT_KEY_PAGE_UP		        104
#define SVT_KEY_PAGE_DOWN		105
#define SVT_KEY_HOME			106
#define SVT_KEY_END			107
#define SVT_KEY_INSERT			108
/* modifier keys */
#define SVT_KEY_CONTROL			200
#define SVT_KEY_SHIFT			201
#define SVT_KEY_ALT			202

typedef int svt_win_handle;

/// create a svt window. Returns the new handle for this window.
DLLEXPORT svt_win_handle svt_createWindow(int pos_x, int pos_y, int width, int height);
/// create a svt fullscreen window. Returns the new handle for this window.
DLLEXPORT svt_win_handle svt_createFullScreenWindow(int _tsx, int _tsy);
/// destroy all windows (cleanup)
DLLEXPORT void svt_destroyAllWindows();
/// destroys a window. Must first be closed and after that destroyed!!
DLLEXPORT bool svt_destroyWindow();
/// open a svt window. Returns true if the window was opened successfully.
DLLEXPORT bool svt_openWindow(svt_win_handle win);
/// close a svt window. Returns true if the window was sucessfully closed.
DLLEXPORT bool svt_closeWindow(svt_win_handle win);
/// change active window.
DLLEXPORT void svt_switchWindow(svt_win_handle win);
/// get the width of a window
DLLEXPORT int svt_getWindowWidth(svt_win_handle win);
/// get the height of a window
DLLEXPORT int svt_getWindowHeight(svt_win_handle win);
/// mark the window for redrawing (it will be redrawed the next time, the main loop thread gets control over the cpu)
DLLEXPORT void svt_redrawWindow(svt_win_handle win);
/// mark all windows for redrawing
DLLEXPORT void svt_redrawAllWindows( );
/// is it a stereo window?
DLLEXPORT bool svt_isWindowStereo(svt_win_handle win);

/// set drawing function for a window
DLLEXPORT void svt_setDrawingFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set the idle function for a window (Attention: YOU SHOULD NOT MAKE ANY OPENGL CALLS IN THE IDLE FUNCTION! You can calculate something and then call the svt_redrawWindow() function, so that the drawing function will make the opengl calls.)
DLLEXPORT void svt_setIdleFunc(void (*func)());
/// set resize window callback function
DLLEXPORT void svt_setResizeWindowFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/** set mouse drag function (mouse moved with left mouse button down)
 *@param win the window responsible for the event
 *@param x x coordinate, the event occured
 *@param y y coordinate, the event occured
 *@param mb the mouse button, the user is holding down
 */
DLLEXPORT void svt_setMouseDragFunc(svt_win_handle win, void (*func)(svt_win_handle win,int x,int y, int mb));
/// set mouse motion function (mouse moved without pressing a mouse button)
DLLEXPORT void svt_setMouseMotionFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));

/// set left mouse button function
DLLEXPORT void svt_setLMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set right mouse button function
DLLEXPORT void svt_setRMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set middle mouse button function
DLLEXPORT void svt_setMMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set mouse wheel up button pressed function
DLLEXPORT void svt_setUMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set mouse wheel down button pressed function
DLLEXPORT void svt_setDMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set left mouse button function
DLLEXPORT void svt_setLMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set right mouse button function
DLLEXPORT void svt_setRMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set middle mouse button function
DLLEXPORT void svt_setMMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set mouse wheel up button release function
DLLEXPORT void svt_setUMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));
/// set mouse wheel down button release function
DLLEXPORT void svt_setDMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int));

/// set the function called, if a key is pressed
DLLEXPORT void svt_setKeyDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,char));
/// set the function called, if a key is released
DLLEXPORT void svt_setKeyUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,char));
/// set the function called, if a special key (e.g. function key) is pressed
DLLEXPORT void svt_setSpecialKeyDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,int));
/// set the function called, if a special key (e.g. function key) is released
DLLEXPORT void svt_setSpecialKeyUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,int));

/// set the current modi for the next window
DLLEXPORT void svt_setDoublebuffer(bool val);
/// set the depth buffer bits
DLLEXPORT void svt_setDepthBufferBits(int bits);
/// should svt open the next window in stereo mode?
DLLEXPORT void svt_setStereo(bool st);
/// should svt open the next window in RGBA mode?
DLLEXPORT void svt_setRGBA(bool r);
/// set the window title for the next windows
DLLEXPORT void svt_setWindowTitle(const char* name);
/// set the display on which the next windows should be opened
DLLEXPORT void svt_setDisplay(const char* name);
/// should svt create threads?
DLLEXPORT void svt_createThreads(bool yesno);
/// are we creating threads?
DLLEXPORT bool svt_creatingThreads();

/// returns the window with the same context as the threads context
DLLEXPORT svt_win_handle svt_getCurrentWindow();

/// returns the number of windows
DLLEXPORT int svt_getWindowsNum();

/// swap the buffers for all windows
DLLEXPORT void svt_swapBuffers();

/// swap the buffers for all windows
DLLEXPORT void svt_swapCurrentWindow();

/// should we sync the swapping of the buffers or not?
DLLEXPORT void svt_setSwapSync(bool ss);
/// are we syncing the swapping of the buffers?
DLLEXPORT bool svt_getSwapSync();
/// returns the window with the same context as the threads context
DLLEXPORT int svt_getCurrentCanvas();
DLLEXPORT void svt_setCurrentCanvas(int);

/// returns the number of windows
DLLEXPORT int svt_getCanvasNum();
DLLEXPORT void svt_setCanvasNum(int);

/// swap the buffers for all windows
DLLEXPORT void svt_swapBuffers();
/// should we sync the swapping of the buffers or not?
DLLEXPORT void svt_setSwapSync(bool ss);
/// are we syncing the swapping of the buffers?
DLLEXPORT bool svt_getSwapSync();
/**
 * Set the number of samples. If 0, the multisample extension is not being used. Default = 2.
 * \param iSamples number of samples
 */
DLLEXPORT void svt_setSamples(int iSamples);
/**
 * Get the number of samples used by the multisample extension. If 0, multisample extension is not being used.
 * \param iWin svt_win_handle
 * \return iSamples number of samples
 */
DLLEXPORT int svt_getSamples(int iWin);

/**
 * set mouse position
 * \param iX new x position
 * \param iY new y position
 */
DLLEXPORT void svt_setMousePos(int iX, int iY);
/**
 * hide mouse
 */
DLLEXPORT void svt_hideMouse();
/**
 * show mouse
 */
DLLEXPORT void svt_showMouse();

/**
 * Wake-up windows - the windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 */
DLLEXPORT void svt_forceUpdateWindows();

/**
 * Wake-up windows - the windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 * Internal function, please call svt_forceUpdateWindows!
 */
void svt_wakeupWindow(svt_win_handle win);

DLLEXPORT void svt_setGUICallback( void (*func)());

/**
 * Wake-up gui windows - the gui windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 */
DLLEXPORT void svt_forceUpdateGUIWindows();

#endif
