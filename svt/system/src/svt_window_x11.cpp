/***************************************************************************
                          svt_window_x11.cpp
                          ------------------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifdef SVT_SYSTEM_X11

#include <svt_system.h>
#include <svt_window.h>
#include <svt_threads.h>
#include <svt_window_int.h>
#include <svt_window_vars.h>
#include <svt_semaphore.h>
#include <svt_time.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <GL/gl.h>

#define EVT_MASKS ExposureMask | PointerMotionMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | KeyPressMask

///////////////////////////////////////////////////////////////////////////////
// variables
///////////////////////////////////////////////////////////////////////////////

svt_semaphore sema;

// configuration array
static int current_conf[256];
int current_conf_counter;

// should we enable threads for x11
bool __svt_x11threads = false;

// gui callback function
extern void (*guiCallback)();

Display* __other_x11display = NULL;
void __svt_setOtherX11Display( Display* pDisp )
{
    __other_x11display = pDisp;
};

static bool bRedrawEvent = false;
int iLastQtUpdate = 0;

int __svt_XlibIOErrorHandler(Display *)
{
    // this will kill also the Qt windows
    if (__other_x11display != NULL)
        XCloseDisplay( __other_x11display );

    exit(0);

    // Pointless, but it shuts up some compiler warning messages
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
// Definitions for the motif window hints (window without decorations)
///////////////////////////////////////////////////////////////////////////////

/* #define HAVE_MOTIF  not all systems have Motif */
/* #define HAVE_MOTIF  not all systems have Motif */
#ifdef HAVE_MOTIF
  #include <X11/Xm/MwmUtil.h>
#else
  /* bit definitions for MwmHints.flags */
  #define MWM_HINTS_FUNCTIONS       (1L << 0)
  #define MWM_HINTS_DECORATIONS     (1L << 1)
  #define MWM_HINTS_INPUT_MODE      (1L << 2)
  #define MWM_HINTS_STATUS          (1L << 3)
  /* bit definitions for MwmHints.decorations */
  #define MWM_DECOR_ALL             (1L << 0)
  #define MWM_DECOR_BORDER          (1L << 1)
  #define MWM_DECOR_RESIZEH         (1L << 2)
  #define MWM_DECOR_TITLE           (1L << 3)
  #define MWM_DECOR_MENU            (1L << 4)
  #define MWM_DECOR_MINIMIZE        (1L << 5)
  #define MWM_DECOR_MAXIMIZE        (1L << 6)
  typedef struct
  {
      unsigned long        flags;
      unsigned long        functions;
      unsigned long        decorations;
      long                 inputMode;
      unsigned long        status;
  } PropMotifWmHints;
  #define PROP_MOTIF_WM_HINTS_ELEMENTS  5
#endif

///////////////////////////////////////////////////////////////////////////////
// forward declarations
///////////////////////////////////////////////////////////////////////////////

void svt_checkEvents(svt_win_handle win);

///////////////////////////////////////////////////////////////////////////////
// helper functions
///////////////////////////////////////////////////////////////////////////////

void __svt_fatalError(const char *s)
{
    printf("SVT - Fatal Error: %s\n", s);
    exit(1);
}

bool __svt_mainloopWindow( void )
{
    int i;
    for (i=1; i<=active_windows_count;i++)
        svt_checkEvents(i);

    return true;
};
void __svt_initWindow( void )
{
};

