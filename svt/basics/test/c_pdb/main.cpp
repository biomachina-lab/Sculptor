/***************************************************************************
                          main
                          ----
    begin                : 02/28/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_point_cloud_pdb.h>
#include <svt_clustering_trn.h>
#include <svt_clustering_gng.h>
#include <svt_vector4.h>

#define C_PDB cout << svt_trimName("c_pdb")

int main(int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////
    // Parsing options
    ///////////////////////////////////////////////////////////////////////////

    if (argc < 3)
    {
	cout << "usage: c_pdb [options] <pdb file> <output pdb file> <number of clusters>" << endl;
        cout << endl;
	cout << "options:" << endl;
	cout << " -li <lambda_i value>  (default: 2.00)" << endl;
	cout << " -lf <lambda_f value>  (default: 0.02)" << endl;
	cout << " -ei <epsilon_i value> (default: 1.00)" << endl;
	cout << " -ef <epsilon_f value> (default: 0.01)" << endl;
	cout << " -s  <number of steps> (default: 100000)" << endl;
	cout << " -r  <number of runs>  (default: 8)" << endl;
	//cout << " -w  ignore water molecules (default: true)" << endl;
	//cout << " -t  <target variance> (default: 1.0E20)" << endl;
        return 0;
    }

    char* pPDB_Fname = NULL;
    char* pOUT_Fname = NULL;
    Real64 fLi = 2.00;
    Real64 fLf = 0.02;
    Real64 fEi = 1.00;
    Real64 fEf = 0.01;
    unsigned int iMaxstep = 100000;
    unsigned int iCluster = 20;
    unsigned int iRuns = 8;
    Real64 fTargetVar = 1.0E20;
    bool bIgnoreWater = true;

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
			C_PDB << "Lambda_i: " << fLi << endl;
		    }
		    if (argv[i][1] == 'f')
		    {
			fLf = atof(argv[i+1]);
			C_PDB << "Lambda_f: " << fLf << endl;
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
			C_PDB << "Epsilon_i: " << fEi << endl;
		    }
		    if (argv[i][1] == 'f')
		    {
			fEf = atof(argv[i+1]);
			C_PDB << "Epsilon_f: " << fEf << endl;
		    }

		    i++;
		}
		break;

		// Runs
	    case 'r':
		if (argc >= i+1)
		{
		    iRuns = atoi( argv[i+1] );
		    C_PDB << "Runs: " << iRuns << endl;

                    i++;
		}
		break;

		// Target Variance
	    case 't':
		if (argc >= i+1)
		{
		    fTargetVar = atof( argv[i+1] );
		    C_PDB << "Target Variance: " << fTargetVar << endl;

                    i++;
		}
		break;

		// Steps
	    case 's':
		if (argc >= i+1)
		{
		    iMaxstep = atoi( argv[i+1] );
		    C_PDB << "Steps: " << iMaxstep << endl;

                    i++;
		}
                break;

		// Ignore Water
	    case 'w':
		bIgnoreWater = true;
                break;
	    }

	} else {

	    if (pPDB_Fname == NULL)
	    {
		pPDB_Fname = argv[i];

		C_PDB << "Input-file : " << pPDB_Fname << endl;

	    } else if (pOUT_Fname == NULL)
	    {
		pOUT_Fname = argv[i];

		C_PDB << "Output-file: " << pOUT_Fname << endl;

	    } else {

		iCluster = atoi(argv[i]);

		C_PDB << "Cluster-centers: " << iCluster << endl;
	    }
	}
    }

    ///////////////////////////////////////////////////////////////////////////
    // Clustering
    ///////////////////////////////////////////////////////////////////////////

    // load structure
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    C_PDB << "Loading pdb file " << pPDB_Fname << endl;
    oPDB.loadPDB( pPDB_Fname );
    oPDB.setIgnoreWater( bIgnoreWater );

    // set parameters
    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setLambda( fLi, fLf );
    oClusterAlgo.setEpsilon( fEi, fEf );
    oClusterAlgo.setMaxstep( iMaxstep );
    svt_sgenrand( svt_getToD() );

    // cluster
    svt_point_cloud_pdb< svt_vector4<Real64> > oCV;
    unsigned int iTime = svt_getToD();
    oCV = oClusterAlgo.cluster(iCluster, iRuns, oPDB);
    iTime = svt_getToD() - iTime;
    C_PDB << "Runtime: " << iTime << endl;

    // average nn distance
    C_PDB << "Average nearest neighbor distance: " << oCV.averageNNDistance( 0.001 ) << endl;

    // save output
    C_PDB << "Saving cluster center pdb file " << pOUT_Fname << endl;
    oCV.writePDB( pOUT_Fname );

    return 0;
};
