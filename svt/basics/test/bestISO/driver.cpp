// Test Driver - Rebecca Cooper - August 2006
// Outputs bestISO correlating thresholds into a data file

#include <svt_basics.h>
#include <svt_volume.h>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[])
{
	//Check Command Line Arguments
	if(argc < 3)
	{ 
		cout << "Usage: bestISO <situs file 1> <situs file 2> <threshold1>" << endl;
		return 1; 
	} 

	//Load Command Line Information
	svt_volume<Real64> oVolume1;
	svt_volume<Real64> oVolume2;
	oVolume1.loadSitus(argv[1]); 
	oVolume2.loadSitus(argv[2]); 

	ofstream fOutFile;
	ofstream fOutFile2; 
	fOutFile.open("thresholds.dat"); 

	Real64 fMax = oVolume1.getMaxDensity(); 
	Real64 fInterval = fMax / 100; 
	Real64 fPrevious = 0; 
	Real64 fSum = 0; 
	
	for(Real64 fThreshold = 0; fThreshold < fMax; fThreshold += fInterval)
	{ 
		Real64 fNew = oVolume2.bestISO(oVolume1, fThreshold); 
		fOutFile << fThreshold << "     " << fNew << endl; 
		fSum += fNew; 
		fPrevious = fNew; 
	} 

	fOutFile.flush(); fOutFile.close();

	return 0; 
}
