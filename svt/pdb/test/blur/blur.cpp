/***************************************************************************
                          blur
                          ----
    begin                : 05/27/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_pdb.h>

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 3)
    {
	cout << "usage: blur <input pdb file> <output situs file> [options]" << endl;
	cout << "options:" << endl;
	cout << " -r <resolution> target resolution in Angstroem (default: 15)" << endl;
	cout << " -m atoms are NOT mass weighted (default: mass weighting on)" << endl;
	cout << " -c <cutoff> b-factor cutoff. Atoms with a higher b-factor are not used (default: 1000)" << endl;
	cout << " -w <width> voxel width in angstroem (default: 3)" << endl;
	cout << " -e do not exclude water molecules (default: excluded)" << endl;
	cout << " -t <1..5> kernel type (default: 1) " << endl;
	cout << "    1: Gaussian, exp(-1.5 r^2 / sigma^2)" << endl;
	cout << "    2: Triangular, max(0, 1 - 0.5 |r| / r-half)" << endl;
	cout << "    3: Semi-Epanechnikov, max(0, 1 - 0.5 |r|^1.5 / r-half^1.5)" << endl;
	cout << "    4: Epanechnikov, max(0, 1 - 0.5 r^2 / r-half^2)" << endl;
	cout << "    5: Hard Sphere, max(0, 1 - 0.5 r^60 / r-half^60)" << endl;
	cout << " -a <kernel amplitude> (default: 1.0)" << endl;
	cout << " -n no kernel amplitude correction (default: correction on)" << endl;

	return 1;
    }

    bool bWater = true;
    Real64 fResolution = 15.0;
    bool bMassWeight = true;
    Real64 fBFactCutoff = 1000.0;
    Real64 fWidth = 3.0;
    int iKernelType = 1;
    Real64 fKernelAmpl = 1.0;
    bool bAmplCorr = true;

    // the rest of the arguments are only optional
    for(int i=3; i<argc; i++)
    {
	if (argv[i][0] == '-' && argv[i][1] != 0)
	    switch(argv[i][1])
	    {
		// target resolution
	    case 'r':
		if (argc >= i+1)
		    fResolution = atof(argv[i+1]);
		break;

		// no mass-weighting
	    case 'm':
		bMassWeight = false;
		break;

		// b-factor cutoff
	    case 'c':
		if (argc >= i+1)
		    fBFactCutoff = atof(argv[i+1]);
		break;

		// voxel width
	    case 'w':
		if (argc >= i+1)
		    fWidth = atof(argv[i+1]);
		break;

		// do not exclude water molecules
	    case 'e':
		bWater = false;
		break;

		// kernel type
	    case 't':
		if (argc >= i+1)
		    iKernelType = atoi(argv[i+1]);

		if (iKernelType < 1 || iKernelType > 5)
		{
		    cout << "blur> Kernel type not recognized: " << iKernelType << endl;
		    exit(1);
		}
		break;

		// kernel amplitude
	    case 'a':
		if (argc >= i+1)
		    fKernelAmpl = atof(argv[i+1]);
		break;

		// amplitude correction off
	    case 'n':
		bAmplCorr = false;
		break;

	    default:
		cout << "blur> Option not recognized: " << argv[i] << endl;
		exit(1);
                break;
	    }
    }

    fResolution *= -1.0;

    // load pdb file
    svt_pdb oPDB( argv[1], SVT_PDB, NULL, SVT_NONE );
    // convolute it with blurring kernel
    svt_volume<Real64>* pVol = oPDB.blur( bWater, bMassWeight, fBFactCutoff, fWidth, fResolution, iKernelType, fKernelAmpl, bAmplCorr );
    // save resulting situs map
    pVol->saveSitus( argv[2] );
}
