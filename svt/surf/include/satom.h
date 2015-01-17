#ifndef __SATOM_H
#define __SATOM_H

typedef struct
{ float			radius;
  float			center[3];
  float			tes_origin[3];
  short			num_cons;
  short			type; 
  short			boundary;
} Gp_Atom;

typedef float VectorType[3];	/* vector in three-space */
typedef float PointType[3];     /* point in homogenous three-space */
typedef struct {
    PointType Coord;
    VectorType Normal;
} VertexType;

#endif
