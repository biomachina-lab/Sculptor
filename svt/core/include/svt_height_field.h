/***************************************************************************
                          svt_height_field.h  -  description
                             -------------------
    begin                : Sep 01 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_HEIGHT_FIELD_H
#define SVT_HEIGHT_FIELD_H

#include <svt_node.h>
#include <svt_pic_reader.h>
#include <svt_color.h>

#include <stdio.h>
#include <svt_iostream.h>

/** A heightfield
  *@author Herwig Zilken
  */
class DLLEXPORT svt_height_field : public svt_node
{

protected:
  void createGrid(void);
  float *points;
  float *tex_coords;
  float *normals;
  svt_pic_reader* reader;

  svt_color* high_color;
  svt_color* mid_color;
  svt_color* low_color;
  void cross_product(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z, float p3x, float p3y, float p3z, float *x, float *y, float *z);

public:
    svt_height_field(char *filename);
    virtual ~svt_height_field();

    /** draw height field */
    void drawGL();
    /** print "Height Field" */
    inline void printName(){ cout << "Height Field" << endl; };
};

#endif
