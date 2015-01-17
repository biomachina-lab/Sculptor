/***************************************************************************
                          svt_init.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_INIT_H
#define SVT_INIT_H

#include <svt_config.h>
#include <svt_scene.h>
#include <svt_device.h>
#include <svt_window.h>
class svt_contextData;

/**
 * Initializes the lib
 *@author Stefan Birmanns
 */

enum
{
    SVT_MOUSE_ROT,
    SVT_MOUSE_TRANS,
    SVT_MOUSE_ZOOM
};

/**
 * Load the configuration file
 */
DLLEXPORT void svt_loadConfig(int argc, char *argv[]);

/**
 * init all the opengl stuff and the window-management
 * \param argc number of commandline arguments
 * \param argv array of pointer to characters with the command line arguments
 */
DLLEXPORT void svt_init(int argc, char *argv[]);

/**
 * set the current scene object (internal - please use svt_start() function)
 * \param pScene pointer to svt_scene object
 * \internal
 */
DLLEXPORT void svt_setScene(svt_scene* pScene);
/**
 * get the current scene object
 * \return pointer to a svt_scene object (NULL if no scene object available)
 */
DLLEXPORT svt_scene* svt_getScene();

/**
 * start the event loop
 * \param pScene pointer to svt_scene object
 */
DLLEXPORT void svt_start(svt_scene* pScene);
/**
 * exit svt
 * \param iError error return code (0 = no error)
 */
DLLEXPORT void svt_exit(int iError);
/**
 * this functions stops the redrawing of svt. A very useful function if you are calculating something in a different thread and you want to stop svt meanwhile.
 * \param bStop if true, svt stops the redrawing
 */
DLLEXPORT void svt_setStop(bool bStop);
/**
 * is svt stopped?
 * \return if true, svt is halted
 */
DLLEXPORT bool svt_getStop();

/**
 * quit because of an error. you should try to find a suitable error text to inform the user why svt stops
 * \param pExitStr pointer to array of char with the exit string
 */
DLLEXPORT void svt_fatalError(const char* pExitStr);

/**
 * get the configuration object
 * \return pointer to svt_config object
 */
DLLEXPORT svt_config* svt_getConfig();
/**
 * set the configuration object
 * \param pConfig pointer to svt_config object
 */
DLLEXPORT void svt_setConfig(svt_config* pConfig);

/**
 * is the scene rendered in a coarse way right now, or fully
 * \return true if coarse render mode is active, if false we have to render everything as precisely as possible
 */
DLLEXPORT bool svt_renderCoarse();

/**
 * set the near clipplane
 * \param fNear float value for the near clipplane
 */
DLLEXPORT void svt_setNear(Real32 fNear);
/**
 * get the near clipplane
 * \return float value for the near clipplane
 */
DLLEXPORT Real32 svt_getNear();
/**
 * set the near clipplane
 * \param fFar float value for the near clipplane
 */
DLLEXPORT void svt_setFar(Real32 fFar);
/**
 * get the near clipplane
 * \return float value for the near clipplane
 */
DLLEXPORT Real32 svt_getFar();

/**
 * set the background color
 * \param pColor pointer to svt_color object with the background color
 */
DLLEXPORT void svt_setBackgroundColor(svt_color* pColor);
/**
 * get the background color
 * \return pointer to svt_color object with the background color
 */
DLLEXPORT svt_color* svt_getBackgroundColor();
/**
 * set the background color
 * \param fRed
 * \param fGreen
 * \param fBlue
 */
DLLEXPORT void svt_setBackgroundColor(float fRed, float fGreen, float fBlue);

/**
 * set the eye separation in the stereo mode
 * \param fEye separation in cm
 */
DLLEXPORT void svt_setEyeSeparation(float fEye);
/**
 * get the eye separation in the stereo mode
 * \return separation in cm
 */
DLLEXPORT float svt_getEyeSeparation();

/**
 * is svt saving the frames as pictures to the disk?
 * \return if true, then svt is saving pictures to disk
 */
