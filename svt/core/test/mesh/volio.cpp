/*********************************************************************
*                             V O L I O                              *
**********************************************************************
* Program is part of the Situs package (c) Willy Wriggers, 1998-2000 *
* URL: http://www.scripps.edu/~wriggers/situs/                       *
**********************************************************************
*                                                                    * 
* Auxiliary program to read and write EM maps in Situs format        *
*                                                                    *
**********************************************************************
* See legal statement for terms of distribution                      *
*********************************************************************/

#include "situs.h"            

// returns true, if data is laplace filtered
bool readvol(char *vol_file, MYFLT *width, MYFLT *gridx, MYFLT *gridy, MYFLT *gridz, int *extx, int *exty, int *extz, MYFLT **phi)

{ unsigned long nvox, count;
  double dgridx, dgridy, dgridz, dwidth;
  double phitest, dtemp;
  FILE *fin;
  bool lap = false;
   
  fin = fopen(vol_file, "r");
  if( fin == NULL ) {
    fprintf(stderr, "volio> Error: Can't open file! %s  [e.c. 22010]\n", vol_file); 
    exit(22010);   
  }

  /* read header and print information */
  fscanf(fin, "%le %le %le %le %d %d %d", &dwidth, &dgridx, &dgridy, &dgridz, extx, exty, extz);
  *width = dwidth; *gridx = dgridx; *gridy = dgridy; *gridz = dgridz;
  printf ("volio> File %s - Header information: \n", vol_file);
  printf ("volio> Columns, rows, and sections: x=1-%d, y=1-%d, z=1-%d\n",*extx,*exty,*extz);
  printf ("volio> 3D coordinates of first voxel (1,1,1): (%f,%f,%f)\n",*gridx,*gridy,*gridz);  
  printf ("volio> Voxel size in Angstrom: %f \n", *width);
  nvox = *extx * *exty * *extz;

  /* allocate memory and read data */
  printf ("volio> Reading density data... \n");
  *phi = (MYFLT *) malloc(nvox * sizeof(MYFLT));
  if (*phi == NULL) {
    fprintf(stderr, "volio> Error: Could not satisfy memory allocation request [e.c. 22110]\n"); 
    exit(22110);
  }

  for (count=0;count<nvox;count++) {
    if (fscanf(fin,"%le", &dtemp) != 1) {
        fprintf(stderr, "volio> Error: file %s is too short (not compatible with header information) or data unreadable [e.c. 22210]\n", vol_file); 
        exit(22210);
    } else {
        if (dtemp < 0) {
            lap = true;
        }
        *(*phi+count) = dtemp;
    }
  }
  if (fscanf(fin,"%le", &phitest) != EOF) {
    fprintf(stderr, "volio> Error: file %s is too long, not compatible with header information [e.c. 22220]\n", vol_file); 
    //exit(22220);
  }
  fclose(fin);  
  if (lap)
      printf ("volio> Volumetric data is Laplace-filtered.\n", vol_file); 
  printf ("volio> Volumetric data read from file %s\n", vol_file); 
  return lap;
}


void writevol(char *vol_file, MYFLT width, MYFLT gridx, MYFLT gridy, MYFLT gridz, int extx, int exty, int extz, MYFLT *phi)

{ unsigned long nvox, count;
  FILE *fout;
   
  nvox = extx * exty * extz;
  fout = fopen(vol_file, "w");
  if( fout == NULL ) {
    fprintf(stderr, "volio> Error: Can't open file! %s  [e.c. 22310]\n", vol_file); 
    exit(22310);   
  }

  printf ("volio> Writing density data... \n");
  fprintf(fout, "  %f %f %f %f %d %d %d\n", width, gridx, gridy, gridz, extx, exty, extz);
  fprintf(fout, "\n");

  for(count=0;count<nvox;count++) {
    if ((count+1)%10 == 0) fprintf (fout," %10.6f \n",*(phi+count));
    else fprintf (fout," %10.6f ",*(phi+count)); 
  }    
  fclose(fout);  
  printf ("volio> Volumetric data written to file %s \n", vol_file);

  /* header information */
  printf ("volio> File %s - Header information: \n", vol_file);
  printf ("volio> Columns, rows, and sections: x=1-%d, y=1-%d, z=1-%d\n",extx,exty,extz);
  printf ("volio> 3D coordinates of first voxel (1,1,1): (%f,%f,%f)\n",gridx,gridy,gridz);
  printf ("volio> Voxel size in Angstrom: %f \n", width);
  return;
}

