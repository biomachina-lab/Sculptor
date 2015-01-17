/***************************************************************************
                          svt_pic_reader_bmp.h  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PIC_READER_BMP_H
#define SVT_PIC_READER_BMP_H

#include <svt_pic_reader_lib.h>

#include <stdio.h>

/** A bmp file reader class. Should not be used directly! Is used by svt_pic_reader!
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_pic_reader_bmp : public svt_pic_reader_lib
{
protected:
    unsigned char* data;
    int size_x;
    int size_y;

public:
    svt_pic_reader_bmp(char* filename);
    ~svt_pic_reader_bmp();

    /// returns the data of the picture
    inline unsigned char* getData(){ return data; };
    /// returns the size of the picture
    inline int getSizeX() { return size_x; };
    /// returns the size of the picture
    inline int getSizeY() { return size_y; };

protected:
    inline void setSizeX(int sx){ size_x = sx; };
    inline void setSizeY(int sy){ size_y = sy; };
    // ReadBMP - reads a BMP
    bool ReadBMP(FILE* inFile);

};

#endif
