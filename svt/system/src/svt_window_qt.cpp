/***************************************************************************
                          svt_window_qt
                          -------------
    begin                : 12/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifdef SVT_SYSTEM_QT

// qt4 includes
#include <QDesktopWidget>
#include <QMessageBox>
#include <QImage>
// svt includes
#include <svt_window.h>
#include <svt_window_qt.h>
#include <svt_window_int.h>
#include <svt_window_vars.h>
#include <svt_system.h>
#include <svt_time.h>
#include <svt_init.h>
// gl/glx includes
#if defined(Q_WS_X11)
#include <GL/glx.h>
#endif
#if defined(Q_WS_MAC)
#include <agl.h>
#endif

QApplication* g_pApp = NULL;
QWidget* g_pMainWidget = NULL;

bool g_bRedrawEvent = false;

///////////////////////////////////////////////////////////////////////////////
// forward declarations
///////////////////////////////////////////////////////////////////////////////

// gui callback function
extern void (*guiCallback)();

///////////////////////////////////////////////////////////////////////////////
// helper functions
///////////////////////////////////////////////////////////////////////////////

void __svt_fatalError(const char *s)
{
    QMessageBox::critical( 0, "SVT",
                          QString(s)
                         );

    if (g_pApp == NULL)
        exit(1);
    else {
        g_pApp->quit();
        exit(1);
    }
}

DLLEXPORT bool __svt_mainloopWindow( void )
{
    g_pApp->exec();

    exit(0);

    return true;
}

void __svt_initWindow( void )
{
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

///////////////////////////////////////////////////////////////////////////////
// svt_app - Implementation
///////////////////////////////////////////////////////////////////////////////

svt_app::svt_app(int argc, char* argv[]) : QApplication(argc, argv)
{
    //setGlobalMouseTracking( TRUE );
};

void svt_app::quit()
{
    QApplication::quit();
    exit(0);
};

///////////////////////////////////////////////////////////////////////////////
// svt_qtGLWidget - Implementation
///////////////////////////////////////////////////////////////////////////////

svt_qtGLWidget::svt_qtGLWidget( QWidget* pParent, int iWin, QGLFormat oFormat )
    : QGLWidget( oFormat, pParent ),
      m_iWin( iWin )
{
    setMouseTracking( TRUE );
    setFocusPolicy( Qt::StrongFocus );
    setEnabled( TRUE );
    setAutoBufferSwap( TRUE );
    setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    if (current_name != NULL)
     	setWindowTitle(QString(current_name));
};

svt_qtGLWidget::~svt_qtGLWidget()
{
};

void svt_qtGLWidget::resizeGL( int iW, int iH )
{
    active_windows[m_iWin-1].size_x = iW;
    active_windows[m_iWin-1].size_y = iH;

    __svt_resizeWindow( m_iWin );
};

void svt_qtGLWidget::paintGL()
{
    setMouseTracking( TRUE );

    if(!svt_getRunning())
    {
        svt_redrawWindow( m_iWin );
        g_bRedrawEvent = false;
    }
    else
    {
	// could call an updateGL, too. the update is qeued, the updateGL immediate
        update();
    }
};

void svt_qtGLWidget::mousePressEvent( QMouseEvent* pEvent )
{
    if(pEvent->button() == Qt::LeftButton)
    {
        active_windows[m_iWin-1].button = SVT_LEFT_MOUSE_BUTTON;
        __svt_lmb_down(m_iWin);
    }
    if(pEvent->button() == Qt::RightButton)
    {
        active_windows[m_iWin-1].button = SVT_RIGHT_MOUSE_BUTTON;
        __svt_rmb_down(m_iWin);
    }
    if(pEvent->button() == Qt::MidButton)
    {
        active_windows[m_iWin-1].button = SVT_MIDDLE_MOUSE_BUTTON;
        __svt_mmb_down(m_iWin);
    }

    setFocus();

    // FIXME update here?
    updateGL();

    __svt_idle();
}

void svt_qtGLWidget::mouseReleaseEvent( QMouseEvent* pEvent )
{
    if(pEvent->button() == Qt::LeftButton)
        __svt_lmb_up(m_iWin);
    if(pEvent->button() == Qt::RightButton)
        __svt_rmb_up(m_iWin);
    if(pEvent->button() == Qt::MidButton)
        __svt_mmb_up(m_iWin);

    active_windows[m_iWin-1].button = 0;

    // FIXME update here?
    updateGL();

    __svt_idle();
}

void svt_qtGLWidget::mouseMoveEvent( QMouseEvent* pEvent )
{
    active_windows[m_iWin-1].mouse_x = pEvent->x();
    active_windows[m_iWin-1].mouse_y = pEvent->y();

    if (active_windows[m_iWin-1].button)
        __svt_dragMouse(m_iWin);
    else
	__svt_moveMouse(m_iWin);

    // FIXME update here?
    //updateGL();

    __svt_idle();

    //qApp->processEvents();

};

void svt_qtGLWidget::wheelEvent( QWheelEvent * pEvent )
{
    if (pEvent->delta() > 0)
        __svt_umb_down(m_iWin);
    else
        __svt_dmb_down(m_iWin);

    // FIXME update here?
    updateGL();

    __svt_idle();
};

void svt_qtGLWidget::keyPressEvent( QKeyEvent* pEvent )
{
    switch (pEvent->key())
    {
	case Qt::Key_F1:       __svt_special_key_pressed(m_iWin,SVT_KEY_F1);        break;
	case Qt::Key_F2:       __svt_special_key_pressed(m_iWin,SVT_KEY_F2);        break;
	case Qt::Key_F3:       __svt_special_key_pressed(m_iWin,SVT_KEY_F3);        break;
	case Qt::Key_F4:       __svt_special_key_pressed(m_iWin,SVT_KEY_F4);        break;
	case Qt::Key_F5:       __svt_special_key_pressed(m_iWin,SVT_KEY_F5);        break;
	case Qt::Key_F6:       __svt_special_key_pressed(m_iWin,SVT_KEY_F6);        break;
	case Qt::Key_F7:       __svt_special_key_pressed(m_iWin,SVT_KEY_F7);        break;
	case Qt::Key_F8:       __svt_special_key_pressed(m_iWin,SVT_KEY_F8);        break;
	case Qt::Key_F9:       __svt_special_key_pressed(m_iWin,SVT_KEY_F9);        break;
	case Qt::Key_F10:      __svt_special_key_pressed(m_iWin,SVT_KEY_F10);       break;
	case Qt::Key_F11:      __svt_special_key_pressed(m_iWin,SVT_KEY_F11);       break;
	case Qt::Key_F12:      __svt_special_key_pressed(m_iWin,SVT_KEY_F12);       break;
	case Qt::Key_PageUp:   __svt_special_key_pressed(m_iWin,SVT_KEY_PAGE_UP);   break;
	case Qt::Key_PageDown: __svt_special_key_pressed(m_iWin,SVT_KEY_PAGE_DOWN); break;
	case Qt::Key_Home:     __svt_special_key_pressed(m_iWin,SVT_KEY_HOME);      break;
	case Qt::Key_End:      __svt_special_key_pressed(m_iWin,SVT_KEY_END);       break;
	case Qt::Key_Insert:   __svt_special_key_pressed(m_iWin,SVT_KEY_INSERT);    break;
	case Qt::Key_Right:    __svt_special_key_pressed(m_iWin,SVT_KEY_RIGHT);     break;
	case Qt::Key_Left:     __svt_special_key_pressed(m_iWin,SVT_KEY_LEFT);      break;
	case Qt::Key_Up:       __svt_special_key_pressed(m_iWin,SVT_KEY_UP);        break;
	case Qt::Key_Down:     __svt_special_key_pressed(m_iWin,SVT_KEY_DOWN);      break;
	case Qt::Key_Control:  __svt_special_key_pressed(m_iWin,SVT_KEY_CONTROL);   break;
	case Qt::Key_Shift:    __svt_special_key_pressed(m_iWin,SVT_KEY_SHIFT);     break;
	case Qt::Key_Alt:      __svt_special_key_pressed(m_iWin,SVT_KEY_ALT);       break;

	default:               __svt_key_pressed(m_iWin, pEvent->key());            break;
    }

    // FIXME update here?
    updateGL();

    pEvent->accept();

    __svt_idle();
}

void svt_qtGLWidget::keyReleaseEvent( QKeyEvent* pEvent )
{
    switch (pEvent->key())
    {
	case Qt::Key_F1:       __svt_special_key_released(m_iWin,SVT_KEY_F1);        break;
	case Qt::Key_F2:       __svt_special_key_released(m_iWin,SVT_KEY_F2);        break;
	case Qt::Key_F3:       __svt_special_key_released(m_iWin,SVT_KEY_F3);        break;
	case Qt::Key_F4:       __svt_special_key_released(m_iWin,SVT_KEY_F4);        break;
	case Qt::Key_F5:       __svt_special_key_released(m_iWin,SVT_KEY_F5);        break;
	case Qt::Key_F6:       __svt_special_key_released(m_iWin,SVT_KEY_F6);        break;
	case Qt::Key_F7:       __svt_special_key_released(m_iWin,SVT_KEY_F7);        break;
	case Qt::Key_F8:       __svt_special_key_released(m_iWin,SVT_KEY_F8);        break;
	case Qt::Key_F9:       __svt_special_key_released(m_iWin,SVT_KEY_F9);        break;
	case Qt::Key_F10:      __svt_special_key_released(m_iWin,SVT_KEY_F10);       break;
	case Qt::Key_F11:      __svt_special_key_released(m_iWin,SVT_KEY_F11);       break;
	case Qt::Key_F12:      __svt_special_key_released(m_iWin,SVT_KEY_F12);       break;
	case Qt::Key_PageUp:   __svt_special_key_released(m_iWin,SVT_KEY_PAGE_UP);   break;
	case Qt::Key_PageDown: __svt_special_key_released(m_iWin,SVT_KEY_PAGE_DOWN); break;
	case Qt::Key_Home:     __svt_special_key_released(m_iWin,SVT_KEY_HOME);      break;
	case Qt::Key_End:      __svt_special_key_released(m_iWin,SVT_KEY_END);       break;
	case Qt::Key_Insert:   __svt_special_key_released(m_iWin,SVT_KEY_INSERT);    break;
	case Qt::Key_Right:    __svt_special_key_released(m_iWin,SVT_KEY_RIGHT);     break;
	case Qt::Key_Left:     __svt_special_key_released(m_iWin,SVT_KEY_LEFT);      break;
	case Qt::Key_Up:       __svt_special_key_released(m_iWin,SVT_KEY_UP);        break;
	case Qt::Key_Down:     __svt_special_key_released(m_iWin,SVT_KEY_DOWN);      break;
	case Qt::Key_Control:  __svt_special_key_released(m_iWin,SVT_KEY_CONTROL);   break;
	case Qt::Key_Shift:    __svt_special_key_released(m_iWin,SVT_KEY_SHIFT);     break;
	case Qt::Key_Alt:      __svt_special_key_released(m_iWin,SVT_KEY_ALT);       break;

	default:               __svt_key_released(m_iWin, pEvent->key());            break;
    }

    // FIXME update here?
    updateGL();
}

void svt_qtGLWidget::closeEvent ( QCloseEvent * pEvent )
{
    QWidget::closeEvent( pEvent );

    g_pApp->exit(0);
    exit(0);
};

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

void __svt_swapBuffers(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;
    active_windows[win-1].swap_buf = false;
};

svt_win_handle svt_getCurrentWindow()
{
    // FIXME: return m_iWin ?
    return 1;
};

bool svt_openWindow(svt_win_handle win)
{
    svt_qtGLWidget* pWidget = (svt_qtGLWidget*)(active_windows[win-1].win);

    if ( active_windows[win-1].pos_x == 0 && active_windows[win-1].pos_y == 0)
        pWidget->resize( active_windows[win-1].size_x, active_windows[win-1].size_y );
    else
        pWidget->setGeometry( QRect(  active_windows[win-1].pos_x, active_windows[win-1].pos_y, active_windows[win-1].size_x, active_windows[win-1].size_y ) );

    if ( active_windows[win-1].border == false )
        pWidget->setWindowState( pWidget->windowState() ^ Qt::WindowFullScreen );

    //alternative implementation of the fullscreen-switch, deprecated.
    //pWidget->reparent( 0, Qt::WStyle_Customize | Qt::WStyle_NoBorder, QPoint( 0, 0 ), false );

    pWidget->show();

    return true;
};

void svt_switchWindow(svt_win_handle win)
{
    if(!__svt_checkWinParam(win)) return;

    svt_qtGLWidget* pWidget = ((svt_qtGLWidget*)(active_windows[win-1].win));
    if (pWidget && pWidget->context()->isValid())
        pWidget->makeCurrent();
};

/**
 * set mouse position
 * \param iX new x position
 * \param iY new y position
 */
