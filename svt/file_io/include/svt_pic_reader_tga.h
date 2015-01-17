/***************************************************************************
                          svt_pic_reader_tga.h  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PIC_READER_TGA_H
#define SVT_PIC_READER_TGA_H

#include <svt_pic_reader_lib.h>

#include <stdio.h>

/** A tga file reader class. Should not be used directly! Is used by svt_pic_reader!
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_pic_reader_tga : public svt_pic_reader_lib
{
protected:
    unsigned char* data;
    FILE* inFile;
    int size_x;
    int size_y;

public:
    svt_pic_reader_tga(char* filename);
    ~svt_pic_reader_tga();

    /// returns the data of the picture
    inline unsigned char* getData(){ return data; };
    /// returns the size of the picture
    inline int getSizeX() { return size_x; };
    /// returns the size of the picture
    inline int getSizeY() { return size_y; };

protected:
    inline void setSizeX(int sx){ size_x = sx; };
    inline void setSizeY(int sy){ size_y = sy; };

    /** mirror the picture at the y axis */
    void mirrorY();
    /** mirror the picture at the x axis */
    void mirrorX();
};

#endif
