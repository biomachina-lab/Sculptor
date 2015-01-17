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
	cout << "usage: regionGrowing <skeleton situs file> <map situs file> <threshold> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );
    svt_volume< PixelType > oVol_Orig;
    oVol_Orig.load( argv[2] );
    svt_volume< PixelType > oVol_Masked = oVol_Orig;

    PixelType fThresh = atof( argv[3] );

    oVol.removeNeighbors( 0.1 );

    //
    // seed points
    //
    vector< svt_vector3<unsigned int> > aSeeds;
    svt_vector3<unsigned int> oVec;
    for(unsigned int iX=0; iX<oVol.getSizeX(); iX++)
        for(unsigned int iY=0; iY<oVol.getSizeY(); iY++)
            for(unsigned int iZ=0; iZ<oVol.getSizeZ(); iZ++)
                if (oVol.at(iX, iY, iZ) == 1.0 )
                {
                    oVec.x( iX );
                    oVec.y( iY );
                    oVec.z( iZ );

                    aSeeds.push_back( oVec );
                }

    FILTER << "Number of seed points: " << aSeeds.size() << endl;

    vector< svt_vector3<unsigned int> > aSeeds_2;
    //aSeeds_2.push_back( aSeeds[10] );
    //aSeeds_2.push_back( aSeeds[20] );
    aSeeds_2.push_back( aSeeds[30] );
    //aSeeds_2.push_back( aSeeds[40] );

    svt_itkVolume< PixelType > oItkMask( oVol_Orig );
    oItkMask.itkConnectedThreshold( fThresh, aSeeds );
    oItkMask.itkExecute();

    //
    // final masking
    //
    for(unsigned int i=0; i<oVol_Orig.size(); i++)
        if (oVol_Orig.at(i) == 0.0 )
            oVol_Masked.setAt(i, 0 );

    FILTER << "Seed Pixel: " << aSeeds[30].x() << " , " << aSeeds[30].y() << " , " << aSeeds[30].z() << endl;

    //
    // write situs file
    //
    oVol_Masked.saveSitus( argv[4] );

    return 0;
}
