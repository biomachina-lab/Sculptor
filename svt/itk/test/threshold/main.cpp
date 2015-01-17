// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>
#include <svt_itkGaussianMask.h>

#define FILTER cout << svt_trimName("filter")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: threshold <situs file> <threshold> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    Real64 fThreshold = atof( argv[2] );

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    unsigned int iNVox = oVol.getSizeX() * oVol.getSizeY() * oVol.getSizeZ();
    unsigned int iCount;

    for(iCount=0; iCount<iNVox; iCount++)
        if (oVol.at( iCount ) < fThreshold)
            oVol.setAt( iCount, (PixelType)(0.0) );

    //
    // write situs file
    //
    oVol.saveSitus( argv[3] );

    return 0;
}
