/***************************************************************************
                          svt_itkVolume
			  -------------
    begin                : 03/21/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ITK_VOLUME_H
#define SVT_ITK_VOLUME_H

// svt includes
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_volume.h>
// itk includes
#include <itkImage.h>
#include <itkCommand.h>
#include <itkImportImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkLinearInterpolateImageFunction.h>
//#include >itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkExtractImageFilter.h>
#include <itkImageIORegion.h>
#include <itkVnlFFTRealToComplexConjugateImageFilter.h>
#include <itkVnlFFTComplexConjugateToRealImageFilter.h>
#include <itkComplexToRealImageFilter.h>
#include <itkComplexToImaginaryImageFilter.h>
#include <itkMultiplyImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkLaplacianImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkPixelAccessor.h>
#include <itkCastImageFilter.h>
#include <itkMesh.h>
#include <itkAddImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkImageFileWriter.h>
//#include >itkWatershedImageFilter.h>
#include <itkHessian3DToVesselnessMeasureImageFilter.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkSymmetricSecondRankTensor.h>
#include <itkBinaryThinningImageFilter.h>
#include <itkBinaryPruningImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkVnlFFTRealToComplexConjugateImageFilter.h>
#include <itkVnlFFTComplexConjugateToRealImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkBilateralImageFilter.h>
#include <itkMinMaxCurvatureFlowImageFilter.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>

extern void svt_displayITKEventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData);

/**
 * A container class for volumetric data using the itk image processing toolkit.
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_itkVolume
{
public:

    // image
    typedef itk::Image< T , 3 > ImageType;
    typedef typename ImageType::Pointer ImagePointerType;
    typedef itk::Image< T , 2 > SliceImageType;
    typedef itk::Image< unsigned char, 2 > FileImageType;
    typedef itk::Image< std::complex< T > , 3 > SpectralImageType;
    typedef typename SpectralImageType::Pointer SpectralPointerType;
    typedef itk::Image< unsigned long , 3 > LabelImageType;
    typedef typename LabelImageType::Pointer LabelImagePointerType;

    typedef itk::Vector<float, 3> VectorPixelType;
    typedef itk::Image<VectorPixelType, 2> VectorImageType;

    // pointer to filters
    typedef itk::ImageSource< ImageType >  ImageSourceType;
    typedef typename ImageSourceType::Pointer ImageSourcePointerType;

    // command for progress callback
    typedef itk::CStyleCommand CommandType;
    typedef typename CommandType::Pointer CommandPointerType;

    // import image filter
    typedef itk::ImportImageFilter< T , 3 > ImportFilterType;
    typedef typename ImportFilterType::Pointer ImportFilterPointerType;

    // file reader/writer
//    typedef itk::ImageFileReader< FileImageType > ImageFileReaderType;
//    typedef itk::ImageFileWriter< FileImageType > ImageFileWriterType;
//    typedef typename ImageFileReaderType::Pointer ImageFileReaderPointerType;
//    typedef typename ImageFileWriterType::Pointer ImageFileWriterPointerType;

protected:

    svt_volume<T>& m_rVolume;

    ImportFilterPointerType m_itkpImportFilter;

    ImagePointerType m_itkpLastImage;

    SpectralPointerType m_itkpSpectralImage;

    CommandPointerType m_itkpProgressCallback;

public:

    /**
     * Constructor
     * \param rVolume reference to the actual svt_volume object that carries the data.
     */
    svt_itkVolume(svt_volume<T>& rVolume) :
        m_rVolume( rVolume )
    {
        this->itkCreateImportFilter();

	// To set an observer on the progress event.
	m_itkpProgressCallback = CommandType::New();
	m_itkpProgressCallback->SetCallback( svt_displayITKEventCallback );
    };

    /**
     * Write image
     */
    void itkWrite(char* pFname)
    {
        typedef itk::ImageFileWriter< ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

	itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetFileName( pFname );

	svt_exception::ui()->progressPopup("Write to file...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();
    };

    /**
     * Normalize the volume
     */
    void itkNormalize()
    {
        // rescale intensity filter
        typedef itk::RescaleIntensityImageFilter< ImageType, ImageType > RescaleIntensityFilterType;
        typedef typename RescaleIntensityFilterType::Pointer RescaleIntensityFilterPointerType;

	RescaleIntensityFilterPointerType itkpNormFilter = RescaleIntensityFilterType::New();

	itkpNormFilter->SetInput( m_itkpLastImage );
	itkpNormFilter->SetOutputMinimum( 0.0 );
	itkpNormFilter->SetOutputMaximum( 1.0 );
	svt_exception::ui()->progressPopup("Normalize...");
	itkpNormFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpNormFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpNormFilter->GetOutput();
    };

    /**
     * Interpolation to a new voxelsize
     */
    void itkInterpolate( double fVoxelWidth )
    {
        // linear interpolator
        typedef itk::LinearInterpolateImageFunction< ImageType, double > LinearInterpolatorType;
        typedef typename LinearInterpolatorType::Pointer LinearInterpolatorPointerType;
        // resample filter
        typedef itk::ResampleImageFilter< ImageType, ImageType> ResampleImageFilterType;
        typedef typename ResampleImageFilterType::Pointer ResampleImageFilterPointerType;
        // affine transform
        typedef itk::AffineTransform< double, 3 > AffineTransformType;
        typedef typename AffineTransformType::Pointer AffineTransformPointerType;


        // resample filter
        ResampleImageFilterPointerType itkpImageFilter = ResampleImageFilterType::New();

        // transformation
        AffineTransformPointerType itkpTransform = AffineTransformType::New();
        itkpImageFilter->SetTransform( itkpTransform );

        // interpolator
        //NNInterpolatorPointerType itkpInterpolator = NNInterpolatorType::New();
        //itkpImageFilter->SetInterpolator( itkpInterpolator );
        LinearInterpolatorPointerType itkpInterpolator = LinearInterpolatorType::New();
        itkpImageFilter->SetInterpolator( itkpInterpolator );

        // default pixel value
        itkpImageFilter->SetDefaultPixelValue( 0 );

        // new voxelwidth
        double aSpacing[ 3 ];
        aSpacing[0] = fVoxelWidth;
        aSpacing[1] = fVoxelWidth;
        aSpacing[2] = fVoxelWidth;
        itkpImageFilter->SetOutputSpacing( aSpacing );

        // origin
        double aOrigin[ 3 ];
        aOrigin[0] = 0.0;
        aOrigin[1] = 0.0;
        aOrigin[1] = 0.0;
        itkpImageFilter->SetOutputOrigin( aOrigin );

        // size
        double fRatio = m_rVolume.getWidth() / fVoxelWidth;
        typename ImageType::SizeType size;
        size[0] = (unsigned int)(m_rVolume.getSizeX() * fRatio) + 1;
        size[1] = (unsigned int)(m_rVolume.getSizeY() * fRatio) + 1;
        size[2] = (unsigned int)(m_rVolume.getSizeZ() * fRatio) + 1;
        itkpImageFilter->SetSize( size );

        // input image
        itkpImageFilter->SetInput( m_itkpLastImage );

        // ok, update
        svt_exception::ui()->progressPopup("Interpolate filter...");
        itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
        itkpImageFilter->Update();
        svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();

        m_rVolume.allocate( size[0], size[1], size[2] );
        m_rVolume.setWidth( fVoxelWidth );
    };

    /**
     * Gradient Magnitude
     */
    void itkGradientMagnitude()
    {
	typedef itk::GradientMagnitudeImageFilter< ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

	itkpImageFilter->SetInput( m_itkpLastImage );

	try
	{
	    svt_exception::ui()->progressPopup("Gradient magnitude filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Curvature Flow
     */
    void itkCurvatureFlow(unsigned int iIterations)
    {
	typedef itk::CurvatureFlowImageFilter< ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetNumberOfIterations( iIterations );
        itkpImageFilter->SetTimeStep( 0.0625 );

	try
	{
	    svt_exception::ui()->progressPopup("Curvature flow filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Min./Max. Curvature Flow
     */
    void itkMinMaxCurvatureFlow()
    {
	typedef itk::MinMaxCurvatureFlowImageFilter< ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetNumberOfIterations( 10 );
        itkpImageFilter->SetTimeStep( 0.0625 );
        itkpImageFilter->SetStencilRadius( 1 );

	try
	{
	    svt_exception::ui()->progressPopup("MinMaxCurvature flow filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Histogram Equalization
     */
    void itkHistogramEqualization(Real64 fAlpha = 0.0, Real64 fBeta = 1.0, Real64 iRadius = 1)
    {
	typedef itk::AdaptiveHistogramEqualizationImageFilter< ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetAlpha(fAlpha);
        itkpImageFilter->SetBeta(fBeta);

        typename FilterType::ImageSizeType oRadius;
  
        oRadius[0] = iRadius; // radius along x
        oRadius[1] = iRadius; // radius along y
        oRadius[2] = iRadius; // radius along z

        itkpImageFilter->SetRadius( oRadius );

	try
	{
	    svt_exception::ui()->progressPopup("Histogram equalization filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Bilateral Filter
     */
    void itkBilateral(Real64 fDomainSigma, Real64 fRangeSigma)
    {
	typedef itk::BilateralImageFilter< ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

        Real64 aDomainSigmas[3] = { fDomainSigma, fDomainSigma, fDomainSigma };

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetDomainSigma( aDomainSigmas );
        itkpImageFilter->SetRangeSigma( fRangeSigma );;

	try
	{
	    svt_exception::ui()->progressPopup("Bilateral filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Band-Pass Filter
     */
    void itkBandPass()
    {
        //
        // Image to Fourier Space
        //
        typedef itk::VnlFFTRealToComplexConjugateImageFilter< ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

	itkpImageFilter->SetInput( m_itkpLastImage );

	try
	{
	    svt_exception::ui()->progressPopup("Band-pass filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        //
        // Masking
        //
        //typedef FilterType::OutputImageType SpectralImageType;
        //typedef itk::MaskImageFilter< SpectralImageType, ImageType, SpectralImageType > MaskFilterType;
        //MaskFilterType::Pointer maskFilter = MaskFilterType::New();

        //
        // Fourier Space to Image
        //
        typedef itk::VnlFFTComplexConjugateToRealImageFilter< ImageType > IFilterType;
        typedef typename IFilterType::Pointer IFilterPointerType;
        IFilterPointerType itkpIImageFilter = IFilterType::New();

        itkpIImageFilter->SetInput( itkpImageFilter->GetOutput() );

        try
	{
	    svt_exception::ui()->progressPopup("Band-pass filter...");
	    itkpIImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpIImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        //
        // End
        //
        m_itkpLastImage = itkpIImageFilter->GetOutput();
    };

    /**
     * Dilation
     */
    void itkDilate()
    {
        typedef itk::BinaryBallStructuringElement< T, 3 > StructuringElementType;
        typedef itk::BinaryDilateImageFilter< ImageType, ImageType, StructuringElementType > FilterType;
	typedef typename FilterType::Pointer PointerType;

        PointerType itkpImageFilter = FilterType::New();

        StructuringElementType oStructuringElement;
        oStructuringElement.SetRadius( 1 );
        oStructuringElement.CreateStructuringElement();

        itkpImageFilter->SetKernel( oStructuringElement );
        itkpImageFilter->SetDilateValue( 1.0 );
	itkpImageFilter->SetInput( m_itkpLastImage );

	try
	{
	    svt_exception::ui()->progressPopup("Dilate filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };


    /**
     * Hessian Vesselness Filter
     */
    void itkVesselness( Real64 fSigma, Real64 fAlpha1, Real64 fAlpha2 )
    {
        typedef itk::HessianRecursiveGaussianImageFilter< ImageType > HessianFilterType;
        typedef typename HessianFilterType::Pointer HessianFilterPointerType;

        typedef itk::Hessian3DToVesselnessMeasureImageFilter< T > VesselnessFilterType;
        typedef typename VesselnessFilterType::Pointer VesselnessFilterPointerType;

        HessianFilterPointerType itkpImageFilter_A = HessianFilterType::New();
        VesselnessFilterPointerType itkpImageFilter_B = VesselnessFilterType::New();

	itkpImageFilter_A->SetInput( m_itkpLastImage );
	itkpImageFilter_B->SetInput( itkpImageFilter_A->GetOutput() );

        itkpImageFilter_A->SetSigma( fSigma );
        itkpImageFilter_B->SetAlpha1( fAlpha1 );
        itkpImageFilter_B->SetAlpha2( fAlpha2 );

	try
	{
	    svt_exception::ui()->progressPopup("Vesselness filter...");
	    itkpImageFilter_B->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter_B->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter_B->GetOutput();
    };

    /**
     * Gaussian blurring
     */
    void itkGaussian(double fVariance)
    {
        typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetVariance( fVariance );

	svt_exception::ui()->progressPopup("Gaussian filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Binary thinning
     */
    void itkBinaryThinning()
    {
        typedef itk::BinaryThinningImageFilter< ImageType, ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );

	svt_exception::ui()->progressPopup("Binary thinning filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Connected threshold image filter
     */
    void itkConnectedThreshold( T fThreshold, vector< svt_vector3<unsigned int> > aSeeds )
    {
        typedef itk::ConnectedThresholdImageFilter< ImageType, ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );

        itkpImageFilter->SetUpper( m_rVolume.getMaxDensity() );
        itkpImageFilter->SetLower( fThreshold );
        itkpImageFilter->SetReplaceValue( 255 );

        itk::Index<3> oIndex;
        for(unsigned int i=0; i<aSeeds.size(); i++)
        {
            oIndex[0] = aSeeds[i].x();
            oIndex[1] = aSeeds[i].y();
            oIndex[2] = aSeeds[i].z();
            itkpImageFilter->AddSeed( oIndex );
        }

	svt_exception::ui()->progressPopup("Connected threshold filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Binary pruning 
     */
    void itkBinaryPruning()
    {
        typedef itk::BinaryPruningImageFilter< ImageType, ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );

	svt_exception::ui()->progressPopup("Binary pruning filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Laplacian
     */
    void itkLaplacian()
    {
	typedef itk::LaplacianImageFilter< ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

	itkpImageFilter->SetInput( m_itkpLastImage );

	try
	{
	    svt_exception::ui()->progressPopup("Laplacian filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Sigmoid Filter
     */
    void itkSigmoid()
    {
	typedef itk::SigmoidImageFilter< ImageType, ImageType > FilterType;
	typename FilterType::Pointer itkpImageFilter = FilterType::New();

        itkpImageFilter->SetOutputMinimum( 0.0 );
	itkpImageFilter->SetOutputMaximum( 1.0 );
	itkpImageFilter->SetAlpha( -0.023 );
        itkpImageFilter->SetBeta( 0.08 );

	itkpImageFilter->SetInput( m_itkpLastImage );

	try
	{
	    svt_exception::ui()->progressPopup("Sigmoid filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();

    };

    /**
     * Binary threshold filter - sets all values below the threshold to 0 and above to 1
     */
    void itkBinaryThreshold( T fLowerThreshold, T fUpperThreshold )
    {
        typedef itk::BinaryThresholdImageFilter< ImageType, ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetOutsideValue( 0 );
        itkpImageFilter->SetInsideValue( 1 );
        itkpImageFilter->SetLowerThreshold( fLowerThreshold );
        itkpImageFilter->SetUpperThreshold( fUpperThreshold );

	svt_exception::ui()->progressPopup("Threshold filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Threshold filter - sets all the values below the given intensity to 0
     * \param fThreshold
     */
    void itkThresholdBelow( T fThreshold )
    {
        typedef itk::ThresholdImageFilter< ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetOutsideValue( 0 );
        itkpImageFilter->ThresholdBelow( fThreshold );

	svt_exception::ui()->progressPopup("Threshold filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Threshold filter - sets all the values above the given intensity to 0
     * \param fThreshold
     */
    void itkThresholdAbove( T fThreshold )
    {
        typedef itk::ThresholdImageFilter< ImageType > FilterType;
        typedef typename FilterType::Pointer PointerType;
        PointerType itkpImageFilter = FilterType::New();

        itkpImageFilter->SetInput( m_itkpLastImage );
        itkpImageFilter->SetOutsideValue( 0 );
        itkpImageFilter->ThresholdAbove( fThreshold );

	svt_exception::ui()->progressPopup("Threshold filter...");
	itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpImageFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Watershed segmentation
     * \fSigma
     */
/*    svt_volume<unsigned long>* itkWatershed( double fLevel, double fThreshold )
    {
        typedef itk::WatershedImageFilter<ImageType> WatershedFilterType;
        typedef typename WatershedFilterType::Pointer WatershedFilterPointerType;

        WatershedFilterPointerType itkpImageFilter = WatershedFilterType::New();

        // parameters
        itkpImageFilter->SetThreshold( fThreshold );
        itkpImageFilter->SetLevel( fLevel );

        // input image
        itkpImageFilter->SetInput( m_itkpLastImage );

        // ok, update
        svt_exception::ui()->progressPopup("Watershed segmentation...");
        itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
        itkpImageFilter->Update();
        svt_exception::ui()->progressPopdown();

        // ok, now process results
        svt_volume<unsigned long>* pVolume = new svt_volume<unsigned long>();
        pVolume->allocate( m_rVolume.getSizeX(), m_rVolume.getSizeY(), m_rVolume.getSizeZ() );

        LabelImagePointerType itkpLabelImage = itkpImageFilter->GetOutput();

        // now copy the segmentation information into a new svt_volume
	itk::ImageRegionConstIterator< LabelImageType > in( itkpLabelImage, itkpLabelImage->GetLargestPossibleRegion() );

	unsigned int i;
	for( i=0, in.GoToBegin(); !in.IsAtEnd(); ++in, i++ )
	    pVolume->setAt( i, in.Get() );

        return pVolume;
    };
*/
    /**
     * Execute filter pipeline
     */
    void itkExecute()
    {
	itkCopyBack( m_itkpLastImage );
    };

    /**
     * Save a slice as image file to disk
     * \param iAxis 0=x 1=y 2=z axis
     * \param iIndex index of slice to extract
     * \param pFname pointer to array of char with the filename
     */
    /*void itkExtractSlice( const unsigned int iAxis, const unsigned int iIndex, const char* pFname )
    {
        // slice extractor
        typedef itk::ExtractImageFilter< ImageType, SliceImageType > ExtractImageFilterType;
        typedef typename ExtractImageFilterType::Pointer ExtractImageFilterPointerType;

        // image writer
        ImageFileWriterPointerType itkpWriter = ImageFileWriterType::New();
        // slice extracter
        ExtractImageFilterPointerType itkpExtract = ExtractImageFilterType::New();

        //
        // extract
        //

        // select x dimension
        typename ImageType::RegionType oInputRegion = m_itkpLastImage->GetLargestPossibleRegion();
        typename ImageType::SizeType oSize = oInputRegion.GetSize();
        oSize[iAxis] = 0;
        // select slice number
        typename ImageType::IndexType oStart = oInputRegion.GetIndex();
        oStart[iAxis] = iIndex;
        // OK, store that info
        typename ImageType::RegionType oDesiredRegion;
        oDesiredRegion.SetSize( oSize );
        oDesiredRegion.SetIndex( oStart );
        itkpExtract->SetExtractionRegion( oDesiredRegion );
        itkpExtract->SetInput( m_itkpLastImage );

        //
        // rescale intensities
        typedef itk::RescaleIntensityImageFilter< SliceImageType, FileImageType > SliceNormFilterType;
        typedef typename SliceNormFilterType::Pointer SliceNormFilterPointerType;

        SliceNormFilterPointerType itkpNormFilter = SliceNormFilterType::New();

	itkpNormFilter->SetInput( itkpExtract->GetOutput() );
	itkpNormFilter->SetOutputMinimum( 0 );
	itkpNormFilter->SetOutputMaximum( 255 );
	svt_exception::ui()->progressPopup("Normalize...");
	itkpNormFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	itkpNormFilter->Update();
	svt_exception::ui()->progressPopdown();

        // set input
        itkpWriter->SetFileName( pFname );
        itkpWriter->SetInput( itkpNormFilter->GetOutput() );

        // go
        try
        {
            itkpWriter->Update();
        }
        catch( itk::ExceptionObject & err )
        {
            svtout << "Error: Could not extract slice with the index " << iIndex << " into " << pFname << endl;
            svtout << "  - " << err << endl;
        }

    };
*/
    /**
     * FFT - real to complex transform
     */
    void itkRealToComplex()
    {
	typedef itk::VnlFFTRealToComplexConjugateImageFilter< T, 3 > FFTFilterType;
	typedef typename FFTFilterType::Pointer FFTFilterPointerType;
	FFTFilterPointerType itkpImageFilter = FFTFilterType::New();

	itkpImageFilter->SetInput( m_itkpLastImage );

	try
	{
	    svt_exception::ui()->progressPopup("FFT (real to complex) filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpSpectralImage = itkpImageFilter->GetOutput();
    };

    /**
     * FFT - complex to realtransform
     */
    void itkComplexToReal()
    {
	typedef itk::VnlFFTComplexConjugateToRealImageFilter< T, 3 > InverseFFTFilterType;
	typedef typename InverseFFTFilterType::Pointer InverseFFTFilterPointerType;
	InverseFFTFilterPointerType itkpImageFilter = InverseFFTFilterType::New();

	itkpImageFilter->SetInput( m_itkpSpectralImage );

	try
	{
	    svt_exception::ui()->progressPopup("FFT (complex to real) filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Multiply
     */
    void itkMultiply(svt_itkVolume<T>& rOther)
    {
	typedef itk::MultiplyImageFilter< ImageType, ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

	itkpImageFilter->SetInput1( m_itkpLastImage );
	itkpImageFilter->SetInput2( rOther.getImage() );

	try
	{
	    svt_exception::ui()->progressPopup("Multiply filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Add
     */
    void itkAdd(svt_itkVolume<T>& rOther)
    {
	typedef itk::AddImageFilter< ImageType, ImageType, ImageType > FilterType;
	typedef typename FilterType::Pointer FilterPointerType;

	FilterPointerType itkpImageFilter = FilterType::New();

	itkpImageFilter->SetInput1( m_itkpLastImage );
	itkpImageFilter->SetInput2( rOther.getImage() );

	try
	{
	    svt_exception::ui()->progressPopup("Add filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpLastImage = itkpImageFilter->GetOutput();
    };

    /**
     * Convolve
     */
    void itkConvolve(svt_itkVolume<T>& rOther)
    {
	typedef itk::MultiplyImageFilter< SpectralImageType, SpectralImageType, SpectralImageType > MultiplyImageFilterType;
	typedef typename MultiplyImageFilterType::Pointer MultiplyImageFilterPointerType;

	MultiplyImageFilterPointerType itkpImageFilter = MultiplyImageFilterType::New();

	itkpImageFilter->SetInput1( m_itkpSpectralImage );
	itkpImageFilter->SetInput2( rOther.getSpectralImage() );

	try
	{
	    svt_exception::ui()->progressPopup("Convolution filter...");
	    itkpImageFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	    itkpImageFilter->Update();
	    svt_exception::ui()->progressPopdown();
	}
	catch( itk::ExceptionObject & excp )
	{
	    svtout << "Error: " << std::endl;
	    svtout << excp << std::endl;
	    return;
	}

        m_itkpSpectralImage = itkpImageFilter->GetOutput();
    };

    /**
     * Inverse
     */
    void itkInverse()
    {
	itk::ImageRegionIterator< ImageType > in( m_itkpLastImage, m_itkpLastImage->GetLargestPossibleRegion() );

	unsigned int i;
	for( i=0, in.GoToBegin(); !in.IsAtEnd(); ++in, i++ )
	    in.Set( 1.0 - in.Get() );
    };

    /**
     * Get the spectral image
     */
    SpectralPointerType getSpectralImage()
    {
        return m_itkpSpectralImage;
    };
    /**
     * Get the image
     */
    ImagePointerType getImage()
    {
        return m_itkpLastImage;
    };

protected:

    /**
     * Create itk representation of volume data
     */
    void itkCreateImportFilter()
    {
        m_itkpImportFilter = ImportFilterType::New();

	// set size
	typename ImportFilterType::SizeType size;
	size[0] = m_rVolume.getSizeX();
	size[1] = m_rVolume.getSizeY();
	size[2] = m_rVolume.getSizeZ();

	// index
	typename ImportFilterType::IndexType start;
	start.Fill( 0 );

	// region
	typename ImportFilterType::RegionType region;
	region.SetIndex( start );
	region.SetSize( size );

	m_itkpImportFilter->SetRegion( region );

	// origin
	double origin[3];
	origin[0] = 0.0; //m_rVolume.getGridX();
	origin[1] = 0.0; //m_rVolume.getGridY();
	origin[2] = 0.0; //m_rVolume.getGridZ();

	m_itkpImportFilter->SetOrigin( origin );

	// spacing
	double spacing[3];
	spacing[0] = m_rVolume.getWidth();
	spacing[1] = m_rVolume.getWidth();
	spacing[2] = m_rVolume.getWidth();

	m_itkpImportFilter->SetSpacing( spacing );

	// now set the pointer to the situs data
	m_itkpImportFilter->SetImportPointer( m_rVolume.getData(), size[0]*size[1]*size[2], false );

	svt_exception::ui()->progressPopup("Import filter...");
	m_itkpImportFilter->AddObserver(itk::ProgressEvent(), m_itkpProgressCallback);
	m_itkpImportFilter->Update();
	svt_exception::ui()->progressPopdown();

        m_itkpLastImage = m_itkpImportFilter->GetOutput();
    };

    /**
     * Copy the data from a filter back to the svt_volume object
     */
    void itkCopyBack(ImagePointerType itkpImage)
    {
	itk::ImageRegionConstIterator< ImageType > in( itkpImage, itkpImage->GetLargestPossibleRegion() );

	unsigned int i;
	for( i=0, in.GoToBegin(); !in.IsAtEnd(); ++in, i++ )
	    m_rVolume.setAt( i, in.Get() );
    };
};

#endif
