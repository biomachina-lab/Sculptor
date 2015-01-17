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
        cout << "usage: freqmap [options]" << endl;
        cout << endl;
        cout << "  options:" << endl;
        cout << "  -i  input file" << endl;
        cout << "  -o  output file" << endl;
        cout << "  -s  start sigma (default: 2.0)" << endl;
        cout << "  -t  end sigma (default: 6.0)" << endl;
        cout << "  -p  increment sigma (default: 0.5)" << endl;
        cout << "  -t  structure threshold (default: 0.6)" << endl;
        exit(1);
    }

    //
    // Parse parameters
    //
    Real64 fSigmaStart = 2.0;
    Real64 fSigmaEnd = 2.0;
    Real64 fSigmaInc = 0.5;
    Real64 fSigmaMask = 1.0;
    Real64 fThreshold = 0.6;

    Real64 fBlur = 0.0;

    char* pInput = NULL;
    char* pOutput = NULL;

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

                if (argc >= i+1)
                {
                    pInput = argv[i+1];

                    FILTER << "Input File: " << pInput << endl;

                    i++;
                }
                break;

                // Output Filenames
            case 'o':

                if (argc >= i+1)
                {
                    pOutput = argv[i+1];

                    FILTER << "Output File: " << pOutput << endl;

                    i++;
                }
                break;

                // Sigma Start
            case 's':
                if (argc >= i+1)
                {
                    fSigmaStart = atof( argv[i+1] );

                    FILTER << "SigmaStart: " << fSigmaStart << endl;

                    i++;
                }
                break;

                // Sigma End
            case 'e':
                if (argc >= i+1)
                {
                    fSigmaEnd = atof( argv[i+1] );

                    FILTER << "SigmaEnd: " << fSigmaEnd << endl;

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

                // Threshold
	    case 't':
		if (argc >= i+1)
                {
                    fThreshold = atof( argv[i+1] );

                    FILTER << "Structure Threshold: " << fThreshold << endl;

                    i++;
                }
                break;

                // Increment
	    case 'p':
		if (argc >= i+1)
                {
                    fSigmaInc = atof( argv[i+1] );

                    FILTER << "Sigma Increment: " << fSigmaInc << endl;

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

	    }

	} 
    }

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

    //
    // Create a local copy for the masked volume
    //
    svt_volume< PixelType > oSigmaVol;
    oSigmaVol.deepCopy( *pVol );
    svt_itkVolume< PixelType > oItkVolSigma( oSigmaVol );

    svt_volume<PixelType> oFinalVol( pVol->getSizeX(), pVol->getSizeY(), pVol->getSizeZ(), (PixelType)(0.0) );

    unsigned int iCount;
    unsigned int iNVox = pVol->getSizeX() * pVol->getSizeY() * pVol->getSizeZ();

    for(iCount=0; iCount<iNVox; iCount++)
        pVol->setAt( iCount, (PixelType)(fabs(pVol->at( iCount ))) );

    //
    // Main Loop
    //
    Real64 fSigma;
    for(fSigma = fSigmaStart; fSigma < fSigmaEnd; fSigma += fSigmaInc )
    {
        FILTER << endl;
        FILTER << "Sigma: " << fSigma << endl;
        FILTER << endl;

        oSigmaVol.deepCopy( *pVol );

        oItkVolSigma.itkGaussian( fSigma );
        oItkVolSigma.itkBinaryThreshold( fThreshold, 1.0 );
        oItkVolSigma.itkExecute();

        for(iCount=0; iCount<iNVox; iCount++)
            if (oSigmaVol.at(iCount) == (PixelType)(1.0))
                oFinalVol.setAt( iCount, fSigma );
    }

    //
    // Save masked map
    //
    if (pOutput != NULL)
        oFinalVol.saveSitus( pOutput );

    return 0;
}
