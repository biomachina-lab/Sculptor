// Test Driver - Rebecca Cooper - August 2006
// Does an interpolated scale of the situs file 

#include <svt_basics.h>
#include <svt_volume.h>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[])
{
	//Check Command Line Arguments
	if(argc < 5)
	{ 
		cout << "Usage: bestISO <situs file 1> <situs file 2> <threshold1> <threshold2>" << endl;
		return 1; 
	} 

	//Load Command Line Information
	svt_volume<Real64> oVolume1;
	svt_volume<Real64> oVolume2;
	oVolume1.loadSitus(argv[1]); 
	oVolume2.loadSitus(argv[2]); 
	Real64 fThreshold1 = atof(argv[3]);
	Real64 fThreshold2 = atof(argv[4]); 

	Real64 fNew1 = oVolume2.bestISO(oVolume1, fThreshold1); 
	Real64 fNew2 = oVolume2.bestISO(oVolume1, fThreshold2); 
	oVolume2.interpolatedScale(fThrehold1, fThreshold2, fNew1, fNew2); 
	oVolume1.saveSitus("debug.situs");

	return 0; 
}
