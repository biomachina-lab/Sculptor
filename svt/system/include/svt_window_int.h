///////////////////////////////////////////////////////////////////////////////
// Everything in here is for internal use only!!!!
///////////////////////////////////////////////////////////////////////////////

#ifdef SVT_SYSTEM_X11
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#endif

struct svt_win
{
    int pos_x;
    int pos_y;
    int size_x;
    int size_y;
    bool border;
    // which thread is responsible for the event handling for this window?
    int thread;
    bool stereo;
    bool doublebuf;
    int mouse_x;
    int mouse_y;
    int button;
    bool active;
    bool redraw;
    bool swap_buf;
    svt_semaphore* sema;
    int current_canvas;
    int samples;

#ifdef SVT_SYSTEM_WIN32
        HWND win;
	HDC hdc;
	HGLRC hglrc;
#endif

#ifdef SVT_SYSTEM_X11
        Window win;
        Display* dpy;
        XVisualInfo *vis;
        GLXContext cx;
        Colormap cmap;
        XSetWindowAttributes swa;
#endif

#ifdef SVT_SYSTEM_QT
        void* win;
#endif

    // callback functions

    // drawing function
    void (*func)(svt_win_handle,int,int);
    // resize function
    void (*resize)(svt_win_handle,int,int);
    // left mouse button down function
    void (*lmb_down)(svt_win_handle,int,int);
    // right mouse button down function
    void (*rmb_down)(svt_win_handle,int,int);
    // middle mouse button down function
    void (*mmb_down)(svt_win_handle,int,int);
    // mouse wheel up button pressed function
    void (*umb_down)(svt_win_handle,int,int);
    // mouse wheel down button pressed function
    void (*dmb_down)(svt_win_handle,int,int);
    // left mouse button up function
    void (*lmb_up)(svt_win_handle,int,int);
    // right mouse button up function
    void (*rmb_up)(svt_win_handle,int,int);
    // middle mouse button up function
    void (*mmb_up)(svt_win_handle,int,int);
    // mouse wheel up button released function
    void (*umb_up)(svt_win_handle,int,int);
    // mouse wheel down button released function
    void (*dmb_up)(svt_win_handle,int,int);
    // mouse moving function
    void (*move)(svt_win_handle,int,int);
    // mouse dragging function
    void (*drag)(svt_win_handle,int,int,int);
    // key pressed function
    void (*key_pressed)(svt_win_handle,int,int, char key);
    // key released function
    void (*key_released)(svt_win_handle,int,int, char key);
    // special key pressed function
    void (*special_key_pressed)(svt_win_handle,int,int,int);
   // special key released function
    void (*special_key_released)(svt_win_handle,int,int,int);
};

// functions defined in the system dependend modules
void __svt_fatalError(char *s);
svt_win_handle __svt_createWindow(int _tx, int _ty, int _tsx, int _tsy, bool _fs, const char* disp);
void __svt_destroyWindow(svt_win_handle win);
bool __svt_mainloopWindow( void );
void __svt_initWindow( void );
#ifdef SVT_SYSTEM_X11
void __svt_special_key_pressed(svt_win_handle win, KeySym ks);
void __svt_special_key_released(svt_win_handle win, KeySym ks);
void __svt_setOtherX11Display( Display* pDisp );
#else
void __svt_special_key_pressed(svt_win_handle win, int key);
void __svt_special_key_released(svt_win_handle win, int key);
#endif

// functions in the main module, called from the system dependend modules
bool __svt_checkWinParam(svt_win_handle win);
void __svt_dragMouse(svt_win_handle win);
void __svt_moveMouse(svt_win_handle win);
void __svt_lmb_down(svt_win_handle win);
void __svt_rmb_down(svt_win_handle win);
void __svt_mmb_down(svt_win_handle win);
void __svt_umb_down(svt_win_handle win);
void __svt_dmb_down(svt_win_handle win);
void __svt_lmb_up(svt_win_handle win);
void __svt_rmb_up(svt_win_handle win);
void __svt_mmb_up(svt_win_handle win);
void __svt_umb_up(svt_win_handle win);
void __svt_dmb_up(svt_win_handle win);
void __svt_key_pressed(svt_win_handle win, char key);
void __svt_key_released(svt_win_handle win, char key);
void __svt_resizeWindow(svt_win_handle win);
void __svt_idle();
void __svt_redrawWindow(svt_win_handle win);
void __svt_idleTest();
void* svt_redrawThread(void* null);
void __svt_swapBuffers(svt_win_handle win);
