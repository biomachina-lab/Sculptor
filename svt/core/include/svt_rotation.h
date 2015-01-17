/***************************************************************************
                          svt_rotation.h  -  description
                             -------------------
    begin                : Tue Feb 29 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ROTATION_H
#define SVT_ROTATION_H

/** This class represents the rotation of an object
  *@author Stefan Birmanns
  */

class DLLEXPORT svt_rotation {

protected:
    float xrot;
    float yrot;
    float zrot;

public: 
    svt_rotation(float x, float y, float z);
    virtual ~svt_rotation();

    /** adjust the current opengl roation */
    void applyProperty();
    /** set the rotation (rotation around x axis, around y axis, around z axis) */
    void setRotation(float x, float y, float z);

    /** get the rotation around x axe */
    float getX(){ return xrot; };
    /** get the rotation around y axe */
    float getY(){ return yrot; };
    /** get the rotation around z axe */
    float getZ(){ return zrot; };

    /** set the rotation around x axe */
    void setX(float tx){ if (tx >= 360) tx -= 360; xrot=tx; };
    /** set the rotation around y axe */
    void setY(float ty){ if (ty >= 360) ty -= 360; yrot=ty; };
    /** set the rotation around z axe */
    void setZ(float tz){ if (tz >= 360) tz -= 360; zrot=tz; };
};

#endif

