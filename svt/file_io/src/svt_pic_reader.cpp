/***************************************************************************
                          svt_pic_reader.cpp  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_pic_reader.h>
#include <svt_pic_reader_lib.h>
#include <svt_pic_reader_bmp.h>
#include <svt_pic_reader_tga.h>
#include <string.h>

svt_pic_reader::svt_pic_reader(char* filename)
{
    if (filetype(filename,"bmp") || filetype(filename,"BMP"))
        reader = (svt_pic_reader_lib*)new svt_pic_reader_bmp(filename);

    if (filetype(filename,"tga") || filetype(filename,"TGA"))
        reader = (svt_pic_reader_lib*)new svt_pic_reader_tga(filename);

    return;
}

svt_pic_reader::~svt_pic_reader()
{
    if (reader != NULL) delete reader;
};

int svt_pic_reader::filetype(const char* str, const char* end)
{
    int i = strlen(str)-3;

    if (i<0) return 0;

    if (str[i+0] != end[0]) return 0;
    if (str[i+1] != end[1]) return 0;
    if (str[i+2] != end[2]) return 0;
    return 1;
}
