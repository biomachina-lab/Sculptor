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
	cout << "usage: gaussian <situs file> <sigma> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    Real64 fSigma = atof( argv[2] );

    //
    // create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );
    oItkVol.itkGaussian(fSigma);
    oItkVol.itkExecute();

    //
    // write situs file
    //
    oVol.save( argv[3] );

    return 0;
}