void __svt_special_key_pressed(svt_win_handle win, KeySym ks)
{
    if(!__svt_checkWinParam(win)) return;

    int key = -1;

    switch (ks) {
    /* function keys */
    case XK_F1:    key = SVT_KEY_F1; break;
    case XK_F2:    key = SVT_KEY_F2; break;
    case XK_F3:    key = SVT_KEY_F3; break;
    case XK_F4:    key = SVT_KEY_F4; break;
    case XK_F5:    key = SVT_KEY_F5; break;
    case XK_F6:    key = SVT_KEY_F6; break;
    case XK_F7:    key = SVT_KEY_F7; break;
    case XK_F8:    key = SVT_KEY_F8; break;
    case XK_F9:    key = SVT_KEY_F9; break;
    case XK_F10:   key = SVT_KEY_F10; break;
    case XK_F11:   key = SVT_KEY_F11; break;
    case XK_F12:   key = SVT_KEY_F12; break;
    /* directional keys */
    case XK_KP_Left:
    case XK_Left:  key = SVT_KEY_LEFT; break;
    case XK_KP_Up: /* Introduced in X11R6. */
    case XK_Up:    key = SVT_KEY_UP; break;
    case XK_KP_Right: /* Introduced in X11R6. */
    case XK_Right: key = SVT_KEY_RIGHT; break;
    case XK_KP_Down: /* Introduced in X11R6. */
    case XK_Down:  key = SVT_KEY_DOWN; break;

    case XK_KP_Prior: /* Introduced in X11R6. */
    case XK_Prior:
        /* XK_Prior same as X11R6's XK_Page_Up */
        key = SVT_KEY_PAGE_UP;
        break;
    case XK_KP_Next: /* Introduced in X11R6. */
    case XK_Next:
        /* XK_Next same as X11R6's XK_Page_Down */
        key = SVT_KEY_PAGE_DOWN;
        break;
    case XK_KP_Home: /* Introduced in X11R6. */
    case XK_Home:
        key = SVT_KEY_HOME;
        break;
#ifdef __hpux
    case XK_Select:
#endif
    case XK_KP_End: /* Introduced in X11R6. */
    case XK_End:
        key = SVT_KEY_END;
        break;
#ifdef __hpux
    case XK_InsertChar:
#endif
    case XK_KP_Insert: /* Introduced in X11R6. */
    case XK_Insert:
        key = SVT_KEY_INSERT;
        break;
    case XK_Control_L:
    case XK_Control_R:
        key = SVT_KEY_CONTROL;
    case XK_Shift_L:
    case XK_Shift_R:
        key = SVT_KEY_SHIFT;
    case XK_Alt_L:
    case XK_Alt_R:
        key = SVT_KEY_ALT;
        break;
    }

    if (active_windows[win-1].special_key_pressed != NULL && key != -1)
    {
        //printf("svt_system: win %i key released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].special_key_pressed)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, key);
    }

};

void __svt_special_key_released(svt_win_handle win, KeySym ks)
{
	if(!__svt_checkWinParam(win)) return;

    int key = -1;

    switch (ks) {
    /* function keys */
    case XK_F1:    key = SVT_KEY_F1; break;
    case XK_F2:    key = SVT_KEY_F2; break;
    case XK_F3:    key = SVT_KEY_F3; break;
    case XK_F4:    key = SVT_KEY_F4; break;
    case XK_F5:    key = SVT_KEY_F5; break;
    case XK_F6:    key = SVT_KEY_F6; break;
    case XK_F7:    key = SVT_KEY_F7; break;
    case XK_F8:    key = SVT_KEY_F8; break;
    case XK_F9:    key = SVT_KEY_F9; break;
    case XK_F10:   key = SVT_KEY_F10; break;
    case XK_F11:   key = SVT_KEY_F11; break;
    case XK_F12:   key = SVT_KEY_F12; break;
    /* directional keys */
    case XK_KP_Left:
    case XK_Left:  key = SVT_KEY_LEFT; break;
    case XK_KP_Up: /* Introduced in X11R6. */
    case XK_Up:    key = SVT_KEY_UP; break;
    case XK_KP_Right: /* Introduced in X11R6. */
    case XK_Right: key = SVT_KEY_RIGHT; break;
    case XK_KP_Down: /* Introduced in X11R6. */
    case XK_Down:  key = SVT_KEY_DOWN; break;

    case XK_KP_Prior: /* Introduced in X11R6. */
    case XK_Prior:
        /* XK_Prior same as X11R6's XK_Page_Up */
        key = SVT_KEY_PAGE_UP;
        break;
    case XK_KP_Next: /* Introduced in X11R6. */
    case XK_Next:
        /* XK_Next same as X11R6's XK_Page_Down */
        key = SVT_KEY_PAGE_DOWN;
        break;
    case XK_KP_Home: /* Introduced in X11R6. */
    case XK_Home:
        key = SVT_KEY_HOME;
        break;
#ifdef __hpux
    case XK_Select:
#endif
    case XK_KP_End: /* Introduced in X11R6. */
    case XK_End:
        key = SVT_KEY_END;
        break;
#ifdef __hpux
    case XK_InsertChar:
#endif
    case XK_KP_Insert: /* Introduced in X11R6. */
    case XK_Insert:
        key = SVT_KEY_INSERT;
        break;
    case XK_Control_L:
    case XK_Control_R:
        key = SVT_KEY_CONTROL;
    case XK_Shift_L:
    case XK_Shift_R:
        key = SVT_KEY_SHIFT;
    case XK_Alt_L:
    case XK_Alt_R:
        key = SVT_KEY_ALT;
        break;
    }

    if (active_windows[win-1].special_key_released != NULL && key != -1)
    {
        //printf("svt_system: win %i key released pos %i,%i\n", win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y);
        (active_windows[win-1].special_key_released)(win,active_windows[win-1].mouse_x, active_windows[win-1].mouse_y, key);
    }

};

