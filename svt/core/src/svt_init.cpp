/***************************************************************************
                          svt_init.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_init.h>
#include <svt_defs.h>
#include <svt_config.h>
#include <svt_system.h>
#include <svt_time.h>
#include <svt_array.h>
#include <svt_window.h>
#include <svt_matrix4f.h>
#include <svt_channel.h>
#include <svt_save_framebuffer.h>
#include <svt_contextData.h>
#include <svt_cmath.h>
#include <svt_matrix4.h>
#include <svt_opengl.h>
#include <svt_semaphore.h>
#include <svt_contextData.h>
#include <svt_pos.h>
// clib includes
#include <stdlib.h>

#ifdef TRUE_WIN32
#define strcasecmp stricmp
#endif

#define SVTINIT cout << svt_trimName("svt_init")

// If SVT_NVPM is defined, performance data of the graphics hard- and software will be
// collected and displayed, see svt_nvpm class
//
#ifdef SVT_NVPM
#include <svt_nvpm.h>
/* an object for GPU performance measurement */
svt_nvpm* g_SVTNVPM;
#endif


///////////////////////////////////////////////////////////////////////////////
// global variables
///////////////////////////////////////////////////////////////////////////////

/* the basic scene object */
svt_scene* g_pCurrentScene;

/** the main configuration object */
svt_config* g_pCurrentConfig;

/* the screenshot filename */
char g_pScreenShotFile[256];

/* the window title */
const char* g_pWindowTitle = NULL;
char  g_pWindowTitleStr[256];

/* are saving frames to disk? */
bool g_bSavingFrames;

/* current frame number */
int g_iCurrentFrame;

/* how many frames per second should the movie have */
int g_iSavingFPS;

/* how many frames */
int g_iSavingFramesMax;

/* how many windows */
int g_iWindowNum;

/* eclapsed time for current frame */
int g_iTimePerFrame;

/* define a drawing canvas */
#ifndef SVT_SKIP_DOC
typedef struct
{
    float perc_x;
    float perc_y;
    float perc_w;
    float perc_h;
    int pos_x;
    int pos_y;
    int size_x;
    int size_y;
    int win;
    svt_matrix4f* mat;
    float world_left;
    float world_top;
    float pixel_size_x;       // horizontal pixel size in mm/pix - fd
    float pixel_size_y;       // vertical   pixel size in mm/pix - fd
    bool  l_eye;              // indicates if current canvas is redered for left eye
} __svt_canvas;
__svt_canvas* g_pCanvas;
#endif

/* the viewer matrix */
svt_matrix4f* g_pViewerMat;
svt_matrix4<Real32> g_oOldViewerMat;

/* near clipping plane */
double g_fNear = 0.5f;
double g_fFar = 50.0f;

/* viewpoint tracker */
svt_device* g_pTrackerDevice;
svt_channel* g_pTrackerChannel;
int g_iTrackerChannelNum;

/* frames counter */
// static int g_iFPSCounter = 0;
static int g_iFPS = 0;
// static int g_iOldSec = 0;

/* svt stopped? */
static bool g_bStop = false;
static unsigned int g_iStop = 0;

/* the mouse matrix */
svt_matrix4f* g_pMouseMat;
int g_iMouseOldX = 0;
int g_iMouseOldY = 0;
int g_iMouseX = 0;
int g_iMouseY = 0;
Real32 g_fMouseRotX = 0;
Real32 g_fMouseRotY = 0;
Real32 g_fMouseTransX = 0;
Real32 g_fMouseTransY = 0;
Real32 g_fMouseTransZ = 0;
bool g_bLMB = false;
unsigned int g_iMouseMode = SVT_MOUSE_ROT;
bool g_bMouseRot = false;
bool g_bMouseZ = false;
bool g_bMouseXY = true; // is the default mouse plane xy or xz?

// background color
svt_color* g_pBackColor = NULL;

/* current eye and canvas matrix*/
bool g_bLeftEye = true;
svt_matrix4f* g_pCurrCanvasMat = NULL;

// shoud we recalc the aspect ratio (only suitable for windowed mode, not on holobench!)
bool g_bAspectRecalc = false;

// context specific data
svt_array<svt_contextData*> g_pContextData;

// alpha values
Real32 g_fCanvasAlphaX = -1.0;
Real32 g_fCanvasAlphaY = -1.0;

// redraw of the scene neccessary?
bool g_bRedraw = true;
// redraw of the scene in a coarse way (full rendering only if system is idle)
bool g_bRedrawCoarse = true;
// was the scene already rendered fully?
bool g_bRedrawFull = false;
// redraw the scene always?
bool g_bAlwaysRedraw = false;

// orthographic projection
bool g_bOrtho = false;

// eye separation in m
float g_fEyeSeparation = 0.07f;

// contextdata protection semaphore
svt_semaphore g_oContextSema;

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

/**
 * is the scene rendered in a coarse way right now, or fully
 * \return true if coarse render mode is active, if false we have to render everything as precisely as possible
 */
bool svt_renderCoarse()
{
    //return g_bRedrawCoarse;
    return false;
};

/**
 * get the configuration object
 * \return pointer to svt_config object
 */
svt_config* svt_getConfig()
{
    return g_pCurrentConfig;
}
/**
 * set the configuration object
 * \param pConfig pointer to svt_config object
 */
void svt_setConfig(svt_config* pConfig)
{
    g_pCurrentConfig = pConfig;
}

/**
 * set the near clipplane
 * \param fNear float value for the near clipplane
 */
void svt_setNear(Real32 fNear)
{
    g_fNear = fNear;
}
/**
 * get the near clipplane
 * \return float value for the near clipplane
 */
Real32 svt_getNear()
{
    return g_fNear;
}
/**
 * set the near clipplane
 * \param fFar float value for the near clipplane
 */
void svt_setFar(Real32 fFar)
{
    g_fFar = fFar;
}
/**
 * get the near clipplane
 * \return float value for the near clipplane
 */
Real32 svt_getFar()
{
    return g_fFar;
}

/**
 * set the background color
 * \param pColor pointer to svt_color object with the background color
 */
void svt_setBackgroundColor(svt_color* pColor)
{
    g_pBackColor = pColor;
}
/**
 * get the background color
 * \return pointer to svt_color object with the background color
 */
svt_color* svt_getBackgroundColor()
{
    return g_pBackColor;
}
/**
 * set the background color
 * \param fRed
 * \param fGreen
 * \param fBlue
 */
void svt_setBackgroundColor(float fRed, float fGreen, float fBlue)
{
    if (!g_pBackColor)
        g_pBackColor = new svt_color();

    g_pBackColor->setR(fRed);
    g_pBackColor->setG(fGreen);
    g_pBackColor->setB(fBlue);
}

/**
 * set the current scene object (internal - please use svt_start() function)
 * \param pScene pointer to svt_scene object
 * \internal
 */
