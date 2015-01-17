/***************************************************************************
                          correlation
                          -----------
    begin                : 02/27/2010
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_system.h>
#include <svt_point_cloud_pdb.h>

#define CORR cout << svt_trimName("correlation")

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 2)
    {
	cout << "usage: correlation <input pdb file> [options]" << endl;
        cout << "options:" << endl;
        cout << " -i number of tests (default: 10)" << endl;
	cout << " -r <resolution> target resolution in Angstroem (default: 10)" << endl;
	cout << " -w <width> voxel width in angstroem (default: 3)" << endl;
	cout << " -t <1..5> correlation type (default: 1) " << endl;
	cout << "    1: Gaussian blur, full correlation" << endl;
	cout << "    2: One-step, with blur" << endl;
	cout << "    3: One-step, only project mass" << endl;

	return 1;
    }

    // parameters
    int iMaxTests = 10;
    Real64 fResolution = 10.0;
    Real64 fWidth = 3.0;
    int iType = 1;
    bool bCA = false;

    // parse other options
    for(int i=2; i<argc; i++)
    {
	if (argv[i][0] == '-' && argv[i][1] != 0)
	    switch(argv[i][1])
	    {
		// number of tests
	    case 'i':
		if (argc >= i+1)
		    iMaxTests = atoi(argv[i+1]);
		break;

                // target resolution
	    case 'r':
		if (argc >= i+1)
		    fResolution = atof(argv[i+1]);
		break;

                // voxel width
	    case 'w':
		if (argc >= i+1)
		    fWidth = atof(argv[i+1]);
		break;

                // test type
	    case 't':
		if (argc >= i+1)
		    iType = atoi(argv[i+1]);

		if (iType < 1 || iType > 3)
		{
		    CORR << "Test type not recognized: " << iType << endl;
		    exit(1);
		}
		break;

	    default:
		CORR << "Option not recognized: " << argv[i] << endl;
		exit(1);
                break;
	    }
    }
    fResolution *= -1.0;

    // load pdb file
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );

    // create volume
    svt_volume<Real64>* pVol = oPDB.blur( fWidth, fResolution );
    Real64 fCorr = 0.0;

    long iTime = svt_getToD();

    // create kernel if necessary
    svt_volume<Real64>* pKernel;
    if (iType == 2)
    {
        pKernel = new svt_volume<Real64>;
        pKernel->createSitusBlurringKernel( fWidth, fResolution);
    }
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_Trans;
    svt_volume<Real64>* pVol_Trans;

    // now transform the molecule randomly a bit and compute the correlation
    for(unsigned int iCount=0; iCount < iMaxTests; iCount++)
    {
        svt_matrix4<Real64> oMat;
        oMat.rotate( 0, deg2rad(svt_genrand()*10) );
        oMat.rotate( 1, deg2rad(svt_genrand()*10) );
        oMat.rotate( 2, deg2rad(svt_genrand()*10) );
        oMat.translate( svt_genrand(), svt_genrand(), svt_genrand() );

        switch( iType )
        {
            // full correlation
        case 1:
            oPDB_Trans = oMat * oPDB;
            pVol_Trans = oPDB_Trans.blur( fWidth, fResolution );
            fCorr = pVol->correlation( *pVol_Trans, false );
            CORR << "Full Correlation: " << fCorr << endl;
            delete pVol_Trans;
            break;

            // blurCorr
        case 2:
            fCorr = oPDB.blurCorr( pKernel, pVol, oMat, bCA);
            CORR << "BlurCorr Correlation: " << fCorr << endl;
            break;

            // projectCorr
        case 3:
            fCorr = oPDB.projectMassCorr( pVol, oMat, bCA);
            CORR << "ProjectCorr Correlation: " << fCorr << endl;
            break;
        };
    };

    iTime = (svt_getToD() - iTime) / (Real64)(iMaxTests);

    CORR << "Average Time: " << iTime << endl;
}
