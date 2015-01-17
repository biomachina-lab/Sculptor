/***************************************************************************
                          fam
                          ---
    begin                : 08/23/2007
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
#include <svt_clustering_fam.h>

#define FAM cout << svt_trimName("fam")

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
	FAM << "usage: fam <input situs file> <input pdb file> <input psf file> <output file map>" << endl;
	return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Load files
    ///////////////////////////////////////////////////////////////////////////

    // load oligomer situs file
    svt_volume< Real64 > oOlig;
    oOlig.load( argv[1] );
    oOlig.setCutoff( 0.0 );

    // some stats about the volume
    FAM << "Min density: " << oOlig.getMinDensity() << endl;
    FAM << "Max density: " << oOlig.getMaxDensity() << endl;

    ///////////////////////////////////////////////////////////////////////////
    // First step: Cluster the PDB File
    ///////////////////////////////////////////////////////////////////////////

    svt_point_cloud_pdb< svt_vector4<Real64> > oCV;
    oCV.loadPDB( argv[2] );
    oCV.loadPSF( argv[3] );

    ///////////////////////////////////////////////////////////////////////////
    // Second step: Cluster the MAP
    ///////////////////////////////////////////////////////////////////////////

    // set parameters for the clustering
    svt_clustering_fam< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setMaxstep( 100000 );
    //oClusterAlgo.setLambda( 1.0f, 0.01f );
    oClusterAlgo.setEpsilon( 0.01f, 0.000000005f );
    svt_sgenrand( svt_getToD() );

    oClusterAlgo.setSystem( oCV );

    ///////////////////////////////////////////////////////////////////////////
    // Clustering of map
    ///////////////////////////////////////////////////////////////////////////

    FAM << "Cluster analysis of the oligomer..." << endl;

    oClusterAlgo.train( oOlig );

    svt_point_cloud< svt_vector4< Real64> > oResult = oClusterAlgo.getCodebook( );
    FAM << "Saving feature vectors for map file " << argv[4] << endl;
    oResult.writePDB( argv[4] );

    return 0;
}
