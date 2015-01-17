/***************************************************************************
                          svt_sphere.h  -  description
                             -------------------
    begin                : Mon Feb 28 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_OPTISPHERE_H
#define SVT_OPTISPHERE_H

#include <svt_node.h>
#include <svt_optisphere_triangle.h>

/**
 * An optimized sphere. This is a optimized sphere. Backfaceculling and a better tesselation algorithm were used.
 *@author Jan Deiterding, Herwig Zilken, Stefan Birmanns
 */

class DLLEXPORT svt_optisphere : public svt_node  {

protected:
    static int sphereDL;
    float xangle;
    float yangle;
    float zangle;
    float dis;
    double mvmatrix[16];
    double radius;
    float det;

public:
    svt_optisphere() : svt_node(){ buildSphereDLs(); radius = 0.025; };
    svt_optisphere(svt_pos* _tpos, double _trad=0.025) : svt_node(_tpos){ buildSphereDLs(); radius = _trad;};
    svt_optisphere(svt_pos* _tpos, svt_properties* _tprop, double _trad=0.025) : svt_node(_tpos,_tprop){ buildSphereDLs(); radius = _trad;};
    virtual ~svt_optisphere(){};

public:
    /** draws a sphere */
    void drawGL();
    /** prints "sphere" */
    void printName();
    /** prints vrml code for the sphere */
    virtual void printVrmlCode();
    /** set radius of the sphere */
    void setRadius(double _trad) { radius = _trad; };

protected:
    /** build the displaylists for the spheres in different resolutions */
    void buildSphereDLs();
    /** divide a triangle to four new triangles */
    svt_optisphere_triangle* divideTriangle(svt_optisphere_triangle* tri);
    /** generate a smoother sphere */
    svt_optisphere_triangle* generateNewSphere(svt_optisphere_triangle* tri);
    /** rotiere die optisphere zum Betrachter */
    void getAngles();
    void actualiseMatrix();
};

#endif
