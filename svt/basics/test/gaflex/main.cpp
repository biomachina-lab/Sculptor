/***************************************************************************
                          svt_flexing_spline
                          --------------------------------------
    begin                : 07/02/2008
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/
#include <stdlib.h>
#include <svt_iostream.h>
#include <svt_basics.h>
#include <svt_types.h>

// the classes of the algorithm
#include <svt_gaflex.h>

// streams
#include <iostream>
using namespace std;

#define SVT_GA cout << svt_trimName("svt_gaflex")
/**
 * get stretch
 */
Real64 getStretch(svt_point_cloud_pdb< svt_vector4<Real64> > oCv, Real64 fValue)
{
    Real64 fDistMax = oCv.maxNNDistance( )+fValue;
    return fDistMax;
}

int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 3)
    {
	SVT_GA << "usage: gaflex <pdb input file 1 (probe)> <input file 2 (target)> <output dir>" << endl;
	return 10;
    }

    svt_sgenrand( svt_getToD() );

    svt_point_cloud_pdb< svt_vector4<Real64> > pPDBA, pPDBB;
    
    pPDBA.loadPDB( argv[1] ); 
    pPDBB.loadPDB( argv[2] ); 
    
    int iGenes = pPDBA.size() * 3;
    Real64 fRadius = 2.0;
    Real64 fStretch = 10.0;
    
    // ok, create object for the algorithm
    svt_gaflex oGA( iGenes );
    oGA.setProbe( pPDBA );
    oGA.setTarget( pPDBB );
    oGA.setSearchSpace( fRadius );
    oGA.setConnStretch( fStretch );
    oGA.setCheck( true );
    
    oGA.centerProbeTarget();

    //make path
    char pPath[1256];
    strcpy(pPath, argv[3]);
    
    char pCmd[1256];
    sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", pPath, pPath, pPath);
    svt_system( pCmd );
     
    vector< vector<float> > oFitArray;
    int iRun;
    int iGen;

    // main parameters
    int iPopSize = 400;
    int iMaxGen = 2000;
    int iMaxRun = 10;
    
    oGA.setPopSize( iPopSize );
    oGA.setMaxGen( iMaxGen );
    oGA.setOutputPath( pPath );

    // run the genetic algorithm
    for( iRun=0; iRun < iMaxRun; iRun++ )
    {
        SVT_GA<< "iRun:" << iRun << endl;
        oGA.setProbe( pPDBA );
        oGA.centerProbeTarget();
        oFitArray.push_back( oGA.run() );
    }

    // analyze the fitness
    vector<float> oAvgFit;
    for( iGen=0; iGen < iMaxGen; iGen++)
    {
        float fAvg = 0.0;
        for( iRun=0; iRun < iMaxRun; iRun++ )
        {
            fAvg += oFitArray[iRun][iGen];
        }

        oAvgFit.push_back( fAvg / iMaxRun );
    }
    for( iGen=0; iGen < iMaxGen; iGen++)
    {
        SVT_GA << oAvgFit[iGen] << endl;
    }

    // output result
        //oGA.printResults();
        //oGA.outputResult();
    
    return 0;
}
