/***************************************************************************
                          svt_node
			  --------
    begin                : 02/22/2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_NODE_H
#define SVT_NODE_H

#include <svt_properties.h>
#include <svt_pos.h>
#include <svt_rotation.h>
#include <svt_texture.h>
#include <svt_texture_3d.h>
#include <svt_matrix4f.h>
#include <svt_matrix.h>
#include <svt_semaphore.h>
#include <svt_Camera.h>
//#include <svt_init.h>

#include <new>
using namespace std;

class svt_displayList;

/**
 * Enumeration of all node commands
 */
enum {
    NODE_CMD_NOTHING, /**< No command set at the moment */
    NODE_CMD_DEL      /**< Command for deleting the node */
};

/**
 * Enumeration of all drawing modes
 */
enum {
    SVT_MODE_GL,
    SVT_MODE_VRML,
    SVT_MODE_POV,
    SVT_MODE_RASTER3D,
    SVT_MODE_IV,
    SVT_MODE_YAFRAY,
    SVT_MODE_OBJ,
    SVT_MODE_SEL
};

/**
 * Enumeration of all node categories
 */
enum {
    SVT_NODE_RENDERING,
    SVT_NODE_CLIPPLANE
};

/**
  * \brief The basic node class (base class for most of the 3d objects).
  * \author Stefan Birmanns
  *
  * This class is the most basic class in the concept of svt. It is the basic node
  * class in the scenegraph architecture, of which all other graphical classes inherit (e.g. svt_sphere).
  * If you want to implement a new feature and want all scenegraph objects to have this new
  * feature, you should extend the class svt_node.
  *
  * \par Scenegraph functions
  * There are some functions in svt_node with which you can manipulate the scenegraph. The scenegraph
  * has a simple hierarchical structure. All nodes are a kind of linked list, which you can access with the
  * getNext() and setNext() command.
  * Every node can have a subtree attachted to it, which you can access with the getSon() and setSon() functions.
  * A remarkably feature of the subtree is, that he resides in the local coordinate system of its parent node.
  * That means, if the parent node is at the position (1,0,0) in the "vr-world" and the son is at position (2,0,0),
  * then the son will sit in the "vr-world" at position (3,0,0).
  * This makes a lot of sense if you image the use of complex objects, composed out of simple basic objects.
  * E.g. image a table node, which you want to construct by the help of several rectangle objects (each one for the legs and
  * one for the table top). You must connect these rectangle objects to a empty "svt_node" object called "table" by the help of
  * the "add" function. They will appear in the scenegraph hierarchy "under" the table object.
  * Now imagine you want to move the table around - it would be a mess if you would have to adjust the coordinates
  * of all rectangle objects every time. Therefor they are placed in the local coordinate system
  * of the table object. If you move the table object, the recangle objects are move automatically to
  * their appropriate position.
  *
  * \par Display List Management
  *
  *
  */
class DLLEXPORT svt_node {

protected:
    svt_node*		m_pNext;
    svt_node*		m_pSon;

    svt_properties*     m_pProp;
    svt_rotation*	m_pRot;
    svt_texture*        m_pTexture;
    svt_texture_3d*     m_pTexture3D;
    svt_pos*            m_pTrans;

    int*	       	dlList;
    bool*               dlActive;
    svt_semaphore       m_oSema;

    bool                m_bVisible;
    float               m_fScaleX;
    float               m_fScaleY;
    float               m_fScaleZ;

    svt_displayList*    m_pDisplayList;

    bool                m_bAutoDL;
    bool                m_bBird;
    bool                m_bDrawBox;

    bool                m_bAlreadyDrawn;
    svt_Camera          m_oCam;

    char                m_pName[50];

    unsigned int        m_iNodeID;

    virtual void do_precomputations() {}

    static vector< void* > g_aMemory;

    bool                m_bDeleteTrans;
    bool                m_bDeleteProp;

    bool                m_bIsFogNode;
    Real32              m_fDistanceToCamera;

    // ignore clip planes? default false
    bool m_bIgnoreClipPlanes;
    // max number of clipplanes
    static int m_iMaxClipPlanes;
    // array with enabled clipplanes indices
    static bool* m_aClipPlanes;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_node(svt_pos* pPos =NULL, svt_properties* pProp =NULL);
    virtual ~svt_node();

    /**
     * \name Scenegraph
     * Scenegraph management functions
     */
    //@{

