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

int main(int argc, char* argv[])
{
    svt_point_cloud_pdb< svt_vector4<Real64> > oPDB;
    unsigned int i,j;

    QPDB << "loading psf file..." << endl;

    oPDB.loadPSF( argv[1] );

    QPDB << "loading dcd file..." << endl;
    
    oPDB.loadDCD(argv[2]);

    for(j=0; j<oPDB.getMaxTimestep(); j++)
    {
        oPDB.setTimestep( j );
        for(i=0; i<oPDB.size(); i++)
            cout << oPDB[i].x() << " " << oPDB[i].y() << " " << oPDB[i].z() << " " << oPDB.atom(i).getMass() << endl;
    }
};