void svt_setScene(svt_scene* pScene)
{
    g_pCurrentScene = pScene;
}
/**
 * get the current scene object
 * \return pointer to a svt_scene object (NULL if no scene object available)
 */
svt_scene* svt_getScene()
{
    return g_pCurrentScene;
}

/**
 * start the event loop
 * \param pScene pointer to svt_scene object
 */
void DLLEXPORT svt_start(svt_scene* pScene)
{
    svt_setScene(pScene);
    svt_system_mainloop();
}

/**
 * exit svt
 * \param iError error return code (0 = no error)
 */
void DLLEXPORT svt_exit(int iError)
{
    svt_destroyAllWindows();
    exit(iError);
#ifdef SVT_NVPM
    // shutdown NVPerfSDK
    if (g_SVTNVPM != NULL)
	g_SVTNVPM->shutdown();
#endif
}
/**
 * quit because of an error. you should try to find a suitable error text to inform the user why svt stops
 * \param pExitStr pointer to array of char with the exit string
 */
DLLEXPORT void svt_fatalError(const char* pExitStr)
{
    cout << pExitStr << endl;
    svt_exit(1);
};

/**
 * this functions returns the current frames per second
 * \return the current number of frames per second
 */
DLLEXPORT int svt_getFPS()
{
    return g_iFPS;
}

/**
 * this functions stops the redrawing of svt. A very useful function if you are calculating something in a different thread and you want to stop svt meanwhile.
 * \param bStop if true, svt stops the redrawing
 */
DLLEXPORT void svt_setStop(bool st)
{
    if (st == true)
        g_iStop++;
    else
        g_iStop--;

    if (g_iStop >= 1)
        g_bStop = true;
    else
        g_bStop = false;

    if (g_iStop==1)
        svt_getDrawingSemaphore()->P();
    if (g_iStop==0)
        svt_getDrawingSemaphore()->V();
}

/**
 * is svt stopped?
 * \return if true, svt is halted
 */
DLLEXPORT bool svt_getStop()
{
    if (g_iStop >= 1)
        return true;
    else
        return false;
}

/**
 * get the current canvas matrix (internal use only!)
 * \return pointer to svt_matrix4f object
 * \internal
 */
DLLEXPORT svt_matrix4f* __svt_getCanvasMatrix()
{
    return g_pCurrCanvasMat;
}

/**
 * is the left or the right eye drawed (internal use only!)
 * \return true if left eyes is drawed
 * \internal
 */
DLLEXPORT bool __svt_currentLeftEye()
{
    return g_bLeftEye;
}

/**
 * set the caption of the svt window
 * Attention: You must set the window caption _before_ calling svt_init.
 * svt_init will create the windows, and after this point it will be to
 * late to change the title string!
 * \param pTitle pointer to the window title string
 */
DLLEXPORT void svt_setWindowCaption(const char* pTitle)
{
    g_pWindowTitle = pTitle;
    if (pTitle)
    {
        strcpy(g_pWindowTitleStr, pTitle);
    }
}

/**
 * get the caption of the svt window
 * \return pointer to the window title string
 */
DLLEXPORT char* svt_getWindowCaption()
{
    return g_pWindowTitleStr;
}

/**
 * Get the contextdata protection semaphore
 * \return reference to svt_semaphore object
 */
DLLEXPORT svt_semaphore& svt_getContextSema()
{
    return g_oContextSema;
}

/**
 * add context specific data
 * \param pContextData pointer to object
 */
DLLEXPORT void svt_addContextData(svt_contextData* pContextData)
{
    if (pContextData == NULL)
        return;

    g_oContextSema.P();
    g_pContextData.addElement(pContextData);
    g_oContextSema.V();
}

/**
 * always redraw - always redraw the scene after the last redraw cycle has finished. This will cause the application to
 * consume all available CPU power, but in VR applications this might be the best way to ensure highest possible frame-rates
 * \param bAlwaysRedraw if true the scene always gets automatically redrawn
 */
DLLEXPORT void svt_setAlwaysRedraw(bool bAlwaysRedraw)
{
    g_bAlwaysRedraw = bAlwaysRedraw;
};
/**
 * always redraw - always redraw the scene after the last redraw cycle has finished. This will cause the application to
 * consume all available CPU power, but in VR applications this might be the best way to ensure highest possible frame-rates
 * \return if true the scene always gets automatically redrawn
 */
DLLEXPORT bool svt_getAlwaysRedraw()
{
    return g_bAlwaysRedraw;
};

///////////////////////////////////////////////////////////////////////////////
// public functions (for the saving frames to disk function)
///////////////////////////////////////////////////////////////////////////////

/**
 * is svt saving the frames as pictures to the disk?
 * \return if true, then svt is saving pictures to disk
 */
DLLEXPORT bool svt_isSavingFrames()
{
    return g_bSavingFrames;
}
/**
 * should we save the frames to disk?
 * \param bSaving
 */
DLLEXPORT void svt_setSavingFrames(bool bSaving)
{
    g_bSavingFrames = bSaving;
}
/**
 * get the current frame number
 * \return the frame number
 */
DLLEXPORT int svt_getFrameNum()
{
    return g_iCurrentFrame;
}
/**
 * init the frame number
 */
DLLEXPORT void svt_initFrameNum()
{
	g_iCurrentFrame = 0;
}
/**
 * get the number of frames per second (for the movie creation) For internal use only!
 * \return the number of frames per second
 */
DLLEXPORT int svt_getFramesPerSecond()
{
    return g_iSavingFPS;
}
/**
 * set the number of frames per second (for the movie creation) For internal use only!
 * \param iFps frames per second
 */
DLLEXPORT void svt_setFramesPerSecond(int iFps)
{
    g_iSavingFPS = iFps;
}
/**
 * get the max number of frames
 * \return the max frame number
 */
DLLEXPORT int svt_getMaxFrameNum()
{
    return g_iSavingFramesMax;
}
/**
 * set the max number of frames
 * \param iMaxFrames the max number of frames
 */
DLLEXPORT void svt_setMaxFrameNum(int iMaxFrames)
{
    g_iSavingFramesMax = iMaxFrames;
}
/**
 * increase the frame num
 */
DLLEXPORT void svt_incFrameNum()
{
    g_iCurrentFrame++;
}

/**
 * returns the mouse matrix
 * \return pointer to svt_matrix4f object
 */
DLLEXPORT svt_matrix4f* svt_getMouseMat()
{
    return g_pMouseMat;
}
/**
 * returns the state of the left mouse button
 * \return true if left mouse button pressed
 */
DLLEXPORT bool svt_getLeftMouseButton()
{
    return g_bLMB;
}
/**
 * should the mouse be default (without mouse button pressed) be moving in the xy plane (true) or in xz plane (false)
 * \param bMouseXY true if xy plane
 */
DLLEXPORT void svt_setMouseXYMode(bool bMouseXY)
{
    g_bMouseXY = bMouseXY;
};
/**
 * should the mouse be default (without mouse button pressed) be moving in the xy plane (true) or in xz plane (false)
 * \return true if xy plane
 */
