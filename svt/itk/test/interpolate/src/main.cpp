// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>

#define INTER cout << svt_trimName("interpolate")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: interpolate <situs file> <new voxelwidth> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef double PixelType;

    //
    // Create and load situs file
    //
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    //
    // Create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );

    //
    // Reinterpolate
    //
    oItkVol.itkInterpolate( atof( argv[2] ) );
    oItkVol.itkExecute();

    //
    // Write situs file
    //
    INTER << "Write out to situs file..." << endl;
    oVol.saveSitus( argv[3] );

    return 0;
}
