#ifndef __statisticsonimage_h
#define __statisticsonimage_h

#include "itkImageFileReader.h"
#include "itkImageMaskSpatialObject.h"
#include "itkStatisticsImageFilterWithMask.h"
#include "itkCastImageFilter.h"
#include "itkDeformationFieldJacobianDeterminantFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkScalarImageToHistogramGenerator2.h"
#include "itkMaskImageFilter.h"

#include "statisticsprinters.h"

/** This file defines two templated functions */

/** 
 * ************************ ComputeStatistics **************************8
 *
 * Generic template function that computes statistics on an input image
 * Assumes that the number of histogram bins and the marginal scale are set
 * in the histogramGenerator, that the maskerOrCopier has been initialised
 * to a (dummy) caster or to a MaskFilterType with mask set, and that the 
 * statistics filter has been initialised.
 * 
 * This function is only to be used by the StatisticsOnImage function.
 * It is quite specific and not really well overthought. Introduced to avoid
 * duplication of code.
 */

template< class TInputImage, class TBaseFilter, class TStatisticsFilter, class THistogramGenerator >
void ComputeStatistics(
  TInputImage * inputImage,
  TBaseFilter * maskerOrCopier,
  TStatisticsFilter * statistics,
  THistogramGenerator * histogramGenerator,
  unsigned int numberOfBins,
  const std::string & histogramOutputFileName)
{
  typedef THistogramGenerator                         HistogramGeneratorType;
  typedef typename 
    HistogramGeneratorType::HistogramType             HistogramType;
  typedef TStatisticsFilter                           StatisticsFilterType;
  typedef typename StatisticsFilterType::PixelType    PixelType;
  
  statistics->SetInput( inputImage );
  std::cout << "\tComputing statistics..." << std::endl;
  statistics->Update();
  std::cout << "\tDone computing statistics." << std::endl;
  PrintStatistics<StatisticsFilterType>(statistics);
   
  maskerOrCopier->SetInput( inputImage );
  std::cout << "\tReplacing all pixels outside the mask by -infinity, to make sure they are not included in the histogram..." << std::endl;
  maskerOrCopier->Update();
  std::cout << "\tDone replacing all pixels outside the mask by -infinity." << std::endl;
  
  /** this code is copied from the ListSampleToHistogramGenerator->GenerateData().
   * It makes sure that the maximum values are also included in the histogram. */
  PixelType histogramMax;
  if ( !itk::NumericTraits< PixelType >::is_integer )
  {
    /** floating pixeltype */
    double marginalScale = 100.0;
    double uppermargin = 
      static_cast<double>( statistics->GetMaximum() - statistics->GetMinimum() ) /
      static_cast<double>(numberOfBins) /
      marginalScale;
    histogramMax = static_cast<PixelType>( statistics->GetMaximum() + uppermargin );
  }
  else
  {
    /** integer pixeltypes. in principle this function will never be called with
     * an integer pixeltype, but just in case this is changed in the future...*/
    PixelType uppermargin = itk::NumericTraits<PixelType>::One;
    histogramMax = static_cast<PixelType>( statistics->GetMaximum() + uppermargin );
  }
  if ( histogramMax <= statistics->GetMaximum() )
  {
    /** overflow occcured; maximum was already maximum of pixeltype;
     * We could solve this somehow, but the situation is quite unlikely;
     * anyway, mostly something is going wrong when a float image has value
     * infinity somewhere */
    std::cerr << "\tError during histogram computation!" << std::endl;
    std::cerr << "\tThe maximum of the image is equal to the maximum of its pixeltype." << std::endl;
    std::cerr << "\tHistogram computation cannot be performed now. pxstatisticsonimage cannot handle this situation." << std::endl;
    itkGenericExceptionMacro(<< "Histogram cannot be computed.");        
  }

  histogramGenerator->SetAutoMinMax(false);
  histogramGenerator->SetNumberOfBins(numberOfBins);
  histogramGenerator->SetHistogramMin( statistics->GetMinimum() );
  histogramGenerator->SetHistogramMax( histogramMax );
  histogramGenerator->SetInput( maskerOrCopier->GetOutput() );
  std::cout << "\tComputing histogram..." << std::endl;
  histogramGenerator->Compute();
  std::cout << "\tDone computing histogram." << std::endl;
  PrintHistogramStatistics<HistogramType>( histogramGenerator->GetOutput(), histogramOutputFileName );

} // end ComputeStatistics


