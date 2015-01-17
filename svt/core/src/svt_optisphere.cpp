/***************************************************************************
                          svt_optisphere.cpp  -  description
                             -------------------
    begin                : Mon Feb 28 2000
    copyright            : (C) 2000 by Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <svt_core.h>
#include <svt_optisphere.h>
#include <svt_iostream.h>
#include <stdio.h>
#include <math.h>

/** the global variable stores the dl */
int svt_optisphere::sphereDL = 0;

/**
 * draws a sphere
 */
void svt_optisphere::drawGL()
{
    float rel_rad;

    float tx, ty, tz, length_t;
    float sx, sy, sz, length_s;
    float rx, ry, rz;
    float rot_angle;

    //actualiseMatrix();
    //det = powf(det, 0.3333333);
    det = 1.0;
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    // grab translation and normalize it
    tx = (float)-mvmatrix[12]; ty = (float)-mvmatrix[13]; tz = (float)-mvmatrix[14];
    length_t = sqrt(tx*tx+ty*ty+tz*tz);
    tx /= length_t; ty /= length_t; tz /= length_t;
    // grab rotated and scaled (0,0,1) vector and normalize it
    sx = (float)mvmatrix[8]; sy = (float)mvmatrix[9]; sz = (float)mvmatrix[10];
    length_s = sqrt(sx*sx+sy*sy+sz*sz);
    sx /= length_s; sy /= length_s; sz /= length_s;

    // calculated cross-produkt of the two vectors and normalize it
    rx = ty*sz - sy*tz;
    ry = tz*sx - sz*tx;
    rz = tx*sy - sx*ty;
    //length_r = sqrt(rx*rx+ry*ry+rz*rz);
    //rx /= length_r; ry /= length_r; rz /= length_r;
    rot_angle = -acos(tx*sx+ty*sy+tz*sz)*180/3.1415927;

    // contruct new Matrix
    glPushMatrix();
    glLoadIdentity();
    glTranslated(mvmatrix[12], mvmatrix[13], mvmatrix[14]);
    glRotatef((GLfloat)rot_angle, (GLfloat)rx, (GLfloat)ry, (GLfloat)rz);
    glTranslated(-mvmatrix[12], -mvmatrix[13], -mvmatrix[14]);
    glMultMatrixd(mvmatrix);

    rel_rad=radius/length_t;
    //getAngles();
    //glLoadMatrixd(mvmatrix);
    glScalef(radius*det, radius*det, radius*det);
    //glRotatef(xangle, 1.0, 0.0, 0.0);
    //glRotatef(yangle, 0.0, 1.0, 0.0);

    if (rel_rad >= 0.06)
        glCallList(sphereDL);
    else if (rel_rad >= 0.03)
        glCallList(sphereDL+1);
    else
        glCallList(sphereDL+2);
    //glScalef(4.0f, 4.0f, 4.0f);
    glPopMatrix();
}

/** prints "sphere" */
void svt_optisphere::printName(){
	cout << "optisphere" << endl;
}

/** divide a triangle to four new triangles */
svt_optisphere_triangle* svt_optisphere::divideTriangle(svt_optisphere_triangle* tri)
{
	svt_pos* A = NULL;
	svt_pos* B = NULL;
	svt_pos* C = NULL;

	svt_pos* _a = NULL;
	svt_pos* _b = NULL;
	svt_pos* _c = NULL;

	svt_pos* a = tri->getA();
	svt_pos* b = tri->getB();
	svt_pos* c = tri->getC();

	// first new triangle
	_a = new svt_pos(a->getX(), a->getY(), a->getZ());
	A = svt_pos::midpos(a,b); A->normalize();
	C = svt_pos::midpos(a,c); C->normalize();
	svt_optisphere_triangle* first = new svt_optisphere_triangle(_a, C, A);

	// second new triangle
	_c = new svt_pos(c->getX(), c->getY(), c->getZ());
	B = svt_pos::midpos(c,b); B->normalize();
	C = svt_pos::midpos(a,c); C->normalize();
	first->add(new svt_optisphere_triangle(C, _c, B));

	// third new triangle
	A = svt_pos::midpos(a,b); A->normalize();
	B = svt_pos::midpos(c,b); B->normalize();
	C = svt_pos::midpos(a,c); C->normalize();
	first->add(new svt_optisphere_triangle(A, C, B));

	// fourth new triangle
	_b = new svt_pos(b->getX(), b->getY(), b->getZ());
	A = svt_pos::midpos(a,b); A->normalize();
	B = svt_pos::midpos(c,b); B->normalize();
	first->add(new svt_optisphere_triangle(A, B, _b));		

	return first;
}

/** generate a smoother sphere */
svt_optisphere_triangle* svt_optisphere::generateNewSphere(svt_optisphere_triangle* tri)
{
	svt_optisphere_triangle* first   = NULL;
	svt_optisphere_triangle* current = tri;

	while(current != NULL)
	{
		if (first == NULL)
			first = divideTriangle(current);
		else
			first->add(divideTriangle(current));

		current = current->getNext();
	}	

	delete(tri);

	return first;
}

