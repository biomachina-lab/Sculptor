/***************************************************************************
                          svt_billboard.cpp
                          -------------------
    begin                : 06/06/2001
    author               : Herwig Zilken, Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_billboard.h>
#include <svt_init.h>
#include <svt_opengl.h>

/**
 * Constructor
 * After this node the next nodes in the scenegraph are aligned towards the viewer
 */
svt_billboard::svt_billboard() : svt_node_noautodl(NULL,NULL)
{
}

/**
 * drawGL is here responsible for the alignement to the viewer
 */
void svt_billboard::drawGL()
{
    // 3 vectors of the 'target'-coordinate system
    // the billboard is rotated onto these 3 vectors
    // right ~~ x-axis
    // up    ~~ y-axis
    // front ~~ z-axis (from eye to object)
    svt_pos up, right, front;

    svt_matrix4f trans("actual opengl transformation-matrix");
    Matrix4f glMat;
    Real32 sp; // scalar-product

    glGetFloatv(GL_MODELVIEW_MATRIX, static_cast<GLfloat *>(&glMat[0][0]));
    trans.setFromMatrix(glMat);
    // eliminate translation to eye-position and rotation for display
    // that means that we are in world-coordinates now
    trans.postMult(__svt_getInvCanvasViewerMat());

    // calculate scaling
    Real32 s1 = sqrt(trans(0,0)*trans(0,0) + trans(0,1)*trans(0,1) + trans(0,2)*trans(0,2));
    Real32 s2 = sqrt(trans(1,0)*trans(1,0) + trans(1,1)*trans(1,1) + trans(1,2)*trans(1,2));
    Real32 s3 = sqrt(trans(2,0)*trans(2,0) + trans(2,1)*trans(2,1) + trans(2,2)*trans(2,2));

    svt_matrix4f tmp(*__svt_getViewerMat(), "tmp");

    // front-vector: vector from eye to object
    front.setPos(tmp(3,0)-trans(3,0), tmp(3,1)-trans(3,1), tmp(3,2)-trans(3,2));
    front.normalize();

    // up-vector: the part of the (0,1,0)-vector which is perpendicular to front-vector
    up.setPos(0.0, 1.0, 0.0);
    //up.setPos(tmp(1,0), tmp(1,1), tmp(2,1));
    sp = up.scalarProduct(front);
    up.setX(up.getX() - sp*front.getX());
    up.setY(up.getY() - sp*front.getY());
    up.setZ(up.getZ() - sp*front.getZ());
    up.normalize();

    // right-vector: vector-product of up and front
    right.setX(up.getY()*front.getZ() - up.getZ()*front.getY());
    right.setY(up.getZ()*front.getX() - up.getX()*front.getZ());
    right.setZ(up.getX()*front.getY() - up.getY()*front.getX());

    tmp.setMat(0,0, right.getX()); tmp.setMat(0,1, right.getY()); tmp.setMat(0,2, right.getZ());
    tmp.setMat(1,0, up.getX()); tmp.setMat(1,1, up.getY()); tmp.setMat(1,2, up.getZ());
    tmp.setMat(2,0, front.getX()); tmp.setMat(2,1, front.getY()); tmp.setMat(2,2, front.getZ());

    tmp.setMat(3,0, 0.0); tmp.setMat(3,1, 0.0); tmp.setMat(3,2, 0.0);

    // cout << s1 << ", " << s2 << ", " << s3 << endl;

    glLoadIdentity();
    __svt_getCanvasViewerMat().applyToGL();
    glTranslatef(trans(3,0), trans(3,1), trans(3,2));
    tmp.applyToGL();
    glScalef(s1, s2, s3);
}
