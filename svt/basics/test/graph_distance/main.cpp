/***************************************************************************
                          main
                          ----
    begin                : 05/12/08
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_exception.h>

#define GRA cout << svt_trimName("graph_distance")

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 2)
    {
	GRA << "usage: graph_distance <input pdb> <input psf>" << endl;
	return 1;
    }
    
    GRA << "PDB file: " << argv[1] << endl;
    GRA << "PSF file: " << argv[2] << endl;
    
    // load data
    svt_point_cloud_pdb<svt_vector4<Real64 > > oPDB;
    
    oPDB.loadPDB( argv[1] );
    oPDB.loadPSF( argv[2] );
    
    oPDB.computePSFGraphDistMat();
    
    svt_matrix<Real64> oMat;
    oMat = oPDB.getGraphDists();
    
    GRA << oMat << endl;
    
    
}
