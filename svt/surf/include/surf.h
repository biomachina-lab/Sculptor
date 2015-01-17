#ifndef __SURF_H
#define __SURF_H

#include	<stdio.h>
#include	<math.h>
#include	<stdlib.h>
#include	<string.h>
#include        <sys/time.h>

#include 	"linalg.h"
#include        "chull.h"

#include        "satom.h"

#ifdef EXTERN
#undef EXTERN
#define EXTERN
#else
#define EXTERN extern
#endif


/*------------------ various consts ----------------------------------*/

#define 	X   		0
#define 	Y   		1
#define 	Z   		2

#define		TRUE		1
#define		FALSE		0

/* constants defined to prevent unnecessary double->float ops on i860's */
#define         ZERO            (float)(0.0)
#define		EPS		(float)(1e-5)
#define		LP_EPS		(float)(1e-5)
#define		GP_EPS	        (float)(1e-3)
#define		HALF	        (float)(0.5)
#define         FCEIL           (float)(0.999999)
#define         ONE             (float)(1.0)
#define         FCEIL_PLUS_1    (float)(1.999999)
#define         TWO_PI      	(float)(6.28318530717958647692)
#define 	DEG_TO_RAD      (float)(.01745329251994329576) /* pi/180 */

#define         MINC            0
#define         MAXC            1

#define		MAX_TESS_LEN	1.2

#define		MAX_TYPES	5

#define         MAX_DENSITY	15 /* max no of atoms in a voxel(odd for algn) */

#define		MAX_CONSTRAINT 	4000 /* 700 */
#define		PERT_Q		4001 /* smallest prime exceeding MAX_CONSTRAINT*/



/*-------------------- general-purpose macros ------------------------*/

#define START gettimeofday(&tm,&tz);\
                et = (tm.tv_sec)+ (0.000001* (tm.tv_usec));

#define STOP gettimeofday(&tm,&tz);\
                et = (tm.tv_sec)+(0.000001*(tm.tv_usec)) - et;

#define  ALLOCN(PTR,TYPE,N) 					\
	{ PTR = (TYPE *) malloc(((unsigned)(N))*sizeof(TYPE));	\
	  if (PTR == NULL) {    				\
	  printf("malloc failed");				\
	  exit(-1);                                             \
	  }							\
	}

#define FMAX(x,y) ((x)>(y) ? (x) : (y))
#define FMIN(x,y) ((x)<(y) ? (x) : (y))
#define FP_EQ_EPS( a, b, c )  ((((a) - (b)) < (c)) && (((a) - (b)) > -(c)))
#define INDEX(x, y, z, num)  (((x)*(num[1]) + (y))*(num[2]) + (z))
#define AINDEX(a, num)       (((a[0])*(num[1]) + (a[1]))*(num[2]) + (a[2]))
#define SQ(a)                ((a)*(a))

/*---------------------------structs-------------------------------------*/
typedef unsigned char   byte;

typedef double		PNT[3];

typedef struct 
{ float         	coeff[4];
  int           	atom_id;
} Vector;

typedef struct 		
{ short			num_neighbors;
  short		        neighbor[MAX_DENSITY];
} Atom_List;

struct Torus
{ int                   face_atoms[2];
  int                   end_atoms[2];
  int                   face_id;
  int                   edge_id[2];
  int                   num_int_verts;
  struct Torus		*next;
  struct Torus		*prev;
};

typedef struct
{ VertexType		vt;
  PNT			tes_dir; 
} Big_Point;


/*---------------------------variables-------------------------------------*/
extern double           find_area();

EXTERN float            Probe_radius;
EXTERN Gp_Atom          *atoms;
EXTERN int              Num_atoms;
EXTERN int              Checks_On;
EXTERN int              Max_Gp_Polys;
EXTERN double           Max_Tess_Len, Max_Tess_Len_Sq;
EXTERN VertexType       **verts;
EXTERN short            *atom_type;
EXTERN int              Num_polys;
EXTERN Vector           New_Origin;  
EXTERN int              Current_atom;
EXTERN FILE            	*Opf;
EXTERN Vector 		temp_cons[2][MAX_CONSTRAINT+6];
EXTERN int              Write_Option;
EXTERN struct timeval   tm;
EXTERN struct timezone  tz;
EXTERN double           et;
EXTERN_LINALG

/**
 * Forward declarations of functions
 */

