/***************************************************************************
                          main
                          ----
    begin                : 05/12/08
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_exception.h>

#define PDF cout << svt_trimName("pdf")

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 2)
    {
	PDF << "usage: pdf <input pdb file1> <bin width 1> <optional input pdb file2> <optional bin width 2> <optional gnom file>" << endl;
	return 1;
    }
    
    PDF << "PDB file: " << argv[1] << endl;
    PDF << "Bins width:" << argv[2] << endl;
    
    Real64 fWidth1 = atof(argv[2]);
    
    Real64 fWidth2;
    if (argc == 5)
        fWidth2 = atof(argv[4]);
    else
        fWidth2 = fWidth1;
    
    // load data
    svt_point_cloud_pdb<svt_vector4<Real64 > > oPDB1, oPDB2 ;
    
    try
    {
        oPDB1.loadPDB( argv[1] );
        
        if (argc >= 4)
            oPDB2.loadPDB( argv[3] );
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
        return 1;
    }

    unsigned long iTime = svt_getToD();
    oPDB1.calcPairDistribution( fWidth1, false );
    float fDiff = (float)(svt_getToD() - iTime) / 1000.0f;

    svt_pair_distribution oPDF1 = oPDB1.getPairDistribution(); 
    oPDF1.print();
    oPDF1.normalize();
    oPDF1.print();
    
    PDF << "Time: " << fDiff << endl;
    
    if (argc >= 4)
    {
    
        oPDB2.calcPairDistribution( fWidth2 );
        svt_pair_distribution oPDF2 = oPDB2.getPairDistribution();
        oPDF2.print();
        oPDF2.normalize();
        oPDF2.print();
        
        svt_pair_distribution oPDF;
    
        oPDF = oPDF1 - oPDF2;
        oPDF.print();
        
    
        oPDF = oPDF2 - oPDF1;
        oPDF.print();
        
        if (argc >= 6)
            oPDF.loadGnom(argv[5]);
        oPDF.print();
    
        PDF << "DistSq:" << oPDF2.distanceSq( oPDF1 ) << endl;
        PDF << "NormSq of the Diff:" << oPDF.getNormSq() << endl;
    }
    return 1;
}
