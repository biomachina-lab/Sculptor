/***************************************************************************
                          svt_pic_reader_tga.cpp  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_basics.h>
#include <svt_pic_reader_tga.h>
#include <svt_file_utils.h>
// clib includes
#include <svt_iostream.h>

svt_pic_reader_tga::svt_pic_reader_tga(char* filename) : svt_pic_reader_lib(filename)
{
    FILE* inFile;
    data = NULL;

    inFile = fopen(filename, "rb"); // open the file
    if (!inFile) {
        fprintf(stderr, "svt_pic_reader_tga: File %s could not be opened!\n",
				filename);
        return;
    }

    // ID
    char id_len = fgetc(inFile);
    // Color Map Type
    char col_len = fgetc(inFile);
    // Image Type
    if (fgetc(inFile) != 2)
    {
        fprintf(stderr, "Sorry only tga type 2 pictures are currently supported.\n");
        return;
    }

    // Color Map Specification
    fgetc(inFile);
    fgetc(inFile);
    fgetc(inFile);
    fgetc(inFile);
    fgetc(inFile);

    // Image Type Specification
    // x-origin
    fgetc(inFile);
    fgetc(inFile);
    // y-origin
    fgetc(inFile);
    fgetc(inFile);
    // width and height
    size_x = (unsigned long int)svt_readInt16(inFile);
    size_y = (unsigned long int)svt_readInt16(inFile);

    // Calculate the image buffer size (24bpp)
    unsigned long int size = size_x * size_y * 3;

    // Allocate the image buffer
    data = new unsigned char[size];
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error!\n");
        return;
    }

    // pixel depth
    if (fgetc(inFile) != 24)
	  {
	  fprintf(stderr, "Sorry only tga 24bpp pictures are currently supported.\n");
	  return;
	  }

    // orientation
    int orien = fgetc(inFile);
    orien = orien >> 4;

    if (id_len != 0)
        fread(data,1,id_len,inFile);
    if (col_len != 0)
        fread(data,1,col_len,inFile);

    // read picture data
    fread(data,1,size,inFile);

    // mirror the pic to the correct orientation
    if (orien >= 2)
        mirrorX();
    if (orien == 1 || orien == 3)
        mirrorY();

    // convert from BGR to RGB
    unsigned long int i; char temp;
    for (i = 0; i < size; i += 3) {
        temp = data[i];
        data[i] = data[i+2];
        data[i+2] = temp;
    }

    fclose(inFile);
}

/* mirror the picture at the y axis */
void svt_pic_reader_tga::mirrorY()
{
    // mirror the picture on the y axis
    int x,y; char tr, tg, tb;

    // mirror at the y axis
    for (y = 0; y < size_y; y++)
        for (x = 0; x < ((size_x >> 1)*3)-3; x+=3)
        {
            tr = data[(y*size_x*3)+x+0];
            tg = data[(y*size_x*3)+x+1];
            tb = data[(y*size_x*3)+x+2];

            data[(y*size_x*3)+x+0] = data[(y*size_x*3)+(size_x*3) - x - 3];
            data[(y*size_x*3)+x+1] = data[(y*size_x*3)+(size_x*3) - x - 2];
            data[(y*size_x*3)+x+2] = data[(y*size_x*3)+(size_x*3) - x - 1];

            data[(y*size_x*3)+(size_x*3) - x - 3] = tr;
            data[(y*size_x*3)+(size_x*3) - x - 2] = tg;
            data[(y*size_x*3)+(size_x*3) - x - 1] = tb;
        }
}

/* mirror the picture at the x axis */
void svt_pic_reader_tga::mirrorX()
{
    // mirror the picture on the y axis
    int x,y; char tr, tg, tb;

    // mirror at the y axis
    for (y = 0; y < (size_y >> 1); y++)
        for (x = 0; x < size_x*3; x+=3)
        {
            tr = data[(y*size_x*3)+x+0];
            tg = data[(y*size_x*3)+x+1];
            tb = data[(y*size_x*3)+x+2];

            data[(y*size_x*3)+x+0] = data[((size_y-y-1)*size_x*3)+ x + 0];
            data[(y*size_x*3)+x+1] = data[((size_y-y-1)*size_x*3)+ x + 1];
            data[(y*size_x*3)+x+2] = data[((size_y-y-1)*size_x*3)+ x + 2];

            data[((size_y-y-1)*size_x*3)+ x + 0] = tr;
            data[((size_y-y-1)*size_x*3)+ x + 1] = tg;
            data[((size_y-y-1)*size_x*3)+ x + 2] = tb;
        }
}

svt_pic_reader_tga::~svt_pic_reader_tga()
{
    if (data != NULL)
        delete data;
}
