/***************************************************************************
                          rawivVIP
                          --------
    begin                : 01/13/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// THIS FILE IS PART OF THE SVT/SENSITUS VOL I/O PLUGIN SDK

// summary of a volumetric dataset
typedef struct
{
    int iExtX; // number of voxels along x
    int iExtY; // number of voxels along y
    int iExtZ; // number of voxels along z

    float fSpanX; // size of voxel in real space (in Angstroem!)
    float fSpanY; // size of voxel in real space (in Angstroem!)
    float fSpanZ; // size of voxel in real space (in Angstroem!)

    float* pData; // pointer to data in memory

} svt_vol;
