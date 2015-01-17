/***************************************************************************
                          qfit
                          ----
    begin                : 04/07/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_iostream.h>
#include <svt_stlVector.h>
#include <svt_rnd.h>
#include <svt_types.h>
#include <svt_vector4.h>

#include <svt_point_cloud.h>

#include <svt_powell_cc.h>

#define QFIT cout << svt_trimName("qfit")

/**
 * load a pdb file
 * \param pFilename pointer to array of char with the pdb filename
 * \return a svt_point_cloud object
 */
svt_point_cloud< svt_vector4<Real64> > loadPDB(char* pFilename, vector<int>& rChainID, Real64& rAvgRms, unsigned int& rAnchorA, unsigned int& rAnchorB, unsigned int& rAnchorC)
{
    svt_point_cloud< svt_vector4<Real64> > oVecs;
    vector< _dist<Real64> > aRms;

    FILE* pFile;

    pFile = fopen( pFilename, "r" );

    if (pFile == NULL)
    {
	QFIT << "failed to open " << pFilename << endl;
        exit(1);
    }
    else
    {

	char pLine[256]; char pCoord[10];
	double fX, fY, fZ;
	Real64 fRMS;
	int i;
        unsigned int iCount = 0;
        int iModel = -1;
        rAvgRms = 0.0;

	while(!feof(pFile))
	{
	    if (fgets( pLine, 256, pFile ) != NULL)
	    {
		if (!feof(pFile) && pLine[0] == 'M' && pLine[1] == 'O' && pLine[2] == 'D' && pLine[3] == 'E' && pLine[4] == 'L')
		{
		    pCoord[0] = pLine[13];
		    pCoord[1] = pLine[14];
		    pCoord[2] = 0;
		    iModel = atoi( pCoord ) - 1;
		    cout << "iModel: " << iModel << endl;
		}

		if (!feof(pFile) && pLine[0] == 'A' && pLine[1] == 'T' && pLine[2] == 'O' && pLine[3] == 'M')
		{
		    // x coordinate
		    for(i=30; i<38; i++)
			pCoord[i-30] = pLine[i];
		    pCoord[i-30] = 0;
		    fX = atof( pCoord );

		    // y coordinate
		    for(i=38; i<46; i++)
			pCoord[i-38] = pLine[i];
		    pCoord[i-38] = 0;
		    fY = atof( pCoord );

		    // z coordinate
		    for(i=46; i<54; i++)
			pCoord[i-46] = pLine[i];
		    pCoord[i-46] = 0;
		    fZ = atof( pCoord );
		    oVecs.addPoint( fX, fY, fZ );

		    // occupancy
		    for(i=55; i<61; i++)
			pCoord[i-55] = pLine[i];
		    pCoord[i-55] = 0;
		    fRMS = atof( pCoord );
		    //rRMS.push_back( fRMS );
		    rAvgRms += fRMS;
		    aRms.push_back( _dist<Real64>( fRMS, iCount ) );

		    // chain information
		    int iChain = (int)(pLine[21]) - 65;
		    // if chain is always 'A' like in a lot of structures, then the model information decides...
		    if (iChain >= iModel)
			rChainID.push_back(iChain);
		    else
			rChainID.push_back(iModel);

		    iCount++;
		}
	    }
	}
	fclose( pFile );
    }

    rAvgRms /= (Real64)(oVecs.size());

    sort( aRms.begin(), aRms.end() );
    rAnchorA = aRms[0].getIndex();
    rAnchorB = aRms[1].getIndex();
    rAnchorC = aRms[2].getIndex();

    return oVecs;
}

/**
 * load a pdb file
 * \param pFilename pointer to array of char with the pdb filename
 * \return a svt_point_cloud object
 */
svt_point_cloud< svt_vector4<Real64> > loadOnlyPDB(char* pFilename)
{
    svt_point_cloud< svt_vector4<Real64> > oVecs;
    FILE* pFile;

    pFile = fopen( pFilename, "r" );

    if (pFile == NULL)
    {
	QFIT << "failed to open " << pFilename << endl;
        exit(1);
    }
    else
    {

	char pLine[256]; char pCoord[10];
	double fX, fY, fZ;
	int i;
        unsigned int iCount = 0;

	while(!feof(pFile))
	{
	    if (fgets( pLine, 256, pFile ) != NULL)
	    {
		if (!feof(pFile) && pLine[0] == 'A' && pLine[1] == 'T' && pLine[2] == 'O' && pLine[3] == 'M')
		{
		    // x coordinate
		    for(i=30; i<38; i++)
			pCoord[i-30] = pLine[i];
		    pCoord[i-30] = 0;
		    fX = atof( pCoord );

		    // y coordinate
		    for(i=38; i<46; i++)
			pCoord[i-38] = pLine[i];
		    pCoord[i-38] = 0;
		    fY = atof( pCoord );

		    // z coordinate
		    for(i=46; i<54; i++)
			pCoord[i-46] = pLine[i];
		    pCoord[i-46] = 0;
		    fZ = atof( pCoord );
		    oVecs.addPoint( fX, fY, fZ );

		    iCount++;
		}
	    }

	}
	fclose( pFile );
    }

    return oVecs;
}

