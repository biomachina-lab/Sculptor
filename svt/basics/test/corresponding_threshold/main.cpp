/***************************************************************************
                          corrthresh
                          ----------
    begin                : 09/25/2007
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
#include <svt_clustering_trn.h>
#include <svt_clustering_mtrn.h>

#define CORR cout << svt_trimName("fam")

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
	CORR << "usage: corrthresh <situs file A> <situs file B> <threshold value of A>" << endl;
	return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Load files
    ///////////////////////////////////////////////////////////////////////////

    // load oligomer situs file
    svt_volume< Real64 > oVol_A;
    oVol_A.load( argv[1] );

    // load oligomer situs file
    svt_volume< Real64 > oVol_B;
    oVol_B.load( argv[2] );

    // threshold value of A
    Real64 fThresh_A = atof( argv[3] );

    ///////////////////////////////////////////////////////////////////////////
    // Compute threshold
    ///////////////////////////////////////////////////////////////////////////

    // now find corresponding threshold value of B
    Real64 fThresh_B = oVol_B.correspondingISO( oVol_A, fThresh_A );

    CORR << "The corresponding threshold value is: " << fThresh_B << endl;

    return 0;
}
