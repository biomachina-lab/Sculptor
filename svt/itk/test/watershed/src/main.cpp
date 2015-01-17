// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>

#define WATER cout << svt_trimName("watershed")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: watershed <situs file> <level> <threshold> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef double PixelType;

    //
    // Create and load situs file
    //
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );
    oVol.threshold( -1.0, 1.0 );

    //
    // Create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );

    //
    // Gradient Magnitude
    //
    oItkVol.itkGradientMagnitude();

    oVol.saveSitus( "gradientmag.situs" );

    //
    // Watershed
    //
    double fThreshold = atof( argv[2] );
    double fLevel = atof( argv[3] );
    svt_volume<unsigned long>* pVolume = oItkVol.itkWatershed( fLevel, fThreshold );

    //
    // Write situs file
    //
    WATER << "Write out to situs file..." << endl;
    pVolume->saveSitus( argv[4] );

    return 0;
}