/** prepare the sphere display lists */
void svt_optisphere::buildSphereDLs()
{
	if (sphereDL != 0)
		return;

	sphereDL = glGenLists(3);

	// erzeuge Level0-Sphere , 4 Seiten, 2x verfeinert
	svt_optisphere_triangle* first = 
		       new svt_optisphere_triangle(new svt_pos(1,0,0),  new svt_pos(0,0,1),  new svt_pos(0,1,0));
	first->add(new svt_optisphere_triangle(new svt_pos(0,1,0),  new svt_pos(0,0,1),  new svt_pos(-1,0,0)));
	first->add(new svt_optisphere_triangle(new svt_pos(-1,0,0), new svt_pos(0,0,1),  new svt_pos(0,-1,0)));
	first->add(new svt_optisphere_triangle(new svt_pos(0,-1,0), new svt_pos(0,0,1),  new svt_pos(1,0,0)));

	first = generateNewSphere(first);
	first = generateNewSphere(first);

	glEnable(GL_NORMALIZE);
	glNewList(sphereDL, GL_COMPILE);
        //glBegin(GL_TRIANGLES);
	first->draw();
        //glEnd();
	glEndList();
	//glDisable(GL_NORMALIZE);	

	// erzeuge Level1-Sphere , 8 Seiten, 1x verfeinert
	svt_optisphere_triangle* third = 
			   new svt_optisphere_triangle(new svt_pos(1,0,0), new svt_pos(0.7f,0.7f,0),new svt_pos(0,0,1));
	third->add(new svt_optisphere_triangle(new svt_pos(0.7f,0.7f,0) ,new svt_pos(0,1,0), new svt_pos(0,0,1)));
	third->add(new svt_optisphere_triangle(new svt_pos(0,1,0) ,new svt_pos(-0.7f,0.7f,0), new svt_pos(0,0,1)));
	third->add(new svt_optisphere_triangle(new svt_pos(-0.7f,0.7f,0) ,new svt_pos(-1,0,0), new svt_pos(0,0,1)));
	third->add(new svt_optisphere_triangle(new svt_pos(-1,0,0) ,new svt_pos(-0.7f,-0.7f,0), new svt_pos(0,0,1)));
	third->add(new svt_optisphere_triangle(new svt_pos(-0.7f,-0.7f,0) ,new svt_pos(0,-1,0), new svt_pos(0,0,1)));
	third->add(new svt_optisphere_triangle(new svt_pos(0,-1,0) ,new svt_pos(0.7f,-0.7f,0), new svt_pos(0,0,1)));
	third->add(new svt_optisphere_triangle(new svt_pos(0.7f,-0.7f,0) ,new svt_pos(1,0,0), new svt_pos(0,0,1)));	

	third = generateNewSphere(third);

	glEnable(GL_NORMALIZE);
	glNewList(sphereDL+1, GL_COMPILE);
        //glBegin(GL_TRIANGLES);
	third->draw();
        //glEnd();
	glEndList();
	//glDisable(GL_NORMALIZE);

	// erzeuge Level2-Sphere , 6 Seiten, 1x verfeinert
	svt_optisphere_triangle* fourth =
			    new svt_optisphere_triangle(new svt_pos(1,0,0), new svt_pos(0.5f,0.87f,0), new svt_pos(0,0,1));
	fourth->add(new svt_optisphere_triangle(new svt_pos(0.5f,0.87f,0), new svt_pos(-0.5f,0.87f,0),new svt_pos(0,0,1)));
	fourth->add(new svt_optisphere_triangle(new svt_pos(-0.5f,0.87f,0), new svt_pos(-1,0,0), new svt_pos(0,0,1)));
	fourth->add(new svt_optisphere_triangle(new svt_pos(-1,0,0), new svt_pos(-0.5f,-0.87f,0), new svt_pos(0,0,1)));
	fourth->add(new svt_optisphere_triangle(new svt_pos(-0.5f,-0.87f,0), new svt_pos(0.5f,-0.87f,0), new svt_pos(0,0,1)));
	fourth->add(new svt_optisphere_triangle(new svt_pos(0.5f,-0.87f,0),new svt_pos(1,0,0), new svt_pos(0,0,1)));

	fourth = generateNewSphere(fourth);

	glEnable(GL_NORMALIZE);
	glNewList(sphereDL+2, GL_COMPILE);
        //glBegin(GL_TRIANGLES);
	fourth->draw();
        //glEnd();
	glEndList();
	//glDisable(GL_NORMALIZE);
	
	delete(first);
	delete(third);
	delete(fourth);
}

void svt_optisphere::actualiseMatrix()
{
	// ?ndere die Modelview-Matrix soweit, da~ alle Rotationen wegfallen
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
        // calculate the determinant of the upper right 3x3 submatrix
        det = mvmatrix[0]*(mvmatrix[5]*mvmatrix[10]-mvmatrix[6]*mvmatrix[9])-mvmatrix[1]*(mvmatrix[4]*mvmatrix[10]-mvmatrix[6]*mvmatrix[8])+mvmatrix[2]*(mvmatrix[4]*mvmatrix[9]-mvmatrix[5]*mvmatrix[8]);

	mvmatrix[0] = 1.0; mvmatrix[1] = 0.0; mvmatrix[2] = 0.0;
	mvmatrix[4] = 0.0; mvmatrix[5] = 1.0; mvmatrix[6] = 0.0;
	mvmatrix[8] = 0.0; mvmatrix[9] = 0.0; mvmatrix[10] =1.0;
}

void svt_optisphere::getAngles()
{
	float px, py, pz, wert;
	
	// der Vektor vom Betrachter zum Objekt
	px = mvmatrix[12];
	py = mvmatrix[13];
	pz = mvmatrix[14];

	// die Winkel, um die das Objekt zum Betrachter gedreht wird
	if (pz > 0) 
		{xangle = 0.0; yangle = 0.0;}
		else 	
		{wert = sqrt(px*px+pz*pz);
		 xangle = (atan(py / wert))*(180.0 / 3.1415)*(1.0);
		 if (wert == 0.0)
			 yangle= 0.0;
			else
			 yangle= (asin(px / wert))*(180.0 / 3.1415)*(-1.0);
		}
}

void svt_optisphere::printVrmlCode()
{
	cout << "geometry Sphere { radius " << radius << " }" << endl;
}