    /**
     * return the node of the next hierarchy level (if there is any, otherwise return NULL).
     * \return pointer of the "son" node.
     */
    svt_node* getSon();
    /**
     * set the node in the next hierarchy level.
     * \param pSon pointer to the new "son" node.
     */
    void setSon(svt_node* pSon);

    /**
     * return the next node in the same hierarchy level.
     * \return pointer to the "next" node.
     */
    svt_node* getNext();
    /**
     * set the next node (same hierarchy level).
     * \param node pointer to the new "next" node.
     */
    void setNext(svt_node* node);

    /**
     * \brief adds a node to the objects subtree.
     * This function will add a new node to the subtree. If the subtree was empty, the new node will be the "son", otherwise it will get connected to end the of the list of sons.
     * \param pNode pointer to the node which should be added.
     */
    void add(svt_node* pNode);

    /**
     * \brief adds a node to the objects subtree.
     * This function will add a new node to the subtree. If the subtree was empty, the new node will be the "son", otherwise it will get connected to _end_ the of the list of sons.
     * \param pNode pointer to the node which should be added.
     */
    void addAtTail(svt_node* pNode);
    /**
     * \brief adds a node to the objects subtree.
     * This function will add a new node to the subtree. The new node will always be the first "son", i.e. it will get added as the head of the list of nodes in the subtree.
     * \param pNode pointer to the node which should be added.
     */
    void addAtHead(svt_node* pNode);
    /**
     * \brief delete a node
     * This function will search the complete subtree of the object to find the node which should be deleted.
     * \param pNode pointer to the node which should be deleted.
     */
    void del(svt_node* pNode);
    /**
     * \brief delete subtree (son node and all following)
     */
    void delSubTree();
    /**
     * delete subtree
     */
    void delSubTree(svt_node* pTree);

    /**
     * Get the node-ID, which is used for the selection mechanism
     * \return unsigned int node-ID
     */
    unsigned int getNodeID();

    /**
     * Custom new operator
     */
    void* operator new(size_t);
    /**
     * Custom delete operator
     */
    void operator delete(void* pPointer);
    /**
     * Print memory table
     */
    static void dumpMemTable();

    //@}

    /**
     * \name Output
     * Output functions
     */
    //@{

    /**
     * draw the object
     * \param iMode mode for the drawing (e.g. SVT_MODE_GL, SVT_MODE_VRML)
     */
    void draw(int iMode =SVT_MODE_GL);

    /**
     * steer the opengl drawing (first applies the properties and then calls drawGL)
     */
    virtual void outputGL();
    /**
     * draws the object with opengl
     */
    virtual void drawGL();

    /**
     * draws normals in OpenGL (to debug normal directions
     */
    virtual void drawGL_Normals() {
        cout << "drawGL_Normals() not implemented for inherited class!";
    }

    /**
     * set all attributes of the object (transformation, rotation, color, material, ....)
     */
    void applyGL();

    /**
     * steer the povray output (first applies the properties and then calls drawPOV)
     */
    virtual void outputPOV();
    /**
     * draws the object with povray
     */
    virtual void drawPOV();

    /**
     * print the iv code (OpenInventor) for the object
     */
    virtual void outputIV();
    /**
     * print the iv code (OpenInventor) for the object
     */
    virtual void drawIV();

    /**
     * print the complete vrml code for the object (incl. properties)
     */
    virtual void outputVrml();
    /**
     * print the vrml code of the object
     */
    virtual void drawVrml();

    /**
     * print the complete yafray code for the object (incl. properties)
     */
    virtual void outputYafray();
    /**
     * print the yafray code of the object
     */
    virtual void drawYafray();

    /**
     * print the complete Alias/Wavefront code for the object (incl. properties)
     */
    virtual void outputOBJ();
    /**
     * print the Alias/Wavefront code of the object
     */
    virtual void drawOBJ();

    /**
     * print the name of the node
     */
    virtual void printName();
    /**
     * set the name of the object
     * \param pName pointer to array of char with the name of the object
     */
    void setName( const char* pName );
    /**
     * get the name of the object
     * \return pointer to array of char with the name of the object
     */
    char* getName( );
    /**
     * print list of all elements
     */
    void printList();
    /**
     * print list of all elements of node and sons
     * \param space number of spacer in front of tree output
     */
    void printTree(int space = 0);
    /**
     * Find selected node
     */
    void findSelected();