void svt_setMousePos(int, int)
{
};

///////////////////////////////////////////////////////////////////////////////
// window create function
///////////////////////////////////////////////////////////////////////////////

svt_win_handle __svt_createWindow(int _tx, int _ty, int _tsx, int _tsy, bool _fs, const char*)
{
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

    active_windows[active_windows_count-1].pos_x = _tx;
    active_windows[active_windows_count-1].pos_y = _ty;
    active_windows[active_windows_count-1].size_x = _tsx;
    active_windows[active_windows_count-1].size_y = _tsy;
    active_windows[active_windows_count-1].border = true;

    active_windows[active_windows_count-1].samples = current_samples;

    // do we have a qapplication already?
    if (g_pApp == NULL)
    {
        if (QApplication::instance() == NULL)
        {
            int iArgc = 0;
            char** pArgv = NULL;

            g_pApp = new svt_app(iArgc, pArgv);
        } else
            g_pApp = (QApplication*)QApplication::instance();
    }

    QGLFormat oFormat( QGL::StencilBuffer | QGL::AlphaChannel );

    // stereo
    oFormat.setStereo( current_stereo );
    // doublebuffering
    oFormat.setDoubleBuffer( current_doublebuf );
    // rgba
    oFormat.setRgba( current_rgba );
    // direct rendering
    oFormat.setDirectRendering( true );
    // sampling
    if (current_samples > 0)
    {
        oFormat.setSampleBuffers(true);
        oFormat.setSamples(current_samples);
    }

    // fullscreen
    if ( _fs )
    {
        active_windows[active_windows_count-1].border = false;
        active_windows[active_windows_count-1].pos_x = 0;
        active_windows[active_windows_count-1].pos_y = 0;
	// FIXME for desktops with multiple screens, the size of the desktop is the union of all the
	// screen sizes, so maybe adapt the following two lines (see doc of qdesktopwidget)
        active_windows[active_windows_count-1].size_x = QApplication::desktop()->size().width();
        active_windows[active_windows_count-1].size_y = QApplication::desktop()->size().height();
    }

    // create window
    svt_qtGLWidget* pWidget = new svt_qtGLWidget( g_pMainWidget, active_windows_count, oFormat );
    active_windows[active_windows_count-1].win = (void*)pWidget;
    //pWidget->setMinimumWidth( _tsx );
    //pWidget->setMinimumHeight( _tsy );

    // check format
    QGLFormat oFinalFormat = pWidget->format();

    if (oFinalFormat.stereo() != current_stereo)
        __svt_fatalError("[svt_system] Error: Stereo visual not available!");
    if (oFinalFormat.doubleBuffer() != current_doublebuf)
        __svt_fatalError("[svt_system] Error: Double-buffered visual not available!");
    if (oFinalFormat.rgba() != current_rgba)
        __svt_fatalError("[svt_system] Error: RGBA visual not available!");
    if (!oFinalFormat.directRendering())
        printf("[svt_system] Warning: No hardware-accelerated OpenGL found, using software-rendering.\n");
    if (current_samples > 0 && !oFinalFormat.sampleBuffers())
    {
        printf("[svt_system] Warning: No multi-sample extension found and therefore no anti-aliasing available.\n");
        active_windows[active_windows_count-1].samples = 0;
    }

    return active_windows_count;
};

