/***************************************************************************
                          main
                          ----
    begin                : 02/14/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_point_cloud_pdb.h>
#include <svt_clustering_trn.h>
#include <svt_clustering_gng.h>
#include <svt_vector4.h>

#define CLUSTER cout << svt_trimName("clustering")

int main(int argc, char** argv)
{
    // prepare objects
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    CLUSTER << "loading pdb file..." << endl;
    oPDB.loadPDB("test.pdb");

    svt_sgenrand( svt_getToD() );

    if (argc < 6)
    {
	CLUSTER << "usage: clustering L_i L_f E_i E_f Steps Resultfile" << endl;
        return 0;
    }

    // prepare clustering of structure
    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setLambda( atof(argv[1]), atof(argv[2]) );
    oClusterAlgo.setEpsilon( atof(argv[3]), atof(argv[4]) );
    oClusterAlgo.setMaxstep( atoi(argv[5]) );

    svt_point_cloud_pdb< svt_vector4<Real64> > oCVPDB;
    oCVPDB = oClusterAlgo.cluster(20, 8, oPDB);

    Real64 fWidth;
    Real64 fRes;

    FILE* pFile = fopen( argv[6], "w");

    for( fRes= 5.0; fRes < 30.0; fRes+=1.0)
    {
	CLUSTER << "blur to " << fRes << " Angstroem" << endl;

	//if (fRes < 8)
	fWidth = 2.0;
	//else if (fRes < 12)
	//    fWidth = 3.0;
	//else if (fRes >= 12)
	//    fWidth = 4.0;

	svt_volume<Real64>* pMap = oPDB.blur( fWidth, fRes );

        svt_point_cloud_pdb< svt_vector4<Real64> > oCVMAP = oClusterAlgo.cluster(20, 8, *pMap);

        Real64 fRMSD = oCVMAP.rmsd_NN( oCVPDB );

	// here better also haussdorff distance, to get an impression in which area the vectors typically deviate most - perhaps a sign for a weaker signal in this
        // area, lower resolution in the em map, less well defined details...

	CLUSTER << "rmsd: " << fRMSD << endl;
	CLUSTER << "max var.: " << oCVMAP.getMaxTempFact() << endl;

	delete pMap;

        fprintf(pFile, "%f %f\n", fRMSD, oCVMAP.getMaxTempFact() );
    }

    fclose( pFile );

    CLUSTER << "end test." << endl;

    return 0;
};

/*int main(int argc, char** argv)
{
    // prepare objects
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    CLUSTER << "loading pdb file..." << endl;
    oPDB.loadPDB("test.pdb");

    svt_sgenrand( svt_getToD() );

    if (argc < 6)
    {
	CLUSTER << "usage: clustering L_i L_f E_i E_f Steps" << endl;
        return 0;
    }

    // prepare clustering of structure
    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setLambda( atof(argv[1]), atof(argv[2]) );
    oClusterAlgo.setEpsilon( atof(argv[3]), atof(argv[4]) );
    oClusterAlgo.setMaxstep( atoi(argv[5]) );

    svt_point_cloud_pdb< svt_vector4<Real64> > oCVPDB;
    svt_point_cloud_pdb< svt_vector4<Real64> > oCVPDB2;

    unsigned int iCount = 0;
    Real64 fRMSD = 0.0;
    Real64 fSumRMSD = 0.0;

    while( true )
    {
	oCVPDB = oClusterAlgo.cluster(20, 8, oPDB);
	oCVPDB2 = oClusterAlgo.cluster(20, 8, oPDB);

        iCount++;
        fRMSD = oCVPDB2.rmsd_NN( oCVPDB );
        fSumRMSD += fRMSD;

	CLUSTER << "RMSD: " << fRMSD << endl;
	CLUSTER << "Avg RMSD: " << fSumRMSD/(Real64)(iCount) << endl;

	if (iCount >= 100)
            return 0;
    }

    return 0;

    Real64 fWidth;
    Real64 fRes;

    for( fRes= 5.0; fRes < 30.0; fRes+=1.0)
    {
	CLUSTER << "blur to " << fRes << " Angstroem" << endl;

	//if (fRes < 8)
	    fWidth = 2.0;
	//else if (fRes < 12)
	//    fWidth = 3.0;
	//else if (fRes >= 12)
	//    fWidth = 4.0;

	svt_volume<Real64>* pMap = oPDB.blur( fWidth, fRes );

        svt_point_cloud_pdb< svt_vector4<Real64> > oCVMAP = oClusterAlgo.cluster(20, 16, *pMap);

        Real64 fRMSD = oCVMAP.rmsd_NN( oCVPDB );

	// here better also haussdorff distance, to get an impression in which area the vectors typically deviate most - perhaps a sign for a weaker signal in this
        // area, lower resolution in the em map, less good defined details...

	CLUSTER << "rmsd: " << fRMSD << endl;

	delete pMap;
    }

    CLUSTER << "end test." << endl;

    return 0;
};
*/
