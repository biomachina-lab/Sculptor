/***************************************************************************
                          main
                          ----
    begin                : 06/14/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_point_cloud_atom.h>
#include <svt_clustering_trn.h>
#include <svt_vector4.h>
#include <svt_powell_cc.h>

#define POWELL cout << svt_trimName("powell")

int main(int, char* argv[])
{
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    vector< svt_point_cloud_pdb< svt_vector4<Real64> > > oPDBs;
    oPDB.loadPDB( argv[1] );
    oPDBs.push_back( oPDB );
    
    svt_matrix4<Real64> oPDBMat;
    vector<svt_matrix4<Real64> > oPDBMats;
    oPDBMats.push_back( oPDBMat );

    // load situs map
    svt_volume<Real64> oVol;
    svt_matrix4<Real64> oVolMat = oVol.load( argv[2] );
    
    

    svt_powell_cc oPowell( oPDBs, oPDBMats, oVol, oVolMat );
    oPowell.setFast( false );
    oPowell.setBlur( true );
    oPowell.setTolerance(1e-6);
    oPowell.setMaxIter2Minimize(5);
    oPowell.maximize();

    POWELL << "Number of score calculation: " << oPowell.getScoreCount() << endl;
    POWELL << "Time of a single score calculation: " << oPowell.getScoreTime() << endl;

    vector< svt_matrix4<Real64> > oOutMat = oPowell.getTrans();
    oPDB = oOutMat[0] * oPDB;

    oPDB.writePDB( argv[3] );
 
    //for comparison
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDBtarget;
    oPDBtarget.loadPDB(argv[4]);

    Real64 fRMSD = oPDBtarget.rmsd(oPDB,false, ALL);
    POWELL << "RMSD: " << fRMSD << endl;
};
