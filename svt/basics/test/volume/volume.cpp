/***************************************************************************
                          volume (test program)
                          ---------------------
    begin                : 02/02/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_system.h>
#include <svt_iostream.h>

#define VOL cout << svt_trimName("volume_test")

int main()
{
    // create new volume
    VOL << "Creating test volume A (16x16x16, 10.0)" << endl;
    svt_volume<Real32> oVolA( 16, 16, 16, 10.0 );
    VOL << "Content of first voxel of A: " << oVolA.getValue(0,0,0) << endl;

    // create id kernel
    VOL << "Convolving with identity kernel" << endl;
    svt_volume<Real32> oId;
    oId.createIdentity();
    oId.print();
    oVolA.convolve(oId, true);
    VOL << "Content of first voxel of A: " << oVolA.getValue(0,0,0) << endl;

    // copy constructor
    VOL << "Creating an empty test volume B" << endl;
    VOL << "Copy A to B" << endl;
    svt_volume<Real32> oVolB( oVolA );

    // change some voxel values
    VOL << "Change voxels in B to 0" << endl;
    oVolB.setValue( 0.0 );
    VOL << "Content of first voxel of A: " << oVolA.getValue(0,0,0) << endl;
    VOL << "Content of first voxel of B: " << oVolB.getValue(0,0,0) << endl;

    // prep gradient calc
    VOL << "Change second voxel in B to 1" << endl;
    oVolB.setValue( 1,0,0, 1.0 );
    VOL << "Content of second voxel of B: " << oVolB.getValue(1,0,0) << endl;

    // test gradient calc
    VOL << "Calculate gradient (CentralDistance) of B" << endl;
    svt_volume<Real32> oGradX, oGradY, oGradZ;
    oVolB.calcGradient( svt_volume<Real32>::CentralDistance, oGradX, oGradY, oGradZ );

    // test assign operator
    VOL << "Assign oGradX to oGradY" << endl;
    oGradY = oGradX;

    // test deep copy operator
    VOL << "Deep copy of oGradZ to oGradY" << endl;
    oGradY.deepCopy( oGradZ );

    // test laplacian
    VOL << "Apply Laplacian to A" << endl;
    oVolA.applyLaplacian();

    // creating new volume
    svt_volume<Real32> oCube(16,16,16,0.0);
    for(unsigned int iX=4; iX<12; iX++)
	for(unsigned int iY=4; iY<12; iY++)
	    for(unsigned int iZ=4; iZ<12; iZ++)
		oCube.setValue(iX, iY, iZ, 1.0);

    oCube.saveSitus("cube.situs");

    return 0;
};
