/***************************************************************************
                          volume_rotate (test program)
                          ---------------------
    begin                : 04/19/2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_system.h>
#include <svt_iostream.h>

#define ROTATE cout << svt_trimName("volume_rotate")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: volume_rotate <input situs file> <output situs file>" << endl;
	return 0;
    }

    svt_volume<Real64> oVol;
    oVol.loadSitus( argv[1] );

    svt_matrix4<Real64> oMat;
    oMat.loadIdentity();
    
    svt_vector4<Real64> oVec;
    oVec.x( 6.0 );
    oVec.y( 3.0 );
    oVec.z( 9.0 );
    oMat.translate( oVec );

    oMat.rotatePTP( deg2rad(265.0), deg2rad(13.0), deg2rad(23.0));

    // crop
    ROTATE << "Create rotate volume..." << endl;
    svt_volume<Real64> oVolRotate;
    oVolRotate = oVol*oMat;

    // save
    ROTATE << "Saving file to " << argv[2] << endl;
    oVolRotate.saveSitus( argv[2] );

    return 0;
};
