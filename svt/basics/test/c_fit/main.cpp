/***************************************************************************
                          c_fit
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
#include <svt_point_cloud_pdb.h>
#include <svt_powell_cc.h>
#include <svt_clustering_trn.h>
//#include <svt_clustering_irank.h>

#include <svt_config.h>

#define C_FIT cout << svt_trimName("c_fit")
#define OUTLIER 0.7

//#define rmsdNN(X) rmsd_NN_Outliers(X,1.0)
#define rmsdNN(X) rmsd_NN(X)

///////////////////////////////////////////////////////////////////////////
// Aux. Classes and Routines
///////////////////////////////////////////////////////////////////////////

/**
 * matching helper class
 */
template<class T> class _neighbor
{
protected:

    T m_fScore;
    unsigned int m_iIndexA;
    unsigned int m_iIndexB;

public:

    _neighbor(T fScore, unsigned int iIndexA, unsigned int iIndexB)
    {
	m_fScore = fScore;
	m_iIndexA = iIndexA;
	m_iIndexB = iIndexB;
    };

    inline T getScore() const
    {
	return m_fScore;
    };
    inline unsigned int getIndexA() const
    {
	return m_iIndexA;
    };
    inline unsigned int getIndexB() const
    {
	return m_iIndexB;
    };

    inline bool operator<(const _neighbor<T>& rR) const
    {
        return m_fScore < rR.m_fScore;
    };
};

/**
 * refine rmsd
 */
void refine_rmsd_1( svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Mono, svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Olig, svt_matrix4<Real64>& rMatrix, Real64 fOrigRMSD )
{
    svt_matrix4<Real64> oMatrix = rMatrix;
    Real64 fBestRMSD = fOrigRMSD;
    svt_matrix4<Real64> oBestMatrix = rMatrix;
    unsigned int j;

    Real64 fRMSD_Diff = -10.0;
    Real64 fRMSD_Old = -10.0;
    Real64 fMaxScore = 15.0;
    Real64 fRMSD = 0.0;

    do
    {
	svt_point_cloud_pdb< svt_vector4<Real64> > oTransCV = oMatrix * oCV_Mono;

	// calculate all neares neighbor relationships
	unsigned int iIndex = 0;
	vector< _neighbor<Real64> > aNeighbors;
	for(j=0; j<oTransCV.size(); j++)
	{
	    iIndex = oCV_Olig.nearestNeighbor( oTransCV[j] );

	    aNeighbors.push_back( _neighbor<Real64>(oTransCV[j].distance( oCV_Olig[iIndex] ), j, iIndex) );
	}
	// sort by distance
	sort( aNeighbors.begin(), aNeighbors.end() );

	// generate best matching on nearest neighbor distance
	unsigned int iMatchC;
	Real64 fMinRMSD = 1.0E10;
	Real64 fPerfCut = 0.0;
	for(Real64 fCutoff = fMaxScore; fCutoff > 2.0; fCutoff -= 0.5)
	{
	    vector< int > aMatchMono;
	    vector< int > aMatchOlig;

	    // add all neighbors within a certain distance to match
	    iMatchC = 0;
	    for(j=0; j<oTransCV.size(); j++)
	    {
		if (aNeighbors[j].getScore() < fCutoff)
		{
		    aMatchMono.push_back( aNeighbors[j].getIndexA() );
		    aMatchOlig.push_back( aNeighbors[j].getIndexB() );
		    iMatchC++;

		} else {

		    aMatchMono.push_back( aNeighbors[j].getIndexA() );
		    aMatchOlig.push_back( NOMATCH );
		}
	    }

	    // OK, now optimize transformation of the new cv's based on the matching generated from the old transformation
	    oMatrix = oTransCV.kearsley( aMatchMono, aMatchOlig, oCV_Mono, oCV_Olig );
	    oTransCV = oMatrix * oCV_Mono;
	    fRMSD = oTransCV.rmsdNN( oCV_Olig );

	    if (fRMSD < fMinRMSD)
	    {
		fMinRMSD = fRMSD;
		fPerfCut = fCutoff;
	    }
	}

	//C_FIT << "DEBUG: Perfect cutoff: " << fPerfCut << endl;

	vector< int > aMatchMono;
	vector< int > aMatchOlig;

	// add all neighbors within a certain distance to match
	iMatchC = 0;
	for(j=0; j<oTransCV.size(); j++)
	{
	    if (aNeighbors[j].getScore() < fPerfCut)
	    {
		aMatchMono.push_back( aNeighbors[j].getIndexA() );
		aMatchOlig.push_back( aNeighbors[j].getIndexB() );
		iMatchC++;

	    } else {

		aMatchMono.push_back( aNeighbors[j].getIndexA() );
		aMatchOlig.push_back( NOMATCH );
	    }
	}

	// OK, now optimize transformation of the new cv's based on the matching generated from the old transformation
	oMatrix = oTransCV.kearsley( aMatchMono, aMatchOlig, oCV_Mono, oCV_Olig );
	oTransCV = oMatrix * oCV_Mono;
	fRMSD = oTransCV.rmsdNN( oCV_Olig );

	// check if this is the best run ever...
	if (fRMSD < fBestRMSD)
	{
	    fBestRMSD = fRMSD;
	    oBestMatrix = oMatrix;
	}

	// difference to previous run?
	if (fRMSD_Diff == -10.0)
	    fRMSD_Diff = fRMSD;
	else
	    fRMSD_Diff = ( fRMSD_Old - fRMSD );

	fRMSD_Old = fRMSD;

	//C_FIT << "CV-RMSD: " << oTransCV.rmsdNN( oCV_Olig ) << endl;
	//C_FIT << "RMSD_DIFF: " << fRMSD_Diff << endl;

    } while( fRMSD_Diff > 1.0E-5 );


    // copy the best result
    rMatrix = oBestMatrix;
};

