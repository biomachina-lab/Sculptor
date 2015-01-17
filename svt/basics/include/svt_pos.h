/***************************************************************************
                          svt_pos.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_POS_H
#define SVT_POS_H

#include <svt_basics.h>
#include <svt_matrix4f.h>

/** Represents the position of a svt object. This is a svt_matrix4f with some functions for easy position access added. This class was added to svt_core, because in most of the time you just want to specify the position of an object and not the position, scaling and orientation as the svt_matrix class does.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_pos : public svt_matrix4f {

public:
    svt_pos(float x =0.0f, float y =0.0f, float z =0.0f);
    ~svt_pos();

public:
    /**@name get positions */
    //@{
    /// get X pos
    float getX();
    /// get Y pos
    float getY();
    /// get Z pos
    float getZ();
    //@}

    /**@name set positions */
    //@{
    /// set X pos
    void setX(float _tx);
    /// set Y pos
    void setY(float _ty);
    /// set Z pos
    void setZ(float _tz);
    /// set the position (x,y,z)
    void setPos(float _tx, float _ty, float _tz);
    //@}

    /** normalize */
    void normalize();
    /** add two pos and calc one new pos */
    static svt_pos* addPos(svt_pos* p1, svt_pos* p2);
    /** calc the normal vector */
    static svt_pos* calcNormalVec(svt_pos* p1, svt_pos* p2, svt_pos* p3);
    /** return the middle position between to pos */
    static svt_pos* midpos(svt_pos* p1, svt_pos* p2);
    /** return the scalar-product with vector p */
    float scalarProduct(svt_pos &);
};

#endif
