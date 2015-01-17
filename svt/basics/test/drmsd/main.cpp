/***************************************************************************
                          main
                          ----
    begin                : 02/12/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_point_cloud_atom.h>
#include <svt_clustering_trn.h>
#include <svt_vector4.h>

#define RMSD cout << svt_trimName("dRMSD")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: drmsd <pdb file a> <pdb file b>" << endl;
	return 1;
    }

    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_A;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_B;

    oPDB_A.loadPDB(argv[1]);
    oPDB_B.loadPDB(argv[2]);
    
    
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_A_trace = oPDB_A.getTrace();
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_B_trace = oPDB_B.getTrace();
    
    RMSD << "rmsd:            " << oPDB_A_trace.rmsd( oPDB_B_trace, true ) << endl;
    RMSD << "drmsd:           " << oPDB_A_trace.drmsd( oPDB_B_trace ) << endl;
    RMSD << "hausedorf        " << oPDB_A_trace.hausdorff( oPDB_B_trace ) << endl;
    

    return 0;
};
