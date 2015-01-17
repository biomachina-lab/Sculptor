/***************************************************************************
                          svt_itkModelTube
			  -------------
    begin                : 04/12/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ITK_MODELTUBE_H
#define SVT_ITK_MODELTUBE_H

// svt includes
#include <svt_itkVolume.h>
// itk includes
#include <itkTubeSpatialObject.h>
#include <itkTubeSpatialObjectPoint.h>
#include <itkSpatialObjectToImageFilter.h>
#include <itkImageToSpatialObjectRegistrationMethod.h>
#include <itkEuler3DTransform.h>
#include <itkImageToSpatialObjectMetric.h>
#include <itkCommand.h>
#include <itkOnePlusOneEvolutionaryOptimizer.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkNormalVariateGenerator.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkBinaryMaskToNarrowBandPointSetFilter.h>
#include <itkPointSet.h>
#include <itkPointSetToImageRegistrationMethod.h>
#include <itkNormalizedCorrelationPointSetToImageMetric.h>

typedef itk::TubeSpatialObject< 3 > SpatialObjectType;
typedef SpatialObjectType::Pointer SpatialObjectTypePointer;
typedef itk::TubeSpatialObjectPoint < 3 > TubePointType;
typedef TubePointType::CovariantVectorType VectorType;
typedef SpatialObjectType::TransformType TubeTransformType;

///////////////////////////////////////////////////////////////////////////////
// Interator output
///////////////////////////////////////////////////////////////////////////////

/**
 * IterationCallback - Will output what the optimizer is doing
 */
template < class TOptimizer > class IterationCallback : public itk::Command
{

public:

    typedef IterationCallback Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;

    itkTypeMacro( IterationCallback, Superclass );
    itkNewMacro( Self );

    /** Type defining the optimizer. */
    typedef    TOptimizer     OptimizerType;

    /** Method to specify the optimizer. */
    void SetOptimizer( OptimizerType * optimizer )
    {
	m_Optimizer = optimizer;
	m_Optimizer->AddObserver( itk::IterationEvent(), this );
    }

    /** Execute method will print data at each iteration */
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
	Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object *, const itk::EventObject & event)
    {
	/*if( typeid( event ) == typeid( itk::StartEvent ) )
	{
	    std::cout << std::endl << "Position              Value";
	    std::cout << std::endl << std::endl;
	}
	else if( typeid( event ) == typeid( itk::IterationEvent ) )
	{
	    std::cout << m_Optimizer->GetCurrentIteration() << "   ";
	    std::cout << m_Optimizer->GetValue() << "   ";
	    std::cout << m_Optimizer->GetCurrentPosition() << std::endl;
	}
	else if( typeid( event ) == typeid( itk::EndEvent ) )
	{
	    std::cout << std::endl << std::endl;
	    std::cout << "After " << m_Optimizer->GetCurrentIteration();
	    std::cout << "  iterations " << std::endl;
	    std::cout << "Solution is    = " << m_Optimizer->GetCurrentPosition();
	    std::cout << std::endl;
	}*/
    }

protected:

    IterationCallback() {};
    itk::WeakPointer<OptimizerType>   m_Optimizer;
};

///////////////////////////////////////////////////////////////////////////////
// Main class
///////////////////////////////////////////////////////////////////////////////

