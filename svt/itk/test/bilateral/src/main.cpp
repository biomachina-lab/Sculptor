// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>

#define SITK cout << svt_trimName("itk")

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	cout << "usage: bilateral <situs file> <sigma> <threshold> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    Real64 fSigma = atof( argv[2] );
    Real64 fThreshold = atof( argv[3] );

    //
    // create svt_itkVolume object
    //
    svt_itkVolume< PixelType > oItkVol( oVol );

    //
    // execute all the filters
    //
    oItkVol.itkCurvatureFlow();
    oItkVol.itkHistogramEqualization();
    oItkVol.itkGaussian( fSigma );
    oItkVol.itkBinaryThreshold( fThreshold, 1.0 );

    //oItkVol.itkBinaryThinning();
    //oItkVol.itkBinaryPruning();
    //oItkVol.itkNormalize();
    oItkVol.itkExecute();

    //
    // write situs file
    //
    oVol.saveSitus( argv[4] );

    return 0;
}