DLLEXPORT bool svt_getMouseXYMode()
{
    return g_bMouseXY;
};

/**
 * Get the mouse mode.
 * \return mouse mode
 */
DLLEXPORT unsigned int svt_getMouseMode( )
{
    return g_iMouseMode;
};

/**
 * Set the mouse mode. If the mouse mode is set to SVT_MOUSE_TRANS_XY the mouse just translates the object on the xy plane if the left mouse button is pressed, etc.
 * \param iMouseMode the new mode
 */
DLLEXPORT void svt_setMouseMode( unsigned int iMouseMode )
{
    g_iMouseMode = iMouseMode;
};

/**
 * set projection mode
 * \param bOrtho if true orthographic projection is used (default: false)
 */
DLLEXPORT void svt_setOrtho( bool bOrtho )
{
    g_bOrtho = bOrtho;

    svt_redraw();
};
/**
 * get projection mode
 * \return if true orthographic projection is used (default: false)
 */
DLLEXPORT bool svt_getOrtho( )
{
    return g_bOrtho;
};

///////////////////////////////////////////////////////////////////////////////
// helper functions
///////////////////////////////////////////////////////////////////////////////

// save a screenshot (internal function).
void __svt_saveScreenshot(char* filen)
{
    if (g_pCanvas != NULL)
        svt_save_framebuffer::save(filen,g_pCanvas[0].size_x, g_pCanvas[0].size_y);
}

// returns the viewer matrix
svt_matrix4f* __svt_getViewerMat( void )
{
    return g_pViewerMat;
}

// calculate the absolute canvas coordinates
void __svt_calcAbsCanvasCoord(int i)
{
    g_pCanvas[i].size_x = (int)((float)g_pCanvas[i].perc_w * (float)svt_getWindowWidth(g_pCanvas[i].win));
    g_pCanvas[i].size_y = (int)((float)g_pCanvas[i].perc_h * (float)svt_getWindowHeight(g_pCanvas[i].win));
    g_pCanvas[i].pos_x = (int)((float)g_pCanvas[i].perc_x * (float)svt_getWindowWidth(g_pCanvas[i].win));
    g_pCanvas[i].pos_y = (int)((float)g_pCanvas[i].perc_y * (float)svt_getWindowHeight(g_pCanvas[i].win));



}

// calculate new world_width and heigth values
void __svt_calcCanvasAspectRatio(int i)
{
    float fAspect = (float)(g_pCanvas[i].size_x) / (float)(g_pCanvas[i].size_y);

    g_pCanvas[i].world_left = 1.0f*fAspect;
    g_pCanvas[i].world_top = 1.0f;
}

void __svt_recalcAllCanvas()
{
    int i;
    for (i=0;i < svt_getCanvasNum(); i++)
    {
	    svt_setCurrentCanvas(i);
        __svt_calcAbsCanvasCoord(i);
        if (g_pCanvas[i].world_left == -1)
            __svt_calcCanvasAspectRatio(i);
    }

	if (g_pCanvas)
	  {
		g_fCanvasAlphaX = (g_pCanvas[0].size_x * g_pCanvas[0].pixel_size_x) / (2* g_pCanvas[0].world_left);
		g_fCanvasAlphaY = (g_pCanvas[0].size_y * g_pCanvas[0].pixel_size_y) / (2* g_pCanvas[0].world_top);
	  }

}

void __svt_initGL()
{
    //glEnable( GL_CULL_FACE );
    //glCullFace(GL_BACK);

    //glEnable(GL_POLYGON_OFFSET);
    //glPolygonOffset(0.0, -3);

    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading
    glEnable(GL_NORMALIZE);  // Automatic Normalization

    // number of available clipping planes
    GLint iMaxClipPlanes = 6;
    glGetIntegerv(GL_MAX_CLIP_PLANES, &iMaxClipPlanes);

    for(int i=0;i<iMaxClipPlanes;i++)
        glDisable((ENUM_GLENUM)(GL_CLIP_PLANE0+i));

    // background color
    if (g_pBackColor != NULL)
        glClearColor(g_pBackColor->getR(),g_pBackColor->getG(),g_pBackColor->getB(), 1.0f);
};

svt_matrix4f __svt_canvas_viewer_mat("canvas_viewer_mat");
svt_matrix4f __svt_inv_canvas_viewer_mat("canvas_viewer_mat");


svt_matrix4f & __svt_getCanvasViewerMat(void)
{
  return __svt_canvas_viewer_mat;
}

svt_matrix4f & __svt_getInvCanvasViewerMat(void)
{
  return __svt_inv_canvas_viewer_mat;
}

