/***************************************************************************
                          svt_clipplane.h  -  description
                             -------------------
    begin                : 02/02/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CLIPPLANE_H
#define SVT_CLIPPLANE_H

#include <svt_core.h>
#include <svt_node.h>

/**
 * A clipping-plane class. You can have at least six clipplane objects (opengl restriction). Under most opengl implementations you can have much more (SGI-Irix: 67634360).
 * But be careful: On most opengl implementations, the performance drops significantly if you use clipplanes.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_clipplane : public svt_node {

protected:
    // array, storing the occupation status for all available clipping planes of the opengl implmentation
    static bool* m_bClipOcc;
    // clipplane equation
    static GLdouble m_iClipEqn[4];
    // sum of all used clipplanes
    static int m_iClipCount;
    // number of the current cliplane
    int m_iClipPln;          
    // number of clipplanes, the implementation is supporting
    static GLint m_iMaxClipPlanes;
    // clipplane rectangle visible?
    bool m_bRectVisible;

public:
    /**
     * Constructor
     * Create a new svt_clipplane object
     */
    svt_clipplane();
    virtual ~svt_clipplane();

    /**
     * get the number of clipplanes your opengl implementation supports
     * \return number of available clipplanes
     */
    static int getNumClipPlanes();

    /**
     * get the number of the currently used clipplanes
     */
    static int getNumUsedClipPlanes();

    /**
     * return a pointer to the clipplane array
     */
    static bool* getClipOcc();

    /**
     * draw the clipplane
     */
    virtual void drawGL();

    /**
     * get the type of the node - e.g. SVT_NODE_RENDERING or SVT_NODE_CLIPPLANE. Default is SVT_NODE_RENDERING.
     */
    virtual int getType();

    /**
     * set if clipping plane rectangle visible
     * \param bVisible if true rectangle is visible
     */
    bool getRectVisible();

    /**
     * set if clipping plane rectangle visible
     * \param bVisible if true rectangle is visible
     */
    void setRectVisible(bool bVisible);

private:
    /**
     * find a free clipplane, not used by another svt_clipplane object
     */
    int findFree();
};

#endif
