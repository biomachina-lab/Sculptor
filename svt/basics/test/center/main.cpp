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

#define QPDB cout << svt_trimName("center")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: center <pdb file> <output file>" << endl;
	exit(1);
    }

    // load pdb file
    QPDB << "loading pdb file: " << argv[1] << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );

    QPDB << "loaded " << oPDB.size() << " atoms" << endl;

    // center mol
    QPDB << "centering... " << endl;
    
    svt_matrix4 <Real64> oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-oPDB.coa());

    int iCurrentTimestep = oPDB.getTimestep();
    for(int iTS=0; iTS<oPDB.getMaxTimestep(); iTS++)
    {
	oPDB.setTimestep( iTS );

	for( unsigned int i=0; i<oPDB.size(); i++)
	{
	    oPDB[i] = oTranslation * oPDB[i];
	}
    }
    oPDB.setTimestep( iCurrentTimestep );


    // save new file
    QPDB << "saving pdb file: " << argv[2] << endl;
    oPDB.writePDB( argv[2] );

    QPDB << "done." << endl;
};
