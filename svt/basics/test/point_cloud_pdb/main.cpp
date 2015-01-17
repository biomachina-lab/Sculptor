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

#define QPDB cout << svt_trimName("point_cloud_pdb")

int main(int args, const char* argv[])
{
    // simple memory leak test - add 10000 atoms and delete them, 100 times.

      //  unsigned int i,j;

 /*   for(j=0;j<100;j++)
    {
        QPDB << "j: " << j << endl;

        for(i=0;i<10000;i++)
        {
            svt_point_cloud_atom oAtom;
            svt_vector4<Real64> oVec;
            oPDB.addAtom( oAtom, oVec );
        }
        oPDB.deleteAllAtoms();
    }

    exit(1);*/

    if (args != 2)
    {
        QPDB << "Error, need 1 PDBID" << endl;
	return 0;
    }
    else
    {
	svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
	const char* PDBID = argv[1];

        QPDB << "fetching pdb file..." <<endl;

        oPDB.fetchPDB(PDBID);
   
    // load the pdb file

  //  QPDB << "loading pdb file..." << endl;
    
  //  oPDB.loadPDB("2rec.pdb");

    // first test: info commands

 /*   QPDB << endl;

    oPDB.calcAllLists();

    QPDB << "Atom Names:" << endl;
    vector<const char*>& aAtomNames = oPDB.getAtomNames();
    for(i=0; i<aAtomNames.size(); i++)
        QPDB << i << " - " << aAtomNames[i] << endl;

    QPDB << "Atom Residue Names:" << endl;
    vector<const char*>& aAtomResnames = oPDB.getAtomResnames();
    for(i=0; i<aAtomResnames.size(); i++)
        QPDB << i << " - " << aAtomResnames[i] << endl;

    QPDB << "Atom Residue Sequence Numbers:" << endl;
    vector<int>& aAtomResidueSeqs = oPDB.getAtomResidueSeqs();
    for(i=0; i<aAtomResidueSeqs.size(); i++)
        QPDB << i << " - " << aAtomResidueSeqs[i] << endl;

    QPDB << "Atom Chain IDs:" << endl;
    vector<char>& aAtomChainIDs = oPDB.getAtomChainIDs();
    for(i=0; i<aAtomChainIDs.size(); i++)
        QPDB << i << " - " << aAtomChainIDs[i] << endl;

    QPDB << "Atom Segment IDs:" << endl;
    vector<const char*>& aAtomSegmentIDs = oPDB.getAtomSegmentIDs();
    for(i=0; i<aAtomSegmentIDs.size(); i++)
        QPDB << i << " - " << aAtomSegmentIDs[i] << endl;

    oPDB.calcSecStruct();

    QPDB << "Atom Sec. Struct. Ds:" << endl;
    vector<char>& aAtomSecStructIDs = oPDB.getAtomSecStructIDs();
    for(i=0; i<aAtomSecStructIDs.size(); i++)
        QPDB << i << " - " << aAtomSecStructIDs[i] << endl;

    QPDB << endl;

    QPDB << "Calc. Bonds" << endl;

    oPDB.calcBonds();

    // test: psf file reading

    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB_B;

    QPDB << "loading pdb file..." << endl;
    oPDB_B.loadPDB("turbotest.pdb");
    QPDB << "loading psf file..." << endl;
    oPDB_B.loadPSF("turbotest.psf");

    // second test: clustering

    svt_clustering_trn< svt_vector4<Real64> > oTRN;

    QPDB << "adding codebook vectors" << endl;

    for(i=0; i<20; i++)
    {
        svt_vector4<Real64> oVec = oPDB.sample();
	oTRN.addVector( oVec );
    }

    QPDB << "clustering..." << endl;

    oTRN.train( oPDB );

    QPDB << "writing file to disk..." << endl;
*/
    if (oPDB.size() > 0)
        oPDB.writePDB(PDBID);

    QPDB << "end test." << endl;
    }
};
