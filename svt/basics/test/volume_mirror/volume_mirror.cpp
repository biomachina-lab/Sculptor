/***************************************************************************
                          volume_mirror (test program)
                          ---------------------
    begin                : 05/24/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_system.h>
#include <svt_iostream.h>

#define MIRROR cout << svt_trimName("volume_mirror")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: volume_mirror <input situs file> <output situs file>" << endl;
	return 0;
    }

    MIRROR << "Loading situs file " << argv[1] << endl;
    svt_volume<Real64> oVol;
    oVol.loadSitus( argv[1] );

    unsigned int iX, iY, iZ;

    // crop
    MIRROR << "Create doubled volume..." << endl;
    svt_volume<Real64> oVolMirror( oVol.getSizeX(), oVol.getSizeY(), oVol.getSizeZ() * 2);

    for(iX=0; iX<oVol.getSizeX(); iX++)
	for(iY=0; iY<oVol.getSizeY(); iY++)
	    for(iZ=0; iZ<oVol.getSizeZ(); iZ++)
                oVolMirror.setAt( iX, iY, iZ, oVol.at( iX, iY, iZ ) );

    for(iX=0; iX<oVol.getSizeX(); iX++)
	for(iY=0; iY<oVol.getSizeY(); iY++)
	    for(iZ=0; iZ<oVol.getSizeZ(); iZ++)
                oVolMirror.setAt( iX, iY, iZ + oVol.getSizeZ(), oVol.at( oVol.getSizeX() - iX - 1, iY, oVol.getSizeZ() - iZ - 1 ) );

    // save
    MIRROR << "Saving file to " << argv[2] << endl;
    oVolMirror.saveSitus( argv[2] );

    return 0;
};