void __svt_swapBuffers(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    active_windows[win-1].swap_buf = false;

    if (active_windows[win-1].doublebuf)
    {
        glXSwapBuffers(active_windows[win-1].dpy,
                       active_windows[win-1].win);
    }

    //printf("svt_system: swapping buffers for window: %i id: %i dsp: %i \n", win, active_windows[win-1].win, active_windows[win-1].dpy);
};


///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

svt_win_handle svt_getCurrentWindow()
{
    GLXContext cx = glXGetCurrentContext();
    int i;

    if (cx != NULL)
        for (i=0;i<active_windows_count; i++)
            if (active_windows[i].cx == cx)
            {
                return i+1;
            }

    printf("svt_system: get current context not sucessful!");

    return 1;
};

bool svt_openWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return false;

    // Map window to display
    XMapWindow(active_windows[win-1].dpy,
               active_windows[win-1].win);

    // Move the window to the correct position
    XMoveWindow(active_windows[win-1].dpy, active_windows[win-1].win,
                active_windows[win-1].pos_x, active_windows[win-1].pos_y);

    // Just a workaround for the fullscreen mode. Otherwise the window will not be initialised correctly.
    if (!active_windows[win-1].border)
        XResizeWindow(active_windows[win-1].dpy, active_windows[win-1].win,
                      DisplayWidth(active_windows[active_windows_count-1].dpy, DefaultScreen(active_windows[active_windows_count-1].dpy)),
                      DisplayHeight(active_windows[active_windows_count-1].dpy, DefaultScreen(active_windows[active_windows_count-1].dpy)));

    return true;
};

void svt_switchWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    // Create OpenGL context
    if (active_windows[win-1].cx == NULL)
    {
        if ((active_windows[win-1].cx = glXCreateContext(active_windows[win-1].dpy,
                                                     active_windows[win-1].vis,
                                                     NULL, True))==NULL)
            __svt_fatalError("svt_system: could not create rendering context!");
    }
    if (glXMakeCurrent(active_windows[win-1].dpy,
                       active_windows[win-1].win,
                       active_windows[win-1].cx) == false)
        printf("svt_system: couldnt switch to other glx context!");
};

/**
 * set mouse position
 * \param iX new x position
 * \param iY new y position
 */
void svt_setMousePos(int iX, int iY)
{
    svt_win_handle handle = svt_getCurrentWindow();

    XWarpPointer(active_windows[handle-1].dpy, None, active_windows[handle-1].win, 0, 0, 0, 0, iX, iY);
    XSync(active_windows[handle-1].dpy, false);
};

/**
 * hide mouse
 */
DLLEXPORT void svt_hideMouse()
{
    svt_win_handle handle = svt_getCurrentWindow();
    Cursor cur = XCreateFontCursor(active_windows[handle-1].dpy, 2);
    XDefineCursor(active_windows[handle-1].dpy, active_windows[handle-1].win, cur);
}
/**
 * show mouse
 */
DLLEXPORT void svt_showMouse()
{
    svt_win_handle handle = svt_getCurrentWindow();
    XUndefineCursor(active_windows[handle-1].dpy, active_windows[handle-1].win);
}

