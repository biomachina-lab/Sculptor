/*====================================================================
  __file: Make.c
  This file contains the functions that build up the convex hull
  ===================================================================*/

#ifdef WIN32
#include <windows.h>
#endif

#include <svt_basics.h>

#include "surf.h"
#include <svt_cmath.h>
#define SURFOUT cout << svt_trimName("svt_surf")

/*----------------------------------------------------------------------
       Make_structs makes a new face and two new edges between the 
  edge and the point that are passed to it. It returns a pointer to
  the new face.
 ----------------------------------------------------------------------*/
struct tface *make_structs(struct tedge* e, struct tvertex *p )
{
    struct tedge *new_edge[2];
    struct tedge *make_edge();
    struct tface *new_face;
    struct tface *make_face();
    int i, j;

    for ( i=0; i < 2; ++i )
        if ( !( new_edge[i] = e->endpts[i]->duplicate) )
        {
            /* if the edge does not already exist, make it */
            new_edge[i] = make_edge();
            new_edge[i]->endpts[0] = e->endpts[i];
            new_edge[i]->endpts[1] = p;
            e->endpts[i]->duplicate = new_edge[i];
        }

    /* make the new face */
    new_face = make_face();
    new_face->edg[0] = e;
    new_face->edg[1] = new_edge[0];
    new_face->edg[2] = new_edge[1];
    make_ccw( new_face, e, p );
    pre_vol( new_face );

    /* set the adjacent face pointers */
    for ( i=0; i < 2; ++i )
        for ( j=0; j < 3; ++j )
            if ( !new_edge[i]->adjface[j] )
            {
                new_edge[i]->adjface[j] = new_face;
                break;
            }

    return new_face;
}

/*------------------------------------------------------------------------
      Make_ccw puts the vetices in the face structure in counter
  clockwise order.  If there is no adjacent face[1] then we know that
  we are working with the first face of the initial tetrahedron.  In this
  case we want to store the vertices in the opposite order from the 
  initial face.  Otherwise, we want to store the vertices in the same order
  as in the visible face.  The third vertex is always p.
  ------------------------------------------------------------------------*/
void make_ccw( struct tface *f, struct tedge *e, struct tvertex *p )
{
        register int i;
        register struct tface *fv;
        
        if ( !e->adjface[1] ) {
             /* if this is the initial triangle */
             fv = e->adjface[0];
        
             /* find the index of endpoint[1] */
             for ( i=0; fv->vert[i] != e->endpts[1]; ++i )
                   ;
             /* put the vertices in the opposite order of fv */
             if ( fv->vert[ (i+1) % 3 ] != e->endpts[0] ) {
                  f->vert[0] = e->endpts[1];  
                  f->vert[1] = e->endpts[0];    
                  }
             else {                               
                  f->vert[0] = e->endpts[0];   
                  f->vert[1] = e->endpts[1];      
                  }
             }
        
        else {
             /* otherwise,  set the visible face */
             if  ( e->adjface[0]->visible )      
                   fv = e->adjface[0];
             else fv = e->adjface[1];
        
             /* find the index of endpoint[1] */
             for ( i = 0; fv->vert[i] != e->endpts[1] ; ++i )  
                   ;    				
        
             /* put the vertices in the same order as fv */
             if ( fv->vert[ (i+1) % 3 ] != e->endpts[0] ) {
                  f->vert[0] = e->endpts[0];     
                  f->vert[1] = e->endpts[1];     
                  }
             else {     
                  f->vert[0] = e->endpts[1];     
                  f->vert[1] = e->endpts[0]; 
                  } 
              }
 
        f->vert[2] = p;
}
 
/*---------------------------------------------------------------------
      Make_edge creates a new cell and initializes all pointers to NULL
   and sets all flags to off.  It returns a pointer to the empty cell.
  ---------------------------------------------------------------------*/
struct tedge *make_edge()
{
    register struct tedge *pNew;

    ALLOCATE( pNew, struct tedge );
    pNew->adjface[0] = pNew->adjface[1] = pNew->adjface[2] = NULL;
    pNew->endpts[0] = pNew->endpts[1] = NULL;
    pNew->deleted = !DELETED;
    ADD_QUEUE( edges, pNew );
    return pNew;
}

