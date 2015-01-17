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
    if (argc < 2)
    {
	cout << "usage: filter <situs file> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    //
    // create and load situs file
    //
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );
    //oVol.threshold( -1.0, 1.0 );

    svt_itkVolume< PixelType > oItkVol( oVol );
    oItkVol.itkNormalize();
    oItkVol.itkInverse();
    oItkVol.itkExecute();

    unsigned int iCount;
    unsigned int iNVox = oVol.getSizeX() * oVol.getSizeY() * oVol.getSizeZ();

    for(iCount=0; iCount<iNVox; iCount++)
        oVol.setAt( iCount, (PixelType)(fabs(oVol.at( iCount ))) );

    //
    // write situs file
    //
    oVol.save( argv[2] );

    return 0;
}
