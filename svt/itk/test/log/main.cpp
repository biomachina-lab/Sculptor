// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>
#include <svt_itkGaussianMask.h>

#define FILTER cout << svt_trimName("LoG")

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
	cout << "usage: log <situs file> <sigma> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );
    PixelType fThresh = atof( argv[2] );

    //
    // subtract
    //
    for(unsigned int i=0; i<oVol.size(); i++)
        if (oVol.at(i) > fThresh )
            oVol.setAt(i, 1 );
        else
            oVol.setAt(i, 0 );

    //svt_volume< PixelType > oVol_Masked = oVol;

    //svt_itkVolume< PixelType > oItkVol_Aktin( oVol );
    //oItkVol_Aktin.itkExtractSlice( 2, 25, "slice.png" );

    //
    // create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );
    //oItkVol.itkLaplacian(  );
    //oItkVol.itkLaplacian(  );
    //oItkVol.itkGaussian( atof(argv[2]) );
    oItkVol.itkBinaryThinning();
    oItkVol.itkExecute();

    //
    // subtract
    //
    //for(unsigned int i=0; i<oVol.size(); i++)
    //    if (oVol.at(i) > 0.0)
    //        oVol_Masked.setAt(i, 0 );


    //
    // write situs file
    //
    //oVol.threshold( -1000.0, 0.0 );
    //oVol.normalize();
    oVol.saveSitus( argv[3] );

    return 0;
}