/*---------------------------------------------------------------------
     Make_face creates a new face structure and initializes all of its
  flags to NULL and sets all the flags to off.  It returns a pointer
  to the empty cell.
 ----------------------------------------------------------------------*/
struct tface *make_face()
{
    struct tface *pNew;
    int i;

	ALLOCATE( pNew, struct tface);
	for ( i=0; i < 3; ++i ) {
    	      pNew->edg[i] = NULL;
    	      pNew->vert[i] = NULL;
    	      }
	pNew->visible = 0; /* FALSE */
	ADD_QUEUE( faces, pNew );
	return pNew;

}
/*====================================================================
  __file: Initial.c   This file contains the functions that build up the  
  the initial tetrahedron.
  ===================================================================*/

/*-----------------------------------------------------------------------
   Init_tet builds the initial tetrahedron.  It first finds 3 non-colinear
 points and makes a face out of them.   It next finds a fourth point that
 is not co-planar with the face.  The vertices are stored in the face
 structure in counter clockwise order so that the volume between the face
 and the point is negative. Lastly, the 3 newfaces to the fourth point
 are made and the data structures are cleaned up. 
 -----------------------------------------------------------------------*/
void init_tet()
{
	register struct tvertex *v1, *v4, *t;
	register struct tface *f;
        //struct tface *make_face(), *make_structs();
	register struct tedge *e1, *e2, *e3, *s;
        //struct tedge *make_edge();
	double vol;
	
	v1 = vertices;

	/* find 3 non-colinear points */
	while ( co_linear( v1, v1->next, v1->next->next ) ) 
            if ( ( v1 = v1->next ) == vertices )
            {
                SURFOUT << "There are no 3 non-colinear points." << endl;
                return;
            }
	
	/* mark the original vertices as used */
	v1->mark = MARKED;  v1->next->mark = MARKED; 
	v1->next->next->mark = MARKED;              
                                               
	/* make the edges of the original triangle */
	e1 = make_edge(); 
	e2 = make_edge();
	e3 = make_edge();
	e1->endpts[0] = v1;              e1->endpts[1] = v1->next;
	e2->endpts[0] = v1->next;        e2->endpts[1] = v1->next->next;
	e3->endpts[0] = v1->next->next;  e3->endpts[1] = v1;

	/* make the face of the original triangle */
	f = make_face();   
	f->edg[0] = e1;   f->edg[1] = e2;   f->edg[2] = e3;
	f->vert[0] = v1;  f->vert[1] = v1->next;
	f->vert[2] = v1->next->next;

	/* set the adjacent faces */
	e1->adjface[0] = e2->adjface[0] = e3->adjface[0] = f; 
	
	v4 = v1->next->next->next;
	pre_vol (f);
	
	/* find a fourth non-coplanar point */
	volume( vol, f, v4 );
	while ( vol <= 0.0 )   {
            if ( ( v4 = v4->next ) == v1 )
            {
                SURFOUT << "There are no 4 non-coplanar points." << endl;
                return;
            }
            volume( vol, f, v4 );
        }
        v4->mark = MARKED;

	/* store points in counter clockwise order */
	if( vol < 0 ) {
            SWAP( t, f->vert[0], f->vert[1] );
            SWAP( s, f->edg[1], f->edg[2] );
  	    }

	pre_vol( f );
	
	/* make the faces and edges between the original
   	   triangle and the fourth point. */
	e1->adjface[1] = make_structs( e1, v4 );
	e2->adjface[1] = make_structs( e2, v4 );
	e3->adjface[1] = make_structs( e3, v4 );

	clean_up();
}

/*===================================================================
  __file: Degen.c  This file contains the functions that check for 
  co-linear and co-planar points.
  ===================================================================*/