///////////////////////////////////////////////////////////////////////////////
// window create function
///////////////////////////////////////////////////////////////////////////////

void __svt_addValueToCA(int val)
{
    current_conf[current_conf_counter] = val;
    current_conf_counter++;
};

void __svt_createConfArray()
{
    current_conf_counter = 0;

    // double buffer
    if (current_doublebuf)
    {
        active_windows[active_windows_count-1].doublebuf = current_doublebuf;
        __svt_addValueToCA(GLX_DOUBLEBUFFER);
    }
    // rgba
    if (current_rgba)
        __svt_addValueToCA(GLX_RGBA);

    __svt_addValueToCA(GLX_RED_SIZE);
    __svt_addValueToCA(1);
    __svt_addValueToCA(GLX_GREEN_SIZE);
    __svt_addValueToCA(1);
    __svt_addValueToCA(GLX_BLUE_SIZE);
    __svt_addValueToCA(1);

    // depth buffer size
    if (current_depth != 0)
    {
        __svt_addValueToCA(GLX_DEPTH_SIZE);
        __svt_addValueToCA(current_depth);
    }

    // stereo mode
    if (current_stereo)
    {
        active_windows[active_windows_count-1].stereo = current_stereo;
        __svt_addValueToCA(GLX_STEREO);
    }

    // end of ca
    __svt_addValueToCA(None);
}

bool __svt_setMwmBorder(Display *dpy, Window w, unsigned long flags)
{
    PropMotifWmHints motif_hints;
    Atom prop, proptype;
    /* setup the property */
    motif_hints.flags = MWM_HINTS_DECORATIONS;
    motif_hints.decorations = flags;
    /* get the atom for the property */
    prop = XInternAtom( dpy, "_MOTIF_WM_HINTS", True );
    if (!prop) {
        /* something went wrong! */
        return false;
    }
    /* not sure this is correct, seems to work, XA_WM_HINTS didn't work */
    proptype = prop;
    XChangeProperty( dpy, w,                         /* display, window */
                     prop, proptype,                 /* property, type */
                     32,                             /* format: 32-bit datums */
                     PropModeReplace,                /* mode */
                     (unsigned char *) &motif_hints, /* data */
                     PROP_MOTIF_WM_HINTS_ELEMENTS    /* nelements */
                   );

    return true;
}

// this function evaluates if a new thread should be generated, handling the window
bool __svt_checkNewThread()
{
    //return true;

    int i;

    for(i=0;i<active_windows_count-1;i++)
    {
        //printf("checkNewThread %i\n", i);

        // Only create a new thread if the new window is on a different screen. If the hostname _and_ the screennumber is equal,
        // dont create a new thread.
        if (strcmp(DisplayString(active_windows[i].dpy),DisplayString(active_windows[active_windows_count-1].dpy))==0)
            return false;
    }

    return true;
};