    /**
     * get the type of the node - e.g. SVT_NODE_RENDERING or SVT_NODE_CLIPPLANE. Default is SVT_NODE_RENDERING.
     */
    virtual int getType();

    //@}

    /**
     * \name Properties
     * Object functions to manipulate the properties of the object
     */
    //@{

    /**
     * set the properties object. This function is virtual. If you create a special node class which consists out of several primitives (e.g. spheres, boxes, cones, etc), you need to change the behaviour of this function. The new properties object must be set in all daughter objects (see svt_solid_cylinder).
     * \param pProp pointer to svt_properties object
     */
    virtual void setProperties(svt_properties* pProp);
    /**
     * get the properties object
     * \return pointer to svt_properties object
     */
    svt_properties* getProperties();
    /**
     * set the scaling of the object (all axis the same factor)
     * \param fScale the scaling factor
     */
    void setScale(float fScale);
    /**
     * set the scaling of the object
     * \param fScaleX the x axis scaling factor
     * \param fScaleY the y axis scaling factor
     * \param fScaleZ the z axis scaling factor
     */
    void setScale(float fScaleX, float fScaleY, float fScaleZ);
    /**
     * get the x axis scaling of the object
     * \return x axis scaling factor
     */
    float getScaleX();
    /**
     * get the y axis scaling of the object
     * \return y axis scaling factor
     */
    float getScaleY();
    /**
     * get the z axis scaling of the object
     * \return z axis scaling factor
     */
    float getScaleZ();

    /**
     * set the texture of the object
     * \param pTexture pointer to svt_texture object
     */
    void setTexture(svt_texture* pTexture);
    /**
     * get the texture of the object
     */
    svt_texture* getTexture();

    /**
     * set the 3d texture of the object
     */
    void setTexture3D(svt_texture_3d* pTexture3D);
    /**
     * get the 3d texture of the object
     */
    svt_texture_3d* getTexture3D();

    /**
     * set the visibility of the object (-subtree)
     * \param bVisible if true the object and its subtree is visible
     */
    void setVisible(bool bVisible);
    bool toggleVisible() {m_bVisible=!m_bVisible; return m_bVisible;}

    /**
     * get the visibility of the object (-subtree)
     * \return if true the object and its subtree is visible
     */
    bool getVisible();

    /**
     * set if this node should set the fog parameters to OpenGL. It is important to choose a the
     * right node because its translation will have an influence on the fog start and end
     * values. this is to facilitate fog that doesn't change on the objects when the whole scene is
     * moved. Note that only one node in the scene should be made the fog node
     */
    void setIsFogNode(bool bIsFogNode);

    /**
     * get if this node should set the fog parameters to OpenGL
     */
    bool getIsFogNode();

    /**
     * Apply the fog settings to OpenGL
     */
    void applyFogParams();

    /**
     * Set if this node should ignore clipping planes
     */
    void setIgnoreClipPlanes(bool bIgnore);
    
    /**
     * Get if this node should ignore clipping planes
     */
    bool getIgnoreClipPlanes();
    
    //@}

    /**
     * \name Position and orientation
     * Functions to manipulate the position and orientation of the object
     */
    //@{

    /**
     * set the position of the object
     * \param pPos pointer to the svt_pos object
     */
    void setPos(svt_pos* pPos);
    /**
     * get the position of the object
     * \return pointer to the svt_pos object
     */
    svt_pos* getPos();

    /**
     * calc the distance to the viewer of the scene
     * \return the distance to the viewer
     */
    float getDistance();

    /**
     * set the rotation of the object. Deprecated: Please use transformation matrix!
     * \param pRot pointer to svt_rotation object
     */
    void setRotation(svt_rotation* pRot);
    /**
     * get the rotation of the object. Deprecated: Please use transformation matrix!
     * \return pointer to svt_rotation object
     */
    svt_rotation* getRotation();

    /**
     * set the transformation matrix.
     * \param pTrans the matrix which should be used as object transformation matrix
     */
    void setTransformation(svt_matrix4f *pTrans);
    /**
     * get the object transformation matrix.
     * \return returns the current object transformation matrix
     */
    svt_matrix4f *getTransformation();

