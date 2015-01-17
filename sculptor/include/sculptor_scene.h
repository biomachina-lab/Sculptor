/***************************************************************************
                          sculptor_scene.h
                          -------------------
    begin                : 27.03.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_SCENE_H
#define SCULPTOR_SCENE_H

// svt includes
#include <svt_basics.h>
#include <svt_scene.h>
#include <svt_forceArrows.h>
// forward decalarations
class svt_animation;
class svt_animation_rotation;
class svt_node;
class svt_clipplane;
class svt_tool;
class svt_move_tool;
class svt_pipette;
class svt_coord_system;
class svt_adv_force_tool;
class sculptor_force_algo;
class svt_lightsource;
// qt4 includes
#include <QString>

/*
 * sculptor_scene class inherits from svt_scene
 */
class sculptor_scene : public svt_scene
{

protected:

    // the scenegraph
    svt_scenegraph* m_pGraph;

    // light
    svt_light* m_pLight;
    svt_lightsource* m_pLightSource;

    // move tool
    svt_move_tool* m_pMoveTool;
    svt_pipette* m_pPipette;
    svt_node* m_pTrackedNode;

    // force tool
    svt_adv_force_tool* m_pForceTool;
    svt_forceArrows *m_pArr;

    // intro data
    svt_node* m_pIntro;

    // node for the clipping planes
    svt_node* m_pClipNode;
    // node for the data objects
    svt_node* m_pDataTopNode;
    // top node for all other nodes
    svt_node* m_pAllNode;
    // top node for all lua nodes
    svt_node* m_pLuaNode;
    // mouse wheel movement
    Real32 m_fMWScale;
    // coordinate system showing rotation
    svt_coord_system * m_pCoordSystem;

    // is the control key pressed right now?
    bool m_bControl;

public:
    /**
     * Constructor
     */
    sculptor_scene();

    /**
     * build the scene
     */
    virtual void buildScene();
    /**
     * build the intro
     */
    void buildIntro();
    /**
     * adjust some opengl settings here
     */
    void prepareGL();

    /**
     * reset the viewpoint
     */
    void resetViewpoint();

    /**
     * switch to the intro
     */
    void switchToIntro();
    /**
     * switch to the data viewer
     */
    void switchToData();

    /**
     * save a screenshot
     * \param pFilename pointer to array of char with the filename
     */
    void saveScreenshot(QString pFilename);
    /**
     * render with povray
     */
    void renderPOV();
    /**
     * export as Alias/Wavefront file
     * \param pFilename pointer to array of char with the base of the filename (no extension)
     */
    void renderOBJ(const char* pFilename);

    /**
     * get the data top node (for connecting new volume/pdb/... files to it)
     */
    svt_node* getDataTopNode();
    /**
     * get the clip top node (for connecting new clipping planes to it)
     */
    svt_node* getClipTopNode();
    /**
     * get the top node for all other nodes
     * \return pointer to the top node
     */
    svt_node* getTopNode();
    /**
     * delete a data object
     */
    void delDataNode(svt_node* node);
    /**
     * delete a clip-plane object
     */
    void delClipNode(svt_node* pNode);

    /**
     * set the node which should be moved around
     * \param pNode pointer to the node
     * \param oOrigin svt_vector4 object with the origin (the pivot point around which the user can rotate everything)
     */
    void setMovedNode(svt_node* pNode, svt_vector4<Real32> oOrigin =svt_vector4<Real32>(0,0,0));
    /**
     * get the currently moved node
     * \return pointer to the node, which is moved around by the user
     */
    svt_node* getMovedNode();
    /**
     * get the pivot point around which we rotate
     * \return vvt_vector4 object with the origin (the pivot point around which the user can rotate everything)
     */
    svt_vector4<Real32> getPivotPoint();

    /**
     * get the node that all the lua svt objects are attached to
     * \return pointer to the node
     */
    svt_node* getLuaNode();

    /**
     * set move tool center mode
     * \param bCenter if true the center mode is enabled
     */
    void setCenterMode(bool bCenter);
    /**
     * get move tool center mode
     * \return if true the center mode is enabled
     */
    bool getCenterMode();
    /**
     * set move tool fly mode
     * \param bFly if true the fly mode is enabled
     */
    void setFlyMode(bool bFly);
    /**
     * get move tool fly mode
     * \return if true the fly mode is enabled
     */
    bool getFlyMode();

    /**
     * disable the move tool
     * \param bDisable if true the move tool is disabled
     */
    void disableMoveTool(bool bDisable);

    /**
     * set force tool center mode
     * \param bCenter if true the center mode is enabled
     */
    void setForceCenterMode(bool bCenter);
    /**
     * get force tool center mode
     * \return if true the center mode is enabled
     */
    bool getForceCenterMode();

    /**
     * set the force algorithm
     * \param pForceAlgo pointer to the sculptor_force_algo object
     */
    void setForceAlgo(sculptor_force_algo* pForceAlgo);
    /**
     * get the force algorithm
     * \return pointer to the sculptor_force_algo object
     */
    sculptor_force_algo* getForceAlgo();
    /**
     * set translation scaling
     * \param fTranslationScaling (factor with which translations are multiplied)
     */
    void setForceTranslationScale(Real32 fTranslationScaling);
    /**
     * get translation scaling
     * \return translation scale factor
     */
    Real32 getForceTranslationScale();
    /**
     * get the force arrows
     * \return pointer to the force arrows object to display torque direction
     */
    svt_forceArrows* getForceArrows();
    /**
     * set, if the force arrows should be shown or not
     * \param bShowArr true if the force arrows should be shown
     */
    void setShowForceArrows(bool bShowArr);
    /**
     * get showing force arrows
     * \return true if the force arrows will be shown
     */
    bool getShowForceArrows();

    /**
     * Set pipette visible
     * \param bVisible if true the 3D cursor is shown
     */
    void setPipetteVisible( bool bVisible );
    /**
     * Get pipette visible
     * \return if true the 3D cursor is shown
     */
    bool getPipetteVisible( );

    /**
     * change visiblity of the coordinate system
     * \param bShowCoordSystem determines if visible or not
     */
    void setCoordinateSystemVisible(bool bShowCoordSystem);
    /**
     * return true or false if coordinate system is visible or not
     * \return true or false
     */
    bool getCoordinateSystemVisible();

    /**
     * get the current force updates per second rate
     */
    int getFUS();

    /**
     * get the move tool
     * \return pointer to the move tool
     */
    svt_move_tool* getMoveTool();

    /**
     * get the force tool
     * \return pointer to the force tool
     */
    svt_adv_force_tool* getForceTool();

    /**
     * up mouse wheel button pressed
     */
    virtual void upMouseWheel(int, int);
    /**
     * down mouse wheel button pressed
     */
    virtual void downMouseWheel(int, int);

    /**
     * key event
     */
    virtual void keyPressed(unsigned char key, int, int);
    /**
     * special key event
     */
    virtual void specialKeyPressed(int key, int, int);
    /**
     * special key event
     */
    virtual void specialKeyReleased(int key, int, int);

    /**
     * Set scale of mouse wheel translations
     * \param fMWScale new scale
     */
    void setMouseWheelScale(Real32 fMWScale);

    /**
     * left mouse button pressed
     */
    virtual void leftMouseButton(int x, int y);
    /**
     * right mouse button pressed
     */
    virtual void rightMouseButton(int x, int y);
};

#endif
