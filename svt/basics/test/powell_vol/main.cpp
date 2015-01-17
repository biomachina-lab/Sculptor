/***************************************************************************
                          main
                          ----
    begin                : 04/21/10
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_point_cloud_atom.h>
#include <svt_clustering_trn.h>
#include <svt_vector4.h>
#include <svt_powell_vol.h>
#include <svt_config.h>

#define POWELL cout << svt_trimName("powell_multi_vol")

int main(int, char* argv[])
{ 
    //
    // Load the map
    //
    POWELL << "Load Tar:" << argv[1] << endl;
    svt_volume<Real64> oTar;
    oTar.loadSitus(argv[1]);	
    
    //
    // Load the map
    //
    POWELL << "Load Probe:" << argv[2] << endl;
    svt_volume<Real64> oProbe;
    oProbe.loadSitus(argv[2]);

    svt_matrix4<Real64> oMat;
    oMat.loadIdentity();	
       
    svt_powell_vol*  pPowell = new svt_powell_vol( oTar, oProbe, oMat);
    pPowell->setMaxIter( 100 );
    pPowell->setTolerance( 1e-7 );
    
    pPowell->maximize();
    
    POWELL << "Time of a single score calculation: " << pPowell->getScoreTime() << endl;

    svt_volume< Real64 > oVol = pPowell->getVolume();
    oVol.saveSitus( argv[3] );
};

