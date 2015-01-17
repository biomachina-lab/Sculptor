/***************************************************************************
                          svt_window.cpp
                          --------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_system.h>
#include <svt_window.h>
#include <svt_threads.h>
#include <svt_window_int.h>
#include <svt_time.h>
#include <svt_opengl.h>

#include <stdlib.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Structures and variables
///////////////////////////////////////////////////////////////////////////////

// array w/ the data for all windows
svt_win* active_windows = NULL;
// how many windows are there?
int active_windows_count = 0;
// how many threads are there?
int thread_count = 0;
// should we create threads?
bool __svt_threads = false;
// window title
const char* current_name = "SVT";
// window display
const char* current_display = NULL;
// window in stereo?
bool current_stereo = false;
// should the window do doublebuffering?
bool current_doublebuf = true;
// depth buffer bits
int  current_depth = 24;
// rgba or rgb?
bool current_rgba = false;
// swap buffers sync?
bool swap_sync = false;
// number of samples (if 0, multisample extension is not being used)
int current_samples = 0;

// how many canvas 
static int g_iCanvasNum=0;

// idle function
void (*idle)();

// the drawing semaphore
extern svt_semaphore g_cDrawingSema;



///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////

void __svt_swapAllBuffers()
{
    int i;
    for (i = 0; i< active_windows_count ; i++)
        if (active_windows[i].swap_buf)
            __svt_swapBuffers(i+1);
}

bool __svt_checkWinParam(svt_win_handle win)
{
	if (win > active_windows_count)
		return false;
	if (win < 1)
		return false;
	if (active_windows[win-1].active != true)
		return false;

	return true;
}

void __svt_dragMouse(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].drag != NULL)
    {
        //printf("svt_system: win %i mouse draggin pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].drag)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, active_windows[win-1].button);
    }
};

void __svt_moveMouse(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].move != NULL)
    {
        //printf("svt_system: win %i mouse move pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].move)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_lmb_down(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].lmb_down != NULL)
    {
        //printf("svt_system: win %i lmb pressed pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].lmb_down)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_rmb_down(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].rmb_down != NULL)
    {
        //printf("svt_system: win %i rmb pressed pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].rmb_down)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_mmb_down(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].mmb_down != NULL)
    {
        //printf("svt_system: win %i mmb pressed pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].mmb_down)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_umb_down(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].umb_down != NULL)
    {
        //printf("svt_system: win %i mmb pressed pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].umb_down)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_dmb_down(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].dmb_down != NULL)
    {
        //printf("svt_system: win %i mmb pressed pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].dmb_down)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_lmb_up(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].lmb_up != NULL)
    {
        //printf("svt_system: win %i lmb released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].lmb_up)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_rmb_up(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].rmb_up != NULL)
    {
        //printf("svt_system: win %i rmb released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].rmb_up)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_mmb_up(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].mmb_up != NULL)
    {
        //printf("svt_system: win %i mmb released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].mmb_up)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_umb_up(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].umb_up != NULL)
    {
        //printf("svt_system: win %i mmb released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].umb_up)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};
void __svt_dmb_up(svt_win_handle win)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].dmb_up != NULL)
    {
        //printf("svt_system: win %i mmb released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].dmb_up)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
    }
};

void __svt_key_pressed(svt_win_handle win, char key)
{
    if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].key_pressed != NULL)
    {
        //printf("svt_system: win %i key pressed pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].key_pressed)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, key);
    }
};

void __svt_key_released(svt_win_handle win, char key)
{
	if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].key_released != NULL)
    {
        //printf("svt_system: win %i key released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].key_released)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, key);
    }
};

void __svt_resizeWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    if (active_windows[win-1].resize != NULL)
    {
        //printf("svt_system: win %i resized to %i,%i\n", win,active_windows[win-1].size_x, active_windows[win-1].size_y);
        (active_windows[win-1].resize)(win,active_windows[win-1].size_x, active_windows[win-1].size_y);
    }
};

void __svt_idle()
{
    if (idle != NULL)
        (idle)();
};

void __svt_redrawWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win))
        return;

    if (active_windows[win-1].func != NULL)
    {
        active_windows[win-1].sema->P();
        g_cDrawingSema.P();
        (active_windows[win-1].func)(win,active_windows[win-1].size_x, active_windows[win-1].size_y);
        g_cDrawingSema.V();
        active_windows[win-1].swap_buf = true;
        active_windows[win-1].redraw = false;
        active_windows[win-1].sema->V();
    }

};

void __svt_idleTest()
{
    int i;

    if (__svt_threads)
    {
        for (i = 0; i< active_windows_count ; i++)
        {
            while(active_windows[i].redraw)
            {
                active_windows[i].sema->P();
                active_windows[i].sema->V();
            }
        }
    }

    if (swap_sync)
        __svt_swapAllBuffers();

    __svt_idle();
};

///////////////////////////////////////////////////////////////////////////////
// Public funtctions
///////////////////////////////////////////////////////////////////////////////

////// window creation

svt_win_handle svt_createWindow(int _tx, int _ty, int _tsx, int _tsy)
{
    return __svt_createWindow(_tx,_ty,_tsx,_tsy,false,current_display);
};

svt_win_handle svt_createFullScreenWindow(int _tsx, int _tsy)
{
    return __svt_createWindow(0,0,_tsx,_tsy,true,current_display);
};

////// window destruction
void svt_destroyAllWindows()
{
    int i;
    for (i = 0; i< active_windows_count ; i++)
        __svt_destroyWindow(i+1);
}

////// window properties

void svt_setDoublebuffer(bool val)
{
    current_doublebuf = val;
};

void svt_setDepthBufferBits(int bits)
{
    current_depth = bits;
};

void svt_setStereo(bool st)
{
    current_stereo = st;
};

void svt_setRGBA(bool r)
{
    current_rgba = r;
};

bool svt_isWindowStereo(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return false;

    return active_windows[win-1].stereo;
}

int svt_getWindowWidth(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return -1;

    return active_windows[win-1].size_x;
};

int svt_getWindowHeight(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return -1;

    return active_windows[win-1].size_y;
};

void svt_setWindowTitle(const char* name)
{
    current_name = name;
};

void svt_setDisplay(const char* name)
{
    current_display = name;
};

void svt_setSamples(int iSamples)
{
    current_samples = iSamples;
};
int svt_getSamples(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return -1;

    return active_windows[win-1].samples;
};

////// callback funcs

void svt_setDrawingFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
    if(!__svt_checkWinParam(win)) return;

	active_windows[win-1].func = func;
};

void svt_setIdleFunc(void (*func)())
{
    idle = func;
};

void svt_setLMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].lmb_down = func;
};
void svt_setRMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
    if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].rmb_down = func;
};
void svt_setMMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].mmb_down = func;
};
void svt_setUMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].umb_down = func;
};
void svt_setDMBDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].dmb_down = func;
};
void svt_setLMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].lmb_up = func;
};
void svt_setRMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].rmb_up = func;
};
void svt_setMMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].mmb_up = func;
};
void svt_setUMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].umb_up = func;
};
void svt_setDMBUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].dmb_up = func;
};
void svt_setKeyDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,char))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].key_pressed = func;
};
void svt_setKeyUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,char))
{
    if(!__svt_checkWinParam(win)) return;
	active_windows[win-1].key_released = func;
};
void svt_setSpecialKeyDownFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].special_key_pressed = func;
};
void svt_setSpecialKeyUpFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].special_key_released = func;
};

void svt_setResizeWindowFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].resize = func;
};

void svt_setMouseDragFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int,int))
{
    if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].drag = func;
};

void svt_setMouseMotionFunc(svt_win_handle win, void (*func)(svt_win_handle,int,int))
{
	if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].move = func;
};

////// threads

void svt_createThreads(bool tf)
{
    __svt_threads = tf;
};

bool svt_creatingThreads()
{
    return __svt_threads;
};

////// redrawing

void svt_redrawWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    // the window is already drawn, but the buffers are not swapped. Then wait until they are swapped, before we draw another picture
    if (active_windows[win-1].swap_buf)
        return;

    // if threads are active, the redrawing is done in svt_redrawThread, otherwise it is done here!
    if (__svt_threads)
    {
        active_windows[win-1].redraw = true;
    }
    else
    {
        svt_switchWindow(win);
        __svt_redrawWindow(win);

	if (!swap_sync)
	    __svt_swapBuffers(win);
    }

#ifdef WIN32
    //InvalidateRect(active_windows[win-1].win,NULL,TRUE);
    //UpdateWindow(active_windows[win-1].win);
#endif

};

void svt_redrawAllWindows( void )
{
    int i;
    for (i = 1; i<= active_windows_count ; i++)
        svt_redrawWindow(i);
}

void* svt_redrawThread(void* null)
{
    int thread = *(int*)null;
    int i;

    printf("svt_system: starting redraw thread %i\n", thread);

    while(true)
    {
	for(i=0;i<active_windows_count;i++)
	{
	    if (active_windows[i].thread == thread && active_windows[i].active && active_windows[i].redraw && !active_windows[i].swap_buf)
	    {
		svt_switchWindow(i+1);
		__svt_redrawWindow(i+1);
		if (!swap_sync)
		    __svt_swapBuffers(i+1);
	    }
	}
    }

    return NULL;
};

////// misc

int svt_getWindowsNum()
{
    return active_windows_count;
}

DLLEXPORT void svt_setSwapSync(bool ss)
{
    swap_sync = ss;
};

bool svt_getSwapSync()
{
    return swap_sync;
}

int svt_getCanvasNum() {return g_iCanvasNum;}
void svt_setCanvasNum(int i) {g_iCanvasNum =i;}

int svt_getCurrentCanvas() 
{
  int iWin=svt_getCurrentWindow()-1;
  if (iWin>=0)
	return active_windows[iWin].current_canvas;
  return -1;
}

void svt_setCurrentCanvas(int iNum) 
{
  int iWin=svt_getCurrentWindow()-1;
  if (iWin>=0)
	active_windows[iWin].current_canvas=iNum;
}

void svt_swapCurrentWindow()
{
  int iWin=svt_getCurrentWindow();
  if (iWin>=0)
	__svt_swapBuffers(iWin);

}

/**
 * Wake-up windows - the windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 */
DLLEXPORT void svt_forceUpdateWindows()
{
    int i;
    for (i = 1; i<= active_windows_count ; i++)
        svt_wakeupWindow(i);
}

// gui callback function
void (*guiCallback)() = NULL;

DLLEXPORT void svt_setGUICallback( void (*func)())
{
    guiCallback = func;
};

/**
 * Wake-up gui windows - the gui windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 */
DLLEXPORT void svt_forceUpdateGUIWindows()
{
    if (guiCallback != NULL)
	(guiCallback)();
};
