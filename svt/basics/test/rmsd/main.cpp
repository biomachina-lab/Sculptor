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

#define RMSD cout << svt_trimName("rmsd")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: rmsd <pdb file a> <pdb file b>" << endl;
	return 1;
    }

    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_A;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_B;

    oPDB_A.loadPDB(argv[1]);
    oPDB_B.loadPDB(argv[2]);

    RMSD << "rmsd:            " << oPDB_A.rmsd( oPDB_B, true ) << endl;
    RMSD << "rmsd (backbone): " << oPDB_A.rmsd( oPDB_B, true, BACKBONE ) << endl;
    RMSD << "rmsd (trace):    " << oPDB_A.rmsd( oPDB_B, true, TRACE ) << endl;

    return 0;
};