/*------------------------------------------------------------------
     Co_linear checks to see if the three points given are colinear.
  It checks to see if the cross product  ( rather, each element of
  the cross product  i, j, and k ) between the two vectors formed by
  the points is 0 or not.  If each element is 0 then the area of the
  triangle is 0 and the points are colinear.
---------------------------------------------------------------------*/
bool co_linear( struct tvertex *a, struct tvertex *b, struct tvertex *c )
{
    return ( c->v[Z] - a->v[Z] ) * ( b->v[Y] - a->v[Y] ) -
        ( b->v[Z] - a->v[Z] ) * ( c->v[Y] - a->v[Y] ) == 0
        && ( b->v[Z] - a->v[Z] ) * ( c->v[X] - a->v[X] ) -
        ( b->v[X] - a->v[X] ) * ( c->v[Z] - a->v[Z] ) == 0
        && ( b->v[X] - a->v[X] ) * ( c->v[Y] - a->v[Y] ) -
        ( b->v[Y] - a->v[Y] ) * ( c->v[X] - a->v[X] ) == 0  ;
}


/*----------------------------------------------------------------------
 Pre_vol calculates the volume information that corresponds to
 each face and stores it in the face structure.
 ----------------------------------------------------------------------*/
void pre_vol( struct tface *f )
{
    double x1, x2, x3, y1, y2, y3, z1, z2, z3;

    x1 = f->vert[0]->v[0];
    y1 = f->vert[0]->v[1];
    z1 = f->vert[0]->v[2];

    x2 = f->vert[1]->v[0] - x1;
    y2 = f->vert[1]->v[1] - y1;
    z2 = f->vert[1]->v[2] - z1;

    x3 = f->vert[2]->v[0] - x1;
    y3 = f->vert[2]->v[1] - y1;
    z3 = f->vert[2]->v[2] - z1;

    f->p[X] =  y3*z2 - y2*z3;
    f->p[Y] =  x2*z3 - x3*z2;
    f->p[Z] =  x3*y2 - x2*y3;
}

/*=========================================================================
  __file: Hull.c    This file contains the functions that make the rest of
  the hull after the initial tetrahedron is built.
 =========================================================================*/

/*-------------------------------------------------------------------------
        Complete_hull goes through the vertex list and adds them to the hull
   if they are not already used.  It will mark the vertex once it is 
   looked at.
  -------------------------------------------------------------------------*/
void complete_hull()
{
    struct tvertex *v, *tmp_vert;

    v = vertices;
    do
    {
        if ( !v->mark )
        {
            v->mark = MARKED;
            if (add_on( v ))
            {
                clean_up();
                v = v->next;
            }
            else
            {
                tmp_vert = v;
                v = v->next;
                DEL_QUEUE(vertices, tmp_vert);
            }
        }
        else
            v = v->next;
    } while ( v != vertices );

    clean_up();
}

/*---------------------------------------------------------------------------
      Add_on is passed a vertex.  It will first determine all faces that
  are visible from that point.  If none are visible then the point is 
  marked not active and 0 is returned (else a 1 is returned).  Then it will 
  go through the edge list.  If both of the edges adjacent faces are  
  visible then the edge is marked deleted.  If one of the adjacent faces is 
  visible then a new face is made and the edges adjacent face[2] pointer is
  set to point to it. A temporary pointer is used so that any new edge just 
  added to the list will not be examined.
  --------------------------------------------------------------------------*/
int add_on( struct tvertex *p )
{
    register struct tface *f;
    //struct tface *make_structs();
    register struct tedge *e;
    struct tedge	*temp;
    int vis = 0; /* FALSE */

    f = faces;
    do {
        /* mark the visible faces and set a flag */
        vis |= f->visible = is_visible(f, p);
        f = f->next;
    } while ( f != faces );

    if ( !vis ) {
        /* if no faces are visible then */
        /* mark the vertex inactive */
        p->active = !ACTIVE;
        return (0);
    }

    e = edges;
    do {
        temp = e->next;
        if ( e->adjface[0]->visible && e->adjface[1]->visible )
            /* if e is an interior edge, delete it */
            e->deleted = DELETED;

        else if ( e->adjface[0]->visible || e->adjface[1]->visible )
            /* if e is a border edge, make a new face */
            e->adjface[2] = make_structs( e, p );
        e = temp;
    } while ( e != edges );

    return(1);
}

/* ==================================================================
 __file: Data.c   This file contains the functions that take care of
 the data structures-  reading the information in, storing it,
 and printing it out.
 ==================================================================*/