void __svt_redrawAllCanvas(svt_win_handle win, bool leye)
{
    Matrix4f canvasViewerMat;

    // which eye is active?
    g_bLeftEye = leye;

    // now draw all canvas on that window
    for(int i = 0; i< svt_getCanvasNum(); i++)
    {
        if (g_pCanvas[i].win == win)
        {
	    svt_setCurrentCanvas(i);
	    g_pCanvas[i].l_eye = leye;

            if (g_pCurrentScene != NULL)
            {
                // Some Basic OpenGL Settings
                __svt_initGL();

                // Stereo
                if (svt_isWindowStereo(win))
		{
		    if (leye)
		    {
                        glDrawBuffer(GL_BACK_LEFT);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // FIXME: only a test for tranparency under stereo, not a final solution!
		    } else {
			glDrawBuffer(GL_BACK_RIGHT);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // FIXME: only a test for tranparency under stereo, not a final solution!
		    }
                }

                // Viewport
                glViewport(g_pCanvas[i].pos_x, g_pCanvas[i].pos_y, g_pCanvas[i].size_x, g_pCanvas[i].size_y);

                // Initialise projection matrix
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();

                //
                //set frustrum
                //
                //1. calculate eye position in local display coordinate system
                //   therefor multiply the viewer matrix (tracker) to the canvas matrix
                //   and after that multiply the eye vector to that matrix
                if (__svt_getViewerMat() == NULL || g_pCanvas[i].mat == NULL) continue;
                svt_matrix4f tmpmat(*(__svt_getViewerMat()), "tmp");
                tmpmat.postMult(*(g_pCanvas[i].mat));
                //g_pCanvas[i].mat->dump();
                //tmpmat.dump();
                svt_pos eye(0,0,0);
                if (svt_isWindowStereo(win) && !leye)
                    eye.setX(g_fEyeSeparation);
                eye.postMult(tmpmat);
                //cout << "Eye pos: " << eye.getX() << "," << eye.getY() << "," << eye.getZ() << endl;
                //2. calculate the frustrum
                if (!isPositive(fabs(eye.getZ())))
                    eye.setZ(0.01f);
                float right  = ((      g_pCanvas[i].world_left  - eye.getX()) * g_fNear) / eye.getZ();
                float left   = ((((-1)*g_pCanvas[i].world_left) - eye.getX()) * g_fNear) / eye.getZ();
                float top    = ((      g_pCanvas[i].world_top   - eye.getY()) * g_fNear) / eye.getZ();
                float bot    = ((((-1)*g_pCanvas[i].world_top)  - eye.getY()) * g_fNear) / eye.getZ();
                if (!g_bOrtho)
                {
                    if (g_pTrackerDevice != NULL)
                        glFrustum(left, right, bot, top, g_fNear, g_fFar);
                    else
                    {
                        Real32 fAspect = g_pCanvas[i].world_left / g_pCanvas[i].world_top;
                        gluPerspective( 30, fAspect, g_fNear, g_fFar );
                    }

                } else {

                    Real32 fAspect = g_pCanvas[i].world_left / g_pCanvas[i].world_top;
                    Real32 fVSize  = g_pCanvas[i].world_top * 3;

                    glOrtho(-0.25 * fVSize * fAspect, 0.25 * fVSize * fAspect, -0.25 * fVSize, 0.25 * fVSize, g_fNear, g_fFar);
                }

                //
                // Initialise modelview matrix
                //
                // OpenGL draws everything with the viewer standpoint at 0,0,0 in mind.
                // So we must move everything to the actual viewer position first.
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                // apply display matrix
                if (g_pCanvas[i].mat != NULL)
                {
                    svt_matrix4f tmp2(*(g_pCanvas[i].mat),"tmp2");
                    //tmp2.setMat(3,0, 0.0);
                    //tmp2.setMat(3,1, 0.0);
                    //tmp2.setMat(3,2, 0.0);
                    //tmp2.invert();
                    tmp2.applyToGL();
                }
                // eye
                svt_pos eye2(0,0,0);
                if (svt_isWindowStereo(win) && !leye)
                {
                    eye2.setX(0.02f);
                    //cout << "svt_init: redrawing (right eye) canvas " << i << endl;
                }
                    //cout << "svt_init: redrawing (left eye) canvas " << i << endl;

                eye2.postMult(*__svt_getViewerMat());
                //cout << "Eye2 pos: " << eye2.getX() << "," << eye2.getY() << "," << eye2.getZ() << endl;

                if (g_pTrackerDevice != NULL)
                    glTranslatef(-eye2.getX(),-eye2.getY(),-eye2.getZ());
                else
                    glTranslatef(-eye2.getX(),-eye2.getY(),-3.0*eye2.getZ());

                glGetFloatv(GL_MODELVIEW_MATRIX, static_cast<GLfloat *>(&canvasViewerMat[0][0]));
                __svt_canvas_viewer_mat.setFromMatrix(canvasViewerMat);
                __svt_inv_canvas_viewer_mat.setFromMatrix(canvasViewerMat);
                __svt_inv_canvas_viewer_mat.invert();

                // set current canvas mat
                g_pCurrCanvasMat = g_pCanvas[i].mat;

		// draw scene
		unsigned long iOldTime = svt_getToD();
		g_pCurrentScene->drawGL();

		unsigned long iTime = svt_getToD() - iOldTime;

                if (iTime > 0)
                {
                    g_iTimePerFrame = iTime;

                    g_iFPS = 1000 / g_iTimePerFrame;
                }

            } else {
                // should we perhaps draw a kind of default scene?
                // cout << "No scene available!" << endl;
            }
        }
    }
};

/** save the frame to disk */
void __svt_saveFrameToDisk()
{
    if (svt_isSavingFrames() == true)
    {
        char filename[255];
        sprintf(filename,"svt_frames%05i.tga",svt_getFrameNum());
        svt_saveScreenshot(filename);
        svt_incFrameNum();
        if (svt_getMaxFrameNum() != -1)
            if (svt_getFrameNum() > svt_getMaxFrameNum())
                exit(0);
    }
}

void __svt_mouseMoved(int x, int y)
{
    g_iMouseX = x;
    g_iMouseY = y;

    if (x == g_iMouseOldX && y == g_iMouseOldY)
        return;

    if (g_bMouseRot == false)
    {
        float mx = ((float)(x - g_iMouseOldX) / g_pCanvas[0].size_x) * (g_pCanvas[0].world_left*2.0f);
        float my = - ((float)(y - g_iMouseOldY) / g_pCanvas[0].size_y) * (g_pCanvas[0].world_top*2.0f);

        if (mx > 10.0)
            mx = 10.0;
        if (my > 10.0)
            my = 10.0;
        if (mx < -10.0)
            mx = -10.0;
        if (my < -10.0)
            my = -10.0;

        if ((g_bMouseZ && g_bMouseXY) || (!g_bMouseZ && !g_bMouseXY))
        {
            g_fMouseTransZ += -my;
            g_fMouseTransX += mx;
            g_pMouseMat->postAddTranslation(mx, 0.0f, -my);
        } else {
            g_fMouseTransX += mx;
            g_fMouseTransY += my;
            g_pMouseMat->postAddTranslation(mx, my, 0.0f);
        }

    } else {

        svt_vector4<Real32> oVecMouse( (Real32)(x), (Real32)(y), 0.0f );
        svt_vector4<Real32> oVecCenter( (Real32)(g_pCanvas[0].size_x) * 0.5f, (Real32)(g_pCanvas[0].size_y) * 0.5f, 0.0f );
        float fDist = oVecMouse.distance( oVecCenter );
        if ( g_pCanvas[0].size_x <= g_pCanvas[0].size_y )
            fDist /= (Real32)(g_pCanvas[0].size_x);
        else
            fDist /= (Real32)(g_pCanvas[0].size_y);

        float mx = ((float)(x - g_iMouseOldX) / g_pCanvas[0].size_x) * 360.0f;
        float my = - ((float)(y - g_iMouseOldY) / g_pCanvas[0].size_y) * 360.0f;

        g_fMouseRotX += mx;
        g_fMouseRotY += my;

        Real32 tx = g_pMouseMat->getTranslationX();
        Real32 ty = g_pMouseMat->getTranslationY();
        Real32 tz = g_pMouseMat->getTranslationZ();

        g_pMouseMat->setTranslationX(0.0f);
        g_pMouseMat->setTranslationY(0.0f);
        g_pMouseMat->setTranslationZ(0.0f);

        if (fDist < 0.4)
        {
            if (mx > 360.0f)
                mx = 360.0f;
            if (my > 360.0f)
                my = 360.0f;
            if (mx < -360.0f)
                mx = -360.0f;
            if (my < -360.0f)
                my = -360.0f;

            g_pMouseMat->postAddRotation(1, (mx*PI)/180.0f);
            g_pMouseMat->postAddRotation(0, (my*PI)/180.0f);

        } else {

            svt_vector4<Real32> oMouse( (Real32)(x), (Real32)(y), 0.0f );
            svt_vector4<Real32> oMouseOld( (Real32)(g_iMouseOldX), (Real32)(g_iMouseOldY), 0.0f );
            svt_vector4<Real32> oCenter( (Real32)(g_pCanvas[0].size_x) * 0.5f, (Real32)(g_pCanvas[0].size_y) * 0.5f, 0.0f );

            svt_vector4<Real32> oNewMouse = oMouse - oCenter;
            svt_vector4<Real32> oNewMouseOld = oMouseOld - oCenter;
            svt_vector4<Real32> oScreen( 0.0f, 0.0f, 1.0f );
            svt_vector4<Real32> oPerp = vectorProduct( oNewMouse, oScreen );

            Real32 fDot = oNewMouseOld * oNewMouse;
            Real32 fDotPerp = oNewMouseOld * oPerp;
            Real32 fInd = oNewMouseOld.length() * oNewMouse.length();
            Real32 fAngle = acos( fDot / fInd );

            if (fAngle > 0.0001)
            {
                if (fAngle > PI*0.5)
                    fAngle = PI*0.5;

                if (fDotPerp < 0)
                    fAngle =- fAngle;

                g_pMouseMat->postAddRotation(2, fAngle );
            }

        }

        g_pMouseMat->postAddTranslation(tx, ty, tz);
    }

    g_iMouseOldX = x;
    g_iMouseOldY = y;

    // call all recalc functions of the nodes
    if (g_pCurrentScene != NULL)
        g_pCurrentScene->recalcAll();

    svt_redraw();
}

