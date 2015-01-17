/***************************************************************************
                          match
                          -----
    begin                : 06/13/2005
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
#include <svt_config.h>

#define MATCH cout << svt_trimName("match")

///////////////////////////////////////////////////////////////////////////
// Aux. Classes and Routines
///////////////////////////////////////////////////////////////////////////

/**
 * Cleanup the result stack, remove abiguous results.
 */
void cleanResults( svt_point_cloud_pdb< svt_vector4<Real64> >& oCV_Mono, vector< svt_matchResult< Real64 > >& rResults )
{
    vector< svt_matchResult< Real64 > > aFinal;
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

    MATCH << "After cleanup " << aFinal.size() << endl;
    rResults = aFinal;
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
    if (argc < 4)
    {
	MATCH << "usage: match [options] <file a> <file b> <output file>" << endl;
	MATCH << endl;
	MATCH << "options: " << endl;
	MATCH << " -n nearest neighbor search only" << endl;
	MATCH << " -l <lambda>  (default: 10.0) Anchor match tolerance " << endl;
	MATCH << " -g <gamma>   (default:  7.0) Neighbor size " << endl;
	MATCH << " -r <runs>    (default:  3)   Number of runs with different anchor points " << endl;
	MATCH << " -w <wc>      (default:  0)   Number of wildcard matches" << endl;
	MATCH << " -p <penalty> (default:  1.0) Penalty for a wildcard match" << endl;
	MATCH << " -m <monomers>(default:  1)   Number of copies in file b" << endl;
	return 1;
    }

    //
    // parameters
    //
    bool bNearest           = false;
    Real64 fLambda          = 10.0;
    Real64 fGamma           = 7.0;
    unsigned int iWildcard  = 0;
    Real64 fWCPenalty       = 1.0;
    unsigned int iRuns      = 3;
    unsigned int iNumMono   = 1;

    char* pA_Fname = NULL;
    char* pB_Fname = NULL;
    char* pOut_Fname = NULL;

    unsigned int i;

    // parse command-line
    for(i=1; i<argc; i++)
    {
	// is this an option or a filename
	if (argv[i][0] == '-' && argv[i][1] != 0)
	{
	    switch(argv[i][1])
	    {
	    case 'n':
		bNearest = true;
                MATCH << "Performing nearest neighbor search only!" << endl;
		break;

		// Runs
	    case 'r':
		if (argc >= i+1)
		{
		    iRuns = atoi( argv[i+1] );
		    MATCH << "Runs: " << iRuns << endl;

                    i++;
		}
		break;

		// Lambda
	    case 'l':
		if (argc >= i+1)
		{
		    fLambda = atof( argv[i+1] );
		    MATCH << "Lambda: " << fLambda << endl;

                    i++;
		}
		break;

		// Gamma
	    case 'g':
		if (argc >= i+1)
		{
		    fGamma = atof( argv[i+1] );
		    MATCH << "Gamma: " << fGamma << endl;

                    i++;
		}
		break;

		// Wildcard matches
	    case 'w':
		if (argc >= i+1)
		{
		    iWildcard = atoi( argv[i+1] );
		    MATCH << "Wildcard: " << iWildcard << endl;

                    i++;
		}
                break;

		// Penalty
	    case 'p':
		if (argc >= i+1)
		{
		    fWCPenalty = atof( argv[i+1] );
		    MATCH << "Wildcard Penalty: " << fWCPenalty << endl;

                    i++;
		}
		break;

		// Monomers
	    case 'm':
		if (argc >= i+1)
		{
		    iNumMono = atoi( argv[i+1] );
		    MATCH << "Monomers: " << iNumMono << endl;

                    i++;
		}
                break;

	    }

	} else {

	    if (pA_Fname == NULL)
	    {
		pA_Fname = argv[i];

		MATCH << "Input-file A: " << pA_Fname << endl;

	    } else if (pB_Fname == NULL)
	    {
		pB_Fname = argv[i];

		MATCH << "Input-file B: " << pB_Fname << endl;

	    } else if (pOut_Fname == NULL)
	    {
		pOut_Fname = argv[i];

		MATCH << "Output-file: " << pOut_Fname << endl;
	    }
	}
    }


    svt_config oConf( argv[1] );

    // load monomer structure
    svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Mono;
    oCV_Mono.loadPDB( pA_Fname );

    // load oligomer structure
    svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Olig;
    oCV_Olig.loadPDB( pB_Fname );

    vector< svt_matchResult<Real64> > aMatches;

    ///////////////////////////////////////////////////////////////////////////
    // Nearest Neighbor Matching
    ///////////////////////////////////////////////////////////////////////////

    if (bNearest)
    {
	MATCH << endl;
	MATCH << "Result: " << endl;
        MATCH << endl;
        MATCH;

        svt_point_cloud_pdb< svt_vector4<Real64> > oSorted;
	int iMatch;
	for(i=0; i<oCV_Mono.size(); i++)
	{
            iMatch = oCV_Olig.nearestNeighbor( oCV_Mono[i] );
	    printf("%02i", iMatch );
	    if ( i<oCV_Mono.size() -1)
		cout << ",";

            if (pOut_Fname != NULL)
		oSorted.addAtom( oCV_Olig.atom( iMatch ), oCV_Olig[ iMatch ] );

	}

        if (pOut_Fname != NULL)
	    oSorted.writePDB( pOut_Fname );

	cout << endl;

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Matching
    ///////////////////////////////////////////////////////////////////////////

    // set parameter for matching algorithm
    //oCV_Mono.setLambda( fCV_Olig_RMS + (8.0*fCV_Mono_RMS) );
    //oCV_Mono.setGamma(  fCV_Olig_RMS + (0.5*fCV_Mono_RMS) );

    oCV_Mono.setLambda( fLambda );
    oCV_Mono.setGamma( fGamma );

    oCV_Mono.setWildcards( (unsigned int)((Real64)(iWildcard)) );
    oCV_Mono.setSkipPenalty( 1.0 );
    oCV_Mono.setRuns( iRuns );
    oCV_Mono.setZoneSize( 3 );

    MATCH << "Lambda = " << oCV_Mono.getLambda() << endl;
    MATCH << "Gamma = " << oCV_Mono.getGamma() << endl;
    MATCH << "Matching Zone Size = " << oCV_Mono.getZoneSize() << endl;
    MATCH << "Number of wildcards = " << oCV_Mono.getWildcards() << endl;
    MATCH << "Number of runs = " << oCV_Mono.getRuns() << endl;

    // run matching routine
    oCV_Mono.setNextPointCOA( false );
    unsigned int iTime = svt_getToD();
    oCV_Mono.match( oCV_Olig, aMatches );
    iTime = svt_getToD() - iTime;
    cleanResults( oCV_Mono, aMatches );

    oCV_Mono.setNextPointCOA( true );
    oCV_Mono.match( oCV_Olig, aMatches );
    cleanResults( oCV_Mono, aMatches );

    MATCH << endl;
    MATCH << "Result: " << endl;
    MATCH << endl;

    for(i=0; i<aMatches.size() && i<5; i++)
        aMatches[i].printMatch();

    MATCH << "Runtime: " << iTime << endl;

    return 0;
}
