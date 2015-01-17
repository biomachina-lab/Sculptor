/***************************************************************************
                          main
                          ----
    begin                : 09/09/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_volume.h>

#define FLOOD cout << svt_trimName("floodfill")

int main(int argc, char* argv[])
{
    if (argc < 7)
    {
	cout << "floodfill <situs file> <x index of start voxel> <y index> <z index> <threshold> <sigma of gaussian> <output situs filename>" << endl;
	exit(1);
    }

    // load situs map
    svt_volume<Real64> oVol;
    svt_matrix4<Real64> oVolMat = oVol.loadSitus( argv[1] );

    unsigned int iX = atoi( argv[2] );
    unsigned int iY = atoi( argv[3] );
    unsigned int iZ = atoi( argv[4] );
    Real64 fThreshold = atof( argv[5] );
    Real64 fGaussian = atof( argv[6] );

    // floodfill
    FLOOD << "Floodfill:" << endl;
    FLOOD << "   start voxel: (" << iX << ", " << iY << ", " << iZ << ")" << endl;
    FLOOD << "   threshold: " << fThreshold << endl;

    oVol.floodfill( iX, iY, iZ, fThreshold, fGaussian );

    // save result
    FLOOD << "   save result to " << argv[7] << endl;
    oVol.saveSitus( argv[7] );
};
