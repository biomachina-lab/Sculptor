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

#define QPDB cout << svt_trimName("extractmodel")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: extractmodel <pdb file> <model number> <output file>" << endl;
	exit(1);
    }

    // load pdb file
    QPDB << "loading pdb file: " << argv[1] << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );

    QPDB << "loaded " << oPDB.size() << " atoms" << endl;

    unsigned int iModel = atoi( argv[2] );

    // extracting chain
    QPDB << "extracting model: " << iModel << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oNewPDB;
    for(unsigned int i=0; i<oPDB.size(); i++)
	if ( oPDB.atom(i).getModel() == iModel )
	    oNewPDB.addAtom( oPDB.atom(i), oPDB[i] );

    oNewPDB.calcAtomModels();

    QPDB << "found " << oNewPDB.size() << " atoms in model " << iModel << "." << endl;

    // save new file
    QPDB << "saving pdb file: " << argv[3] << endl;
    oNewPDB.writePDB( argv[3] );

    QPDB << "done." << endl;
};
