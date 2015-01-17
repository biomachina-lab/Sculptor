// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>

#define FILTER cout << svt_trimName("filter")

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
	cout << "usage: curvatureflow <situs file> <iterations> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;
    unsigned int iIterations = atoi( argv[2] );

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    //
    // create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );
    oItkVol.itkCurvatureFlow( iIterations );
    oItkVol.itkExecute();

    //
    // write situs file
    //
    oVol.saveSitus( argv[3] );

    return 0;
}
