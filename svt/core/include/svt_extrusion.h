/***************************************************************************
                          svt_extrusion.h
			  ---------------
    begin                : 9/26/03
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_EXTRUSION_H
#define SVT_EXTRUSION_H

#include <svt_node.h>
#include <svt_array.h>
#include <svt_vector4.h>
#include <svt_container.h>

/**
 * An extrusion object. It takes an array of points (2D) and extrudes them along a path formed by an array of 3D points.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_extrusion : public svt_node
{
protected:
    svt_container m_oCont;
    svt_array<svt_vec4real32> m_aPoints;
    svt_array<svt_vec4real32> m_aPath;

public:
    /**
     * Constructor
     */
    svt_extrusion();
    virtual ~svt_extrusion(){};

    /**
     * set path
     * \param aPath svt_array object with the points that form the line along the object is extruded.
     */
    void setPath(svt_array<svt_vec4real32> aPath);
    /**
     * get path
     * \return svt_array object with the points that form the line along the object is extruded.
     */
    svt_array<svt_vec4real32> getPath();

    /**
     * set shape
     * \param aPoints svt_array with the points that form the shape of the object that gets extruded
     */
    void setShape(svt_array<svt_vec4real32> aPoints);
    /**
     * get shape
     * \return svt_array with the points that form the shape of the object that gets extruded
     */
    svt_array<svt_vec4real32> getShape();

    /**
     * extrude
     */
    void extrude();
};

#endif
