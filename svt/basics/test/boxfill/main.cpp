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

#define BOXFILL cout << svt_trimName("floodfill")

int main(int argc, char* argv[])
{
    if (argc < 7)
    {
	cout << "boxfill <situs file> <x min index> <x max index> <y min index> <y max index> <z min index> <z max index> <fill value> <output situs filename>" << endl;
	exit(1);
    }

    // load situs map
    svt_volume<Real64> oVol;
    svt_matrix4<Real64> oVolMat = oVol.loadSitus( argv[1] );

    unsigned int iMinX = atoi( argv[2] );
    unsigned int iMaxX = atoi( argv[3] );
    unsigned int iMinY = atoi( argv[4] );
    unsigned int iMaxY = atoi( argv[5] );
    unsigned int iMinZ = atoi( argv[6] );
    unsigned int iMaxZ = atoi( argv[7] );
    Real64 fValue = atof( argv[8] );

    // boxfill
    BOXFILL << "Boxfill:" << endl;
    BOXFILL << "   index range: x min: " << iMinX << " - x max: " << iMaxX << endl;
    BOXFILL << "                y min: " << iMinY << " - y max: " << iMaxY << endl;
    BOXFILL << "                z min: " << iMinZ << " - z max: " << iMaxZ << endl;
    BOXFILL << "   fill with: " << fValue << endl;

    for(unsigned int iZ=iMinZ; iZ<=iMaxZ; iZ++)
	for(unsigned int iY=iMinY; iY<=iMaxY; iY++)
	    for(unsigned int iX=iMinX; iX<=iMaxX; iX++)
		oVol.setValue( iX, iY, iZ, fValue );

    // save result
    BOXFILL << "   save result to " << argv[9] << endl;
    oVol.saveSitus( argv[9] );
};
