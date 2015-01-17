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

#define QPDB cout << svt_trimName("applyMat")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: applyMat <pdb file> <output file> <matrix>" << endl;
	exit(1);
    }

    // load pdb file
    QPDB << "loading pdb file: " << argv[1] << endl;
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    oPDB.loadPDB( argv[1] );

    QPDB << "loaded " << oPDB.size() << " atoms" << endl;

    svt_matrix4<Real64> oMat;

    oMat[0][0] = atof(argv[3]);
    oMat[0][1] = atof(argv[4]);
    oMat[0][2] = atof(argv[5]);
    oMat[0][3] = atof(argv[6]);

    oMat[1][0] = atof(argv[7]);
    oMat[1][1] = atof(argv[8]);
    oMat[1][2] = atof(argv[9]);
    oMat[1][3] = atof(argv[10]);

    oMat[2][0] = atof(argv[11]);
    oMat[2][1] = atof(argv[12]);
    oMat[2][2] = atof(argv[13]);
    oMat[2][3] = atof(argv[14]);

    oMat[3][0] = atof(argv[15]);
    oMat[3][1] = atof(argv[16]);
    oMat[3][2] = atof(argv[17]);
    oMat[3][3] = atof(argv[18]);

    oPDB = oMat * oPDB;

    // save new file
    QPDB << "saving pdb file: " << argv[2] << endl;
    oPDB.writePDB( argv[2] );

    QPDB << "done." << endl;
};
