// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>
#include <svt_itkModelTube.h>

#define MODEL cout << svt_trimName("model")

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cout << "usage: model <situs file> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef double PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    //
    // create svt_itkVolume object
    //
    //svt_itkVolume< PixelType > oItkVol( oVol );

    //
    // run tube model registration
    //
    svt_itkModelTube< PixelType > oItkModel( oVol );
    oItkModel.run( );
    oItkModel.itkExecute();

    oVol.saveSitus( argv[2] );

    return 0;
}