void __svt_destroyWindow(svt_win_handle win)
{
    svt_qtGLWidget* pWindow = (svt_qtGLWidget*)(active_windows[win-1].win);
    pWindow->hide();
    delete pWindow;
    active_windows[win-1].win = NULL;
}

/**
 * Wake-up windows - the windows typically wait for a new event from the window-manager and only react when one arrives. Sometimes one would like to enforce an update, and therefore this function can be called.
 * Internal function, please call svt_forceUpdateWindows!
 */
void svt_wakeupWindow(svt_win_handle win)
{
    if(g_bRedrawEvent == false)
    {
        g_bRedrawEvent = true;
        svt_qtGLWidget* pWidget = (svt_qtGLWidget*)(active_windows[win-1].win);
        pWidget->update();
    }
};

/**
 * Get the main qt widget
 */
QWidget* svt_qtGetWidget()
{
    return (svt_qtGLWidget*)(active_windows[0].win);
};

/**
 * Set workspace/parent widget
 */
void svt_qtSetMainWidget(QWidget* pMainWidget)
{
    g_pMainWidget = pMainWidget;
};

QImage* svt_qtGetFramebuffer()
{
    svt_qtGLWidget* pWidget = (svt_qtGLWidget*)svt_qtGetWidget();
    QImage *pImage = NULL;

    if (pWidget)
    {
        // Make GL Context current
        pWidget->makeCurrent();

        // Copy from OpenGL
        pImage = new QImage( pWidget->grabFrameBuffer() );
    }

    return pImage;
};

#endif
