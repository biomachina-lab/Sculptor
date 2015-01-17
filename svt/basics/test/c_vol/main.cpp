/***************************************************************************
                          main
                          ----
    begin                : 02/28/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_point_cloud_pdb.h>
#include <svt_clustering_trn.h>
#include <svt_clustering_irank.h>
#include <svt_clustering_gng.h>
#include <svt_vector4.h>

#define C_VOL cout << svt_trimName("c_vol")

int main(int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////
    // Parsing options
    ///////////////////////////////////////////////////////////////////////////

    if (argc < 3)
    {
	cout << "usage: c_vol [options] <situs file> <output pdb file> <number of clusters>" << endl;
        cout << endl;
	cout << "options:" << endl;
	cout << " -li <lambda_i value>    (default: 0.2)" << endl;
	cout << " -lf <lambda_f value>    (default: 0.02)" << endl;
	cout << " -ei <epsilon_i value>   (default: 0.1)" << endl;
	cout << " -ef <epsilon_f value>   (default: 0.01)" << endl;
	cout << " -s  <number of steps>   (default: 100000)" << endl;
	cout << " -r  <number of runs>    (default: 8)" << endl;
	cout << " -c  <cutoff>            (default: 0)" << endl;
        cout << " -t  <topology filename> (psf format)" << endl;
        cout << endl;
	cout << " -f  <precision>         (default: 0.01)" << endl;
	cout << "     perform feature vector fluctuation test" << endl;
        return 0;
    }

    char* pVOL_Fname = NULL;
    char* pOUT_Fname = NULL;
    char* pTOP_Fname = NULL;
    Real64 fLi = 0.2;
    Real64 fLf = 0.02;
    Real64 fEi = 0.1;
    Real64 fEf = 0.01;
    unsigned int iMaxstep = 100000;
    unsigned int iCluster = 20;
    unsigned int iRuns = 8;
    bool bIgnoreWater = false;
    Real64 fCutoff = 0.0;
    bool bFluctTest = false;
    Real64 fFluctPrec = 0.01;

    // parse command-line
    for(int i=1; i<argc; i++)
    {
        // is this an option or a filename
	if (argv[i][0] == '-' && argv[i][1] != 0)
	{
	    switch(argv[i][1])
	    {
                // Lambda parameters
	    case 'l':
		if (argc >= i+1)
		{
		    if (argv[i][1] == 'i')
		    {
			fLi = atof(argv[i+1]);
			C_VOL << "Lambda_i: " << fLi << endl;
		    }
		    if (argv[i][1] == 'f')
		    {
			fLf = atof(argv[i+1]);
			C_VOL << "Lambda_f: " << fLf << endl;
		    }

		    i++;
		}
		break;

		// Epsilon parameters
	    case 'e':
		if (argc >= i+1)
		{
		    if (argv[i][1] == 'i')
		    {
			fEi = atof(argv[i+1]);
			C_VOL << "Epsilon_i: " << fEi << endl;
		    }
		    if (argv[i][1] == 'f')
		    {
			fEf = atof(argv[i+1]);
			C_VOL << "Epsilon_f: " << fEf << endl;
		    }

		    i++;
		}
		break;

		// Runs
	    case 'r':
		if (argc >= i+1)
		{
		    iRuns = atoi( argv[i+1] );
		    C_VOL << "Runs: " << iRuns << endl;

                    i++;
		}
		break;

		// Topology
	    case 't':
		if (argc >= i+1)
		{
		    pTOP_Fname = argv[i+1];
		    C_VOL << "Topology filename: " << pTOP_Fname << endl;

                    i++;
		}
		break;

		// Cutoff
	    case 'c':
		if (argc >= i+1)
		{
		    fCutoff = atof( argv[i+1] );
		    C_VOL << "Cutoff: " << fCutoff << endl;

                    i++;
		}
		break;

		// Steps
	    case 's':
		if (argc >= i+1)
		{
		    iMaxstep = atoi( argv[i+1] );
		    C_VOL << "Steps: " << iMaxstep << endl;

                    i++;
		}
                break;

		// Feature vector fluctuation test
	    case 'f':
		bFluctTest = true;
		if (argc >= i+1)
		{
		    fFluctPrec = atof( argv[i+1] );
		    if (fFluctPrec > 0.0)
		    {
			C_VOL << "Fluctation test with a precision: " << fFluctPrec << endl;
			i++;
		    } else
                        fFluctPrec = 0.01;
		}
                break;

		// Ignore Water
	    case 'w':
		bIgnoreWater = true;
                break;
	    }

	} else {

	    if (pVOL_Fname == NULL)
	    {
		pVOL_Fname = argv[i];

		C_VOL << "Input-file : " << pVOL_Fname << endl;

	    } else if (pOUT_Fname == NULL)
	    {
		pOUT_Fname = argv[i];

		C_VOL << "Output-file: " << pOUT_Fname << endl;

	    } else {

		iCluster = atoi(argv[i]);

		C_VOL << "Cluster-centers: " << iCluster << endl;
	    }
	}
    }

    ///////////////////////////////////////////////////////////////////////////
    // Clustering
    ///////////////////////////////////////////////////////////////////////////

    // load volume
    svt_volume< Real64 > oVol;
    C_VOL << "Loading situs file " << pVOL_Fname << endl;
    oVol.loadSitus( pVOL_Fname );

    // some stats about the volume
    C_VOL << "Min density: " << oVol.getMinDensity() << endl;
    C_VOL << "Max density: " << oVol.getMaxDensity() << endl;

    // set parameters
    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setLambda( fLi, fLf );
    oClusterAlgo.setEpsilon( fEi, fEf );
    oClusterAlgo.setMaxstep( iMaxstep );
    oClusterAlgo.setConnOnline( false );
    svt_sgenrand( svt_getToD() );
    oVol.setCutoff( fCutoff );

    // cluster
    if (bFluctTest)
    {
	vector< svt_point_cloud_pdb< svt_vector4<Real64> > > aCV;
	for(unsigned int i=0; i<20; i++ )
	{
	    C_VOL << "Fluctuation test run: " << i << endl;
	    aCV.push_back( oClusterAlgo.cluster(iCluster, iRuns, oVol) );
	}

        Real64 fAccRMSD = 0.0;
	Real64 fRMSD = 0.0;
	Real64 fCount = 0.0;
	Real64 fOldRmsd = 1.0E10;
	unsigned int iOldRun_A = 0;
        unsigned int iOldRun_B = 0;

	while(fCount < 100000.0 && fabs(fOldRmsd - fRMSD) > fFluctPrec )
	{
	    fOldRmsd = fRMSD;

	    unsigned int iRun_A = (unsigned int)(svt_genrand()*20.0);
	    unsigned int iRun_B = (unsigned int)(svt_genrand()*20.0);

	    while(iRun_B == iRun_A || (iOldRun_A == iRun_A && iOldRun_B == iRun_B))
	    {
		iRun_A = (unsigned int)(svt_genrand()*20.0);
		iRun_B = (unsigned int)(svt_genrand()*20.0);
	    }

	    iOldRun_A = iRun_A;
            iOldRun_B = iRun_B;

	    fAccRMSD += aCV[iRun_A].rmsd_NN( aCV[iRun_B] );
            fCount += 1.0;
	    fRMSD = fAccRMSD / fCount;

	    C_VOL << "Fluctuation: " << fRMSD << endl;

	}

	C_VOL << "Average fluctuation: " << fRMSD << endl;

	return 0;
    }

    svt_point_cloud_pdb< svt_vector4<Real64> > oCV;
    oCV = oClusterAlgo.cluster(iCluster, iRuns, oVol);

    // save output
    C_VOL << "Saving cluster center pdb file " << pOUT_Fname << endl;
    oCV.writePDB( pOUT_Fname );

    return 0;
};
