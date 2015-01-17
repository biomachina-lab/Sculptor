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
#include <svt_powell_saxs.h>

#define POLO cout << svt_trimName("powell_loops")

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 2)
    {
	POLO << "usage: pdf <input pdb file1 model> <input pdb cv> <input pdb bead model> <output pdb cv> <selected cv start> <selected cv end> <selected res start> <selected res end>" << endl;
	return 1;
    }
    
    POLO << " Model file : " << argv[1] << endl;
    POLO << " CV In file : " << argv[2] << endl;
    POLO << " Bead model : " << argv[3] << endl;
    
    if (argc >= 5)
        POLO << "CV Out file : " << argv[4] << endl;
    
    if (argc >= 7)
    {
        POLO << "Sel Cv start: " << argv[5] << endl;
        POLO << "Sel Cv end  : " << argv[6] << endl;
    }
    
        if (argc >= 9)
    {
        POLO << "Sel Res start: " << argv[7] << endl;
        POLO << "Sel Res end  : " << argv[8] << endl;
    }
    
    svt_point_cloud_pdb<svt_vector4<Real64 > > oPDB;
    svt_point_cloud< svt_vector4<Real64 > > oCvIn, oCvOut, oPDBtmp;
    svt_point_cloud< svt_vector4<Real64 > > oBeads;
    svt_pair_distribution *oPdfM, *oPdfE;
    Real64 fWidth = 1;
    unsigned int iCvStart, iCvEnd;
    unsigned int iResStart, iResEnd;
    
    oPdfE = new svt_pair_distribution();
    try
    {
        oPDB.loadPDB( argv[1] );
        oPDBtmp.loadPDB( argv[1] );
        oCvIn.loadPDB( argv[2] );
        oBeads.loadPDB( argv[3] );
        
        if (argc >= 5)
            oCvOut.loadPDB( argv[4] );
        
        if (argc >= 7)
        {
            iCvStart = atoi( argv[5] );
            iCvEnd = atoi( argv[6] );
        }
        
        if (argc >= 9)
        {
            iResStart = atoi( argv[7] );
            iResEnd = atoi( argv[8] );
        }
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
        return 1;
    }
         
    vector< unsigned int > aSelCv;
    for (unsigned int iIndex = iCvStart-1; iIndex<=iCvEnd-1; iIndex++)
        aSelCv.push_back( iIndex );
    
    for (unsigned int iIndex = iResStart; iIndex<=iResEnd; iIndex++)
        oPDB.selectAtomResidueSeq( iIndex );
    
    oBeads.calcPairDistribution( fWidth );
    oPdfE = oBeads.getPairDistribution(); 
    oPdfE->normalize();
    
//     oPDB.calcPairDistribution( fWidth );
//     oPdfM = oPDB.getPairDistribution(); 
//     oPdfM->normalize();
//     oPdfM->print();
/*    
    POLO << "Score:" << oPdfE->distanceSq( *oPdfM ) << endl;*/
  
    char pFname[1256];
    char pFnameCv[1256]; 
    strcpy(pFname, "model2.pdb");
    strcpy(pFnameCv, "cv2.pdb");
    
    svt_powell_saxs *oPowell = new svt_powell_saxs(oPDB, oCvIn, *oPdfE, pFname, pFnameCv, &aSelCv);
    oPowell->setTolerance( 1e-20 );
    oPowell->maximize(); 
    
    POLO << "Time of a single score calculation: " << oPowell->getScoreTime() << endl;

    
    return 1;
}
