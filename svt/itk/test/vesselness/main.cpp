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
    if (argc < 5)
    {
	cout << "usage: vesselness <situs file> <sigma> <alpha 1> <alpha 2> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    //
    // create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );
    oItkVol.itkVesselness( atof(argv[2]), atof(argv[3]), atof(argv[4]) );
    oItkVol.itkExecute();

    //
    // write situs file
    //
    FILTER << "Write to situs file " << argv[5] << endl;
    oVol.saveSitus( argv[5] );

    return 0;
}
