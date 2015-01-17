/***************************************************************************
                          svt_laser.cpp  -  description
                             -------------------
    begin                : Thu May 19 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include "svt.h"
#include <math.h>

/** draw the Laser */
void svt_laser::drawGL(){
	//Koordinaten fÅr den Normalen Vektor
	x = 0.0 ;  y = 10.0 ; z = 0.0;
/*
	//Deckel
	glBegin(GL_POLYGON);
		glVertex3d(-X1*0.05,	Y1*0.05,	-X2*0.05);
		glVertex3d(-X2*0.05,	Y1*0.05,	-X1*0.05);
		glVertex3d(-X2*0.05,	Y1*0.05,	+X1*0.05);
		glVertex3d(-X1*0.05,	Y1*0.05,	+X2*0.05);
		glVertex3d(+X1*0.05,	Y1*0.05,	+X2*0.05);
		glVertex3d(+X2*0.05,	Y1*0.05,	+X1*0.05);
		glVertex3d(+X2*0.05,	Y1*0.05,	-X1*0.05);
		glVertex3d(+X1*0.05,	Y1*0.05,	-X2*0.05);
	glEnd();
	//Boden
	glBegin(GL_POLYGON);
		glVertex3d(-X1*0.05,	Y2*0.05,	-X2*0.05);
		glVertex3d(-X2*0.05,	Y2*0.05,	-X1*0.05);
		glVertex3d(-X2*0.05,	Y2*0.05,	+X1*0.05);
		glVertex3d(-X1*0.05,	Y2*0.05,	+X2*0.05);
		glVertex3d(+X1*0.05,	Y2*0.05,	+X2*0.05);
		glVertex3d(+X2*0.05,	Y2*0.05,	+X1*0.05);
		glVertex3d(+X2*0.05,	Y2*0.05,	-X1*0.05);
		glVertex3d(+X1*0.05,	Y2*0.05,	-X2*0.05);
	glEnd();
*/
   //1:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(-X1*0.05,	Y1*0.05,	-X2*0.05);
		glVertex3d(-X2*0.05,	Y1*0.05,	-X1*0.05);
		glVertex3d(-X2*0.05,	Y2*0.05,	-X1*0.05);
		glVertex3d(-X1*0.05,	Y2*0.05,	-X2*0.05);
	glEnd();
	//2:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(-X2*0.05,	Y1*0.05,	-X1*0.05);
		glVertex3d(-X2*0.05,	Y1*0.05,	+X1*0.05);
		glVertex3d(-X2*0.05,	Y2*0.05,	+X1*0.05);
		glVertex3d(-X2*0.05,	Y2*0.05,	-X1*0.05);		
	glEnd();
	//3:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(-X2*0.05,	Y1*0.05,	+X1*0.05);
		glVertex3d(-X1*0.05,	Y1*0.05,	+X2*0.05);
		glVertex3d(-X1*0.05,	Y2*0.05,	+X2*0.05);
		glVertex3d(-X2*0.05,	Y2*0.05,	+X1*0.05);		
	glEnd();
	//4:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(-X1*0.05,	Y1*0.05,	+X2*0.05);
		glVertex3d(+X1*0.05,	Y1*0.05,	+X2*0.05);
		glVertex3d(+X1*0.05,	Y2*0.05,	+X2*0.05);
		glVertex3d(-X1*0.05,	Y2*0.05,	+X2*0.05);		
	glEnd();
	//5:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(+X1*0.05,	Y1*0.05,	+X2*0.05);
		glVertex3d(+X2*0.05,	Y1*0.05,	+X1*0.05);
		glVertex3d(+X2*0.05,	Y2*0.05,	+X1*0.05);
		glVertex3d(+X1*0.05,	Y2*0.05,	+X2*0.05);		
	glEnd();
	//6:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(+X2*0.05,	Y1*0.05,	+X1*0.05);
		glVertex3d(+X2*0.05,	Y1*0.05,	-X1*0.05);
		glVertex3d(+X2*0.05,	Y2*0.05,	-X1*0.05);
		glVertex3d(+X2*0.05,	Y2*0.05,	+X1*0.05);
	glEnd();
	//7:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(+X2*0.05,	Y1*0.05,	-X1*0.05);
		glVertex3d(+X1*0.05,	Y1*0.05,	-X2*0.05);
		glVertex3d(+X1*0.05,	Y2*0.05,	-X2*0.05);
		glVertex3d(+X2*0.05,	Y2*0.05,	-X1*0.05);		
	glEnd();
	//8:
	glBegin(GL_QUADS);
		glNormal3d(x,y,z);
		glVertex3d(-X1*0.05,	Y1*0.05,	-X2*0.05);
		glVertex3d(+X1*0.05,	Y1*0.05,	-X2*0.05);
		glVertex3d(+X1*0.05,	Y2*0.05,	-X2*0.05);
		glVertex3d(-X1*0.05,	Y2*0.05 ,	-X2*0.05);		
	glEnd();
}
