/***************************************************************************
                          mtrn
                          ----
    begin                : 07/09/2007
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
#include <svt_clustering_mtrn.h>

#define MTRN cout << svt_trimName("mtrn")

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
	MTRN << "usage: mtrn <input situs file> <input monomer pdb file> <number of monomers> <output file>" << endl;
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
    MTRN << "Min density: " << oOlig.getMinDensity() << endl;
    MTRN << "Max density: " << oOlig.getMaxDensity() << endl;

    // load monomer structure
    svt_point_cloud_pdb< svt_vector4<Real64> > oMono;
    MTRN << "Loading monomer pdb file: " << argv[2] << endl;
    oMono.loadPDB( argv[2] );
    oMono.setIgnoreWater( false );

    // number of monomers
    unsigned int iNumMono = atoi( argv[3] );
    MTRN << "Number of monomers to be detected: " << iNumMono << endl;

    // set parameters for the clustering
    svt_clustering_mtrn< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setMaxstep( 10000 );
    oClusterAlgo.setLambda( 1000.0f, 1.0f );
    oClusterAlgo.setEpsilon( 1.5f, 0.01f );
    //svt_sgenrand( svt_getToD() );
    svt_vector4< Real64 > oVec;
    svt_point_cloud_atom oAtom;
    for(unsigned int i=0; i<iNumMono; i++)
    {
        svt_point_cloud_pdb< svt_vector4<Real64> > oTmp;
        for(unsigned j=0; j<oMono.size(); j++)
        {
            oAtom.setPDBIndex( j );
            oVec = oOlig.sample();

            oTmp.addAtom( oAtom, oVec );
        }

        oClusterAlgo.addSystem( oMono, oTmp );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Clustering of map
    ///////////////////////////////////////////////////////////////////////////

    MTRN << "Cluster analysis of the oligomer..." << endl;

    oClusterAlgo.train( oOlig );

    svt_point_cloud_pdb< svt_vector4< Real64> > oResult;

    for(unsigned int i=0; i<iNumMono; i++)
    {
        svt_point_cloud< svt_vector4< Real64> > oTmpPC = oClusterAlgo.getCodebook( i );
        svt_point_cloud_pdb< svt_vector4< Real64> > oTmp;

        for(unsigned int j=0; j<oTmpPC.size(); j++)
            oTmp.addAtom( oAtom, oTmpPC[i] );

        oResult.append( oTmp, i );
    }

    oResult.writePDB( argv[4] );

    return 0;
}