DLLEXPORT bool svt_isSavingFrames();
/**
 * should we save the frames to disk?
 * \param bSaving
 */
DLLEXPORT void svt_setSavingFrames(bool bSaving);
/**
 * get the current frame number
 * \return the frame number
 */
DLLEXPORT int svt_getFrameNum();
/**
 * init the frame number
 */
DLLEXPORT void svt_initFrameNum();
/**
 * increase the frame num
 */
DLLEXPORT void svt_incFrameNum();
/**
 * get the max number of frames
 * \return the max frame number
 */
DLLEXPORT int svt_getMaxFrameNum();
/**
 * set the max number of frames
 * \param iMaxFrames the max number of frames
 */
DLLEXPORT void svt_setMaxFrameNum(int iMaxFrames);
/**
 * get the number of frames per second (for the movie creation) For internal use only!
 * \return the number of frames per second
 */
DLLEXPORT int svt_getFramesPerSecond();
/**
 * set the number of frames per second (for the movie creation) For internal use only!
 * \param iFps frames per second
 */
DLLEXPORT void svt_setFramesPerSecond(int iFps);

/**
 * get the current viewer matrix
 * \return pointer to svt_matrix4f object
 * \internal
 */
DLLEXPORT svt_matrix4f* __svt_getViewerMat();
/**
 * returns the tracker device
 * \return pointer to svt_device object
 */
DLLEXPORT svt_device* svt_getTrackerDevice();

/**
 * this functions returns the current frames per second
 * \return the current number of frames per second
 */
DLLEXPORT int svt_getFPS();
DLLEXPORT int svt_getTimePerFrame();

/**
 * save a screenshot (as a tga picture). If dont specify a filename, svt will save the screenshot as svt_screenshot00001.tga and will increase the number with every screenshot.
 * \param pFilename pointer to array of char with the filename
 */
DLLEXPORT void svt_saveScreenshot(std::string oFilename = "");

/**
 * returns the mouse matrix
 * \return pointer to svt_matrix4f object
 */
DLLEXPORT svt_matrix4f* svt_getMouseMat();
/**
 * returns the state of the left mouse button
 * \return true if left mouse button pressed
 */
DLLEXPORT bool svt_getLeftMouseButton();
/**
 * should the mouse be default (without mouse button pressed) be moving in the xy plane (true) or in xz plane (false)
 * \param bMouseXY true if xy plane
 */
DLLEXPORT void svt_setMouseXYMode(bool bMouseXY);
/**
 * should the mouse be default (without mouse button pressed) be moving in the xy plane (true) or in xz plane (false)
 * \return true if xy plane
 */
DLLEXPORT bool svt_getMouseXYMode();

/**
 * Get the mouse mode.
 * \return mouse mode
 */
DLLEXPORT unsigned int svt_getMouseMode( );

/**
 * Set the mouse mode. If the mouse mode is set to SVT_MOUSE_TRANS the mouse just translates the object on the xy plane if the left mouse button is pressed, etc.
 * \param iMouseMode the new mode
 */
DLLEXPORT void svt_setMouseMode( unsigned int iMouseMode );
/**
 * Return current mouse position - x coordinate
 */
DLLEXPORT int svt_getMouseX();
/**
 * Return current mouse position - y coordinate
 */
DLLEXPORT int svt_getMouseY();

/**
 * set projection mode
 * \param bOrtho if true orthographic projection is used (default: false)
 */
DLLEXPORT void svt_setOrtho( bool bOrtho );
/**
 * get projection mode
 * \return if true orthographic projection is used (default: false)
 */
DLLEXPORT bool svt_getOrtho( );

/**
 * get the current canvas matrix (internal use only!)
 * \return pointer to svt_matrix4f object
 * \internal
 */
DLLEXPORT svt_matrix4f* __svt_getCanvasMatrix();

/**
 * is the left or the right eye drawed (internal use only!)
 * \return true if left eyes is drawed
 * \internal
 */
DLLEXPORT bool __svt_currentLeftEye();

