/***************************************************************************
                          svt_pic_reader.h  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : (C) 2000 by Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PIC_READER
#define SVT_PIC_READER

#include <svt_pic_reader_lib.h>

/** A picture reader class. As a user you can be sure that you will get a 24 Bit, true color picture
  * in the buffer. The picture reader will read in BMP an TGA files and will convert
  * colormap pictures (e.g. 256 Color BMPs) into 24 Bit.
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_pic_reader
{
protected:
    svt_pic_reader_lib* reader;

public:
    /// reads in the filename
    svt_pic_reader(char* filename);
    ~svt_pic_reader();

    /// returns a pointer to the buffer, where the picture is stored
    inline unsigned char* getData(){ if (reader != NULL) return reader->getData(); else return NULL; };
    /// returns the size of the picture
    inline int getSizeX(){ if (reader != NULL) return reader->getSizeX(); else return -1; };
    /// returns the size of the picture
    inline int getSizeY(){ if (reader != NULL) return reader->getSizeY(); else return -1; };

protected:
    int filetype(const char* str, const char* end);
};

#endif