svt_win_handle __svt_createWindow(int _tx, int _ty, int _tsx, int _tsy, bool _fs, const char* disp)
{
    static char def_disp[] = "localhost";
    sema.P();

    if (/*__svt_threads &&*/ !__svt_x11threads)
    {
        if (XInitThreads() == 0)
            printf("svt_system: warning! Your x server is not thread safe! Please use X11R6 or higher.\n");
        //    __svt_fatalError("svt_system: your x server is not thread safe! Please use X11R6 or higher.");
        __svt_x11threads = true;
    }

    if (active_windows != NULL)
    {
        active_windows = (svt_win*)realloc(active_windows, (active_windows_count+1) * sizeof(svt_win));
    } else
        active_windows = (svt_win*)malloc(sizeof(svt_win));

    if (active_windows == NULL)
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
    active_windows[active_windows_count-1].cx = NULL;
    active_windows[active_windows_count-1].win = 0;
    active_windows[active_windows_count-1].current_canvas = 0;
    active_windows[active_windows_count-1].dpy = 0;
    active_windows[active_windows_count-1].redraw = true;
    active_windows[active_windows_count-1].swap_buf = false;
    active_windows[active_windows_count-1].sema = new svt_semaphore(1);

    // create display
    if (!disp)
        disp=getenv("DISPLAY");
    if (!disp || strlen(disp)==0)
        disp=def_disp;
    active_windows[active_windows_count-1].dpy = XOpenDisplay(disp);

    if (!active_windows[active_windows_count-1].dpy)
        __svt_fatalError("svt_system: couldnt connect to X server!");

    // init size of window
    if (_fs)
    {
        active_windows[active_windows_count-1].pos_x = 0;
        active_windows[active_windows_count-1].pos_y = 0;
        active_windows[active_windows_count-1].size_x = DisplayWidth(active_windows[active_windows_count-1].dpy, DefaultScreen(active_windows[active_windows_count-1].dpy));
        active_windows[active_windows_count-1].size_y = DisplayHeight(active_windows[active_windows_count-1].dpy, DefaultScreen(active_windows[active_windows_count-1].dpy));
        active_windows[active_windows_count-1].border = false;

    } else {
        active_windows[active_windows_count-1].pos_x = _tx;
        active_windows[active_windows_count-1].pos_y = _ty;
        active_windows[active_windows_count-1].size_x = _tsx;
        active_windows[active_windows_count-1].size_y = _tsy;
        active_windows[active_windows_count-1].border = true;
    }

    // Can we use OpenGL?
    if (!glXQueryExtension(active_windows[active_windows_count-1].dpy, NULL, NULL))
        __svt_fatalError("svt_system: X server has no OpenGL GLX extension");

    // Create Configuration Array
    __svt_createConfArray();

    // Create visual
    if ((active_windows[active_windows_count-1].vis = glXChooseVisual(active_windows[active_windows_count-1].dpy,
                                                                      DefaultScreen(active_windows[active_windows_count-1].dpy),
                                                                      current_conf))==NULL)
    {
        // ok, lets toggle rgba...
        current_rgba = !current_rgba;
        __svt_createConfArray();

        if ((active_windows[active_windows_count-1].vis = glXChooseVisual(active_windows[active_windows_count-1].dpy,
                                                                          DefaultScreen(active_windows[active_windows_count-1].dpy),
                                                                          current_conf))==NULL)
            __svt_fatalError("svt_system: desired visual not available");
    }

    // Create ColorMap
    active_windows[active_windows_count-1].cmap = XCreateColormap(
                                                                  active_windows[active_windows_count-1].dpy,
                                                                  RootWindow(active_windows[active_windows_count-1].dpy,
                                                                             active_windows[active_windows_count-1].vis->screen),
                                                                  active_windows[active_windows_count-1].vis->visual,
                                                                  AllocNone);
    // Attributes
    active_windows[active_windows_count-1].swa.colormap = active_windows[active_windows_count-1].cmap;
    active_windows[active_windows_count-1].swa.background_pixmap = None;
    active_windows[active_windows_count-1].swa.event_mask = EVT_MASKS;
    active_windows[active_windows_count-1].swa.border_pixel = 0;

    // Create Window
    active_windows[active_windows_count-1].win = XCreateWindow(
                                                               active_windows[active_windows_count-1].dpy,
                                                               RootWindow(active_windows[active_windows_count-1].dpy,
                                                                          active_windows[active_windows_count-1].vis->screen),
                                                               active_windows[active_windows_count-1].pos_x,
                                                               active_windows[active_windows_count-1].pos_y,
                                                               active_windows[active_windows_count-1].size_x,
                                                               active_windows[active_windows_count-1].size_y,
                                                               0,
                                                               active_windows[active_windows_count-1].vis->depth,
                                                               InputOutput,
                                                               active_windows[active_windows_count-1].vis->visual,
                                                               CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask,
                                                               &active_windows[active_windows_count-1].swa);

    // Ok?
    if (!active_windows[active_windows_count-1].win)
        __svt_fatalError("svt_system: cannot open window");

    XSelectInput(active_windows[active_windows_count-1].dpy,
                 active_windows[active_windows_count-1].win,
                 EVT_MASKS
                 );

    // Set Properties
    XSetStandardProperties(active_windows[active_windows_count-1].dpy,
                           active_windows[active_windows_count-1].win,
                           current_name,
                           current_name,
                           None,
                           NULL,
                           0,
                           NULL);

    // Fullscreen Mode?
    if (!active_windows[active_windows_count-1].border)
        __svt_setMwmBorder(active_windows[active_windows_count-1].dpy,
                           active_windows[active_windows_count-1].win,
                           0);

    // initialize redrawing
    if (__svt_threads)
    {
        // Is it a new display? Then start a new thread for redrawing
        if (__svt_checkNewThread())
        {
            //printf("svt_system: create new thread!\n");
            active_windows[active_windows_count-1].thread = thread_count;
            svt_createThread((void*(*)(void*))svt_redrawThread,new int(thread_count));
            thread_count++;
        } else {
            //printf("svt_system: no new thread created!\n");
            active_windows[active_windows_count-1].thread = thread_count-1;
        }

        printf("svt_system: creating window for thread %i\n", active_windows[active_windows_count-1].thread);
    }
    sema.V();

    return active_windows_count;
};