/*--------------------------------------------------------------------
 Read_vertices reads the vertices in and puts them in a circular,
 doubly linked list of tvertex structures.  The duplicate pointer is
 set to NULL and the active flag is set off.
 --------------------------------------------------------------------*/
void read_vertices()
{
    register struct tvertex *pNew;
    double x, y, z;
    int    i = 0;

    while ( scanf ("%lf%lf%lf", &x, &y, &z ) != EOF )
    {
        ALLOCATE( pNew, struct tvertex );
        pNew->v[X] = x;
        pNew->v[Y] = y;
        pNew->v[Z] = z;
        pNew->vnum[0] = pNew->vnum[1] = pNew->vnum[2] = ++i;
        pNew->vnum[1] *= pNew->vnum[0];
        pNew->vnum[2] *= pNew->vnum[1];
        pNew->duplicate = NULL;
        pNew->active = !ACTIVE;
        pNew->mark = !MARKED;
        ADD_QUEUE( perm_vert, pNew );
    }

}

/*--------------------------------------------------------------------
 Copies the vertices from the perm_vert queue into the vertices queue.
 --------------------------------------------------------------------*/
void copy_vertices()
{
    register struct tvertex *pNew, *v1;

    v1 = perm_vert;
    do {
        ALLOCATE( pNew, struct tvertex );
        memcpy(pNew, v1, sizeof (struct tvertex));
        ADD_QUEUE( vertices, pNew );
    } while((v1 = v1->prev)!= perm_vert);

}

/* ==================================================================
 __file: Clean.c   This file contains the functions that take care of
 cleaning up the data structures.
 ==================================================================*/

/*-------------------------------------------------------------------------
 Meta_clean goes through each data structure list and clears all
 flags and NULLs out all pointers and frees them.
 ------------------------------------------------------------------------*/
void meta_clean()
{
    register struct tedge *e;
    register struct tvertex *v;
    register struct tface *f;

    while ( edges )
    {
        e = edges;
        DEL_QUEUE ( edges, e );
    }

    while ( vertices )
    {
        v = vertices;
        DEL_QUEUE ( vertices, v );
    }

    while ( faces ) {
        f = faces;
        DEL_QUEUE ( faces, f );
    }
}
/*-------------------------------------------------------------------------
 Clean_up goes through each data structure list and clears all
 flags and NULLs out some pointers.  In the vertex list,  the active
 flag is set to 0 and the duplicate pointer is set to NULL. In the
 edge list any struture that is marked deleted gets deleted from the
 edge list, and in the vertex list any face that is marked visible gets
 deleted from the face list.
 ------------------------------------------------------------------------*/
void clean_up()
{
    clean_edges();
    clean_faces();
    clean_vertices();

}

/*------------------------------------------------------------------------
 Clean_edges runs through the edge list and cleans up the structure.
 If there is a face in the edges adjacent face[2] then it will put
 that face in place of the visible face and set adjacent face[2] to
 NULL.  It will also delete any edge marked deleted.
 -----------------------------------------------------------------------*/
void clean_edges()
{
    register struct tedge *e, *tmp_edge;

    /* replace the visible face with the new face */
    e = edges;
    if ( edges )
        do
    {
        if ( e->adjface[2] ) {
            if ( e->adjface[0]->visible )
                e->adjface[0] = e->adjface[2];
            else e->adjface[1] = e->adjface[2];
            e->adjface[2] = NULL;
        }
        e = e->next;
    } while ( e != edges );

    /* delete any edges marked deleted */
    while ( edges && edges->deleted ) {
        e = edges;
        DEL_QUEUE ( edges, e );
    }
    if ( edges )  {
        e = edges->next;
        do {
            if ( e->deleted ) {
                tmp_edge = e;
                e = e->next;
                DEL_QUEUE( edges, tmp_edge );
            }
            else e = e->next;
        } while ( e != edges );

    }

}

/*------------------------------------------------------------------------
 Clean_faces runs through the face list and deletes any face
 marked visible.
 -----------------------------------------------------------------------*/
void clean_faces()
{
    register struct tface *f, *tmp_face;

    while ( faces && faces->visible )
    {
        f = faces;
        DEL_QUEUE( faces, f );
    }
    if ( faces )
    {
        f = faces->next;
        do
        {
            if ( f->visible )
            {
                tmp_face = f;
                f = f->next;
                DEL_QUEUE( faces, tmp_face );
            }
            else f = f->next;
        } while ( f != faces );
    }

}

