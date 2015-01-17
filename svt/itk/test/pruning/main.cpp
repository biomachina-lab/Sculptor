// svt includes
#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_volume.h>
#include <svt_string.h>
#include <svt_itkVolume.h>
#include <svt_itkGaussianMask.h>

#define FILTER cout << svt_trimName("pruning")

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
    if (argc < 3)
    {
	cout << "usage: pruning <situs file> <maxlength> <output file>" << endl;
        exit(1);
    }

    // main data type
    typedef float PixelType;

    // create and load situs file
    svt_volume< PixelType > oVol;
    oVol.load( argv[1] );

    int iMaxLength = atoi( argv[2] );

    //
    // Delete all areas with a smaller length then x
    //
    svt_volume<PixelType>* pNewMaskVol = new svt_volume<PixelType>( oVol.getSizeX(), oVol.getSizeY(), oVol.getSizeZ(), (PixelType)(0.0) );
    vector< svt_vector3< unsigned int > > aRegions;
    unsigned int iCount;
    unsigned int iNVox = oVol.getSizeX() * oVol.getSizeY() * oVol.getSizeZ();
    PixelType fCount = (PixelType)(2.0);
    int iOVox;
    svt_vector3< unsigned int > oVec;
    svt_volume<char>* pVisited = new svt_volume<char>( oVol.getSizeX(), oVol.getSizeY(), oVol.getSizeZ(), (char)0 );
    unsigned int iNumStructs = 0;

    if (iMaxLength != 0)
    {
        try
        {
            svt_exception::ui()->progressPopup("Graph cleanup...", 0, oVol.getSizeX() );

            for( unsigned int iX=0; iX<oVol.getSizeX(); iX++ )
            {
                svt_exception::ui()->progress( iX, oVol.getSizeX() );

                for( unsigned int iY=0; iY<oVol.getSizeY(); iY++ )
                    for( unsigned int iZ=0; iZ<oVol.getSizeZ(); iZ++ )
                    {
                        if ( oVol.at( iX, iY, iZ ) == (PixelType)(1.0) && pNewMaskVol->at(iX, iY, iZ) == (PixelType)(0.0) )
                        {
                            fCount++;

                            specialFloodfill( &oVol, *pVisited, iX, iY, iZ, 0.5, fCount );

                            iOVox = oVol.getOccupied( fCount );

                            if ( iOVox > 0 && (( iMaxLength > 0 && iOVox >= iMaxLength ) || ( iMaxLength < 0 && iOVox <= (-iMaxLength) ) ))
                            {
                                for(iCount=0; iCount<iNVox; iCount++)
                                    if (oVol.at( iCount ) == fCount)
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

        pNewMaskVol->deepCopy( oVol );

    }

    FILTER << "The program has found " << iNumStructs << " Structures of the given size and length - " << endl;
    FILTER << "  out of " << fCount << " initial seed areas." << endl;

    svt_exception::ui()->progressPopdown();

    //
    // write situs file
    //
    pNewMaskVol->saveSitus( argv[3] );

    return 0;
}
