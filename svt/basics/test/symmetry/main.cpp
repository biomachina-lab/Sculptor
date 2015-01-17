/***************************************************************************
                          main
                          ----
    begin                : 06/18/08
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_exception.h>
#include <svt_powell_saxs.h>

#define SYMM cout << svt_trimName("symmetry")

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 2)
    {
	SYMM << "usage: symmetry <number> <centerx> <centerz> <centerz> <pointx> <pointy> <pointz> <input pdb file> <output pdb file> " << endl;
	return 1;
    }
    
    SYMM << " Input    : " << argv[9] << endl;
    SYMM << " Output    : " << argv[8] << endl;
        
    svt_point_cloud_pdb<svt_vector4<Real64 > > oPDBMono,oPDBOlig;
    
    try
    {
        oPDBMono.loadPDB( argv[8] );
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
        return 1;
    }
    
    svt_vector4<Real64> oCenter;
    oCenter.x(atof(argv[2]));
    oCenter.y(atof(argv[3]));
    oCenter.z(atof(argv[4]));
    
   // the axis the rotation is the line define by center an point
   // the point is found on the axis of rotation
   //
    svt_vector4<Real64> oPoint;
    oPoint.x(atof(argv[5]));
    oPoint.y(atof(argv[6]));
    oPoint.z(atof(argv[7]));

    svt_vector4<Real64> oRotAxis;
    oRotAxis = oPoint - oCenter;


    svt_vector4<Real64> oPdbCenter = oPDBMono.coa();

    svt_matrix4<Real64> oMat;
    oMat.loadIdentity();
    
    oMat.translate(-oCenter);
    oPDBMono = oMat*oPDBMono;

    oMat.loadIdentity();
    oMat.rotate(2,atan2(oRotAxis.x(),oRotAxis.y()) );
    oPDBMono = oMat*oPDBMono;
    oPoint = oMat*oRotAxis;
    oMat.loadIdentity();
    oMat.rotate(0,atan2(oPoint.y(), oPoint.z()) );
    oPDBMono = oMat*oPDBMono;

    oPDBOlig = oPDBMono.applySymmetry( atoi(argv[1]), SYMMETRY_C,'z', 0, 0);

    oMat.loadIdentity();
    oMat.rotate(0,-atan2(oPoint.y(), oPoint.z()) );
    oPDBOlig = oMat*oPDBOlig;
    
    oMat.loadIdentity();
    oMat.rotate(2,-atan2(oRotAxis.x(),oRotAxis.y()) );
    oPDBOlig = oMat*oPDBOlig;


    oMat.loadIdentity();
    oMat.translate(oCenter);
    oPDBOlig = oMat*oPDBOlig;
    oPDBOlig.writePDB( argv[9] );
    
    return 1;
}