/**
 * refine rmsd
 */
void refine_rmsd_2( svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Mono, svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Olig, svt_matrix4<Real64>& rMatrix, Real64 fOrigRMSD )
{
    svt_matrix4<Real64> oMatrix = rMatrix;
    Real64 fBestRMSD = fOrigRMSD;
    svt_matrix4<Real64> oBestMatrix = rMatrix;
    unsigned int j;

    unsigned int iMaximum = (unsigned int)((Real64)(oCV_Mono.size()) * 0.5);

    if (iMaximum < 5)
        iMaximum = 5;

    do
    {
	Real64 fRMSD_Diff = -10.0;
	Real64 fRMSD_Old = -10.0;
	Real64 fRMSD = 0.0;

	do
	{
	    svt_point_cloud_pdb< svt_vector4<Real64> > oTransCV = oMatrix * oCV_Mono;

	    // calculate all neares neighbor relationships
	    unsigned int iIndex = 0;
	    vector< _neighbor<Real64> > aNeighbors;
	    for(j=0; j<oTransCV.size(); j++)
	    {
		iIndex = oCV_Olig.nearestNeighbor( oTransCV[j] );

		aNeighbors.push_back( _neighbor<Real64>(oTransCV[j].distance( oCV_Olig[iIndex] ), j, iIndex) );
	    }
	    // sort by distance
	    sort( aNeighbors.begin(), aNeighbors.end() );

	    // generate best matching on nearest neighbor distance
	    vector< int > aMatchMono;
	    vector< int > aMatchOlig;

	    // add all neighbors within a certain distance to match
	    for(j=0; j<oTransCV.size(); j++)
	    {
		if (j < iMaximum)
		{
		    aMatchMono.push_back( aNeighbors[j].getIndexA() );
		    aMatchOlig.push_back( aNeighbors[j].getIndexB() );

		} else {

		    aMatchMono.push_back( aNeighbors[j].getIndexA() );
		    aMatchOlig.push_back( NOMATCH );
		}
	    }

	    // OK, now optimize transformation of the new cv's based on the matching generated from the old transformation
	    oMatrix = oTransCV.kearsley( aMatchMono, aMatchOlig, oCV_Mono, oCV_Olig );
	    oTransCV = oMatrix * oCV_Mono;
	    fRMSD = oTransCV.rmsdNN( oCV_Olig );

	    // check if this is the best run ever...
	    if (fRMSD < fBestRMSD)
	    {
		fBestRMSD = fRMSD;
		oBestMatrix = oMatrix;
	    }

	    // difference to previous run?
	    if (fRMSD_Diff == -10.0)
		fRMSD_Diff = fRMSD;
	    else
		fRMSD_Diff = ( fRMSD_Old - fRMSD );

	    fRMSD_Old = fRMSD;

	    //C_FIT << "CV-RMSD: " << oTransCV.rmsdNN( oCV_Olig ) << endl;
	    //C_FIT << "RMSD_DIFF: " << fRMSD_Diff << endl;

	} while( fRMSD_Diff > 1.0E-5 );

        iMaximum++;

    } while( iMaximum < oCV_Mono.size() );

    // copy the best result
    rMatrix = oBestMatrix;
};

// refine a matching
void refine( svt_clustering< svt_vector4<Real64> >& rClusterAlgo, svt_volume<Real64>& rMonoMap, svt_volume<Real64>& rOligMap, unsigned int iNumMono, svt_matrix4<Real64>& rMatrix, Real64 fOrigRMSD, unsigned int iCV_Start, unsigned int iIter )
{
    unsigned int iCV = iCV_Start;
    unsigned int iCount = 0;
    Real64 fBestRMSD = fOrigRMSD;
    Real64 fRMSD = fOrigRMSD;
    svt_matrix4<Real64> oBestMatrix = rMatrix;
    svt_matrix4<Real64> oMatrix = rMatrix;

    for(unsigned int i=0; i<iIter; i++)
    {
	iCV += iCV;
    }

	C_FIT << "///////////////////////////////////////////////////////////////////////////" << endl;
	C_FIT << "// Clustering with " << iCV << " cluster centers" << endl;
	C_FIT << "// Monomer : " << iCV << endl;
	C_FIT << "// Oligomer: " << iCV*iNumMono << endl;
	C_FIT << "///////////////////////////////////////////////////////////////////////////" << endl;

	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Mono   = rClusterAlgo.cluster(iCV,          8, rMonoMap);
	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Olig   = rClusterAlgo.cluster(iCV*iNumMono, 8, rOligMap);

        // calculate RMSD
	svt_point_cloud_pdb< svt_vector4<Real64> > oTransCV   = oMatrix * oCV_Mono;
	fRMSD = oTransCV.rmsdNN( oCV_Olig );

	refine_rmsd_2( oCV_Mono, oCV_Olig, oMatrix, fRMSD );

	// calculate RMSD
	oTransCV = oMatrix * oCV_Mono;
	fRMSD = oTransCV.rmsdNN( oCV_Olig );

	refine_rmsd_1( oCV_Mono, oCV_Olig, oMatrix, fRMSD );

        // calculate RMSD
	oTransCV = oMatrix * oCV_Mono;
	fRMSD = oTransCV.rmsdNN( oCV_Olig );

        // DEBUG
        oTransCV.writePDB( "nachRefine_cv.pdb" );
        oCV_Olig.writePDB( "nachRefine_cv_olig.pdb" );

        // DEBUG!
	//char pFname[256];
        //sprintf( pFname, "debug_%i.pdb", iCV );
        //oTransCV.writePDB( pFname );

	// check if this is the best run ever...
	if (fRMSD < fBestRMSD)
	{
	    fBestRMSD = fRMSD;
	    oBestMatrix = oMatrix;
	}

	C_FIT << "CV-RMSD: " << fRMSD << endl;
    //}

    // copy the best result
    rMatrix = oBestMatrix;

    if (fBestRMSD < fOrigRMSD)
	C_FIT << "Erfolg, refine():" << fOrigRMSD << " -> " << fBestRMSD << endl;
    else
	C_FIT << "KEIN Erfolg, refine()!!" << endl;

    C_FIT << "fBestRMSD: " << fBestRMSD << endl;
};

