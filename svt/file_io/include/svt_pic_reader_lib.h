/***************************************************************************
                          svt_pic_reader_lib.h  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PIC_READER_LIB
#define SVT_PIC_READER_LIB

#include <svt_basics.h>

/** This is an abstract picture file reader class. Not useful for the application programmer!
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_pic_reader_lib
{
public:
    /// reads in the file
    svt_pic_reader_lib(char*){ };
    virtual ~svt_pic_reader_lib() { };

    /// returns the data of the picture
    virtual unsigned char* getData(){ return NULL; };
    /// returns the size of the picture
    virtual int getSizeX(){ return -1; };
    /// returns the size of the picture
    virtual int getSizeY(){ return -1; };
};

#endif