//
// tessel_convex
//
void initialize_r(Big_Point* big_p, Big_Point* r, VertexType* q, int num_p, int* num_r, int num_q, int same_order, int atom_id, int full_torus);
void gen_convex(PNT* comp_verts, VertexType* p, PNT* probe_centers, VertexType* q, int num_p, int num_q, int same_order, int flip, int atom_id, int full_torus);
void gen_linear_recurse(VertexType p0, VertexType p1, VertexType* r, int* j, float center[3], float radius, int convex);
void new_gen_sphere_tris(Big_Point* bp0, Big_Point* bp1, Big_Point* bp2, int atom_id);
void multi_gen_spherical_recurse(Big_Point* pt0, Big_Point* pt1, Big_Point* pt2, int atom_id);
void find_mid_pt(Big_Point* mid_pt, Big_Point* tri_pt0, Big_Point* tri_pt1, int atom_id);
void gen_tris(VertexType* pt0, VertexType* pt1, VertexType* pt2);
void add_tri(VertexType* pt0, VertexType* pt1, VertexType* pt2, int atype);
//
// io
//
void input_dataset(char* filename);
void begin_output_dataset(char outfilename[128]);
void write_archive_tri(VertexType* pt0, VertexType* pt1, VertexType* pt2, int atype);
void output_dataset();
void end_output_dataset();
//
// chull
//
struct tface *make_structs(struct tedge* e, struct tvertex *p );
void make_ccw( struct tface *f, struct tedge *e, struct tvertex *p );
struct tedge *make_edge();
struct tface *make_face();
void init_tet();
bool co_linear( struct tvertex *a, struct tvertex *b, struct tvertex *c );
void pre_vol( struct tface *f );
void complete_hull();
int add_on( struct tvertex *p );
void read_vertices();
void copy_vertices();
void print();
void meta_clean();
void clean_up();
void clean_edges();
void clean_faces();
void clean_vertices();
void consistency();
void convexity();
void check_faces(int cvertices, int cfaces );
void check_edges( int cvertices, int cedges );
void checks();
//
// compute
//
void init_and_compute();
void compute_extents();
void compute_bounding_tetra();
void compute_components(int atom_id, Vector *constraints);
int compute_neighbors(int i, int* num_cons);
void compute_planes(int atom_id, int num_cons, Vector* cons);
int find_origin(int atom_id, Vector* cons, int num_cons);
void find_sol_cons(Vector *cons, int num_cons, int sol_cons[3], Vector sol);
void transform_and_add_extents(int atom_id, Vector *cons, int* num_cons);
void find_components(int atom_id, Vector *cons);
void find_tes_origin(int atom_id, Vector* cons);
int point_in_region(PNT test_point, Vector* cons);
int all_verts_in_sphere(struct rvertex *rverts, int num_rverts, double sq_radius);
int gen_component(int atom_id, int face_id, Vector* cons);
void get_torus_elt(int atom_id, int face_id, int* edge_id, Vector* cons, struct Torus *torus_elt);
void quick_sort_planes(Vector* cons, int m, int n);
//
// dual
//
void dualize_hull(int atom_id, Vector* cons);
void alloc_fc_verts();
void alloc_fc_faces();
void alloc_fc_edges();
void setup_vert_edge_ptrs();
void clean_fc();
void gen_fc_face(struct	tvertex	*ch_vert, struct tface *ch_face);
void dualize_face_to_point(int atom_id, struct tface *face_ptr, Vector* cons);
void dualize_planes_to_points(Vector* plane, int num_planes);
void find_edge_sphere_intersection(int atom_id, struct redge *edge_ptr);
void print_cell();
void display_face(int atom_id, struct rface region_face, Vector	*cons);
//
// utils
//
void find_plane_eq(float pt0[3], float pt1[3], float pt2[3], float plane_eq[4]);
void compute_tri_plane_int(Vector plane0, Vector plane1, Vector plane2, double result[3]);
void find_ray_sphere_int(float int_point[3], float ray_pt[3], float ray_dir[3], float center[3], float radius);
void find_line_plane_int(float int_point[3], float line_pt[3], float line_dir[3], float plane_eq[4]);
void find_line_seg_plane_int(float int_point[3], float point0[3], float point1[3], float plane_eq[4]);
double find_angle(float p0[3], float p1[3], float plane_eq[4], float org[3]);
double sph_dist(float p0[3], float p1[3], float center[3]);
//
// tessel_cases
//
void gen_case_I_II(struct Torus	*temp_torus, Vector *cons);
void gen_case_III_IV(int atom_id, int face_id, Vector *cons);
void gen_case_III(int atom_id, int face_id, Vector* cons);
void gen_case_IV(int atom_id, int face_id, Vector* cons);
void compute_transf_mat_I_II(PNT pt0, PNT pt1, MatrixD4 inv_transf, PNT center, double radius, int acute);
void compute_transf_mat_III(Vector constraint, MatrixD4 inv_transf, PNT center, double radius);
double find_torus_angle(PNT *pts, int num_pts, PNT center);
int find_circle(int face_atoms[2], Vector plane, PNT center, double* radius);
void find_concave_center(int face_atoms[2], int k, PNT probe_pos, VertexType* concave_center);
//
// tessel_concave
//
void gen_concave(VertexType **tor_pts, VertexType center[2], int flip_verts, VertexType cusp_pts[2], PNT probe[2], int n, struct Torus *cur_torus, int torus_cusp);
void gen_concave_tri(VertexType point0, VertexType point1, VertexType point2, int flip, PNT center, double radius);
void gen_cuspy_concave_tri(VertexType point0, VertexType point1, VertexType point2, int flip, PNT center, double radius, Vector* plane_eq);
void gen_cuspy_sphere_tris(VertexType pt0, VertexType pt1, VertexType pt2, float center[3], double radius, Vector* plane_eq);
void gen_cuspy_spherical_recurse(VertexType *points[3], double sq_side_len[3], float center[3], Vector* plane_eq, double radius);
void gen_sphere_tris(VertexType	*pt0, VertexType *pt1, VertexType *pt2, float center[3], double radius, int convex);
void gen_spherical_recurse(VertexType *points[3], double sq_side_len[3], float center[3], double radius, int convex);
//
// tessel_torus
//
void get_full_torus_mesh(Big_Point *sph, int num_sph, Big_Point	*torus, int num_torus, int flip, int atom_id);
void get_partial_torus_mesh(Big_Point *sph, int num_sph, Big_Point *torus, int num_torus, int flip, int atom_id);
void gen_torus(VertexType **tor_pts, PNT* probe_centers, int num_verts, int flip, double radius, int cusp);
//
// tessel_patches
//
void gen_patches_I_II(int face_atoms[2], PNT* comp_verts, int num_verts, VertexType concave_center[2], double cir_radius, MatrixD4 inv_transf, double angle, struct Torus* cur_torus, int torus_cusp);
void gen_patches_III(int face_atoms[2], PNT* comp_verts, int num_verts, PNT cir_center, double cir_radius, MatrixD4 inv_transf, int torus_cusp);
void gen_patches_IV(int atom_id, PNT* comp_verts, int num_verts);
void gen_arc_recurse(Big_Point *bp0, Big_Point *bp1, Big_Point *sp_points, int* j, int atom_id, int convex);
void find_sphere_points(VertexType* sph_pts, int j, PNT comp_vert, Gp_Atom* atom_ptr);
void find_torus_points(VertexType **tor_pts, VertexType mid_cusp_pts[2], int num_pts, Gp_Atom *atom_ptr[2], PNT *probe_centers, double cir_radius, int cusp);
int find_vertex_order(VertexType *pt0, Big_Point* verts, int num_verts);
int find_order(PNT *verts, int num_verts, int atom[2]);
//
// lp
//
int linear_prog(int dim, Vector obj, float extents[3][2], Vector* A, int num_A, Vector* sol, int sol_cons[3]);
//
// tessel_convex
//
void gen_convex(PNT* comp_verts, VertexType* p, PNT* probe_centers, VertexType* q, int num_p, int num_q, int same_order, int flip, int atom_id, int full_torus);
void initialize_r(Big_Point* big_p, Big_Point* r, VertexType* q, int num_p, int* num_r, int num_q, int same_order, int atom_id, int full_torus);
void gen_linear_recurse(VertexType p0, VertexType p1, VertexType* r, int* j, float center[3], float radius, int convex);
void gen_flipped_sphere_tris(Big_Point* p0, Big_Point* p1, Big_Point* p2, int atom_id, int flip);
void new_gen_sphere_tris(Big_Point* bp0, Big_Point* bp1, Big_Point* bp2, int atom_id);
void multi_gen_spherical_recurse(Big_Point* pt0, Big_Point* pt1, Big_Point* pt2, int atom_id);
void find_mid_pt(Big_Point* mid_pt, Big_Point* tri_pt0, Big_Point* tri_pt1, int atom_id);
void gen_tris(VertexType* pt0, VertexType* pt1, VertexType* pt2);
void add_tri(VertexType* pt0, VertexType* pt1, VertexType* pt2, int atype);

#endif
