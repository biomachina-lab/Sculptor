/***************************************************************************
                          crop (test program)
                          ---------------------
    begin                : 05/24/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_system.h>
#include <svt_iostream.h>

#define CROP cout << svt_trimName("crop_test")

int main(int argc, char* argv[])
{
    /*if (argc < 7)
    {
	cout << "usage: crop <input situs file> <new min x> <new min y> <new min z> <new max x> <new max y> <new max z> <output file>" << endl;
	return 0;
    }

    CROP << "Loading volume file " << argv[1] << endl;
    svt_volume<Real64> oVol;
    oVol.load( argv[1] );

    unsigned int iMinX = atoi(argv[2]);
    unsigned int iMinY = atoi(argv[3]);
    unsigned int iMinZ = atoi(argv[4]);
    unsigned int iMaxX = atoi(argv[5]);
    unsigned int iMaxY = atoi(argv[6]);
    unsigned int iMaxZ = atoi(argv[7]);

    oVol.crop( iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ );

    oVol.saveSitus( argv[8] );

    return 0;
    */

    /*if (argc < 7)
    {
	cout << "usage: crop <input situs file> <center point x> <center point y> <center point z> <radius> <output file>" << endl;
	return 0;
    }

    CROP << "Loading situs file " << argv[1] << endl;
    svt_volume<Real64> oVol;
    oVol.loadSitus( argv[1] );

    // crop
    CROP << "Cutting out sphere..." << endl;
    svt_volume<Real64> oVolCrop;
    oVolCrop = oVol.cutSphere( atof( argv[2] ), atof( argv[3] ),atof( argv[4] ), atof( argv[5] ) );

    // save
    CROP << "Saving file to " << argv[6] << endl;
    oVolCrop.saveSitus( argv[6] );
    */
    
    if (argc < 7)
    {
	cout << "usage: crop <input situs file> <center point x> <center point y> <center point z> <radius> <output file>" << endl;
	return 0;
    }

    CROP << "Loading situs file " << argv[1] << endl;
    svt_volume<Real64> oVol;
    oVol.loadSitus( argv[1] );

    // crop
    CROP << "Cutting out sphere..." << endl;
    svt_volume<Real64> oVolCrop;
    oVolCrop = oVol.copySphere( atof( argv[2] ), atof( argv[3] ),atof( argv[4] ), atof( argv[5] ) );

    // save
    CROP << "Saving file to " << argv[6] << endl;
    oVolCrop.saveSitus( argv[6] );


    return 0;

};
