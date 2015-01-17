/***************************************************************************
                          svt_md3.h  -  description
                             -------------------
    begin                : Jul 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_MD3_H
#define SVT_MD3_H

#include <svt_node.h>
#include <svt_texture.h>

#include <stdio.h>
#include <svt_iostream.h>

#ifndef SVT_SKIP_DOC

typedef struct
{
    float vec[3];
    float nvec[3];
} vertex;

typedef struct
{
    float x;
    float y;
} textur;

typedef struct
{
    long int a;
    long int b;
    long int c;
} triangle;

typedef struct
{
    long int x;
    long int y;
    long int z;
} normal;

typedef struct
{
    vertex* vertices;
    triangle* triangles;
    textur* textures;
    normal* normals;
    int frame_num;
    int tri_num;
    int skin_num;
    int ver_num;
    int cur_frame;
} mesh;

typedef struct
{
    float x,y,z;
    char name[64];
    float matrix[16];
} tag;

typedef struct
{
    float x, y, z;
    float scale;
    tag* tags;
} bone;

#endif

/** A md3 file loader. Create a svt object of a md3 file (3d object file format).
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_md3 : public svt_node
{
protected:
    int tag_num;
    int bone_num;
    int tag_start;
    int tag_end;
    int mesh_num;
    int tri_start;
    int tex_start;
    int ver_start;

    long int mesh_off;
    int mesh_size;

    mesh* meshes;
    svt_texture* text;

    bone* bones;

    svt_node* frames;

public:
    svt_md3(svt_pos* _ta, char* file, char* texture) : svt_node(NULL,NULL,_ta)
    {
        loadMD3(file, texture);
    };

    /* delete everything */
    virtual ~svt_md3()
    {
    };

public:
    /* print "md3" */
    inline void printName(){ cout << "md3" << endl; };

    /* draw the md3 model */
    virtual void drawGL();

protected:
    /* load md3 file */
    void loadMD3(char* filename, char* texture);

    /* read a md3 header */
    void readHeader(FILE* file);
    /* read a md3 tag */
    void readTag(FILE* file, tag* t);
    /* read a md3 bone */
    void readBone(FILE* file, int i);
    /* read a md3 mesh */
    void readMesh(FILE* file, int mesh);
    /* read triangles */
    void readTriangles(FILE* file, int mesh);
    /* read vertices */
    void readVertices(FILE* file, int mesh);
    /* read texture coords */
    void readTextures(FILE* file, int mesh);
    /* calc the normal vectors for each triangle of each frame */
    void calcNormals();

public:
    /* name of tag to apply */
    void applyTag(char* name);
};

#endif