    /**
     * Set if the object should delete the transformation matrix by itself or wait for somebody else to do it
     * \param bDeleteTrans if true, the object will delete the transformation matrix in the destructor
     */
    void setDeleteTrans( bool bDeleteTrans );
    /**
     * Get if the object should delete the transformation matrix by itself or wait for somebody else to do it
     * \return if true, the object will delete the transformation matrix in the destructor
     */
    bool getDeleteTrans( );
    /**
     * Set if the object should delete the properties object by itself or wait for somebody else to do it
     * \param bDeleteProp if true, the object will delete the properties object in the destructor
     */
    void setDeleteProp( bool bDeleteProp );
    /**
     * Get if the object should delete the properties object by itself or wait for somebody else to do it
     * \return if true, the object will delete the properties object in the destructor
     */
    bool getDeleteProp( );

    /**
     * get the global transformation matrix. It collects all the matrices of the parent nodes and calculates a global transformation matrix.
     * \return pointer to the global transformation matrix
     */
    //svt_matrix4f* getGlobalTransformation();

    /**
     * get the x position of the node
     * \return x position
     */
    float getPosX();
    /**
     * get the y position of the node
     * \return y position
     */
    float getPosY();
    /**
     * get the z position of the node
     * \return z position
     */
    float getPosZ();

    //@}

    /**
     * get node lock semaphore
     * \return pointer to a svt_semaphore for the node locking
     */
    svt_semaphore* getNodeLock();

    /**
     * \name Calculation
     * Calculation
     */
    //@{

    /**
     * rewrite this function to implement a special dynamic class (e.g. morphing)
     * \return bool Your function should return false if the following subtree should not be recalculated!
     */
    virtual bool recalc();
    /**
     * this function recalcs the complete subtree - it is called automatically
     * \iternal
     */
    void recalcAll();

    //@}

    /**
     * \name Node Commands
     * functions for the node commands
     * \internal
     */
    //@{

    /**
     * \brief set the current command for the node (e.g. NODE_CMD_DEL)
     * \warning if there is a previous command is active this will get overridden
     * \todo implement a kind of command list
     * \param i The command.
     * \param data Data needed for the execution of the command.
     */
    void setCommand(int i, void* data);

    /**
     * Get the object's bounding box as matrix (3,2).
     * If not overlaoded, the result defaults to <br>
     *           ( 0 1 )                         <br>
     *           ( 0 1 )                         <br>
     *           ( 0 1 )                         <br>
     * from implementation of base class.
     */
    virtual const svt_matrix<Real32>& box() const;

    /**
     * set bird perspecitve on/off
     */
    void setBird(bool b) {m_bBird=b;}
    bool getBird() const {return m_bBird;}
    bool toggleBird() {m_bBird=!m_bBird; return m_bBird;}

    /**
     * switch bpunding box drawing  on/off
     */
    void drawBox(bool b) {m_bDrawBox=b;}
    bool drawBox() const {return m_bDrawBox;}
    bool toggleBox() {m_bDrawBox=!m_bDrawBox; return m_bDrawBox;}

    void saveTransformation(const char* filename) const;
    void readTransformation(const char* filename);

protected:

    /**
     * \brief execute the current command
     */
    void execCommand();

    //@}

    /**
     * \name Display Lists
     * functions to manipulate the display list of the object
     */
    //@{

public:

    /**
     * \brief Rebuild all display lists  - must be called if the geometry of the object changed (not if just properties changed, like scaling, color, etc).
     * Typically you will write a new recalc() function and make there some calculations
     * which will change the geometry of the object. At the end of the recalc function you must call rebuildDL() to make the changes visible. But it makes also
     * sense to call rebuildDL() from another class if this class has changed the geometry of the object.
     */
    virtual void rebuildDL();

    const svt_Camera& camera() const { return m_oCam;}
    svt_Camera& camera() { return m_oCam;}

    /**
     * enable/disable the automatical generation of display lists
     * \param bAutoDL if true the display lists are generated automatically (default)
     */
    void setAutoDL(bool bAutoDL);
    /**
     * get the state of the automatical generation of display lists
     * \return true if the display lists are generated automatically (default)
     */
    bool getAutoDL();


protected:

    /**
     * \brief create a display list for the current node (it will be used automatically in the draw routine)
     */
    void createDL();

    //@}

private:

    /** search and delete a node in the subtree */
    void searchAndDelete(svt_node* node);
  
    static svt_properties sm_oDefaultProperties;
};

#endif