void __svt_destroyWindow(svt_win_handle)
{
};

///////////////////////////////////////////////////////////////////////////////
// window event function
///////////////////////////////////////////////////////////////////////////////

// check the x11 events for one window
void svt_checkEvents(svt_win_handle win)
{
    XEvent event;
    KeySym ks;
    char tmp;

    XSetIOErrorHandler(&__svt_XlibIOErrorHandler);

    if (__other_x11display != NULL)
    {
	int fd_1 = ConnectionNumber(active_windows[win-1].dpy);
	int fd_2 = ConnectionNumber(__other_x11display);

	// set up the read mask for select()
	fd_set read_mask, write_mask, excep_mask;

	FD_ZERO(&read_mask);
	FD_ZERO(&write_mask);
	FD_ZERO(&excep_mask);

        FD_SET(fd_1, &read_mask);
	FD_SET(fd_2, &read_mask);

        int num_fds;
        if (fd_1 > fd_2)
	    num_fds = fd_1 + 1;
        else
	    num_fds = fd_2 + 1;

	timeval oTimeout; memset( &oTimeout, 0, sizeof( timeval ) );
	oTimeout.tv_sec = 0;
        oTimeout.tv_usec = 100;

	select(num_fds,
	       &read_mask,	                // read mask
	       &write_mask,	        // no write mask
	       &excep_mask,	        // no exception mask
	       &oTimeout);	                // 1sec timeout
        //(struct timeval *)0);	// no timeout
    }

    if (XPending(active_windows[win-1].dpy))
    {
	while (XCheckWindowEvent(active_windows[win-1].dpy, active_windows[win-1].win, EVT_MASKS, &event))
	{
	    //printf("svt_system: event type %i occured for window %i\n", event.type, win);
	    switch(event.type)
	    {
		// resize
	    case ConfigureNotify:
		while (XCheckWindowEvent(active_windows[win-1].dpy, active_windows[win-1].win, StructureNotifyMask, &event)) {};
		//printf("svt_system(%i): configurenotify event for win %i occured\n",thread,win);

		// check for "reasonable" dimensions to avoid strange display problems
		// (sometimes the first event has screwed values)
		if (
		    event.xconfigure.width >0 && event.xconfigure.width <1000000  &&
		    event.xconfigure.height>0 && event.xconfigure.height<1000000    )
		{
		    active_windows[win-1].pos_x  = event.xconfigure.x;
		    active_windows[win-1].pos_y  = event.xconfigure.y;
		    active_windows[win-1].size_x = event.xconfigure.width;
		    active_windows[win-1].size_y = event.xconfigure.height;
		}

		//XResizeWindow(active_windows[win-1].dpy, active_windows[win-1].win, active_windows[win-1].size_x, active_windows[win-1].size_y);
		//XMoveWindow(active_windows[win-1].dpy, active_windows[win-1].win,
		//            active_windows[win-1].pos_x, active_windows[win-1].pos_y);
		//glXWaitX();
		__svt_resizeWindow(win);
		break;
		// mouse move
	    case MotionNotify:
		while (XCheckWindowEvent(active_windows[win-1].dpy, active_windows[win-1].win, PointerMotionMask, &event)) {};
		//printf("svt_system: mouse motion event for win %i occured\n",win);
		active_windows[win-1].mouse_x = event.xmotion.x;
		active_windows[win-1].mouse_y = event.xmotion.y;
		//printf("svt_system: mouse at position %i,%i\n", event.xmotion.x, event.xmotion.y);
		if (active_windows[win-1].button)
		    __svt_dragMouse(win);
		else
		    __svt_moveMouse(win);
		break;

		// button down event
	    case ButtonPress:
		//printf("svt_system: button press event for win %i occured\n",win);
		switch(event.xbutton.button)
		{
		case Button1:
		    active_windows[win-1].button = SVT_LEFT_MOUSE_BUTTON;
		    __svt_lmb_down(win);
		    break;
		case Button2:
		    active_windows[win-1].button = SVT_MIDDLE_MOUSE_BUTTON;
		    __svt_mmb_down(win);
		    break;
		case Button3:
		    active_windows[win-1].button = SVT_RIGHT_MOUSE_BUTTON;
		    __svt_rmb_down(win);
                    break;
                case Button4:
		    active_windows[win-1].button = SVT_MOUSE_WHEEL_UP;
		    __svt_umb_down(win);
                    break;
                case Button5:
		    active_windows[win-1].button = SVT_MOUSE_WHEEL_DOWN;
		    __svt_dmb_down(win);
                    break;
		default:
		    active_windows[win-1].button = 0;
		    break;
		}
		break;
		// button up event
	    case ButtonRelease:
		//printf("svt_system: button release event for win %i occured\n",win);
		active_windows[win-1].button = 0;
		if (event.xbutton.button == Button1)
		    __svt_lmb_up(win);
		if (event.xbutton.button == Button2)
		    __svt_mmb_up(win);
		if (event.xbutton.button == Button3)
                    __svt_rmb_up(win);
		if (event.xbutton.button == Button4)
		    __svt_umb_down(win);
		if (event.xbutton.button == Button5)
		    __svt_dmb_down(win);
		break;
		// key pressed event
	    case KeyPress:
		//printf("svt_system(%i): Key press event!\n", thread);
		XLookupString(&event.xkey, &tmp, 1,NULL, NULL);
		__svt_key_pressed(win, tmp);
		ks = XLookupKeysym((XKeyEvent *) & event, 0);
		__svt_special_key_pressed(win,ks);
		break;
		// key released event
	    case KeyRelease:
		//printf("svt_system(%i): Key release event!\n", thread);
		XLookupString(&event.xkey, &tmp, 1,NULL, NULL);
		__svt_key_released(win, tmp);
		break;
		// redraw
	    case Expose:
		//printf("svt_system: expose event count %i occured %i\n",event.xexpose.count, svt_getToD());
		while (XCheckWindowEvent(active_windows[win-1].dpy, active_windows[win-1].win, ExposureMask, &event)) {};
		//printf("svt_system: expose event count %i occured\n",event.xexpose.count);
		if (event.xexpose.count == 0)
		    svt_redrawWindow(win);
                bRedrawEvent = false;
                break;

	    default:
	        break;
	    }
	}
    }

    if ( svt_getToD() - iLastQtUpdate > 100)
    {
	if (guiCallback != NULL)
	    (guiCallback)();

        iLastQtUpdate = svt_getToD();
    }
}

/**
 * Wake-up windows - the windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 * Internal function, please call svt_forceUpdateWindows!
 */
void svt_wakeupWindow(svt_win_handle win)
{
    if (bRedrawEvent == false)
    {
	bRedrawEvent = true;

	XEvent event;
	memset(&event, 0x00, sizeof(event));

	event.type = Expose;
	event.xexpose.type = Expose;
	event.xexpose.send_event = true;
	event.xexpose.window = active_windows[win-1].win;
	event.xexpose.x = 0;
	event.xexpose.y = 0;
	event.xexpose.width = active_windows[win-1].size_x;
	event.xexpose.height = active_windows[win-1].size_y;

	XSendEvent(active_windows[win-1].dpy, active_windows[win-1].win, false, ExposureMask, &event);

	XFlush(active_windows[win-1].dpy);
    }
}

#endif