void __svt_resetMouse()
{
    float mx = ((float)(g_iMouseOldX - (g_pCanvas[0].size_x >> 1)) / g_pCanvas[0].size_x) * (g_pCanvas[0].world_left*2.0f);
    float my = - ((float)(g_iMouseOldY - (g_pCanvas[0].size_y >> 1)) / g_pCanvas[0].size_y) * (g_pCanvas[0].world_top*2.0f);

    g_bMouseRot = false;
    g_bMouseZ = false;
    g_bLMB = false;

    if ((g_bMouseZ && g_bMouseXY) || (!g_bMouseZ && !g_bMouseXY))
    {
        g_pMouseMat->setFromTranslation(mx, 0.0f, -my);
    } else {
        g_pMouseMat->setFromTranslation(mx, my, 0.0f);
    }

}

///////////////////////////////////////////////////////////////////////////////
// "callback" functions
///////////////////////////////////////////////////////////////////////////////

/**
 * call this function if the graphics have changed and a redraw of the scene is neccessary
 */
void svt_redraw()
{
#ifdef SVT_SYSTEM_QT
    svt_forceUpdateWindows();
#endif

    g_bRedraw = true;
};

/**
 * the basic redraw routine (callback function for svt_system)
 * \internal
 */
void svt_redraw_system(svt_win_handle win, int, int)
{
    // do context specific stuff.
    g_oContextSema.P();
    unsigned int iNumCD = g_pContextData.numElements();
    for(unsigned int i=0;i<iNumCD;i++)
    {
        if (g_pContextData[i] && g_pContextData[i]->shouldDestroy())
        {
            g_pContextData[i]->destroy(win);
            if (g_pContextData[i]->isDestroyed())
            {
                delete g_pContextData[i];
                g_pContextData[i] = NULL;
                g_pContextData.delElement(i);
                i--;
                iNumCD--;
            }
        }
    }
    g_oContextSema.V();

    // background color
    if (g_pBackColor != NULL)
        glClearColor(g_pBackColor->getR(),g_pBackColor->getG(),g_pBackColor->getB(), 1.0f);

    // clear the buffers
    if (!svt_isWindowStereo(win))
    {
        // set the viewport
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else {
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    // redraw the canvas belonging to the window
    if (!svt_isWindowStereo(win))
        __svt_redrawAllCanvas(win, true);
    else {
        __svt_redrawAllCanvas(win, true);
        __svt_redrawAllCanvas(win, false);
    }

#ifdef SVT_NVPM
    // acquire NVPerfSDK sample data
    // this must be done per frame
    if (g_SVTNVPM != NULL)
    {
        g_SVTNVPM->sampleData();
        g_SVTNVPM->outputData(SVT_NVPM_AVERAGE_FRAMES, 10);
        //g_SVTNVPM->outputData(SVT_NVPM_AVERAGE_NOT);
    }
#endif

    // the picture is redrawed, now we can make a screenshot
    if (g_pScreenShotFile[0] != 0)
    {
        __svt_saveScreenshot(g_pScreenShotFile);
        g_pScreenShotFile[0] = 0;
    }
}
/**
 * the basic idle routine (callback function for svt_system)
 * \internal
 */
void svt_idle()
{
    //    static int s_iBegin=0;

    if (!g_bStop)
    {

	// call all recalc functions of the nodes
        if (g_pCurrentScene != NULL)
            g_pCurrentScene->recalcAll();

        // recalc the tracker data
        if (g_pTrackerDevice != NULL)
            g_pTrackerDevice->update();

        // update the view matrix
        if (g_pTrackerChannel != NULL)
            g_pViewerMat = g_pTrackerChannel->getMatrix();

        //g_pViewerMat->dump();

	// should we really redraw? Only if the viewer has moved or one of the nodes has changed.
	//svt_matrix4<Real32> oCurrMat(*(__svt_getViewerMat()));
        //if (g_bAlwaysRedraw || oCurrMat != g_oOldViewerMat)
        //{
	//    svt_redraw();
	//}
	//g_oOldViewerMat = oCurrMat;

        // if we have to redraw, then the user is manipulating the scene and we should only redraw in a coarse way
	/*if (g_bRedraw == true && !g_bAlwaysRedraw && g_bLMB)
	{
	    g_bRedrawCoarse = true;
	    g_bRedrawFull = false;
	}
	if ((g_bRedraw == true && !g_bLMB) || g_bAlwaysRedraw)
	    g_bRedrawCoarse = false;

        // nothing to redraw?
	if (g_bRedraw == false && g_bRedrawFull)
	    return;

	if (g_bRedraw == false && !g_bLMB)
	{
	    g_bRedrawCoarse = false;
	    g_bRedrawFull = true;
            }*/

        if (g_bRedraw == false)
            return;

	svt_forceUpdateWindows();
	if (g_bAlwaysRedraw)
	    g_bRedraw = true;

        // are we saving the frames to disk?
        if (svt_isSavingFrames())
            __svt_saveFrameToDisk();
    }
}
/**
 * the basic resize routine (callback function for svt_system)
 * \internal
 */
void svt_resize(svt_win_handle win, int w, int h)
{
    // new aspect ratio must be calculated
    if (g_bAspectRecalc)
    {
        int i;
        for (i=0;i < svt_getCanvasNum(); i++)
            if (g_pCanvas[i].win == win)
                g_pCanvas[i].world_left = -1.0f;
    }

    __svt_recalcAllCanvas();

    if (svt_getScene() != NULL)
        svt_getScene()->resizeScene(0,0,w,h);
}
/**
 * what should happen, when the user press a key?
 * \internal
 */
void svt_keyPressed(svt_win_handle, int x, int y, char key)
{
    if (g_pCurrentScene != NULL)
        g_pCurrentScene->keyPressed(key,x,y);
}
/**
 * what should happen, when the user press a key?
 * \internal
 */
void svt_specialKeyPressed(svt_win_handle, int x, int y, int key)
{
    //cout << "special key pressed: " << key << endl;

    if (key == 9)
        g_bMouseRot = true;
    if (key == 10)
        g_bMouseZ = true;
    if (key == 8)
        __svt_resetMouse();

    if (g_pCurrentScene != NULL)
        g_pCurrentScene->specialKeyPressed(key,x,y);
}
/**
 * what should happen, when the user press a key?
 * \internal
 */
void svt_specialKeyReleased(svt_win_handle, int x, int y, int key)
{
    if (g_pCurrentScene != NULL)
        g_pCurrentScene->specialKeyReleased(key,x,y);
}

/**
 * left mouse button up callback function
 * \internal
 */
void lmb_up(svt_win_handle, int, int)
{
    g_bLMB = false;
}
/**
 * right mouse button up callback function
 * \internal
 */
void rmb_up(svt_win_handle, int, int)
{
    g_bLMB = false;
}
/**
 * middle mouse button up callback function
 * \internal
 */
void mmb_up(svt_win_handle, int, int)
{
    if (!svt_getOrtho())
        g_bLMB = false;
}

/**
 * left mouse button down callback function
 * \internal
 */
void lmb_down(svt_win_handle, int x, int y)
{
    switch( g_iMouseMode )
    {
    case SVT_MOUSE_TRANS:
        g_bMouseRot = false;
        g_bMouseZ = false;
        break;

    case SVT_MOUSE_ZOOM:
        g_bMouseRot = false;
        g_bMouseZ = true;
        break;

    default:
    case SVT_MOUSE_ROT:
        g_bMouseRot = true;
        g_bMouseZ = false;
        break;
    };

    g_iMouseOldX = x;
    g_iMouseOldY = y;
    g_bLMB = true;

    if (g_pCurrentScene != NULL)
	g_pCurrentScene->leftMouseButton(x,y);
}
/**
 * right mouse button down callback function
 * \internal
 */
void rmb_down(svt_win_handle, int x, int y)
{
    g_bMouseRot = false;
    g_bMouseZ = false;
    g_iMouseOldX = x;
    g_iMouseOldY = y;
    g_bLMB = true;

    if (g_pCurrentScene != NULL)
	g_pCurrentScene->rightMouseButton(x,y);
}
/**
 * middle mouse button down callback function
 * \internal
 */
void mmb_down(svt_win_handle, int x, int y)
{
    if (!svt_getOrtho())
    {
        g_bMouseRot = false;
        g_bMouseZ = true;
        g_iMouseOldX = x;
        g_iMouseOldY = y;
        g_bLMB = true;
    }
}

/**
 * up mouse wheel button callback function
 * \internal
 */
void umb_down(svt_win_handle, int x, int y)
{
    if (g_pCurrentScene != NULL)
	g_pCurrentScene->upMouseWheel(x,y);
}
/**
 * down mouse wheel button callback function
 * \internal
 */
void dmb_down(svt_win_handle, int x, int y)
{
    if (g_pCurrentScene != NULL)
	g_pCurrentScene->downMouseWheel(x,y);
}

/**
 * mouse moved
 * \internal
 */
void svt_mouseActiveMoved(svt_win_handle, int x, int y, int)
{
    __svt_mouseMoved(x,y);

    //if (g_pCurrentScene != NULL)
    //    g_pCurrentScene->mouseActiveMoved(x,y);
}
/**
 * mouse moved
 * \internal
 */
void svt_mousePassiveMoved(svt_win_handle, int x, int y)
{
    g_bLMB = false;
    __svt_resetMouse();

    __svt_mouseMoved(x,y);

    if (g_pCurrentScene != NULL)
        g_pCurrentScene->mousePassiveMoved(x,y);
}

/**
 * Return current mouse position - x coordinate
 */
DLLEXPORT int svt_getMouseX()
{
    return g_iMouseX;
}
/**
 * Return current mouse position - y coordinate
 */
DLLEXPORT int svt_getMouseY()
{
    return g_iMouseY;
}

///////////////////////////////////////////////////////////////////////////////
// init function
///////////////////////////////////////////////////////////////////////////////

/**
 * Load the configuration file
 */
DLLEXPORT void svt_loadConfig(int argc, char *argv[])
{
    // check the parameters
    int i;
    for(i = 1; i < argc; i++)
    {
        // config filename
        if (i+1 < argc && (strcmp(argv[i],"-c")==0 || strcmp(argv[i],"-C")==0))
        {
            svt_setConfig(new svt_config(argv[i+1]));
            i++;
            continue;
        }
    }

    // check the enviroment variable SVT_CONFIG
    if (svt_getConfig() == NULL && getenv("SVT_CONFIG") != NULL)
    {
	FILE* pFile = fopen( getenv("SVT_CONFIG"), "r" );
	if (pFile != NULL)
	{
            fclose(pFile);
	    svt_setConfig(new svt_config(getenv("SVT_CONFIG")));

	    SVTINIT << "Configuration file loaded " << getenv("SVT_CONFIG") << endl;

	} else

	    SVTINIT << "Error: SVT_CONFIG variable set, but config file not found: " << getenv("SVT_CONFIG") << endl;
    }

    // is a .svtrc in the current directory?
    if (svt_getConfig() == NULL)
    {
	FILE* pFile = fopen( ".svtrc", "r" );
	if (pFile != NULL)
	{
            fclose(pFile);
	    svt_setConfig(new svt_config(".svtrc"));

	    SVTINIT << "Configuration file loaded (.svtrc in current directory)" << endl;

	}
    }

    // is a .svtrc in home?
    if (svt_getConfig() == NULL && getenv("HOME") != NULL)
    {
	string oFilename = getenv("HOME");
        oFilename = oFilename + "/.svtrc";
	FILE* pFile = fopen( oFilename.c_str(), "r" );
	if (pFile != NULL)
	{
            fclose(pFile);
	    svt_setConfig(new svt_config(oFilename.c_str()));

	    SVTINIT << "Configuration file loaded " << oFilename << endl;
	}
    }

    // default values
    if (svt_getConfig() == NULL)
        svt_setConfig(new svt_config(NULL));
};


/**
 * init all the opengl stuff and the window-management
 * \param argc number of commandline arguments
 * \param argv array of pointer to characters with the command line arguments
 */
DLLEXPORT void svt_init(int argc, char *argv[])
{
    int i;

    // check type
    svt_types oTypes;
    oTypes.checkTypes();

    // if the function svt_loadConfig() was called earlier, the config was already loaded, if not, we have to do it here
    if (svt_getConfig() == NULL)
        svt_loadConfig( argc, argv );

    // at startup no scene available
    svt_setScene(NULL);

    // init the screenshot var
    g_pScreenShotFile[0] = 0;

    // init svt_system lib
    svt_system_init();

    // init the display mode
    svt_setDoublebuffer(true);
    svt_setRGBA(svt_getConfig()->getValue("RGBA",true));

    // window title
    if (g_pWindowTitle == NULL)
    {
        svt_setWindowTitle(svt_getConfig()->getValue("WindowTitle","SVT"));
        svt_setWindowCaption(svt_getConfig()->getValue("WindowTitle","SVT"));
    } else {
        svt_setWindowTitle(svt_getWindowCaption());
    }

    // stereo?
    svt_setStereo(svt_getConfig()->getValue("Stereo",false));
    if (svt_getConfig()->getValue("Stereo",false) == true)
	SVTINIT << "Stereoscopic rendering activated!" << endl;

    // multisample extension
    svt_setSamples(svt_getConfig()->getValue("Samples",0));

    // always redraw
    g_bAlwaysRedraw = svt_getConfig()->getValue("AlwaysRedraw",false);

    // sync the swapping of front and back buffer?
    svt_setSwapSync(svt_getConfig()->getValue("SwapSync",false));

    // threads?
    svt_createThreads(svt_getConfig()->getValue("Threads",false));

    // create viewer matrix
    g_pViewerMat = new svt_matrix4f("viewer_mat");
    g_pViewerMat->setFromTranslation(0.0,0.0,1.0);

    // create the tracker
    if (svt_getConfig()->getValue("Tracker",false) == true)
    {
	SVTINIT << "Tracker activated!" << endl;

        g_iTrackerChannelNum = svt_getConfig()->getValue("TrackerChannel", (int)1);
        g_pTrackerDevice = new svt_device(svt_getConfig()->getValue("TrackerDevice", "mouse"));
        g_pTrackerChannel = g_pTrackerDevice->getChannel(g_iTrackerChannelNum);
        g_pViewerMat = g_pTrackerChannel->getMatrix();
    } else {
        g_pTrackerDevice = NULL;
	g_pTrackerChannel = NULL;
        if (svt_getConfig()->getValue("PointerDevice",(char*)NULL) != NULL)
	    if (strcasecmp(svt_getConfig()->getValue("PointerDevice",(char*)NULL),"Tracker") == 0)
		svt_fatalError("svt_init: No tracker configured, please choose other PointerDevice!");
    }

    // create a mouse matrix
    g_pMouseMat = new svt_matrix4f("mouse mat");

    // create windows
    svt_win_handle win;
    char param[256];
    const char* str;
    int x,y,w,h;
    g_iWindowNum = svt_getConfig()->getValue("WinNum",(int)1);
    for(i=1;i<=g_iWindowNum;i++)
    {
        // display
        sprintf(param,"Win%iDisplay", i);
        str = svt_getConfig()->getValue(param,(const char*)NULL);
        svt_setDisplay(str);
        // fullscreen window?
        sprintf(param,"Win%iFullscreen", i);
        if (svt_getConfig()->getValue(param,false))
        {
            sprintf(param,"Win%iWidth", i);
            w = svt_getConfig()->getValue(param,(int)640);
            sprintf(param,"Win%iHeight", i);
            h = svt_getConfig()->getValue(param,(int)400);
            win = svt_createFullScreenWindow(w,h);
        } else {
            // get coordinates and size
            sprintf(param,"Win%iPosX", i);
            x = svt_getConfig()->getValue(param,(int)50);
            sprintf(param,"Win%iPosY", i);
            y = svt_getConfig()->getValue(param,(int)50);
            sprintf(param,"Win%iWidth", i);
            w = svt_getConfig()->getValue(param,(int)640);
            sprintf(param,"Win%iHeight", i);
            h = svt_getConfig()->getValue(param,(int)400);

	    win = svt_createWindow(x,y,w,h);
        }

        /* Register the function to do all our OpenGL drawing. */
        svt_setDrawingFunc(win, svt_redraw_system);
        /* Even if there are no events, redraw our gl scene. */
        svt_setIdleFunc(svt_idle);
        /* Register the function called when our window is resized. */
        svt_setResizeWindowFunc(win, svt_resize);
        /* Register the function called when the keyboard is pressed. */
        svt_setKeyDownFunc(win, svt_keyPressed);
        /* Register the function called when a special key is pressed. */
        svt_setSpecialKeyDownFunc(win, svt_specialKeyPressed);
        /* Register the function called when a special key is released. */
        svt_setSpecialKeyUpFunc(win, svt_specialKeyReleased);
        /* Register mouse */
        svt_setMouseDragFunc(win, svt_mouseActiveMoved);
        svt_setMouseMotionFunc(win, svt_mousePassiveMoved);
        svt_setLMBDownFunc(win, lmb_down);
        svt_setLMBUpFunc(win, lmb_up);
        svt_setRMBDownFunc(win, rmb_down);
        svt_setRMBUpFunc(win, rmb_up);
        svt_setMMBDownFunc(win, mmb_down);
        svt_setMMBUpFunc(win, mmb_up);
        svt_setUMBDownFunc(win, umb_down);
        svt_setDMBDownFunc(win, dmb_down);
        
        /* Initialize our window. */
        svt_openWindow(win);

	svt_switchWindow( win );

        /* Initialize some basic variables */
	svt_setBackgroundColor(new svt_color(0.0f,0.0f,0.0f));

	/* Check for the opengl extensions */
        svt_checkEXT();
    }

    // create the canvas
    const char* tmpstr;
    svt_setCanvasNum(svt_getConfig()->getValue("CanvasNum",(int)1));
    g_pCanvas = new(__svt_canvas[svt_getCanvasNum()]);
    if (g_pCanvas == NULL)
        exit(1);
    for(i=1;i<=svt_getCanvasNum();i++)
    {
	  int iRes, iSize;
        sprintf(param,"Canvas%iResX", i);
        iRes = svt_getConfig()->getValue(param,(int)1280);
        sprintf(param,"Canvas%iAbsWidth", i);
        iSize = svt_getConfig()->getValue(param,(int)405);
        g_pCanvas[i-1].pixel_size_x = ((float)iSize)/((float)iRes);

        sprintf(param,"Canvas%iResY", i);
        iRes = svt_getConfig()->getValue(param,(int)1024);
        sprintf(param,"Canvas%iAbsHeight", i);
        iSize = svt_getConfig()->getValue(param,(int)305);
        g_pCanvas[i-1].pixel_size_y = ((float)iSize)/((float)iRes);

        sprintf(param,"Canvas%iPosX", i);
        g_pCanvas[i-1].perc_x = svt_getConfig()->getValue(param,(float)0.0);
        sprintf(param,"Canvas%iPosY", i);
        g_pCanvas[i-1].perc_y = svt_getConfig()->getValue(param,(float)0.0);
        sprintf(param,"Canvas%iWin",i);
        g_pCanvas[i-1].win = svt_getConfig()->getValue(param,(int)1);
        sprintf(param,"Canvas%iWidth", i);
        g_pCanvas[i-1].perc_w = svt_getConfig()->getValue(param,(float)1.0);
        sprintf(param,"Canvas%iHeight", i);
        g_pCanvas[i-1].perc_h = svt_getConfig()->getValue(param,(float)1.0);
        sprintf(param,"Canvas%iWorldWidth", i);
        g_pCanvas[i-1].world_left = svt_getConfig()->getValue(param,(float)-1.0f);
        if (g_pCanvas[i-1].world_left == -1)
            g_bAspectRecalc = true;
        sprintf(param,"Canvas%iWorldHeight", i);
        g_pCanvas[i-1].world_top = svt_getConfig()->getValue(param,(float)1.0);
        sprintf(param,"Canvas%iMatrix", i);
        tmpstr = svt_getConfig()->getValue(param,(char*)NULL);
        if (tmpstr != NULL)
        {
            g_pCanvas[i-1].mat = new svt_matrix4f("Canvas Matrix");
            g_pCanvas[i-1].mat->setFromString(tmpstr);
            //cout << "TMP:" << tmpstr << endl;
            //g_pCanvas[i-1].mat->dump();
        } else
            g_pCanvas[i-1].mat = new svt_matrix4f("Canvas Matrix");
    }

    __svt_recalcAllCanvas();


    // init frame num
    svt_initFrameNum();

    // do we want to create a movie?
    svt_setSavingFrames(svt_getConfig()->getValue("SavingFrames",(bool)false));

    // how many frames per second should the movie later have?
    svt_setFramesPerSecond(svt_getConfig()->getValue("SavingFramesPerSecond",(int)25));

    // how many frames
    svt_setMaxFrameNum(svt_getConfig()->getValue("SavingFrameNum",(int)-1));

    // move the mouse to the origin
    g_pMouseMat->setFromTranslation(-g_pCanvas[0].world_left,g_pCanvas[0].world_top, 0.0f);

#ifdef SVT_NVPM
    // initialize NVPerfSDK
    g_SVTNVPM = new svt_nvpm();
    if (!g_SVTNVPM->init())
    {
	delete g_SVTNVPM;
	g_SVTNVPM = NULL;
    }
#endif
};

/**
 * returns the tracker device
 * \return pointer to svt_device object
 */
svt_device* svt_getTrackerDevice()
{
    return g_pTrackerDevice;
};
/**
 * save a screenshot (as a tga picture). If dont specify a filename, svt will save the screenshot as svt_screenshot00001.tga and will increase the number with every screenshot.
 * \param pFilename pointer to array of char with the filename
 */
void svt_saveScreenshot(std::string filen)
{
    if (filen.empty())
    {
        sprintf(g_pScreenShotFile,"svt_screenshot%05i.tga",svt_getFrameNum());
        cout << "svt_init: saving screenshot to " << g_pScreenShotFile << endl;
        svt_incFrameNum();
    }
    else
    {
        strncpy(g_pScreenShotFile,filen.c_str(), 255);
    }
}

bool svt_renderingForRightEye()
{

  int iCanvas = svt_getCurrentCanvas();

  if (iCanvas>=0)
	return !g_pCanvas[iCanvas].l_eye;

  
  return false;

}

/**
 * get some frustum information for current canvas
 */

Real32 svt_getFrustumLeft()
{

  int iCanvas = svt_getCurrentCanvas();

  if (iCanvas>=0)
	return g_pCanvas[iCanvas].world_left;

  
  return 1.0f;

}

Real32 svt_getFrustumTop()
{

  int iCanvas = svt_getCurrentCanvas();

  if (iCanvas>=0)
	return g_pCanvas[iCanvas].world_top;

  
  return 1.0f;

}

/**
 * get display width of the specified canvas viewport
 */
int svt_getDisplayWidth(int iCanvas) 
{ 
  if (iCanvas>=0)
	{
	__svt_canvas& oCan=g_pCanvas[iCanvas];
	return oCan.size_x;
	}

  return -1;
}

/**
 * set display width of the specified canvas viewport
 */
void svt_setDisplayWidth(int iCanvas, int iWidth) 
{ 
  if (iCanvas>=0)
	g_pCanvas[iCanvas].size_x = iWidth;
}

/**
 * get display height of current canvas viewport
 */
int svt_getDisplayHeight(int iCanvas)
{
  if (iCanvas>=0)
	{
	__svt_canvas& oCan=g_pCanvas[iCanvas];
	return oCan.size_y;
	}

  return -1;
}

/**
 * set display height of the specified canvas viewport
 */
void svt_setDisplayHeight(int iCanvas, int iHeight) 
{ 
  if (iCanvas>=0)
	g_pCanvas[iCanvas].size_y = iHeight;
}

/**
 * get physical display width of the specified canvas viewport in mm
 */
Real32 svt_getPhysicalDisplayWidth(int iCanvas) 
{ 
  if (iCanvas>=0)
	{
	__svt_canvas& oCan=g_pCanvas[iCanvas];
	return oCan.size_x*oCan.pixel_size_x;
	}

  return -1.0;

}

/**
 * get physical display height of current canvas viewport in mm
 */
Real32 svt_getPhysicalDisplayHeight(int iCanvas)
{
  if (iCanvas>=0)
	{
	__svt_canvas& oCan=g_pCanvas[iCanvas];
	return oCan.size_y*oCan.pixel_size_y;
	}

  return -1.0;

}

/**
 * Get the alpha value of the specified canvas.
 * This value is usefull if you want to compute the physical object width w_p
 * of an object with virtual width w_v and virtual distance d to the current
 * viewpoint on the screen: w_p = alpha_x * w_v / d
 */
Real32 svt_getCanvasAlphaX()
{
  return g_fCanvasAlphaX;
}

/**
 * Get the alpha value of the specified canvas.
 * This value is usefull if you want to compute the physical object height h_p
 * of an object with virtual height h_v and virtual distance d to the current
 * viewpoint on the screen: h_p = alpha_y * h_v / d
 */
Real32 svt_getCanvasAlphaY()
{
  return g_fCanvasAlphaY;
}

int svt_getTimePerFrame()
{
  return g_iTimePerFrame;
}

/**
 * Set perspective of canvas
 */
void svt_setPerspective( int i )
{
    Real32 fAspect = g_pCanvas[i].world_left / g_pCanvas[i].world_top;
    gluPerspective( 30, fAspect, g_fNear, g_fFar );
}

/**
 * set the eye separation in the stereo mode
 * \param fEye separation in m
 */
DLLEXPORT void svt_setEyeSeparation(float fEye)
{
    g_fEyeSeparation = fEye;
};
/**
 * get the eye separation in the stereo mode
 * \return separation in cm
 */
DLLEXPORT float svt_getEyeSeparation()
{
    return g_fEyeSeparation;
};
