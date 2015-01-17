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
	cout << "usage: equalization <situs file> <output file>" << endl;
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
    oItkVol.itkHistogramEqualization();
    oItkVol.itkExecute();

    //
    // write situs file
    //
    oVol.saveSitus( argv[2] );

    return 0;
}
