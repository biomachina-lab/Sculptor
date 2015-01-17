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
    if (argc < 4)
    {
	cout << "usage: thinning <situs file> <threshold> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    PixelType fThresh = atof( argv[2] );

    //
    // binary
    //
    for(unsigned int i=0; i<oVol.size(); i++)
        if (oVol.at(i) > fThresh )
            oVol.setAt(i, 1 );
        else
            oVol.setAt(i, 0 );

    //
    // create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );
    oItkVol.itkBinaryThinning();
    oItkVol.itkBinaryPruning();
    oItkVol.itkExecute();

    //
    // write situs file
    //
    oVol.saveSitus( argv[3] );

    return 0;
}
