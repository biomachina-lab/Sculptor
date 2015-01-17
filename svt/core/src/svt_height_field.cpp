/***************************************************************************
                          svt_height_field.cpp
			  --------------------
    begin                : Sep 01 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_height_field.h>
#include <svt_file_utils.h>
#include <svt_opengl.h>
#include <stdlib.h>

svt_height_field::~svt_height_field()
{
    delete reader;

    if (points != NULL) {
      free(points);
    }
    if (tex_coords != NULL) {
      free(tex_coords);
    }
    if (normals != NULL) {
      free(normals);
    }
}


svt_height_field::svt_height_field(char *filename)
{
    // colors
    high_color = new svt_color(1.0f,1.0f,1.0f);
    mid_color = new svt_color(0.3f,0.7f,0.4f);
    low_color = new svt_color(0.4f,0.4f,0.4f);

    // read picture file
    reader = new svt_pic_reader(filename);
    if (reader->getData() == NULL)
    {
        cerr << "svt_height_field: File " << filename << " could not be opened!\n";
        exit(1);
    }

    // create mesh
    createGrid();
}


void svt_height_field::cross_product(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z, float p3x, float p3y, float p3z, float *x, float *y, float *z)
{
float x1, y1, z1, x2, y2, z2;
float l;
  x1 = p2x-p1x; y1 = p2y-p1y; z1 = p2z-p1z;
  x2 = p3x-p1x; y2 = p3y-p1y; z2 = p3z-p1z;


  *x = -(y1*z2 - z1*y2);
  *y = -(z1*x2 - x1*z2);
  *z = -(x1*y2 - y1*x2);

  l = sqrt(*x**x + *y**y + *z**z);
  *x /= l; *y /= l; *z /= l;
}


void svt_height_field::createGrid(void)
{
    int i,j;
    float height;
    float *ptemp;
    float *ttemp;
    float *ntemp;

    unsigned char* data = reader->getData();
    int size_x = reader->getSizeX();
    int size_y = reader->getSizeY();

    cout << "svt_height_field: imgage size: (" << size_x << "x" << size_y << ")"  << endl;
    cout << "svt_height_field: creating grid" << endl;
    points = (GLfloat*) malloc(sizeof(GLfloat)*(size_x-1)*size_y*2 * 3);
    normals = (GLfloat*) malloc(sizeof(GLfloat)*(size_x-1)*size_y*2 * 3);
    tex_coords = (GLfloat*) malloc(sizeof(GLfloat)*(size_x-1)*size_y*2 * 2);
    if (points==NULL) {
        cerr << "svt_height_field: memory allocation error!\n";
        exit(1);
    }
    if (normals==NULL) {
        cerr << "svt_height_field: memory allocation error!\n";
        exit(1);
    }
    if (tex_coords==NULL) {
        cerr << "svt_height_field: memory allocation error!\n";
        exit(1);
    }
    ptemp=points;
    ntemp=normals;
    ttemp=tex_coords;
    for (i=0; i<size_x-1; i++) {
        for (j=0; j<size_y; j++) {
            height = ((GLfloat) data[(i+j*size_x)*3]) + ((GLfloat) data[(i+j*size_x)*3+1]) + ((GLfloat) data[(i+j*size_x)*3+2]);
            // x value in OpenGL coordinate system
            *(ptemp++) = (GLfloat) 0.5*(2.0*i-size_x)/(GLfloat)size_x;
            *(ttemp++) = (GLfloat) i/(GLfloat)size_x;
            // y value in OpenGL coordinate system
            *(ptemp++) = height/(3.0*255.0)*.05;
            // z value in OpenGL coordinate system
            *(ptemp++) = (GLfloat) +0.5*(2.0*j-size_y)/(GLfloat)size_y;
            *(ttemp++) = (GLfloat) j/(GLfloat)size_y;


            // second point in row
            height = ((GLfloat) data[((i+1)+j*size_x)*3]) + ((GLfloat) data[((i+1)+j*size_x)*3+1]) + ((GLfloat) data[((i+1)+j*size_x)*3+2]);
            // x value in OpenGL coordinate system
            *(ptemp++) = (GLfloat) 0.5*(2.0*(i+1)-size_x)/(GLfloat)size_x;
            *(ttemp++) = (GLfloat) (i+1)/(GLfloat)size_x;
            // y value in OpenGL coordinate system
            *(ptemp++) = height/(3.0*255.0)*.05;
            // z value in OpenGL coordinate system
            *(ptemp++) = (GLfloat) +0.5*(2.0*j-size_y)/(GLfloat)size_y;
            *(ttemp++) = (GLfloat) j/(GLfloat)size_y;
            if (j==1) {
              if (i==0) cross_product(*(ptemp-12), *(ptemp-11), *(ptemp-10),
                            *(ptemp-9), *(ptemp-8), *(ptemp-7),  
                            *(ptemp-6), *(ptemp-5), *(ptemp-4),
                            ntemp, ntemp+1, ntemp+2);
              else {
               *ntemp = *(ntemp-6*size_y+3);
               *(ntemp+1) = *(ntemp-6*size_y+4);
               *(ntemp+2) = *(ntemp-6*size_y+5);
              }
              ntemp += 3;
              cross_product(*(ptemp-9), *(ptemp-8), *(ptemp-7),  
                            *(ptemp-6), *(ptemp-5), *(ptemp-4),
                            *(ptemp-12), *(ptemp-11), *(ptemp-10),
                            ntemp, ntemp+1, ntemp+2);
              ntemp += 3;          
	    }
            if (j>0) {
              if (i==0) cross_product(*(ptemp-6), *(ptemp-5), *(ptemp-4),
                            *(ptemp-12), *(ptemp-11), *(ptemp-10),
                            *(ptemp-9), *(ptemp-8), *(ptemp-7),                            
                            ntemp, ntemp+1, ntemp+2);
              else {
               *ntemp = *(ntemp-6*size_y+3);
               *(ntemp+1) = *(ntemp-6*size_y+4);
               *(ntemp+2) = *(ntemp-6*size_y+5);
              }
              ntemp += 3;          
              cross_product(*(ptemp-3), *(ptemp-2), *(ptemp-1),
                            *(ptemp-6), *(ptemp-5), *(ptemp-4),  
                            *(ptemp-9), *(ptemp-8), *(ptemp-7),
                            ntemp, ntemp+1, ntemp+2);

              ntemp += 3;        
	    }
        }
    }
}

void svt_height_field::drawGL()
{
    int i,j;
    float *ptemp;
    float *ttemp;
    float *ntemp;
    float x,y;
    int data_x, data_y;
    // float r,g,b;
    int size_x = reader->getSizeX();
    int size_y = reader->getSizeY();

    //glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    // glDisable(GL_CULL_FACE);
    //BindTexture();
    ptemp = points;
    ntemp = normals;
    ttemp = tex_coords;
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, texture);

    for (i=0; i<size_x-1; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (j=0; j<size_y*2; j++ ) {
            //glTexCoord2fv(ttemp);
            x = *ptemp;
            y = *ptemp+1;
            //*(ptemp++) = (GLfloat) (2.0*(i+1)-size_x)/(GLfloat)size_x;
            data_x = (int) ((x*size_x + size_x)/2.0);
            data_y = (int) ((y*size_y + size_y)/2.0);
            // cout << data_x << ", " << data_y << endl;
            if (data_x<0) data_x=0;
            if (data_x>=size_x) data_x=size_x-1;
            if (data_y<0) data_y=0;
            if (data_y>=size_y) data_y=size_y-1;
            //cout << data_x << ", " << data_y << endl;
            //r = (float)data[(data_x+data_y*size_x)*3]/256.0;
            //g = (float)data[(data_x+data_y*size_x)*3+1]/256.0;
            //b = (float)data[(data_x+data_y*size_x)*3+2]/256.0;
            //color.setR(r); color.setG(g); color.setB(b);
            //color.applyProperty();
            
            glNormal3fv(ntemp);
            glVertex3fv(ptemp);

            ntemp += 3;
            ptemp += 3;
            ttemp += 2;
        }
        glEnd();
    }

    /*    ptemp = points;
    for (i=0; i<size_x-1; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (j=0; j<size_y*2; j++ ) {
            if (ptemp[1] <= 0.3f)
                low_color->applyProperty();
            else if (ptemp[1] <= 0.6f)
                mid_color->applyProperty();
            else if (ptemp[1] <= 1.0f )
                high_color->applyProperty();
            glVertex3fv(ptemp);
            ptemp += 3;
        }
        glEnd();
    } */

};
