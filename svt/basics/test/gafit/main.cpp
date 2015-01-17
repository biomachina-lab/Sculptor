// the classes of the algorithm
#include <svt_gafit.h>

// streams
#include <iostream>
using namespace std;

#define SVT_GA cout << svt_trimName("svt_ga")
// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 3)
    {
	SVT_GA << "usage: gafit <pdb input file 1 (model)> <input file 2 (scene)> <output dir>" << endl;
	return 10;
    }

    svt_sgenrand( svt_getToD() );

    svt_point_cloud_pdb< svt_vector4<Real64> > pPDBA, pPDBB;
    
    pPDBA.loadPDB( argv[1] ); 
    pPDBB.loadPDB( argv[2] ); 
    
    // ok, create object for the algorithm
    svt_gafit oGA;
    oGA.setModel( pPDBA );
    oGA.setScene( pPDBB );
    
    oGA.centerModelScene();

    //make path
    char pPath[1256];
    strcpy(pPath, argv[3]);
    
    char pCmd[1256];
    sprintf( pCmd, "mkdir %s", pPath);
    svt_system( pCmd );
    
    vector< vector<float> > oFitArray;
    int iRun;
    int iGen;

    // main parameters
    int iPopSize = 150;
    int iMaxGen = 1000;
    int iMaxRun = 30;
    
    oGA.setPopSize( iPopSize );
    oGA.setMaxGen( iMaxGen );
    oGA.setOutputPath( pPath );

    // run the genetic algorithm
    for( iRun=0; iRun < iMaxRun; iRun++ )
    {
        SVT_GA<< "iRun:" << iRun << endl;
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
