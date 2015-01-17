// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>

#define FILTER cout << svt_trimName("filter")

// main data type
typedef float PixelType;

class vIndex
{
    short int m_oV[3];

public:

    vIndex( short int iX = 0, short int iY = 0, short int iZ = 0)
    {
	m_oV[0] = iX;
	m_oV[1] = iY;
	m_oV[2] = iZ;
    };

    vIndex( const vIndex& rOther )
    {
        m_oV[0] = rOther.m_oV[0];
        m_oV[1] = rOther.m_oV[1];
        m_oV[2] = rOther.m_oV[2];
    };

    inline short int& operator[](unsigned i)
    {
	return m_oV[i];
    };
};


/**
 * Non-recursive flood-fill algorithm. The algorithm only fills the voxels that are above the threshold (and that are connected to the start voxel) with the specified value.
 * \param iStartX x index of the start voxel
 * \param iStartY y index of the start voxel
 * \param iStartZ z index of the start voxel
 * \param fTreshold threshold for the floodfill
 * \param fGaussian sigma of the gaussian the mask is convoluted with (if 0, no blurring of the mask is applied)
*/
void specialFloodfill( svt_volume<PixelType>* pVol, svt_volume<char>& oVisited, unsigned int iStartX, unsigned int iStartY, unsigned int iStartZ, PixelType fThreshold, PixelType fFillValue )
{
    stack< vIndex > oQueue;
    vIndex oStart(iStartX, iStartY, iStartZ);
    oQueue.push( oStart );

    //if (rVol.getValue( iStartX, iStartY, iStartZ ) < fThreshold)
    //    FILTER << "Warning: Start voxel is lower than threshold in floodfill!" << endl;

    vIndex oCurrent;
    vIndex oTemp;

    while( oQueue.size() > 0)
    {
	oCurrent = oQueue.top(); oQueue.pop();

	if (oVisited.getValue(oCurrent[0], oCurrent[1], oCurrent[2]) == 0)
	{
	    if (pVol->getValue(oCurrent[0], oCurrent[1], oCurrent[2]) > fThreshold)
	    {
                oVisited.setValue( oCurrent[0], oCurrent[1], oCurrent[2], 1 );
                pVol->setValue(oCurrent[0], oCurrent[1], oCurrent[2], fFillValue);

		// z = 0

		// x-1 y+1 z+0
		if (oCurrent[0]-1 >=0 && oCurrent[1]+1 < (int)pVol->getSizeY())
		{
		    oTemp = oCurrent;
		    oTemp[0] = oTemp[0] - 1;
		    oTemp[1] = oTemp[1] + 1;
		    oQueue.push( oTemp );
		}
		// x+0 y+1 z+0
		if (oCurrent[1]+1 < (int)pVol->getSizeY())
		{
		    oTemp = oCurrent;
		    oTemp[1] = oTemp[1] + 1;
		    oQueue.push( oTemp );
		}
		// x+1 y+1 z+0
		if (oCurrent[0]+1 < (int)pVol->getSizeX() && oCurrent[1]+1 < (int)pVol->getSizeY())
		{
		    oTemp = oCurrent;
		    oTemp[0] = oTemp[0] + 1;
		    oTemp[1] = oTemp[1] + 1;
		    oQueue.push( oTemp );
		}

		// x-1 y+0 z+0
		if (oCurrent[0]-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp[0] = oTemp[0] - 1;
		    oQueue.push( oTemp );
		}
		// x+1 y+0 z+0
		if (oCurrent[0]+1 < (int)pVol->getSizeX())
		{
		    oTemp = oCurrent;
		    oTemp[0] = oTemp[0] + 1;
		    oQueue.push( oTemp );
		}

		// x-1 y-1 z+0
		if (oCurrent[0]-1 >=0 && oCurrent[1]-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp[0] = oTemp[0] - 1;
		    oTemp[1] = oTemp[1] - 1;
		    oQueue.push( oTemp );
		}
		// x+0 y-1 z+0
		if (oCurrent[1]-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp[1] = oTemp[1] - 1;
		    oQueue.push( oTemp );
		}
		// x+1 y-1 z+0
		if (oCurrent[0]+1 < (int)pVol->getSizeX() && oCurrent[1]-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp[0] =  oTemp[0] + 1;
		    oTemp[1] = oTemp[1] - 1;
		    oQueue.push( oTemp );
		}

		// z = 1
		if (oCurrent[2]+1 < (int)pVol->getSizeZ())
		{

		    // x-1 y+1 z+1
		    if (oCurrent[0]-1 >=0 && oCurrent[1]+1 < (int)pVol->getSizeY())
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] - 1;
			oTemp[1] = oTemp[1] + 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }
		    // x+0 y+1 z+1
		    if (oCurrent[1]+1 < (int)pVol->getSizeY())
		    {
			oTemp = oCurrent;
			oTemp[1] = oTemp[1] + 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }
		    // x+1 y+1 z+1
		    if (oCurrent[0]+1 < (int)pVol->getSizeX() && oCurrent[1]+1 < (int)pVol->getSizeY())
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] + 1;
			oTemp[1] = oTemp[1] + 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }

		    // x-1 y+0 z+1
		    if (oCurrent[0]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] - 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }
		    // x+0 y+0 z+1
		    {
			oTemp = oCurrent;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }
		    // x+1 y+0 z+1
		    if (oCurrent[0]+1 < (int)pVol->getSizeX())
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] + 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }

		    // x-1 y-1 z+1
		    if (oCurrent[0]-1 >=0 && oCurrent[1]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] - 1;
			oTemp[1] = oTemp[1] - 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }
		    // x+0 y-1 z+1
		    if (oCurrent[1]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[1] = oTemp[1] - 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }
		    // x+1 y-1 z+1
		    if (oCurrent[0]+1 < (int)pVol->getSizeX() && oCurrent[1]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] + 1;
			oTemp[1] = oTemp[1] - 1;
			oTemp[2] = oTemp[2] + 1;
			oQueue.push( oTemp );
		    }

		}

		// z = -1
		if (oCurrent[2]-1 >=0)
		{

		    // x-1 y+1 z-1
		    if (oCurrent[0]-1 >=0 && oCurrent[1]+1 < (int)pVol->getSizeY())
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] - 1;
			oTemp[1] = oTemp[1] + 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }
		    // x+0 y+1 z-1
		    if (oCurrent[1]+1 < (int)pVol->getSizeY())
		    {
			oTemp = oCurrent;
			oTemp[1] = oTemp[1] + 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }
		    // x+1 y+1 z-1
		    if (oCurrent[0]+1 < (int)pVol->getSizeX() && oCurrent[1]+1 < (int)pVol->getSizeY())
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] + 1;
			oTemp[1] = oTemp[1] + 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }

		    // x-1 y+0 z-1
		    if (oCurrent[0]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] - 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }
		    // x+0 y+0 z-1
		    {
			oTemp = oCurrent;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }
		    // x+1 y+0 z-1
		    if (oCurrent[0]+1 < (int)pVol->getSizeX())
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] + 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }

		    // x-1 y-1 z-1
		    if (oCurrent[0]-1 >=0 && oCurrent[1]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] - 1;
			oTemp[1] = oTemp[1] - 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }
		    // x+0 y-1 z-1
		    if (oCurrent[1]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[1] = oTemp[1] - 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }
		    // x+1 y-1 z-1
		    if (oCurrent[0]+1 < (int)pVol->getSizeX() && oCurrent[1]-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp[0] =  oTemp[0] + 1;
			oTemp[1] = oTemp[1] - 1;
			oTemp[2] = oTemp[2] - 1;
			oQueue.push( oTemp );
		    }

		}

	    } else
		oVisited.setValue( oCurrent[0], oCurrent[1], oCurrent[2], 2 );
	}
    }

    //unsigned int iNum = rVol.getSizeX() * rVol.getSizeY() * rVol.getSizeZ();
    //for(unsigned i=0; i<iNum; i++)
	//if (oVisited.at(i) == 1)
	//    rVol.setAt(i, fFillValue);
};


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "usage: denoise [options]" << endl;
        cout << endl;
        cout << "  options:" << endl;
        cout << "  -i  input file" << endl;
        cout << "  -im input mask file" << endl;
        cout << "  -is input segmentation file" << endl;
        cout << "  -it input topology file" << endl;
        cout << "  -o  output file" << endl;
        cout << "  -om output mask file" << endl;
        cout << "  -os output segmentation file" << endl;
        cout << "  -ot output topology file" << endl;
        cout << "  -s  structure size (default: 2.0)" << endl;
        cout << "  -t  structure threshold (default: 0.6)" << endl;
        cout << "  -b  final gaussian blur (default: 0.0)" << endl;
        cout << "  -m  mask blur (default: 0.0)" << endl;
        cout << "  -l  minimal filament length (default: 150)" << endl;
        cout << "  -n  no topology analysis" << endl;
        cout << "  -d  direct mask" << endl;
        exit(1);
    }

    //
    // Parse parameters
    //
    Real64 fSigma = 2.0;
    Real64 fSigmaMask = -1.0;
    Real64 fThreshold = 0.6;
    Real64 fBlur = 0.0;

    char* pInput = NULL;
    char* pInputMask = NULL;
    char* pInputSegm = NULL;
    char* pInputTopo = NULL;

    char* pOutput = NULL;
    char* pOutputMask = NULL;
    char* pOutputSegm = NULL;
    char* pOutputTopo = NULL;

    int iMaxLength = 150;

    bool bNoTopo = false;
    bool bDirectMask = false;

    int i;
    for(i=1; i<argc; i++)
    {
	// is this an option or a filename
	if (argv[i][0] == '-' && argv[i][1] != 0)
	{
	    switch(argv[i][1])
            {

                // Input Filenames
            case 'i':

                // Input Filename
                if (argv[i][2] == 0)
                    if (argc >= i+1)
                    {
                        pInput = argv[i+1];

                        FILTER << "Input File: " << pInput << endl;

                        i++;

                        break;
                    }

                // Input Mask Filename
                if (argv[i][2] == 'm')
                    if (argc >= i+1)
                    {
                        pInputMask = argv[i+1];

                        FILTER << "Input Mask File: " << pInputMask << endl;

                        i++;

                        break;
                    }

                // Input Segm Filename
                if (argv[i][2] == 's')
                    if (argc >= i+1)
                    {
                        pInputSegm = argv[i+1];

                        FILTER << "Input Segmentation File: " << pInputSegm << endl;

                        i++;

                        break;
                    }

                // Input Topology Filename
                if (argv[i][2] == 't')
                    if (argc >= i+1)
                    {
                        pInputTopo = argv[i+1];

                        FILTER << "Input Topology File: " << pInputTopo << endl;

                        i++;

                        break;
                    }

                break;

                // Output Filenames
            case 'o':

                // Output Filename
                if (argv[i][2] == 0)
                    if (argc >= i+1)
                    {
                        pOutput = argv[i+1];

                        FILTER << "Output File: " << pOutput << endl;

                        i++;

                        break;
                    }

                // Output Mask Filename
                if (argv[i][2] == 'm')
                    if (argc >= i+1)
                    {
                        pOutputMask = argv[i+1];

                        FILTER << "Output Mask File: " << pOutputMask << endl;

                        i++;

                        break;
                    }

                // Output Segm Filename
                if (argv[i][2] == 's')
                    if (argc >= i+1)
                    {
                        pOutputSegm = argv[i+1];

                        FILTER << "Output Segmentation File: " << pOutputSegm << endl;

                        i++;

                        break;
                    }

                // Ouput Topology Filename
                if (argv[i][2] == 't')
                    if (argc >= i+1)
                    {
                        pOutputTopo = argv[i+1];

                        FILTER << "Output Topology File: " << pOutputTopo << endl;

                        i++;

                        break;
                    }

                break;


                // Sigma
	    case 's':
		if (argc >= i+1)
                {
                    fSigma = atof( argv[i+1] );

                    FILTER << "Structure Size: " << fSigma << endl;

                    i++;
                }
                break;

                // Mask-Sigma
	    case 'm':
		if (argc >= i+1)
                {
                    fSigmaMask = atof( argv[i+1] );

                    FILTER << "Mask Sigma: " << fSigmaMask << endl;

                    i++;
                }
                break;

                // Filament length
	    case 'l':
		if (argc >= i+1)
                {
                    iMaxLength = atoi( argv[i+1] );

                    FILTER << "Min. Filament Length: " << iMaxLength << endl;

                    i++;
                }
                break;

                // Threshold
	    case 't':
		if (argc >= i+1)
                {
                    fThreshold = atof( argv[i+1] );

                    FILTER << "Structure Threshold: " << fThreshold << endl;

                    i++;
                }
                break;

                // Blur
	    case 'b':
		if (argc >= i+1)
                {
                    fBlur = atof( argv[i+1] );

                    FILTER << "Final Gaussian Blur: " << fBlur << endl;

                    i++;
                }
                break;

                // Do not use topology for masking
	    case 'n':
                FILTER << "No Topology Analysis!" << endl;
                bNoTopo = true;
                break;

                // Do not use topology for masking
	    case 'd':
                FILTER << "Topology information will not get used for mask!" << endl;
                bDirectMask = true;
                break;

	    }

	} 
    }

    if (fSigmaMask == -1.0)
        if (bDirectMask)
            fSigmaMask = 1.5;
        else
            fSigmaMask = fSigma;

    if (pInput == NULL)
    {
        FILTER << "You have to specify an input file (option -i)!" << endl;
    }

    FILTER << endl;
    FILTER << "Importing Data" << endl;
    FILTER << endl;

    //
    // Create and load situs file
    //
    svt_volume< PixelType >* pVol = new svt_volume<PixelType>();
    pVol->load( pInput );
    svt_itkVolume< PixelType > oItkVol( *pVol );

    //
    // Create a local copy for the masked volume
    //
    svt_volume< PixelType > oMaskVol;
    oMaskVol.deepCopy( *pVol );
    svt_itkVolume< PixelType > oItkVolCopy( oMaskVol );

    //
    // Variables
    //
    svt_volume<PixelType>* pNewMaskVol = new svt_volume<PixelType>( pVol->getSizeX(), pVol->getSizeY(), pVol->getSizeZ(), (PixelType)(0.0) );
    svt_volume< char >* pVisited = NULL;
    vector< svt_vector3< unsigned int > > aRegions;
    unsigned int iCount;
    unsigned int iNVox = pVol->getSizeX() * pVol->getSizeY() * pVol->getSizeZ();

    //
    // Topology Analysis
    //
    if (!bNoTopo)
    {
        FILTER << endl;
        FILTER << "Topology-Analysis" << endl;
        FILTER << endl;

        //
        // Generate Topology
        //
        oItkVol.itkGaussian( fSigma );
        oItkVol.itkBinaryThreshold( (PixelType)(fThreshold), (PixelType)(1.0) );
        oItkVol.itkBinaryThinning();
        oItkVol.itkBinaryPruning();
        oItkVol.itkExecute();

        //
        // Load topology file from previous run
        //
        if (pInputTopo != NULL)
        {
            svt_volume< PixelType >* pOldTopo = new svt_volume<PixelType>();
            pOldTopo->load( pInputTopo );

            svt_itkVolume< PixelType > oItkTopo( *pOldTopo );
            oItkTopo.itkGaussian( 2.5 );
            oItkTopo.itkNormalize();
            oItkTopo.itkBinaryThreshold( 0.01, 1.0 );
            oItkTopo.itkExecute();

            for(iCount=0; iCount<iNVox; iCount++)
                if (pOldTopo->at( iCount ) != (PixelType)(0.0))
                    pVol->setAt( iCount, (PixelType)(0.0) );

            delete pOldTopo;
        }
    }

    if ( bNoTopo || bDirectMask )
    {
        oItkVol.itkGaussian( fSigma );
        oItkVol.itkBinaryThreshold( fThreshold, 1.0 );
        oItkVol.itkExecute();
    }

    //
    // Delete all areas with a smaller length then x
    //
    PixelType fCount = (PixelType)(2.0);
    int iOVox;
    svt_vector3< unsigned int > oVec;
    pVisited = new svt_volume<char>( pNewMaskVol->getSizeX(), pNewMaskVol->getSizeY(), pNewMaskVol->getSizeZ(), (char)0 );
    unsigned int iNumStructs = 0;

    if (iMaxLength != 0)
    {
        try
        {
            svt_exception::ui()->progressPopup("Graph cleanup...", 0, pVol->getSizeX() );

            for( unsigned int iX=0; iX<pVol->getSizeX(); iX++ )
            {
                svt_exception::ui()->progress( iX, pVol->getSizeX() );

                for( unsigned int iY=0; iY<pVol->getSizeY(); iY++ )
                    for( unsigned int iZ=0; iZ<pVol->getSizeZ(); iZ++ )
                    {
                        if ( pVol->at( iX, iY, iZ ) == (PixelType)(1.0) && pNewMaskVol->at(iX, iY, iZ) == (PixelType)(0.0) )
                        {
                            fCount++;

                            specialFloodfill( pVol, *pVisited, iX, iY, iZ, 0.5, fCount );

                            iOVox = pVol->getOccupied( fCount );

                            if ( iOVox > 0 && (( iMaxLength > 0 && iOVox >= iMaxLength ) || ( iMaxLength < 0 && iOVox <= (-iMaxLength) ) ))
                            {
                                for(iCount=0; iCount<iNVox; iCount++)
                                    if (pVol->at( iCount ) == fCount)
                                        pNewMaskVol->setAt( iCount, (PixelType)(1.0) );

                                oVec.x( iX ); oVec.y( iY ); oVec.z( iZ );
                                aRegions.push_back( oVec );

                                iNumStructs++;

                            }
                        }
                    }
            }

        } catch (svt_userInterrupt&)
        {
            FILTER << "Warning: Graph cleanup calculation was aborted, data might be incomplete..." << endl;
        }

    } else {

        pNewMaskVol->deepCopy( *pVol );

    }

    FILTER << "The program has found " << iNumStructs << " Structures of the given size and length - " << endl;
    FILTER << "  out of " << fCount << " initial seed areas." << endl;

    svt_exception::ui()->progressPopdown();

    delete pVol;
    delete pVisited;

    //
    // Save topology file
    //
    if (pOutputTopo != NULL)
        pNewMaskVol->saveSitus( pOutputTopo );

    //
    // Create mask
    //
    svt_itkVolume<PixelType> oItkVolMask( *pNewMaskVol );
    oItkVolMask.itkGaussian( fSigmaMask );
    oItkVolMask.itkBinaryThreshold( 1.0E-3, 1.0 );
    oItkVolMask.itkExecute();

    //
    // Load Mask from previous run
    //
    if (pInputMask != NULL)
    {
        svt_volume< PixelType >* pOldMask = new svt_volume<PixelType>();
        pOldMask->load( pInputMask );

        for(iCount=0; iCount<iNVox; iCount++)
            if (pOldMask->at( iCount ) != (PixelType)(0))
                pNewMaskVol->setAt( iCount, pOldMask->at( iCount) );

        delete pOldMask;
    }

    //
    // Save Mask
    //
    if (pOutputMask != NULL)
        pNewMaskVol->saveSitus( pOutputMask );

    //
    // Apply mask
    //
    oItkVolCopy.itkMultiply( oItkVolMask );
    oItkVolCopy.itkExecute();

    //
    // Segmentation, but only if segments were really calculated before...
    //
    if (pOutputSegm != NULL && aRegions.size() != 0)
    {
        pNewMaskVol->saveSitus( "debug-nmv-before.situs" );

        //
        // Make segmentation a bit larger
        //
        oItkVolMask.itkDilate();
        oItkVolMask.itkExecute();

        pNewMaskVol->saveSitus( "debug-nmv-after.situs" );

        pVisited = new svt_volume<char>( pNewMaskVol->getSizeX(), pNewMaskVol->getSizeY(), pNewMaskVol->getSizeZ(), (char)0 );

        try
        {
            svt_exception::ui()->progressPopup("Generate Segmentation Map...", 0, aRegions.size() );

            for(iCount=0; iCount<aRegions.size(); iCount++)
            {
                svt_exception::ui()->progress( iCount, aRegions.size() );

                specialFloodfill( pNewMaskVol, *pVisited, aRegions[iCount].x(), aRegions[iCount].y(), aRegions[iCount].z(), 0.1, (PixelType)(iCount) );
            }

        } catch (svt_userInterrupt&)
        {
            FILTER << "Warning: Segmentation map calculation was aborted, data might be incomplete..." << endl;
        }

        svt_exception::ui()->progressPopdown();
        delete pVisited;

        //
        // Load old segmentation map
        //
        if (pInputSegm != NULL)
        {
            svt_volume< PixelType >* pOldSegm = new svt_volume<PixelType>();
            pOldSegm->load( pInputMask );

            PixelType fMax = pOldSegm->getMaxDensity();
            fMax += pNewMaskVol->getMaxDensity();

            for(iCount=0; iCount<iNVox; iCount++)
                if (pOldSegm->at( iCount ) != (PixelType)(0))
                    pNewMaskVol->setAt( iCount, pOldSegm->at( iCount) + fMax );

            delete pOldSegm;
        }

        //
        // Save Segmentation
        //
        if (pOutputSegm != NULL)
            pNewMaskVol->saveSitus( pOutputSegm );
    }

    //
    // Apply a final Gaussian
    //
    if (fBlur != 0.0)
    {
        oItkVolCopy.itkGaussian( fBlur );
        oItkVolCopy.itkExecute();
    }

    //
    // Save masked map
    //
    if (pOutput != NULL)
        oMaskVol.saveSitus( pOutput );

    return 0;
}
