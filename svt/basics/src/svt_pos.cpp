/***************************************************************************
                          svt_pos.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_basics.h>
#include <svt_pos.h>
#include <svt_iostream.h>
#include <svt_opengl.h>
#include <svt_cmath.h>

svt_pos::svt_pos(float _tx, float _ty, float _tz) : svt_matrix4f("svt_pos")
{
    setPos(_tx, _ty, _tz);
}
svt_pos::~svt_pos()
{
}

/* set the position */
void svt_pos::setPos(float _tx, float _ty, float _tz){
    setFromTranslation(_tx, _ty, _tz);
}

float svt_pos::getX()
{
    return getTranslationX();
}

float svt_pos::getY()
{
    return getTranslationY();
}

float svt_pos::getZ()
{
    return getTranslationZ();
}

/* calculate midpoint on the line between the two points */
svt_pos* svt_pos::midpos(svt_pos *p1, svt_pos *p2)
{
    float x = (p1->getX() + p2->getX()) * 0.5;
    float y = (p1->getY() + p2->getY()) * 0.5;
    float z = (p1->getZ() + p2->getZ()) * 0.5;

    return new svt_pos(x,y,z);
}

/* add two pos and calc one new pos */
 svt_pos* svt_pos::addPos(svt_pos* p1, svt_pos* p2)
{
    return new svt_pos(p1->getX() + p2->getX(),
                       p1->getY() + p2->getY(),
                       p1->getZ() + p2->getZ());
}

/* calc the normal vector */
 svt_pos* svt_pos::calcNormalVec(svt_pos* p1, svt_pos* p2, svt_pos* p3)
{
    GLfloat x1, y1, z1, x2, y2, z2;
    GLfloat x,y,z;
    float l;

    x1 = p2->getX()-p1->getX();
    y1 = p2->getY()-p1->getY();
    z1 = p2->getZ()-p1->getZ();
    x2 = p3->getX()-p1->getX();
    y2 = p3->getY()-p1->getY();
    z2 = p3->getZ()-p1->getZ();

    x = -(y1*z2 - z1*y2);
    y = -(z1*x2 - x1*z2);
    z = -(x1*y2 - y1*x2);

    l = sqrt(x*x + y*y + z*z);
    x /= l; y /= l; z /= l;

    return new svt_pos(x,y,z);
}

/* Normalize the point */
void svt_pos::normalize()
{
	double mag;

	mag = getX() * getX() + getY()*getY() + getZ()*getZ();
	if (mag != 0.0){
		mag = 1.0 / sqrt(mag);

		setX(getX()*mag);
		setY(getY()*mag);
		setZ(getZ()*mag);
	}
}

void svt_pos::setX(float _tx)
{
    setTranslationX(_tx);
}

void svt_pos::setY(float _ty)
{
    setTranslationY(_ty);
}

void svt_pos::setZ(float _tz)
{
    setTranslationZ(_tz);
}

float svt_pos::scalarProduct(svt_pos &p)
{
  return getX()*p.getX() + getY()*p.getY() + getZ()*p.getZ();
}