/**
 * output result
 */
void outputResult(svt_matrix4<Real64>& rMatrix, svt_vector4<Real64> oCenter, svt_vector4<Real64> oTrans, char* pFnameIn, char* pFnameOut )
{
    FILE* pFileOut;
    FILE* pFileIn;

    pFileOut = fopen(pFnameOut, "w");
    pFileIn = fopen(pFnameIn, "r");

    if (pFileIn == NULL)
    {
	QFIT << "failed to open " << pFnameIn << endl;
        exit(1);
    }
    if (pFileOut == NULL)
    {
	QFIT << "failed to open " << pFnameOut << endl;
        exit(1);
    }

    char pLine[256]; char pCoord[100];
    double fX, fY, fZ;
    int i;

    while(!feof(pFileIn))
    {
	if (fgets( pLine, 256, pFileIn ) != NULL)
	{
	    if (!feof(pFileIn) && pLine[0] == 'A' && pLine[1] == 'T' && pLine[2] == 'O' && pLine[3] == 'M')
	    {
		// x coordinate
		for(i=30; i<38; i++)
		    pCoord[i-30] = pLine[i];
		pCoord[i-30] = 0;
		fX = atof( pCoord );

		// y coordinate
		for(i=38; i<46; i++)
		    pCoord[i-38] = pLine[i];
		pCoord[i-38] = 0;
		fY = atof( pCoord );

		// z coordinate
		for(i=46; i<54; i++)
		    pCoord[i-46] = pLine[i];
		pCoord[i-46] = 0;
		fZ = atof( pCoord );

		svt_vector4<Real64> oVec( fX, fY, fZ, 1.0 );
		oVec = oVec - oCenter;
		oVec = rMatrix * oVec;
		oVec += oTrans;
		sprintf(pCoord,"%8.3f%8.3f%8.3f\n", oVec.x(), oVec.y(), oVec.z());

		for(i=30; i<54; i++)
		    pLine[i] = pCoord[i-30];

		fprintf(pFileOut, "%s", pLine);
	    }
	}
    }

    fclose( pFileIn );
    fclose( pFileOut );
};

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 3)
    {
	cout << "usage: qfit <vectors for monomer> <vectors for oligomer> <full monomer> [options]" << endl;
	cout << "options:" << endl;
        cout << " -o output (transformed full monomer) filename" << endl;
        cout << " -c output (transformed vectors) filename" << endl;
	cout << " -s solution, meaning a full high-res oligomeric structure, used for testing purposes only!" << endl;
	cout << " -g gamma parameter, maximal deviation of oligomeric vectors in the matching" << endl;
	cout << " -l lambda parameter, maximal deviation of oligomeric vectors for the initial anchor pair matching" << endl;
	cout << " -z size of matching zone (default: 3)" << endl;
	//cout << " -n number of outputted solutions (default: 1)" << endl;
	cout << " -u rmsd threshold for uniqueness of a solution (default: 2.0)" << endl;
	cout << " -i number of runs with different anchor point sets (default: 1)" << endl;
        cout << " -r refinement <oligomer situs file> <vectors for monomer used for refinement>" << endl;
	return 1;
    }

    unsigned int iAnchorA = 0; unsigned int iDumA;
    unsigned int iAnchorB = 0; unsigned int iDumB;
    unsigned int iAnchorC = 0; unsigned int iDumC;

    // argument 1 - PDB file 1 - Model
    vector<int> aPDB_A_Chain;
    Real64 fPDB_A_RMS;
    QFIT << "Reduced model of probe-molecule: " << argv[1] << endl;
    svt_point_cloud< svt_vector4<Real64> > aPDB_A = loadPDB( argv[1], aPDB_A_Chain, fPDB_A_RMS, iAnchorA, iAnchorB, iAnchorC );
    unsigned int iNumA = aPDB_A.size();
    svt_vector4<Real64> oModelCenter;

    Real64 fAvgDist = aPDB_A.averageNNDistance( 0.1 );
    Real64 fMaxDist = aPDB_A.maxNNDistance( );
    QFIT << "  Average nearest neighbor distance: " << fAvgDist << endl;
    QFIT << "  Maximum nearest neighbor distance: " << fMaxDist << endl;
    QFIT << "  Average codebook vector rms deviation (probe): " << fPDB_A_RMS << endl;

    // argument 2 - PDB file 2 - Scene
    vector<int> aPDB_B_Chain;
    Real64 fPDB_B_RMS;
    QFIT << "Reduced model of target map: " << argv[2] << endl;
    svt_point_cloud< svt_vector4<Real64> > aPDB_B = loadPDB( argv[2], aPDB_B_Chain, fPDB_B_RMS, iDumA, iDumB, iDumC );
    unsigned int iNumB = aPDB_B.size();
    svt_vector4<Real64> oSceneCenter;
    QFIT << "  Average codebook vector rms deviation (target): " << fPDB_B_RMS << endl;

    aPDB_A.setLambda( fPDB_B_RMS + (8.0*fPDB_A_RMS) );
    aPDB_A.setGamma( fPDB_B_RMS + (0.5*fPDB_A_RMS) );

    // argument 3 - PDB file 3 - Full Monomer
    svt_point_cloud< svt_vector4<Real64> > oMono;
    Real64 fMonoRMS;
    vector<int> aMonoChain;
    svt_vector4<Real64> oMonoCenter;
    QFIT << "Full model of probe-molecule: " << argv[3] << endl;
    oMono = loadPDB( argv[3], aMonoChain, fMonoRMS, iDumA, iDumB, iDumC );
    unsigned int iNumMono = oMono.size();

    // Refinement (optional)
    char* pHighCVFname = NULL;
    char* pSitusFname = NULL;

    // Full Oligomer (optional)
    svt_point_cloud< svt_vector4<Real64> > oOlig;
    Real64 fOligRMS;
    vector<int> aOligChain;
    svt_vector4<Real64> oOligCenter;
    unsigned int iNumOlig = 0;

    // parameters
    char pOutfile[1024];
    sprintf( pOutfile, "result.pdb" );
    char pCVOutfile[1024];
    pCVOutfile[0] = 0;
    // rmsd threshold for the uniqueness of a solution
    Real64 fUnique = 2.0;
    // maximum number of generated output files
    unsigned int iMaxOutFiles = 1;
    // maximum number of wildcard matches
    unsigned int iWildcard = iNumA;
    // number of runs
    unsigned int iRuns = 1;

    // the rest of the arguments are only optional
    for(int i=4; i<argc; i++)
    {
	if (argv[i][0] == '-' && argv[i][1] != 0)
	{
	    switch(argv[i][1])
	    {
		// solution is given
	    case 's':
		if (argc >= i+1)
		{
		    QFIT << "Loading solution file: " << argv[i+1] << endl;
		    oOlig = loadPDB( argv[i+1], aOligChain, fOligRMS, iDumA, iDumB, iDumC );
		    iNumOlig = oOlig.size();
		}
		break;

		// lambda
	    case 'l':
		if (argc >= i+1)
		    aPDB_A.setLambda( atof(argv[i+1]) );
		break;

		// gamma
	    case 'g':
		if (argc >= i+1)
		    aPDB_A.setGamma( atof(argv[i+1]) );
		break;

		// matching zone size
	    case 'z':
		if (argc >= i+1)
		    aPDB_A.setZoneSize( atoi(argv[i+1]) );
		break;

		// output filename
	    case 'o':
		if (argc >= i+1)
		    strncpy( pOutfile, argv[i+1], 1024);
		break;

		// codebook vector output filename
	    case 'c':
		if (argc >= i+1)
		    strncpy( pCVOutfile, argv[i+1], 1024);
		break;

		// maximum number of outputted solutions
	    case 'n':
		if (argc >= i+1)
		{
		    iMaxOutFiles = atoi(argv[i+1]);
		    QFIT << "Maximum number of outputted solutions: " << iMaxOutFiles << endl;
		}
		break;

		// number of runs
	    case 'i':
		if (argc >= i+1)
		{
		    iRuns = atoi(argv[i+1]);
		    QFIT << "Number of runs with different anchor points: " << iRuns << endl;
                    aPDB_A.setRuns( iRuns );
		}
		break;

		// rmsd threshold for uniqueness of solution
	    case 'u':
		if (argc >= i+1)
		{
		    fUnique = atof(argv[i+1]);
		    QFIT << "rmsd threshold for uniqueness of a solution: " << iMaxOutFiles << endl;
		}
		break;

		// index of first anchor point
	    case '1':
		if (argc >= i+1)
		{
		    iAnchorA = atoi(argv[i+1]);
		    QFIT << "Index of first anchor: " << iAnchorA << endl;
		}
		break;
		// index of second anchor point
	    case '2':
		if (argc >= i+1)
		{
		    iAnchorB = atoi(argv[i+1]);
		    QFIT << "Index of second anchor: " << iAnchorB << endl;
		}
		break;
		// index of third anchor point
	    case '3':
		if (argc >= i+1)
		{
		    iAnchorC = atoi(argv[i+1]);
		    QFIT << "Index of third anchor: " << iAnchorC << endl;
		}
		break;

		// wildcards
	    case 'w':
		if (argc >= i+1)
		{
		    iWildcard = atoi(argv[i+1]);
		    QFIT << "Max. number of wildcard matches: " << iWildcard << endl;
		}
		break;

                // refinement
	    case 'r':
		if (argc >= i+2)
		{
                    pSitusFname = argv[i+1];
		    pHighCVFname = argv[i+2];
                    QFIT << "Refinement using " << pHighCVFname << " sampling " << pSitusFname << endl;
		}
                break;
	    }
	}
    }

    aPDB_A.setWildcards( iWildcard );
    QFIT << "Lambda = " << aPDB_A.getLambda() << endl;
    QFIT << "Gamma = " << aPDB_A.getGamma() << endl;
    QFIT << "Matching Zone Size = " << aPDB_A.getZoneSize() << endl;
    QFIT << "Number of wildcards = " << aPDB_A.getWildcards() << endl;

    // run matching routine
    vector< unsigned int > aMatch;
    vector< svt_matrix4<Real64> > aMatrix;

    iAnchorA = 0;
    iAnchorB = 0;
    iAnchorC = 0;

    Real64 fCV_RMSD;
    Real64 fMinRMSD = 1.0E10;
    svt_point_cloud< svt_vector4<Real64> > oTransModel;

    for(int iCount = 0; iCount < 1; iCount++)
    {
	vector< svt_matrix4<Real64> > aTmpMatrix;

	aPDB_A.match( aPDB_B, aMatch, aTmpMatrix ); //, iAnchorA, iAnchorB, iAnchorC );

	// analyze the found solution
	if (aTmpMatrix.size() > 0)
	{
	    oTransModel = aTmpMatrix[0] * aPDB_A;
	    fCV_RMSD = oTransModel.rmsd( aPDB_B );
	    QFIT << "Minimal RMSD between the two codebooks: " << fCV_RMSD << endl;

	    if (fCV_RMSD < fMinRMSD)
	    {
		fMinRMSD = fCV_RMSD;
		aMatrix = aTmpMatrix;
	    }
	}

	//iAnchorA = (unsigned int)(svt_genrand()*(Real64)(iNumA));
	//while((iAnchorB = (unsigned int)(svt_genrand()*(Real64)(iNumA))) == iAnchorA);
        //iAnchorC = (unsigned int)(svt_genrand()*(Real64)(iNumA));
	//while(iAnchorC == iAnchorA || iAnchorC == iAnchorB)
	//    iAnchorC = (unsigned int)(svt_genrand()*(Real64)(iNumA));
    }
    // best solution
    oTransModel = aMatrix[0] * aPDB_A;
    fCV_RMSD = oTransModel.rmsd( aPDB_B );
    QFIT << "Absolut minimal RMSD between the two codebooks: " << fCV_RMSD << endl;

    QFIT << "Number of possible solutions " << aMatrix.size() << endl;

    if (pHighCVFname != NULL && pSitusFname != NULL)
    {
	QFIT << "Refinement Step:" << endl;
	svt_volume<Real64> oVol;
	QFIT << "Loading situs file: " << pSitusFname << endl;
	svt_matrix4<Real64> oVolMat = oVol.loadSitus( pSitusFname );
	QFIT << "Loading codebook file: " << pHighCVFname << endl;
	svt_point_cloud< svt_vector4<Real64> > aHighCV = loadOnlyPDB( pHighCVFname );
	QFIT << "Number of CV:" << aHighCV.size() << endl;

	svt_powell_cc oPowell( aHighCV, aMatrix[0], oVol, oVolMat );
	oPowell.setTolerance( 1.0E-15 );
	oPowell.setMaxIter( 1000 );
	oPowell.maximize();
	vector<double> oPowVec = oPowell.getVector();

	QFIT << "Result of refinement:" << endl;
	QFIT << "Translation: (" << oPowVec[0] << ", " << oPowVec[1] << ", " << oPowVec[2] << ")" << endl;
	QFIT << "Rotation: (" << oPowVec[3] << ", " << oPowVec[4] << ", " << oPowVec[5] << ")" << endl;

	aMatrix[0] = oPowell.getTrans();
    }

    // if the user has supplied a solution, then calculate the rmsd to the oligomeric structure
    Real64 fError = 0.0;
    unsigned int j;
    if (iNumOlig != 0)
    {
	svt_point_cloud< svt_vector4<Real64> > oTransMono;
	oTransMono = aMatrix[0] * oMono;

        // Multimolecule docking or full molecule
	if (oTransMono.size() < oOlig.size())
	{
	    // and calculate the rmsd to the oligomeric subcomponent
	    int aChains[256];
	    for(j=0; j<256; j++)
		aChains[j] = 0;
	    // calculate the nearest neighbor of each atom in the monomer regarding the oligomer and record the chain. The chain that occurs most is the chain that the rmsd is computed relative to.
	    for(j=0; j<oTransMono.size(); j++)
	    {
		unsigned int iIndex = oOlig.nearestNeighbor( oTransMono[j] );
		aChains[aOligChain[iIndex]]++;
	    }

	    int iMax = 0;
	    unsigned int iMaxIndex = 0;
	    for(j=0; j<256; j++)
	    {
		if (aChains[j] > iMax)
		{
		    iMax = aChains[j];
		    iMaxIndex = j;
		}
	    }

	    int iChain = iMaxIndex * oMono.size();
	    QFIT << "Compare to chain: " << iMaxIndex << endl;

	    for(unsigned int j=0; j<oTransMono.size(); j++)
		fError += oTransMono[j].distanceSq( oOlig[j+iChain] );
	    fError /= oMono.size();
	    fError = sqrt( fError );

	} else {

	    for(j=0; j<oTransMono.size(); j++)
		fError += oTransMono[j].distanceSq( oOlig[j] );
	    fError /= oMono.size();
	    fError = sqrt( fError );
	}

	QFIT << "Error RMSD to the known solution: " << fError << endl;
    }

    // output a single file or multiple ones? In the case of multiples, we have to investigate if all top 10 solutions are really unique...
    if (iMaxOutFiles != 1)
    {
	vector< svt_point_cloud< svt_vector4<Real64> > > aResults;
        vector< svt_matrix4<Real64> > aResultsMat;
        unsigned int i;

        // scan for unique results...
	for(i=0; i<aMatrix.size(); i++)
	{
	    svt_point_cloud< svt_vector4<Real64> > oRes = aMatrix[i] * aPDB_A;

	    bool bNoAdd = false;
            // add this solution only if the new result is really very different from the already found ones...
	    for(unsigned int j=0; j<aResults.size(); j++)
	    {
		if (aResults[j].rmsd( oRes ) < fUnique)
                    bNoAdd = true;
	    }

	    if (bNoAdd == false)
	    {
		aResults.push_back( oRes );
                aResultsMat.push_back( aMatrix[i] );

		QFIT << "Result " << aResults.size()+1 << " CV_RMSD: " << oRes.rmsd( aPDB_B ) << endl;
	    }
	}

	// now output these...
	for(i=0; i<aResultsMat.size() && i<iMaxOutFiles; i++)
	{
	    char pFname[1024];
            sprintf( pFname, "%s_%i.pdb", pOutfile, i);
	    QFIT << "Writing result to " << pFname << endl;
	    outputResult( aResultsMat[i], oModelCenter, oSceneCenter, argv[3], pFname );
	}

    } else {

	QFIT << "Writing result to " << pOutfile << endl;
	outputResult( aMatrix[0], oMonoCenter, oOligCenter, argv[3], pOutfile );
        if (pCVOutfile[0] != 0)
	{
	    QFIT << "Writing result to " << pCVOutfile << endl;
	    outputResult( aMatrix[0], oModelCenter, oSceneCenter, argv[1], pCVOutfile );
	}
    }

    // FIXME: Remove!!!!
    //FILE* pFileLog = fopen("../results.txt", "a");
    //fprintf(pFileLog, "%3.3f %3.3f %3.3f\n", fPDB_B_RMS, fCV_RMSD, fError );
    //fclose(pFileLog);

    return 0;
}
