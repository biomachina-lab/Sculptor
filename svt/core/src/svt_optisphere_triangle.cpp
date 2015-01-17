/***************************************************************************
                          svt_optisphere
                          --------------
    begin                : Mon Feb 28 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_optisphere_triangle.h>
#include <svt_iostream.h>
#include <svt_opengl.h>

svt_optisphere_triangle::~svt_optisphere_triangle()
{
	if (a != NULL)
		delete(a);
	if (b != NULL)
		delete(b);
	if (c != NULL)
		delete(c);
	if (na != NULL)
		delete(na);
	if (nb != NULL)
		delete(nb);
	if (nc != NULL)
		delete(nc);

	if (getNext()!=NULL)
		delete(getNext());

#ifdef _DEBUG
	cout << "destructing svt_optisphere_triangle object" << endl;;
#endif
}

void svt_optisphere_triangle::add(svt_optisphere_triangle *_tnext)
{
	svt_optisphere_triangle* old_next = next;
	setNext(_tnext);

	//is this a single triangle, or a field of triangles?
	if (_tnext->getNext() != NULL)
		while(_tnext->getNext() != NULL) _tnext = _tnext->getNext();

	_tnext->setNext(old_next);
}

void svt_optisphere_triangle::setNext(svt_optisphere_triangle *_tnext)
{
	next = _tnext;
}

svt_optisphere_triangle* svt_optisphere_triangle::getNext()
{
	return next;
}

void svt_optisphere_triangle::printName()
{
	cout << "Triangle: A_X:" << a->getX() << " A_Y: " << a->getY() << " A_Z:" << a->getZ();
	cout << " B_X:" << b->getX() << " B_Y: " << b->getY() << " B_Z:" << b->getZ();
	cout << " C_X:" << c->getX() << " C_Y: " << c->getY() << " C_Z:" << c->getZ();
	cout << endl;
}

void svt_optisphere_triangle::draw()
{
	drawGL();
	if (getNext() != NULL)
		getNext()->draw();
}

void svt_optisphere_triangle::drawGL()
{
  glBegin(GL_TRIANGLES);
  glNormal3f(na->getX(), na->getY(), na->getZ());
  glVertex3f(a->getX(), a->getY(), a->getZ());
  glNormal3f(nb->getX(), nb->getY(), nb->getZ());
  glVertex3f(b->getX(), b->getY(), b->getZ());
  glNormal3f(nc->getX(), nc->getY(), nc->getZ());
  glVertex3f(c->getX(), c->getY(), c->getZ());
  glEnd();
}

/** print list of all elements of one level */
void svt_optisphere_triangle::printList()
{
	svt_optisphere_triangle* current = this;

	while (current != NULL)
	{
		current->printName();
		current = current->getNext();
	}
}
