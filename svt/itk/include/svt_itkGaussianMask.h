/***************************************************************************
                          svt_itkGaussianMask
			  -------------
    begin                : 04/12/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ITK_GAUSSIANMASK_H
#define SVT_ITK_GAUSSIANMASK_H

#include <svt_itkVolume.h>

/**
 * A container class for volumetric data using the itk image processing toolkit.
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_itkGaussianMask : public svt_itkVolume<T>
{

public:

    /**
     * Constructor
     * \param rVolume reference to the actual svt_volume object that carries the data.
     */
    svt_itkGaussianMask( svt_volume<T>& rVolume ) : svt_itkVolume<T>( rVolume)
    {
    };

    /**
     * Run the filter
     * \fSigma sigma of sphere thats correlation is calculated.
     */
    bool run( double fSigma, double fThreshold, double fBlur =6.0 )
    {
        // create a local copy
        svt_volume<T> oMaskVol;
        oMaskVol.deepCopy( this->m_rVolume );
        // create itk object
        svt_itkVolume<T> oItkVolMask( oMaskVol );

        //
        // create mask
        //
        oItkVolMask.itkGaussian( fSigma );
        oItkVolMask.itkNormalize();
        oItkVolMask.itkBinaryThreshold( fThreshold, 1.0 );
        oItkVolMask.itkBinaryThinning();
        oItkVolMask.itkBinaryPruning();
        oItkVolMask.itkExecute();

        //
        // Delete all areas with a smaller length then x
        //
        T fCount = (T)(2.0);
        unsigned int iCount;
        unsigned int iNVox = oMaskVol.getSizeX() * oMaskVol.getSizeY() * oMaskVol.getSizeZ();
        unsigned int iOVox;

        svt_volume<T> oNewMaskVol( oMaskVol.getSizeX(), oMaskVol.getSizeY(), oMaskVol.getSizeZ(), (T)(0.0) );

        for( unsigned int iX=0; iX<oMaskVol.getSizeX(); iX++ )
            for( unsigned int iY=0; iY<oMaskVol.getSizeY(); iY++ )
                for( unsigned int iZ=0; iZ<oMaskVol.getSizeZ(); iZ++ )
                {
                    if ( oMaskVol.at( iX, iY, iZ ) == (T)(1.0) && oNewMaskVol.at(iX, iY, iZ) == (T)(0.0) )
                    {
                        fCount++;

                        oMaskVol.floodfill( iX, iY, iZ, 0.1, fCount );

                        iOVox = oMaskVol.getOccupied( fCount-(T)(0.1) );

                        if ( iOVox > 50 )
                        {
                            for(iCount=0; iCount<iNVox; iCount++)
                                if (oMaskVol.at( iCount ) == fCount)
                                    oNewMaskVol.setAt( iCount, (T)(1.0) );
                        }
                    }
                }


        svt_itkVolume<T> oItkVolMask_New( oNewMaskVol );

        //
        // create mask
        //
        oItkVolMask_New.itkBinaryThreshold( 0.1, 1.0 );
        oItkVolMask_New.itkGaussian( fBlur );
        oItkVolMask_New.itkNormalize();
        oItkVolMask_New.itkBinaryThreshold( 0.1, 1.0 );
        oItkVolMask_New.itkNormalize();
        oItkVolMask_New.itkExecute();

        //
        // apply mask
        //
        this->itkMultiply( oItkVolMask_New );
        this->itkExecute();

        return true;
    };
};

#endif
