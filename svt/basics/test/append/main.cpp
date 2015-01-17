/***************************************************************************
                          main
                          ----
    begin                : 06/08/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_point_cloud_atom.h>
#include <svt_vector4.h>

#define QPDB cout << svt_trimName("append")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: append <pdb files> <model number> <output file>" << endl;
	exit(1);
    }

    // load pdb file
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_A;
    oPDB_A.loadPDB( argv[1] );
    unsigned int iModelNum = atoi( argv[argc-2] );

    for(unsigned int i=2; i<(unsigned int)(argc-2); i++)
    {
	// load pdb file
	svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_B;
	oPDB_B.loadPDB( argv[i] );

	// combine the two files
	oPDB_A.append( oPDB_B, iModelNum + i - 1 );
	QPDB << "appended file has " << oPDB_A.size() << " atoms." << endl;
    }

    // save new file
    QPDB << "saving pdb file: " << argv[argc-1] << endl;
    oPDB_A.writePDB( argv[argc-1] );

    QPDB << "done." << endl;
};
