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
#include <svt_vector4.h>

#define QPDB cout << svt_trimName("extractca")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: extractca <pdb file> <output file>" << endl;
	exit(1);
    }

    // load pdb file
    QPDB << "loading pdb file: " << argv[1] << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );

    QPDB << "loaded " << oPDB.size() << " atoms" << endl;

    // extracting ca
    svt_point_cloud_pdb< svt_vector4<Real64> > oNewPDB;
    for(unsigned int i=0; i<oPDB.size(); i++)
	if ( oPDB.atom(i).getName()[0] == 'C' && oPDB.atom(i).getRemoteness() == 'A' )
	    oNewPDB.addAtom( oPDB.atom(i), oPDB[i] );

    oNewPDB.calcAtomModels();

    QPDB << "found " << oNewPDB.size() << " ca atoms." << endl;

    // save new file
    QPDB << "saving pdb file: " << argv[2] << endl;
    oNewPDB.writePDB( argv[2] );

    QPDB << "done." << endl;
};