/**
 * get best model
 */
svt_matchResult<Real64> getBestResult( svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Mono, svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Olig, svt_point_cloud_pdb< svt_vector4<Real64> >& oMono, svt_point_cloud_pdb< svt_vector4<Real64> >& oOlig, vector< svt_matchResult<Real64> >& rResults, Real64 fOutlierPerc )
{
    /*unsigned int iNum = rResults.size();
    svt_point_cloud_pdb< svt_vector4<Real64> > oTransCV;

    for(unsigned int i=0; i<iNum; i++)
    {
        svt_matrix4<Real64> oMatrix = rResults[i].getMatrix();
	oTransCV = oMatrix * oMono;

	rResults[i].setScore( oTransCV.rmsd_NN( oOlig ) );

        //C_FIT << i << " - " << rResults[i].getScore() << endl;
    }*/

    sort( rResults.begin(), rResults.end() );

    return rResults[0];
}

/**
 * Cleanup the result stack, remove abiguous results.
 */
void cleanResults( svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Mono, vector< svt_matchResult< Real64 > >& rResults )
{
    /*vector< svt_matchResult< Real64 > > aFinal;
    vector< svt_vector4<Real64> > aComs;
    unsigned int iNum,i,j;
    svt_vector4<Real64> oCOA = oCV_Mono.coa();
    svt_vector4<Real64> oTransCOA;

    iNum = rResults.size();

    for(i=0; i<iNum; i++)
    {
        oTransCOA = rResults[i].getMatrix() * oCOA;

	for(j=0; j<aComs.size(); j++)
	{
	    if (aComs[j].distance( oTransCOA ) < 0.5)
		break;
	}

	if (j >= aComs.size() && aFinal.size() < 100)
	{
	    aComs.push_back( oTransCOA );
            aFinal.push_back( rResults[i] );
	}
    }

    C_FIT << "Before cleanup: " << iNum << endl;
    C_FIT << "After  cleanup: " << aFinal.size() << endl;
    rResults = aFinal;*/
};

///////////////////////////////////////////////////////////////////////////
// Main Routine
///////////////////////////////////////////////////////////////////////////

