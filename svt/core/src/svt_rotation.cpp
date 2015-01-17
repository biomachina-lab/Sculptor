/***************************************************************************
                          svt_rotation
                          ------------
    begin                : Tue Feb 29 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_rotation.h>
#include <svt_opengl.h>

svt_rotation::svt_rotation(float x, float y, float z){

    xrot = x;
    yrot = y;
    zrot = z;
}

svt_rotation::~svt_rotation(){
}

/* set the rotation (rotation around x axis, around y axis, around z axis) */
void svt_rotation::setRotation(float x, float y, float z){

    setX(x);
    setY(y);
    setZ(z);
}
/* adjust the current opengl roation */
void svt_rotation::applyProperty(){
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
}