/**
 * get the current canvas matrix
 * \return pointer to svt_matrix4f object
 */
DLLEXPORT svt_matrix4f & __svt_getCanvasViewerMat(void);
/**
 * get the inverse current canvas matrix
 * \return pointer to the inverse svt_matrix4f object
 */
DLLEXPORT svt_matrix4f & __svt_getInvCanvasViewerMat(void);

/**
 * set the caption of the svt window
 * \param pTitle pointer to the window title string
 */
DLLEXPORT void svt_setWindowCaption(const char* pTitle);
/**
 * get the caption of the svt window.
 * Attention: You must set the window caption _before_ calling svt_init.
 * svt_init will create the windows, and after this point it will be too
 * late to change the title string!
 * \return pointer to the window title string
 */
DLLEXPORT char* svt_getWindowCaption();

/**
 * add context specific data
 * \param pContextData pointer to object
 */
DLLEXPORT void svt_addContextData(svt_contextData* pContextData);
/**
 * Get the contextdata protection semaphore
 * \return reference to svt_semaphore object
 */
DLLEXPORT svt_semaphore& svt_getContextSema();

/**
 * query if rendering for current canvas is done for richt eye
 * returns false if not in stereo mode
 */
DLLEXPORT bool svt_renderingForRightEye();

/**
 * get display width of the specified canvas viewport in mm
 */
DLLEXPORT int svt_getDisplayWidth(int iCanvas);
/**
 * set display width of the specified canvas viewport in mm
 */
DLLEXPORT void svt_setDisplayWidth(int iCanvas, int iWidth);
/**
 * get display height of current canvas viewport in mm
 */
DLLEXPORT int svt_getDisplayHeight(int iCanvas);
/**
 * set display height of the specified canvas viewport in mm
 */
DLLEXPORT void svt_setDisplayHeight(int iCanvas, int iHeight);

/**
 * get physical display width of specified current canvas' viewport in mm
 */
DLLEXPORT Real32 svt_getPhysicalDisplayWidth(int iCanvas);

/**
 * get physical display height of specified canvas' viewport in mm
 */
DLLEXPORT Real32 svt_getPhysicalDisplayHeight(int iCanvas);




/**
 * Get the alpha value of the specified canvas.
 * This value is usefull if you want to compute the physical object width w_p
 * of an object with virtual width w_v and virtual distance d to the current
 * viewpoint on the screen: w_p = alpha_x * w_v / d
 */
//DLLEXPORT float svt_getCanvasAlphaX(int iCanvas);
DLLEXPORT Real32 svt_getCanvasAlphaX();

/**
 * Get the alpha value of the specified canvas.
 * This value is usefull if you want to compute the physical object height h_p
 * of an object with virtual height h_v and virtual distance d to the current
 * viewpoint on the screen: h_p = alpha_y * h_v / d
 */
//DLLEXPORT float svt_getCanvasAlphaY(int iCanvas);
DLLEXPORT Real32 svt_getCanvasAlphaY();

DLLEXPORT Real32 svt_getFrustumLeft();
DLLEXPORT Real32 svt_getFrustumTop();

/**
 * call this function if the graphics have changed and a redraw of the scene is neccessary
 */
DLLEXPORT void svt_redraw();

/**
 * always redraw - always redraw the scene after the last redraw cycle has finished. This will cause the application to
 * consume all available CPU power, but in VR applications this might be the best way to ensure highest possible frame-rates
 * \param bAlwaysRedraw if true the scene always gets automatically redrawn
 */
DLLEXPORT void svt_setAlwaysRedraw(bool bAlwaysRedraw);
/**
 * always redraw - always redraw the scene after the last redraw cycle has finished. This will cause the application to
 * consume all available CPU power, but in VR applications this might be the best way to ensure highest possible frame-rates
 * \return if true the scene always gets automatically redrawn
 */
DLLEXPORT bool svt_getAlwaysRedraw();

/**
 * Set perspective of canvas
 */
void svt_setPerspective( int i );

#endif
