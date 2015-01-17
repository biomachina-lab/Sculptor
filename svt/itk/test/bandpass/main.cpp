// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>

#define FILTER cout << svt_trimName("filter")

// main data type
typedef float PixelType;

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
	cout << "usage: bandpass <situs file> <sigma> <threshold> <output file>" << endl;
        exit(1);
    }

    //
    // create svt_itkVolume object
    //
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );
    svt_itkVolume< PixelType > oItkVol( oVol );

    //
    // bandpass
    //
    PixelType fSigma = atof( argv[2] );
    PixelType fThreshold = atof( argv[3] );

    FILTER << "Sigma: " << fSigma << endl;
    FILTER << "Threshold: " << fThreshold << endl;

    oItkVol.itkGaussian( fSigma );
    oItkVol.itkBinaryThreshold( fThreshold, 1.0 );
    oItkVol.itkExecute();

    //
    // save data
    //
    oVol.save( argv[4] );

    return 0;
}
