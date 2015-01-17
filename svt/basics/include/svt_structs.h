/***************************************************************************
                          svt_structs.h  -  description
                             -------------------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_STRUCTS_H
#define SVT_STRUCTS_H

// Some structures, for internal use only!
typedef struct
{
    float x;
    float y;
    float nx;
    float ny;
} svt_p2d;

typedef struct
{
    svt_p2d p[3];
} svt_t2d;

typedef struct
{
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
} svt_p3d;

typedef struct
{
    svt_p3d p[3];
} svt_t3d;

#endif
