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

#define QPDB cout << svt_trimName("extractchain")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: extractchain <pdb file> <chain id> <output file>" << endl;
	exit(1);
    }

    // load pdb file
    QPDB << "loading pdb file: " << argv[1] << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );

    QPDB << "loaded " << oPDB.size() << " atoms" << endl;

    char cLower = (char)tolower( argv[2][0] );
    char cUpper = (char)toupper( argv[2][0] );

    // extracting chain
    QPDB << "extracting chain: " << cUpper << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oNewPDB;
    for(unsigned int i=0; i<oPDB.size(); i++)
	if ( oPDB.atom(i).getChainID() == cLower || oPDB.atom(i).getChainID() == cUpper )
	    oNewPDB.addAtom( oPDB.atom(i), oPDB[i] );

    oNewPDB.calcAtomModels();

    QPDB << "found " << oNewPDB.size() << " atoms in chain " << cUpper << "." << endl;

    // save new file
    QPDB << "saving pdb file: " << argv[3] << endl;
    oNewPDB.writePDB( argv[3] );

    QPDB << "done." << endl;
};