// main routine
int main(int argc, char* argv[])
{
    ///////////////////////////////////////////////////////////////////////////
    // Preparation
    ///////////////////////////////////////////////////////////////////////////

    // check command line arguments
    if (argc < 2)
    {
	C_FIT << "usage: c_fit <config file>" << endl;
	return 1;
    }

    svt_config oConf( argv[1] );

    // load monomer structure
    svt_point_cloud_pdb< svt_vector4<Real64> > oMono;
    C_FIT << "Loading monomer pdb file: " << oConf.getValue("Monomer", "monomer.pdb") << endl;
    oMono.loadPDB( oConf.getValue("Monomer", "monomer.pdb") );
    oMono.setIgnoreWater( false );

    // load oligomer structure
    svt_point_cloud_pdb< svt_vector4<Real64> > oOlig;
    C_FIT << "Loading oligomer pdb file: " << oConf.getValue("Oligomer", "oligomer.pdb") << endl;
    oOlig.loadPDB(  oConf.getValue("Oligomer", "oligomer.pdb") );
    oOlig.setIgnoreWater( false );

    // number of monomers
    unsigned int iNumMono = oConf.getValue("NumMono", 6);

    // prepare special structure for powell
    unsigned int i;
    svt_point_cloud_pdb< svt_vector4<Real64> > oMono_CA;
    for(i=0; i<oMono.size(); i++)
	if (oMono.atom(i).getName()[0] == 'C' && oMono.atom(i).getRemoteness() == 'A')
            oMono_CA.addAtom( oMono.atom(i), oMono[i] );
    svt_point_cloud_pdb< svt_vector4<Real64> > oMono_Powell;
    for(i=0; i<oMono_CA.size(); i++)
        oMono_Powell.addAtom( oMono_CA.atom(i), oMono_CA[i] );

    // open files for data and parameters
    char pFnameDat[2560];
    sprintf( pFnameDat, "%s.dat", argv[1] );
    char pFnameParam[2560];
    sprintf( pFnameParam, "%s.param", argv[1] );

    FILE* pFile = fopen( pFnameDat, "w" );
    FILE* pFileParam = fopen( pFnameParam, "w" );

    //
    // parameters
    //
    unsigned int iCV_Start  = oConf.getValue("CV_Start", 20);
    unsigned int iCV_End    = oConf.getValue("CV_End", 21);
    Real64 fRes_Start       = oConf.getValue("Res_Start", 4.0);
    Real64 fRes_End         = oConf.getValue("Res_End", 36.0);
    bool bLaplacian         = oConf.getValue("Laplacian", true );
    Real64 fTargetVar       = oConf.getValue("TargetVar", 400.0 );
    Real64 fLi              = oConf.getValue("Lambda_i", 0.2 );
    Real64 fLf              = oConf.getValue("Lambda_f", 0.02 );
    Real64 fEi              = oConf.getValue("Epsilon_i", 0.1 );
    Real64 fEf              = oConf.getValue("Epsilon_f", 0.01 );
    unsigned int iMaxStep   = oConf.getValue("MaxStep", 100000 );
    bool bPowell            = oConf.getValue("Powell", true );
    bool bFastPowell        = oConf.getValue("FastPow", true );
    Real64 fNoise           = oConf.getValue("Noise", 0.0 );
    Real64 fMatchLambda     = oConf.getValue("Lambda", 10.0 );
    Real64 fMatchGamma      = oConf.getValue("Gamma", 7.0 );
    unsigned int iWildcard  = oConf.getValue("Wildcards", 2 );
    Real64 fWCPenalty       = oConf.getValue("WCPenalty", 1.0 );
    unsigned int iMatchRuns = oConf.getValue("MatchRuns", 3 );
    bool bSimple            = oConf.getValue("Simple", false );
    unsigned int iResRun    = oConf.getValue("ResRuns", 3 );
    bool bPowLapl           = oConf.getValue("PowLapl", bLaplacian );
    bool bCOAAnchors        = oConf.getValue("COAAnch", false );
    Real64 fFilter          = oConf.getValue("Filter", 3.0 );
    unsigned int iRefine    = oConf.getValue("Refine", 0 );
    Real64 fCutoff          = oConf.getValue("Cutoff", 0.8 );

    // print params to file so that we remember...

    fprintf( pFileParam, "Monomer  : %s\n", oConf.getValue("Monomer", "monomer.pdb") );
    fprintf( pFileParam, "Oligomer : %s\n", oConf.getValue("Oligomer", "oligomer.pdb") );
    fprintf( pFileParam, "Results  : %s\n", oConf.getValue("Results", "results.pdb") );
    fprintf( pFileParam, "iCV_Start: %i\n", iCV_Start );
    fprintf( pFileParam, "iCV_End  : %i\n", iCV_End );
    fprintf( pFileParam, "iNumMono : %i\n", iNumMono );
    fprintf( pFileParam, "fRes_Strt: %f\n", fRes_Start );
    fprintf( pFileParam, "fRes_End : %f\n", fRes_End );

    if (bLaplacian)
	fprintf( pFileParam, "Laplacian: TRUE\n" );
    else
	fprintf( pFileParam, "Laplacian: FALSE\n" );

    fprintf( pFileParam, "Lambda_i = %f\n", fLi );
    fprintf( pFileParam, "Lambda_f = %f\n", fLf );
    fprintf( pFileParam, "Epsilon_i= %f\n", fEi );
    fprintf( pFileParam, "Epsilon_f= %f\n", fEf );
    fprintf( pFileParam, "iMaxStep = %i\n", iMaxStep );
    fprintf( pFileParam, "TargetV  = %f\n", fTargetVar );
    fprintf( pFileParam, "Noise    = %f\n", fNoise );
    fprintf( pFileParam, "Lambda   = %f\n", fMatchLambda );
    fprintf( pFileParam, "Gamma    = %f\n", fMatchGamma );
    fprintf( pFileParam, "Wildcards= %i\n", iWildcard );
    fprintf( pFileParam, "WCPenalty= %f\n", fWCPenalty );
    fprintf( pFileParam, "MatchRuns= %i\n", iMatchRuns );
    fprintf( pFileParam, "ResRuns  = %i\n", iResRun );
    fprintf( pFileParam, "Filter   = %f\n", fFilter );
    fprintf( pFileParam, "Refine   = %i\n", iRefine );
    fprintf( pFileParam, "Cutoff   = %f\n", fCutoff );

    if (bPowell)
	fprintf( pFileParam, "Powell   = TRUE\n" );
    else
	fprintf( pFileParam, "Powell   = FALSE\n" );

    if (bFastPowell)
	fprintf( pFileParam, "FastPow  = TRUE\n" );
    else
	fprintf( pFileParam, "FastPow  = FALSE\n" );

    if (bSimple)
	fprintf( pFileParam, "Simple   = TRUE\n" );
    else
	fprintf( pFileParam, "Simple   = FALSE\n" );

    if (bPowLapl)
	fprintf( pFileParam, "PowLapl  = TRUE\n" );
    else
	fprintf( pFileParam, "PowLapl  = FALSE\n" );

    if (bCOAAnchors)
	fprintf( pFileParam, "COAAnch  = TRUE\n" );
    else
	fprintf( pFileParam, "COAAnch  = FALSE\n" );

    fclose( pFileParam );

    ///////////////////////////////////////////////////////////////////////////
    // Test a range of codebook vectors
    ///////////////////////////////////////////////////////////////////////////

    for(Real64 fInd = 0.0; fInd < fRes_Start; fInd += 1.0)
    {
	fprintf( pFile, "%f %f ", fInd, 0.0 );
	fprintf( pFile, "%i ", 0 );
	fprintf( pFile, "%f ", 0.0 );
	fprintf( pFile, "%f ", 0.0 );
	fprintf( pFile, "%f ", 0.0 );
	fprintf( pFile, "%f ", 0.0 );
	fprintf( pFile, "%i ", 0 );
	fprintf( pFile, "%f ", 0.0 );
	fprintf( pFile, "%f ", 0.0 );
	fprintf( pFile, "%f\n", 0.0 );
    }

    for(unsigned int iNumCV = iCV_Start; iNumCV < iCV_End; iNumCV += 2)
    {

	C_FIT << endl;
        C_FIT << "/////////////////////////////////////////////////////////////////" << endl;
	C_FIT << "// Now testing " << iNumCV << " codebook vectors!" << endl;
	C_FIT << "/////////////////////////////////////////////////////////////////" << endl;
        C_FIT << endl;

	///////////////////////////////////////////////////////////////////////////
	// Test a range of resolutions
	///////////////////////////////////////////////////////////////////////////

	Real64 fWidth = 3.0;
	Real64 fRes = 10.0;
	Real64 fNoiseVar = 0.0;
        Real64 fNoiseMax = 0.0;
	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Mono;
	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Mono_A;
	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Mono_B;

	// set parameters for the clustering
	svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
        oClusterAlgo.setConnOnline( false );
	oClusterAlgo.setLambda( fLi, fLf );
	oClusterAlgo.setEpsilon( fEi, fEf );
	oClusterAlgo.setMaxstep( iMaxStep );
	svt_sgenrand( svt_getToD() );

	for(fRes = fRes_Start; fRes < fRes_End; fRes += 1.0)
	{
	    if (fRes < 6)
                fWidth = 2.0;
	    else if (fRes < 8)
		fWidth = 2.0;
	    else if (fRes < 12)
		fWidth = 3.0;
	    else if (fRes >= 12)
		fWidth = 4.0;

            fprintf( pFile, "%f %f ", fRes, fWidth );
	    fprintf( pFile, "%i ", iNumCV );

	    ///////////////////////////////////////////////////////////////////////////
	    // Within resolution do a couple of tests and keep the result with the best
            // CV rmsd...
	    ///////////////////////////////////////////////////////////////////////////

	    svt_matrix4<Real64> oWinnerMat;
	    Real64 fMinRMSD = 1.0E10;
	    Real64 fMinCVRMSD = 1.0E10;
	    Real64 fCV_RMSD = 1.0E10;
	    Real64 fPDB_RMSD = 1.0E10;
	    unsigned long iTime = 0;
	    svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Olig_A;
	    svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Olig_B;
	    svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Olig;
	    svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Olig_Back;
	    Real64 fAvgTemp = 1.0E10;
	    Real64 fMaxTemp = 1.0E10;
            Real64 fHausdorff = 1.0E10;

            // blur monomer and oligomer
	    C_FIT << "Low-pass filtering of the monomer to a resolution of " << fRes << " Angstroem" << endl;
	    svt_volume<Real64>* pMonoMap;
	    if (fNoise == 0.0)
		pMonoMap = oMono.blur( fWidth, fRes );
	    else
		pMonoMap = NULL;

	    // laplacian
	    if (bLaplacian == true)
	    {
		pMonoMap->applyLaplacian();
		(*pMonoMap) * (-1.0);
		pMonoMap->threshold(0.0);
	    }

            // normalize map
	    if (pMonoMap != NULL)
		pMonoMap->normalize();


	    C_FIT << "Low-pass filtering of the oligomer to a resolution of " << fRes << " Angstroem" << endl;
	    svt_volume<Real64>* pMap = NULL;
	    if (fNoise == 0.0)
		pMap = oOlig.blur( fWidth, fRes );
	    else
		pMap = NULL;

	    // laplacian
	    if (fNoise == 0.0 && bLaplacian == true)
	    {
		pMap->applyLaplacian();
		(*pMap) * (-1.0);
		pMap->threshold(0.0);
	    }

            // normalize oligomer map
            if (pMap != NULL)
		pMap->normalize();

	    for(unsigned int iResRunInd = 0; iResRunInd < iResRun; iResRunInd++)
	    {

		///////////////////////////////////////////////////////////////////////////
		// Clustering of monomer
		///////////////////////////////////////////////////////////////////////////

		Real64 fMaxDensity = 0.0;

		if (fNoise != 0.0)
		{
		    oCV_Mono = oMono;

		    svt_vector4<Real64> oVec;
		    Real64 fLength;
		    for(unsigned int i=0; i<oCV_Mono.size(); i++)
		    {
			oVec.x( svt_genrand() );
			oVec.y( svt_genrand() );
			oVec.z( svt_genrand() );
			oVec.w( 0.0 );
			oVec.normalize();
			oVec *= (svt_genrand() * fNoise * fRes);

			fLength = oVec.length();

			oCV_Mono[i] = oCV_Mono[i] + oVec;
			oCV_Mono.atom( i ).setTempFact( fLength );
		    }
		}

                // with noise-test no clustering, the pdb files contain the clustered feature vectors
		if (fNoise == 0.0)
		{
		    fMaxDensity = pMonoMap->getMaxDensity();
		    pMonoMap->setCutoff( fMaxDensity * fCutoff );

		    C_FIT << "Cutoff: " << fMaxDensity * fCutoff << endl;

		    // cluster the monomer
		    C_FIT << "Cluster analysis of the monomer..." << endl;

		    if (fNoiseVar == 0.0)
		    {
			oCV_Mono_A = oClusterAlgo.cluster(iNumCV, 8, oMono);
			oCV_Mono_B = oClusterAlgo.cluster(iNumCV, 8, oMono);
			//oCV_Mono_A = oClusterAlgo.cluster(iNumCV, 8, *pMonoMap);
			//oCV_Mono_B = oClusterAlgo.cluster(iNumCV, 8, *pMonoMap);
		    }
		    unsigned int iNeighbor;
		    Real64 fNDist;
		    // only add stable points
		    oCV_Mono.deleteAllAtoms();
		    for(i=0; i<oCV_Mono_A.size(); i++)
		    {
			iNeighbor = oCV_Mono_B.nearestNeighbor( oCV_Mono_A[i] );
			fNDist = oCV_Mono_A[i].distance( oCV_Mono_B[iNeighbor] );
			if (fNDist < fFilter)
			{
			    if (oCV_Mono_A.getAvgTempFact() < oCV_Mono_B.getAvgTempFact())
			    {
				oCV_Mono_A.atom(i).setTempFact( fNDist );
				oCV_Mono.addAtom( oCV_Mono_A.atom(i), oCV_Mono_A[i] );
			    }
			    else
			    {
				oCV_Mono_B.atom(i).setTempFact( fNDist );
				oCV_Mono.addAtom( oCV_Mono_B.atom(i), oCV_Mono_B[i] );
			    }
			}

			C_FIT << fNDist << endl;
		    }

		    if (oCV_Mono.size() < 4)
			oCV_Mono = oCV_Mono_A;
		} 

		C_FIT << "Final monomer codebook has " << oCV_Mono.size() << " points" << endl;

		Real64 fCV_Mono_RMS = oCV_Mono.getMaxTempFact();

		///////////////////////////////////////////////////////////////////////////
		// Cluster oligomer
		///////////////////////////////////////////////////////////////////////////

		// if not the special noise test, we cluster the blured oligomer
		if (fNoise == 0.0)
		{
		    fMaxDensity = pMap->getMaxDensity();
		    pMap->setCutoff( fMaxDensity * fCutoff );
		    C_FIT << "Cluster analysis of the blured oligomer..." << endl;

		    oCV_Olig = oClusterAlgo.cluster(iNumCV*iNumMono, 8, *pMap);

		    /*oCV_Olig_A = oClusterAlgo.cluster(iNumCV*iNumMono, 8, *pMap);
		    oCV_Olig_B = oClusterAlgo.cluster(iNumCV*iNumMono, 8, *pMap);

		    if (oCV_Olig_A.getAvgTempFact() < oCV_Olig_B.getAvgTempFact())
			oCV_Olig = oCV_Olig_A;
                    else
			oCV_Olig = oCV_Olig_B;*/

		    C_FIT << "Final oligomer codebook has " << oCV_Olig.size() << " points" << endl;

		    Real64 fCV_Olig_RMS = oCV_Olig.getMaxTempFact();

		} else {

		    oCV_Olig = oOlig;

		}
                /*
		    svt_vector4<Real64> oVec;
		    fNoiseVar = 0.0;
		    fNoiseMax = 0.0;
		    unsigned int iNumNoise = 10;
		    Real64 fLength;
		    vector<unsigned int> aMani;

		    for(unsigned int i=0; i<oCV_Olig.size(); i++)
			aMani.push_back( i );

		    for(unsigned int i=0; i<aMani.size(); i++)
		    {
			oVec.x( svt_genrand() );
			oVec.y( svt_genrand() );
			oVec.z( svt_genrand() );
			oVec.w( 0.0 );
			oVec.normalize();
			oVec *= (svt_genrand() * fNoise * fRes);

			fLength = oVec.length();

			if ( fLength > fNoiseMax )
			    fNoiseMax = fLength;

			fNoiseVar += fLength;

			oCV_Olig[aMani[i]] = oCV_Olig[aMani[i]] + oVec;
			oCV_Olig.atom( aMani[i] ).setTempFact( fLength );
		    }

		    fNoiseVar /= oCV_Olig.size();
		}*/

		Real64 fCV_Olig_RMS = oCV_Olig.getMaxTempFact();

		///////////////////////////////////////////////////////////////////////////
		// Hausdorff
		///////////////////////////////////////////////////////////////////////////

		/*unsigned int iNeighbor;
		 Real64 fNDist;
		 for(i=0; i<oCV_Mono.size(); i++)
		 {
		 iNeighbor = oCV_Olig.nearestNeighbor( oCV_Mono[i] );
		 fNDist = oCV_Mono[i].distance( oCV_Olig[iNeighbor] );

		 C_FIT << i << " - " << iNeighbor << " : " << fNDist << " - " << oCV_Mono.atom(i).getTempFact() << endl;
		 }

		 oCV_Mono.writePDB("oCV_Mono.pdb");
		 oCV_Olig.writePDB("oCV_Olig.pdb");

		 exit(1);*/

		/*svt_point_cloud_pdb< svt_vector4<Real64> > oTmp;
		 for(i=0; i<oCV_Mono.size(); i++)
		 if (oCV_Mono.atom(i).getTempFact() < 3.0)
		 oTmp.addAtom( oCV_Mono.atom(i), oCV_Mono[i] );

		 oCV_Mono = oTmp;*/

		vector<_cluster_dist> oDists;
		for(i=0; i<oCV_Mono.size(); i++)
		    oDists.push_back( _cluster_dist( i, oCV_Mono.atom(i).getTempFact() ) );
		sort( oDists.begin(), oDists.end() );

		///////////////////////////////////////////////////////////////////////////
		// Matching
		///////////////////////////////////////////////////////////////////////////

		// set parameter for matching algorithm
		//oCV_Mono.setLambda( fCV_Olig_RMS + (8.0*fCV_Mono_RMS) );
		//oCV_Mono.setGamma(  fCV_Olig_RMS + (0.5*fCV_Mono_RMS) );
		if (fNoise == 0.0)
		{
		    //oCV_Mono.setLambda( fMatchLambda + (fRes*0.2) );
		    //oCV_Mono.setGamma( fMatchGamma + (fRes*0.1) );
		    oCV_Mono.setLambda( fMatchLambda );
		    oCV_Mono.setGamma( fMatchGamma );
		} else {
		    oCV_Mono.setLambda( fMatchLambda );
		    oCV_Mono.setGamma( fMatchGamma );
		}
		// rmsd threshold for the uniqueness of a solution
		Real64 fUnique = 2.0;

		oCV_Mono.setWildcards( (unsigned int)((Real64)(iWildcard) * ((Real64)(oCV_Mono.size()) / (Real64)(iNumCV))) );
                oCV_Mono.setSkipPenalty( 1.0 );
		oCV_Mono.setRuns( iMatchRuns );
		oCV_Mono.setSimple( bSimple );
                oCV_Mono.setZoneSize( 3 );

		C_FIT << "Lambda = " << oCV_Mono.getLambda() << endl;
		C_FIT << "Gamma = " << oCV_Mono.getGamma() << endl;
		C_FIT << "Matching Zone Size = " << oCV_Mono.getZoneSize() << endl;
		C_FIT << "Number of wildcards = " << oCV_Mono.getWildcards() << endl;
		C_FIT << "Number of runs = " << oCV_Mono.getRuns() << endl;

		// run matching routine
		vector< svt_matchResult<Real64> > aMatches;

		// runs
		//if (bCOAAnchors)
		{
                    oCV_Mono.setNextPointCOA( false );
		    iTime = svt_getToD();
		    oCV_Mono.match( oCV_Olig, aMatches );
		    iTime = svt_getToD() - iTime;
                    cleanResults( oCV_Mono, aMatches );

		    C_FIT << "Runtime: " << (Real64)(iTime) * 1.0E-3 << endl;

		    if (aMatches.size() > 0)
		    {
                        svt_matchResult<Real64> oBest = getBestResult( oCV_Mono, oCV_Olig, oMono, oOlig, aMatches, OUTLIER );

			// rmsd of the codebooks
			svt_point_cloud< svt_vector4<Real64> > oTransCV = oBest.getMatrix() * oCV_Mono;
			fCV_RMSD = aMatches[0].getScore(); //oTransCV.rmsdNN( oCV_Olig );

			// rmsd of the structures
			svt_point_cloud_pdb< svt_vector4<Real64> > oTransMono = oBest.getMatrix() * oMono;
			if (fNoise == 0.0)
			    fPDB_RMSD = oTransMono.rmsd( oOlig );
			else
			    fPDB_RMSD = fCV_RMSD;

			if (fCV_RMSD < fMinCVRMSD)
			{
			    fMinRMSD = fPDB_RMSD;
			    fMinCVRMSD = fCV_RMSD;
			    oWinnerMat = oBest.getMatrix();
			    fHausdorff = oTransMono.hausdorff( oOlig );
			    fAvgTemp = oCV_Olig.getAvgTempFact();
			    fMaxTemp = oCV_Olig.getMaxTempFact();
			}

		    } else {

			fCV_RMSD = 10.0;
			fPDB_RMSD = 10.0;
		    }

		    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;
		    C_FIT << "// AFTER COAANCH: (NextPoint CLS)" << endl;
		    C_FIT << "// Final RMSD between the two structures: " << fMinRMSD << endl;
		    C_FIT << "// Final RMSD between the two codebooks: " << fMinCVRMSD << endl;
		    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;

		}

		{
                    oCV_Mono.setNextPointCOA( true );
		    oCV_Mono.match( oCV_Olig, aMatches );
                    cleanResults( oCV_Mono, aMatches );

		    if (aMatches.size() > 0)
		    {
                        svt_matchResult<Real64> oBest = getBestResult( oCV_Mono, oCV_Olig, oMono, oOlig, aMatches, OUTLIER );

			// rmsd of the codebooks
			svt_point_cloud< svt_vector4<Real64> > oTransCV = oBest.getMatrix() * oCV_Mono;
			fCV_RMSD = aMatches[0].getScore();

			// rmsd of the structures
			svt_point_cloud_pdb< svt_vector4<Real64> > oTransMono = oBest.getMatrix() * oMono;
			if (fNoise == 0.0)
			    fPDB_RMSD = oTransMono.rmsd( oOlig );
			else
			    fPDB_RMSD = fCV_RMSD;

			if (fCV_RMSD < fMinCVRMSD)
			{
			    fMinRMSD = fPDB_RMSD;
			    fMinCVRMSD = fCV_RMSD;
			    oWinnerMat = oBest.getMatrix();
			    fHausdorff = oTransMono.hausdorff( oOlig );
			    fAvgTemp = oCV_Olig.getAvgTempFact();
			    fMaxTemp = oCV_Olig.getMaxTempFact();
			}

		    } else {

			fCV_RMSD = 10.0;
			fPDB_RMSD = 10.0;
		    }

		    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;
		    C_FIT << "// AFTER COAANCH: (NextPoint COA)" << endl;
		    C_FIT << "// Final RMSD between the two structures: " << fMinRMSD << endl;
		    C_FIT << "// Final RMSD between the two codebooks: " << fMinCVRMSD << endl;
		    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;

		}


		//else
		{

		    for(i=0; i<iMatchRuns; i++)
		    {
			if (oDists.size() > i+2)
			{
			    oCV_Mono.match( oCV_Olig, aMatches, oDists[i +0].getIndex(), oDists[i +1].getIndex(), oDists[i +2].getIndex() );
			    cleanResults( oCV_Mono, aMatches );

			    if (aMatches.size() > 0)
			    {
				svt_matchResult<Real64> oBest = getBestResult( oCV_Mono, oCV_Olig, oMono, oOlig, aMatches, OUTLIER );

				// rmsd of the codebooks
				svt_point_cloud< svt_vector4<Real64> > oTransCV = oBest.getMatrix() * oCV_Mono;
				fCV_RMSD = aMatches[0].getScore();

				// rmsd of the structures
				svt_point_cloud_pdb< svt_vector4<Real64> > oTransMono = oBest.getMatrix() * oMono;
                                if (fNoise == 0.0)
				    fPDB_RMSD = oTransMono.rmsd( oOlig );
				else
                                    fPDB_RMSD = fCV_RMSD;

				if (fCV_RMSD < fMinCVRMSD)
				{
				    fMinRMSD = fPDB_RMSD;
				    fMinCVRMSD = fCV_RMSD;
				    oWinnerMat = oBest.getMatrix();
				    fHausdorff = oTransMono.hausdorff( oOlig );
				    fAvgTemp = oCV_Olig.getAvgTempFact();
				    fMaxTemp = oCV_Olig.getMaxTempFact();
				}

			    } else {

				fCV_RMSD = 10.0;
				fPDB_RMSD = 10.0;
			    }

			}

		    } // Matchruns

		    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;
		    C_FIT << "// AFTER STABANCH: " << endl;
		    C_FIT << "// Final RMSD between the two structures: " << fMinRMSD << endl;
		    C_FIT << "// Final RMSD between the two codebooks: " << fMinCVRMSD << endl;
		    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;

		} // bCOAAnchors

	    } // ResRuns

	    C_FIT << "// REFINEMENT: " << endl;
	    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;

	    fprintf( pFile, "%f ", fMinCVRMSD );
	    fprintf( pFile, "%f ", fMinRMSD );

	    ///////////////////////////////////////////////////////////////////////////
	    // Refinement
	    ///////////////////////////////////////////////////////////////////////////

	    svt_matrix4<Real64> oFinalMat = oWinnerMat;

	    // refine initial matrix first
	    svt_point_cloud< svt_vector4<Real64> > oTransCV = oFinalMat * oCV_Mono;

            // CV-based refinement
	    if (iRefine > 0)
	    {
                // now do a cv refinement
		refine( oClusterAlgo, *pMonoMap, *pMap, iNumMono, oFinalMat, fMinCVRMSD, oCV_Mono.size(), iRefine );
	    }
	    
	    vector< svt_matrix4< Real64 > > oFinalMats;
	    oFinalMats.push_back(oFinalMat);

            // powell-correlation-based refinement
	    if (bPowell)
	    {

		C_FIT << "Refinement Step:" << endl;
		svt_matrix4<Real64> oVolMat;
		oVolMat.translate( pMap->getGridX(), pMap->getGridY(), pMap->getGridZ() );

		if (bLaplacian == true || bPowLapl == true)
		{
		    delete pMap;
		    pMap = oOlig.blur( fWidth, fRes );
		    pMap->applyLaplacian();
		}
		
		vector<svt_point_cloud_pdb<svt_vector4 <Real64> > > oPDBs;
		oPDBs.push_back( oMono );
		
		vector< svt_matrix4< Real64 > > oMats,oFinalMats;
		oMats.push_back( oWinnerMat );

		vector<double> oPowVec;
		if (bFastPowell)
		{
		    //svt_powell_cc oPowell( oMono_Powell, oWinnerMat, *pMap, oVolMat );
		    svt_powell_cc oPowell( oPDBs, oMats, *pMap, oVolMat );
		    oPowell.setTolerance( 1.0E-9 );
		    oPowell.setMaxIter( 50 );
		    oPowell.setLaplacian( bPowLapl );
		    oPowell.setResolution( fRes );
		    oPowell.setFast(true);
		    oPowell.maximize();
		    oPowVec = oPowell.getVector();
		    oFinalMats = oPowell.getTrans();

		} else {

		    svt_powell_cc oPowell( oPDBs, oMats, *pMap, oVolMat );
		    oPowell.setTolerance( 1.0E-6 );
		    oPowell.setMaxIter( 50 );
		    oPowell.setLaplacian( bPowLapl );
		    oPowell.setResolution( fRes );
		    oPowell.maximize();
		    oPowVec = oPowell.getVector();
		    oFinalMats = oPowell.getTrans();
		}

		C_FIT << "Result of refinement:" << endl;
		C_FIT << "Translation: (" << oPowVec[0] << ", " << oPowVec[1] << ", " << oPowVec[2] << ")" << endl;
		C_FIT << "Rotation: (" << oPowVec[3] << ", " << oPowVec[4] << ", " << oPowVec[5] << ")" << endl;

	    }

	    svt_point_cloud_pdb< svt_vector4<Real64> > oTransMono = oFinalMats[0] * oMono;
            if (fNoise == 0.0)
		fMinRMSD = oTransMono.rmsd( oOlig );
	    else
		fMinRMSD = oTransMono.rmsd_NN( oOlig );
	    oTransCV = oFinalMats[0] * oCV_Mono;
	    fMinCVRMSD = oTransCV.rmsdNN( oCV_Olig );

	    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;
	    C_FIT << "// AFTER REFINEMENT: " << endl;
	    C_FIT << "// Final RMSD between the two codebooks : " << fMinCVRMSD << endl;
	    C_FIT << "// Final RMSD between the two structures: " << fMinRMSD << endl;
	    C_FIT << "/////////////////////////////////////////////////////////////////" << endl;

            fprintf( pFile, "%f ", fMinRMSD );
	    fprintf( pFile, "%f ", (Real64)(iTime) * 1.0E-3 );
	    fprintf( pFile, "%i ", oCV_Mono.getNumSeeds() );

	    if (fNoise == 0.0)
	    {
		fprintf( pFile, "%f ", fMaxTemp );
		fprintf( pFile, "%f ", fAvgTemp );

	    } else {

		fprintf( pFile, "%f ", fNoiseMax);
		fprintf( pFile, "%f ", fNoiseVar );
	    }

	    fprintf( pFile, "%f ", fHausdorff );
            fprintf( pFile, "%f %f\n", oCV_Mono.getAvgTempFact(), oCV_Olig.getAvgTempFact() );
            fflush( pFile );

	    ///////////////////////////////////////////////////////////////////////////
	    // Cleanup
	    ///////////////////////////////////////////////////////////////////////////

            C_FIT << "Cleanup, deleting maps" << endl;

            if (pMap != NULL)
		delete pMap;
            if (pMonoMap != NULL)
		delete pMonoMap;

	} // resolution

    } // codebook vectors

    fclose( pFile );

    return 0;
}
