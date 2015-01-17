/*********************************************************************
*                           s i t u s . h                            *
**********************************************************************
* Header file for the Situs package    (c) Willy Wriggers, 1998-2000 *
* URL: http://www.scripps.edu/~wriggers/situs/                       *
**********************************************************************
* See legal statement for terms of distribution                      *
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
//#include <strings.h>
#include <math.h>
#include <float.h>

#ifndef SITUS_H
#define SITUS_H

/********************************************************************/
/**** Warning: changes in the following affect the performance ******/
/**** of the programs. See documentation for more information. ******/
/********************************************************************/

#define NNMIN 3       /* minimum possible # of codebook vectors     */
#define NNMAX 20      /* maximum possible # of codebook vectors     */
#define MAXCYCLE 8    /* max # of cycles in cluster analysis        */
#define SMAXS 100000  /* # of neural gas iteration steps            */      
#define MAXPDB 100000 /* maximum number of lines in pdb file        */
#define BINS 50       /* number of voxel histogram bins             */
#define BARL 70       /* available space for histogram bars         */
#define MYFLT double  /* float type used: 'float' or 'double'       */

/********************************************************************/
/***** Warning: changes below this line are not recommended! ********/
/********************************************************************/

#define MAXDT (NNMAX*MAXCYCLE) 
#define PI 3.1415926535
#define	TRUE 1
#define	FALSE 0

typedef struct pdbRec
{
/* a component of the DOWSER program
 * a product of UNC Computational Structural Biology group
 * authors: Li Zhang, Xinfu Xia, Jan Hermans, Univ. of North Carolina */

  char  recdName[7];    /*       1 -  6 */
  int   serial;         /*       7 - 11 */
  char  atomType[3];
  char  atomLoc[3];
  char  altLoc[2];      /*           17 */
  char  resName[5];     /*      18 - 21 */
  char  chainID[2];     /*           22 */
  int   resSeq;         /*      23 - 26 */
  char  iCode[2];       /*           27 */
  float x;              /*      31 - 38 */
  float y;              /*      39 - 46 */
  float z;              /*      47 - 54 */
  float occupancy;      /*      55 - 60 */
  float tempFactor;     /*      61 - 66 */
  int   ftNote;         /*      68 - 70 */
  char  segID[5];       /*      73 - 76 */
  char  element[3];     /*      77 - 78 */
  char  charge[3];      /*      79 - 80 */
} PDB;

typedef MYFLT Rmat3 [3][3];
typedef int Iseq3 [3];
typedef MYFLT Rseq3 [3];
typedef Rseq3 Rmat3NN [NNMAX];
typedef Rseq3 Rmat3DT [MAXDT];
typedef int IseqNN [NNMAX];
typedef MYFLT RseqNN [NNMAX];
typedef int ImatNNDT [NNMAX][MAXDT];

#endif