/**
 * ******************* StatisticsOnImage *******************
 *
 * The real functionality or pxstatisticsonimage,
 * templated over image properties.
 */

template< class ComponentType, unsigned int Dimension, unsigned int NumberOfComponents >
void StatisticsOnImage(
  const std::string & inputFileName,
  const std::string & maskFileName, 
  const std::string & histogramOutputFileName,
  bool useMagnitude,
  bool useJacobian,
  unsigned int numberOfBins)
{
	/** Typedefs. */
  typedef ComponentType ScalarPixelType;
  typedef float InternalPixelType;
  typedef unsigned char MaskPixelType;
  typedef itk::Vector<ComponentType, NumberOfComponents>  VectorPixelType;
  typedef itk::Vector<ComponentType, Dimension>           DeformationVectorPixelType;
  typedef itk::Image<ScalarPixelType, Dimension>          ScalarImageType;
  typedef itk::Image<VectorPixelType, Dimension>          VectorImageType;
  typedef itk::Image<
    DeformationVectorPixelType, Dimension>                DeformationVectorImageType;
  
  typedef itk::Image<InternalPixelType, Dimension>    InternalImageType;
  typedef itk::Image<MaskPixelType, Dimension>        MaskImageType;
  typedef itk::ImageMaskSpatialObject< Dimension>     MaskSpatialObjectType;
  
  typedef itk::ImageToImageFilter<
    InternalImageType, InternalImageType>             BaseFilterType;
	typedef itk::ImageFileReader< ScalarImageType >			ScalarReaderType;
  typedef itk::ImageFileReader< VectorImageType >			VectorReaderType;
  typedef itk::ImageFileReader< MaskImageType >       MaskReaderType;
  typedef itk::CastImageFilter<
    ScalarImageType, InternalImageType >              CasterType;
  typedef itk::CastImageFilter<
    InternalImageType, InternalImageType>             CopierType;
  typedef itk::GradientToMagnitudeImageFilter<
    VectorImageType, InternalImageType >              MagnitudeFilterType;
  typedef itk::DeformationFieldJacobianDeterminantFilter<
    DeformationVectorImageType, InternalPixelType >   JacobianFilterType;
  typedef itk::StatisticsImageFilter<
    InternalImageType >                               StatisticsFilterType;
  typedef itk::Statistics::ScalarImageToHistogramGenerator2<
    InternalImageType>                                HistogramGeneratorType;
  typedef itk::MaskImageFilter< InternalImageType,
    MaskImageType, InternalImageType>                 MaskerType;
  typedef typename 
    HistogramGeneratorType::HistogramType             HistogramType;

  /** Read mask */
  typename MaskSpatialObjectType::Pointer mask = 0;
  typename MaskReaderType::Pointer maskReader =
    MaskReaderType::New();
  typename BaseFilterType::Pointer maskerOrCopier = CopierType::New();
  if ( maskFileName != "" )
  {
    /** Read mask */
    maskReader->SetFileName( maskFileName.c_str() );
    std::cout << "Reading Mask..." << std::endl;
    maskReader->Update();
    std::cout << "Done reading Mask." << std::endl;

    /** Convert to spatial object */
    mask = MaskSpatialObjectType::New();
    mask->SetImage( maskReader->GetOutput() );

    /** Prepare filter that applies mask to an image (by replacing all pixels
     * that fall outside the mask by -infinity */
    typename MaskerType::Pointer maskFilter = MaskerType::New();
    maskFilter->SetInput2( maskReader->GetOutput() );
    maskFilter->SetOutsideValue(
      itk::NumericTraits<InternalPixelType>::NonpositiveMin() );
    maskerOrCopier = maskFilter;
  }
  
  /** Create StatisticsFilter */
  typename StatisticsFilterType::Pointer statistics = 
    StatisticsFilterType::New();
  statistics->SetMask(mask);

  typename HistogramGeneratorType::Pointer histogramGenerator = 
    HistogramGeneratorType::New();

  if ( NumberOfComponents == 1 )
  {
    typename ScalarReaderType::Pointer reader = ScalarReaderType::New();
    typename CasterType::Pointer caster = CasterType::New();

    std::cout << "InputImage is a scalar image." << std::endl;
    std::cout << "Statistics are computed on the gray values." << std::endl;

    reader->SetFileName( inputFileName.c_str() );
    std::cout << "\tReading input image..." << std::endl;
    reader->Update();
    std::cout << "\tDone reading input image." << std::endl;
    
    caster->SetInput( reader->GetOutput() );
    std::cout << "\tCasting input image to float..." << std::endl;
    caster->Update();
    std::cout << "\tDone casting input image to float." << std::endl;

    std::string histout = "";
    if ( histogramOutputFileName != "" )
    {
      histout = histogramOutputFileName + "INTENSITY.txt";
    }

    /** Call the generic ComputeStatistics function */
    ComputeStatistics<
      InternalImageType,
      BaseFilterType,
      StatisticsFilterType,
      HistogramGeneratorType>(
        caster->GetOutput(),
        maskerOrCopier,
        statistics,
        histogramGenerator,
        numberOfBins,
        histout);
        
  } // end scalar images
  else
  {
    typename VectorReaderType::Pointer reader = VectorReaderType::New();
    
    std::cout << "InputImage is a vector image." << std::endl;
    
    reader->SetFileName( inputFileName.c_str() );
    std::cout << "\tReading input image..." << std::endl;
    reader->Update();
    std::cout << "\tDone reading input image." << std::endl;
  
    if (useMagnitude)
    {
      typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
      
      std::cout << "Statistics are computed on the magnitude of the vectors." << std::endl;

      magnitudeFilter->SetInput( reader->GetOutput() );
      std::cout << "\tComputing magnitude image..." << std::endl;
      magnitudeFilter->Update();
      std::cout << "\tDone computing magnitude image." << std::endl;
      
      std::string histout = "";
      if ( histogramOutputFileName != "" )
      {
         histout = histogramOutputFileName + "MAGNITUDE.txt";
      }

      /** Call the generic ComputeStatistics function */
      ComputeStatistics<
        InternalImageType,
        BaseFilterType,
        StatisticsFilterType,
        HistogramGeneratorType>(
          magnitudeFilter->GetOutput(),
          maskerOrCopier,
          statistics,
          histogramGenerator,
          numberOfBins,
          histout);

    } // end useMagnitude
    if (useJacobian && NumberOfComponents == Dimension )
    {
      typename JacobianFilterType::Pointer jacobianFilter = JacobianFilterType::New();
      
      std::cout << "Statistics are computed on the jacobian of the vectors." << std::endl;

      /** Hack to make the template function compile for the case NumberOfComponents != Dimension.  */
      DeformationVectorImageType::Pointer deformationVectorImage =
        dynamic_cast< DeformationVectorImageType * >( reader->GetOutput() );

      jacobianFilter->SetInput( deformationVectorImage );
      std::cout << "\tComputing jacobian image..." << std::endl;
      jacobianFilter->Update();
      std::cout << "\tDone computing jacobian image." << std::endl;

      std::string histout = "";
      if ( histogramOutputFileName != "" )
      {
        histout = histogramOutputFileName + "JACOBIAN.txt";
      }

      /** Call the generic ComputeStatistics function */
      ComputeStatistics<
        InternalImageType,
        BaseFilterType,
        StatisticsFilterType,
        HistogramGeneratorType>(
          jacobianFilter->GetOutput(),
          maskerOrCopier,
          statistics,
          histogramGenerator,
          numberOfBins,
          histout);

    } // end useJacobian
    if (useJacobian && NumberOfComponents != Dimension )
    {
      std::cout << "The Jacobian cannot be computed, because the image dimension is not equal to the number of components of the pixels." << std::endl;
    }
  } // end vector images

} // end StatisticsOnImage


#endif // #ifndef __statisticsonimage_h