/*-------------------------------------------------------------------------
 Clean_vertices runs through the vertex list and deletes the
 vertices that are marked as used but are not incident to any undeleted
 edges. It will also set the duplicate pointer to NULL and the active
 flag to not active.
 -------------------------------------------------------------------------*/
void clean_vertices()
{
    struct tedge *e;
    struct tvertex *v, *tmp_vert;

    /* mark all vertices that are incident */
    /* to an edge as active */
    e = edges;
    if ( edges )
        do {
            e->endpts[0]->active = e->endpts[1]->active = ACTIVE;
            e = e->next;
        } while (e != edges);

    /* delete all vertices that are */
    /* marked but not active */
    while ( vertices && vertices->mark && !vertices->active )
    {
        v = vertices;
        DEL_QUEUE( vertices, v );
    }

    v = vertices;
    if ( vertices )
    {
        do {
            if ( v->mark && !v->active )
            {
                tmp_vert = v;
                v = v->next;
                DEL_QUEUE( vertices, tmp_vert );
            }
            else v = v->next;
        } while ( v != vertices);
    }

    /* reset active flag and duplicate pointer */
    v = vertices;
    if ( vertices )
        do {
            v->duplicate = NULL;
            v->active = !ACTIVE;
            v = v->next;
        } while ( v != vertices );

}
/*=====================================================================
 __file: Checks.c   This file is used whenever the test flag is set.
 These checks are done after the hull has been made.
 =====================================================================*/

/*------------------------------------------------------------------------
 Consistency runs through the edge list and checks that all
 adjacent faces have their endpoints in opposite order.  This verifies
 that the vertices are in counter clockwise order.
 -----------------------------------------------------------------------*/
void consistency()
{
    register struct tedge *e;
    register int i, j;

    e = edges;

    do {
        /* find index of endpoint[0] in adjacent face[0] */
        for ( i = 0; e->adjface[0]->vert[i] != e->endpts[0]; ++i )
            ;

        /* find index of endpoint[0] in adjacent face[1] */
        for ( j = 0; e->adjface[1]->vert[j] != e->endpts[0]; ++j )
            ;

        /* check if the endpoints occur in opposite order */
        if ( !( e->adjface[0]->vert[ (i+1) % 3 ] ==
               e->adjface[1]->vert[ (j+2) % 3 ] ||
               e->adjface[0]->vert[ (i+2) % 3 ] ==
               e->adjface[1]->vert[ (j+1) % 3 ] )  )
            break;
        e = e->next;

    } while ( e != edges );

    if ( e != edges )
        fprintf( stderr, "Edges are NOT consistent.\n");

}

/*----------------------------------------------------------------------
 Convexity checks that the volume between every face and every
 point is negative.  This shows that each point is inside every face
 and therefore the hull is convex.
 ---------------------------------------------------------------------*/
void convexity()
{
    register struct tface *f;
    register struct tvertex *v;
    int vol;

    f = faces;

    do {
        v = vertices;
        do {
            volume( vol, f, v );
            if ( vol < 0 )
                break;
            v = v->next;
        } while ( v != vertices );
        if ( v != vertices )
            break;
    } while ( f != faces );

    if ( f != faces )
        fprintf( stderr, "It is NOT convex.\n");

}

/*----------------------------------------------------------------------
 Check_faces checks that the number of faces is correct according
 to the number of vertices that are in the hull.
 ---------------------------------------------------------------------*/
void check_faces(int cvertices, int cfaces )
{
    if ( cfaces != 2 * cvertices - 4 )
        fprintf( stderr, "The number of faces is NOT correct\n");

}

/*----------------------------------------------------------------------
      Check_edges checks that the number of edges is correct according
  to the number of  vertices that are in the hull.
  ---------------------------------------------------------------------*/
void check_edges( int cvertices, int cedges )
{
    if ( 2 * cedges != 3 * cvertices )
        fprintf( stderr, "The number of edges is NOT correct.\n");
}
/*-----------------------------------------------------------------------*/
void checks()
{
    consistency();
    convexity();
}
