/***************************************************************************
                          svt_optisphere_triangle.h  -  description
                             -------------------
    begin                : Wed Mar 8 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_OPTISPHERE_TRIANGLE_H
#define SVT_OPTISPHERE_TRIANGLE_H

#include <svt_pos.h>
#include <svt_iostream.h>

class DLLEXPORT svt_optisphere_triangle
{

protected:
    svt_pos* a;
    svt_pos* b;
    svt_pos* c;
    svt_pos* na;
    svt_pos* nb;
    svt_pos* nc;

    svt_optisphere_triangle* next;

public:
    svt_optisphere_triangle(svt_pos* _ta, svt_pos* _tb, svt_pos* _tc)
    {
	a = _ta; b = _tb; c = _tc;

	na = new svt_pos(a->getX(), a->getY(), a->getZ()); na->normalize();
	nb = new svt_pos(b->getX(), b->getY(), b->getZ()); nb->normalize();
	nc = new svt_pos(c->getX(), c->getY(), c->getZ()); nc->normalize();

	next = NULL;

    };

    virtual ~svt_optisphere_triangle();

public:
    svt_pos* getA() { return a; };
    svt_pos* getB() { return b; };
    svt_pos* getC() { return c; };
    void drawGL();
    void draw();
    svt_optisphere_triangle* getNext();
    void setNext(svt_optisphere_triangle* _tnext);
    void add(svt_optisphere_triangle* _tnext);
    void printName();
    void printList();
};

#endif