/**
 * A container class for volumetric data using the itk image processing toolkit.
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_itkModelTube : public svt_itkVolume<T>
{

public:

    typedef itk::Image< T , 3 > ImageType;
    typedef typename ImageType::Pointer ImagePointerType;

protected:



public:

    /**
     * Constructor
     * \param rVolume reference to the actual svt_volume object that carries the data.
     */
    svt_itkModelTube( svt_volume<T>& rVolume ) : svt_itkVolume<T>( rVolume)
    {
    };

    /**
     * Run the filter
     */
    bool run( )
    {
        unsigned int i,j;

        //
        // create a local copy
        //
        svt_volume<T> oMaskVol;
        oMaskVol.deepCopy( this->m_rVolume );
        // create itk object
        svt_itkVolume<T> oItkVolMask( oMaskVol );

	//
	// Prepare tube
        //
        typename SpatialObjectType::Pointer itkpSpatialObject = SpatialObjectType::New();
	typename SpatialObjectType::PointListType oList;

	for( i=0; i<15; i++)
	{
	    TubePointType oP;
	    oP.SetPosition(i*5,1,1);
	    oP.SetRadius(3);
	    VectorType oNormal1;
	    VectorType oNormal2;
	    for(j=0;j<3;j++)
	    {
		oNormal1[j]=j;
		oNormal2[j]=j*2;
	    }
	    oP.SetNormal1(oNormal1);
	    oP.SetNormal2(oNormal2);
	    oP.SetColor(1,0,0,1);
	    oList.push_back(oP);
        }
        itkpSpatialObject->GetProperty()->SetName("Tube1");
        itkpSpatialObject->SetId(1);
        itkpSpatialObject->SetPoints(oList);

        itkpSpatialObject->ComputeObjectToWorldTransform();

        //
        // Rasterize of the tube
        //
        typedef unsigned char MaskPixelType;
        typedef itk::Image< MaskPixelType, 3 > MaskImageType;
        typename ImageType::RegionType  oRegion    = this->m_itkpLastImage->GetLargestPossibleRegion();
        typename ImageType::SizeType    oImageSize = oRegion.GetSize();
        //typename ImageType::SpacingType oSpacing   = this->m_itkpLastImage->GetSpacing();
        typename ImageType::PointType   oOrigin;

        oOrigin[0] = ( oImageSize[0] ) / 2.0;
        oOrigin[1] = ( oImageSize[1] ) / 2.0;
        oOrigin[2] = ( oImageSize[2] ) / 2.0;

        typedef itk::SpatialObjectToImageFilter< SpatialObjectType, ImageType > SpatialObjectToImageFilterType;
        typename SpatialObjectToImageFilterType::Pointer itkpRasterizationFilter = SpatialObjectToImageFilterType::New();

        itkpRasterizationFilter->SetInput( itkpSpatialObject );
        itkpRasterizationFilter->SetSize( oImageSize );
        //itkpRasterizationFilter->SetSpacing( oSpacing );
        //itkpRasterizationFilter->SetOrigin( oOrigin );

	svt_exception::ui()->progressPopup("Rasterization filter...");
	itkpRasterizationFilter->AddObserver(itk::ProgressEvent(), this->m_itkpProgressCallback);
        itkpRasterizationFilter->Update( );
	svt_exception::ui()->progressPopdown();

        //
        // Narrowband filter creates point set
        //
        typedef itk::PointSet< float, 3 > FixedPointSetType;
        typedef itk::BinaryMaskToNarrowBandPointSetFilter< ImageType, FixedPointSetType > NarrowBandFilterType;
        typename NarrowBandFilterType::Pointer itkpNarrowBandPointSetFilter = NarrowBandFilterType::New();

        itkpNarrowBandPointSetFilter->SetBandWidth( 2.0 );
        itkpNarrowBandPointSetFilter->SetInput( itkpRasterizationFilter->GetOutput() );

	svt_exception::ui()->progressPopup("Point set filter...");
	itkpNarrowBandPointSetFilter->AddObserver(itk::ProgressEvent(), this->m_itkpProgressCallback);
        itkpNarrowBandPointSetFilter->Update();
	svt_exception::ui()->progressPopdown();

        typename FixedPointSetType::Pointer itkpFixedPointSet = itkpNarrowBandPointSetFilter->GetOutput();
        itkpFixedPointSet->Print( std::cout );

        //
        // Registration
        //
        typedef itk::PointSetToImageRegistrationMethod< FixedPointSetType, ImageType  > RegistrationType;
	typename RegistrationType::Pointer itkpRegistration = RegistrationType::New();

        // metric
        typedef itk::NormalizedCorrelationPointSetToImageMetric< FixedPointSetType, ImageType > MetricType;
        typename MetricType::Pointer itkpMetric = MetricType::New();

        // interpolator
	typedef itk::LinearInterpolateImageFunction< ImageType, double > InterpolatorType;
	typename InterpolatorType::Pointer itkpInterpolator = InterpolatorType::New();

        // optimizer
        //typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
        typedef itk::OnePlusOneEvolutionaryOptimizer OptimizerType;
        typename OptimizerType::Pointer itkpOptimizer  = OptimizerType::New();

        // transform
        typedef itk::Euler3DTransform<> TransformType;
        typename TransformType::Pointer itkpTransform = TransformType::New();

        svtout << "Prepare random number generator..." << endl;

        // random number generator
        itk::Statistics::NormalVariateGenerator::Pointer itkpGenerator = itk::Statistics::NormalVariateGenerator::New();
        itkpGenerator->Initialize(12345);
        itkpOptimizer->SetNormalVariateGenerator( itkpGenerator );
        itkpOptimizer->Initialize( 50 );
        itkpOptimizer->SetMaximumIteration( 40000 );

        // init parameters
        typename TransformType::ParametersType oParametersScale;
        oParametersScale.set_size(6);
        oParametersScale[0] = 1000; // angle scale
        oParametersScale[1] = 1000; // angle scale
        oParametersScale[2] = 1000; // angle scale
        oParametersScale[3] = 2; // offset scale
        oParametersScale[4] = 2; // offset scale
        oParametersScale[5] = 2; // offset scale
        itkpOptimizer->SetScales( oParametersScale );

        // iteration debug output
        typedef IterationCallback< OptimizerType > IterationCallbackType;
        typename IterationCallbackType::Pointer itkpCallback = IterationCallbackType::New();
        itkpCallback->SetOptimizer( itkpOptimizer );

        // set registration
        //itkpRegistration->SetFixedImage( this->m_itkpLastImage );
        //itkpRegistration->SetMovingSpatialObject( itkpSpatialObject );

        itkpRegistration->SetMovingImage( this->m_itkpLastImage );
        itkpRegistration->SetFixedPointSet( itkpFixedPointSet );
        itkpRegistration->SetTransform( itkpTransform );
        itkpRegistration->SetInterpolator( itkpInterpolator );
        itkpRegistration->SetOptimizer( itkpOptimizer );
        itkpRegistration->SetMetric( itkpMetric );

        // initial parameters
        typename TransformType::ParametersType oInitialParameters( itkpTransform->GetNumberOfParameters() );
        oInitialParameters[0] = 0.0;     // Angle
        oInitialParameters[1] = 0.0;     // Angle
        oInitialParameters[2] = 0.0;     // Angle
        oInitialParameters[3] = 93.0;    // Offset X
        oInitialParameters[4] = 184.0;   // Offset Y
        oInitialParameters[5] = 37.0;    // Offset Z
        itkpRegistration->SetInitialTransformParameters( oInitialParameters );
        std::cout << "Initial Parameters  : " << oInitialParameters << std::endl;

        // set optimizer to maximize
        itkpOptimizer->MaximizeOn();

        // run
        try
        {
            itkpRegistration->StartRegistration();
        }
        catch( itk::ExceptionObject & exp )
        {
            std::cerr << "Exception caught ! " << std::endl;
            std::cerr << exp << std::endl;
        }

        // output solution
        typename RegistrationType::ParametersType oFinalParameters = itkpRegistration->GetLastTransformParameters();
        std::cout << "Final Solution is : " << oFinalParameters << std::endl;

	//
	// Spatial Object to Image
        //
	typedef itk::SpatialObjectToImageFilter< SpatialObjectType , ImageType > SpatialObjectToImageFilterType;
        typename SpatialObjectToImageFilterType::Pointer itkpImageFilter = SpatialObjectToImageFilterType::New();
        // set transform
        itkpTransform->SetParameters( oFinalParameters );
        itkpSpatialObject->GetObjectToNodeTransform()->SetMatrix( itkpTransform->GetMatrix() );
        itkpSpatialObject->GetObjectToNodeTransform()->SetOffset( itkpTransform->GetOffset() );
        itkpSpatialObject->ComputeObjectToWorldTransform();

	itkpImageFilter->SetInput( itkpSpatialObject );
	typename ImageType::SizeType oSize;
	oSize[0] = this->m_rVolume.getSizeX();
	oSize[1] = this->m_rVolume.getSizeY();
	oSize[2] = this->m_rVolume.getSizeZ();
	itkpImageFilter->SetSize( oSize );
        itkpImageFilter->Update();

        this->m_itkpLastImage = itkpImageFilter->GetOutput();

	//this->itkInverse();
        this->itkAdd( oItkVolMask );
	this->itkExecute();

	return true;
    };
};

#endif
