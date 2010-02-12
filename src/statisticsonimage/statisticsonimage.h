#ifndef __statisticsonimage_h
#define __statisticsonimage_h

#include "itkImageFileReader.h"
#include "itkImageMaskSpatialObject.h"
#include "itkStatisticsImageFilterWithMask.h"
#include "itkCastImageFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkScalarImageToHistogramGenerator2.h"
#include "itkMaskImageFilter.h"
#include "itkLogImageFilter.h"

#include "statisticsprinters.h"

/** This file defines two templated functions */

/** 
 * ************************ ComputeStatistics **************************8
 *
 * Generic template function that computes statistics on an input image
 * Assumes that the number of histogram bins and the marginal scale are set
 * in the histogramGenerator, that the maskerOrCopier has been initialized
 * to a (dummy) caster or to a MaskFilterType with mask set, and that the 
 * statistics filter has been initialized.
 * 
 * This function is only to be used by the StatisticsOnImage function.
 * It is quite specific and not really well over thought. Introduced to avoid
 * duplication of code.
 */

template< class TInputImage, class TBaseFilter, class TStatisticsFilter, class THistogramGenerator >
void ComputeStatistics(
  TInputImage * inputImage,
  TBaseFilter * maskerOrCopier,
  TStatisticsFilter * statistics,
  THistogramGenerator * histogramGenerator,
  unsigned int numberOfBins,
  const std::string & histogramOutputFileName,
  const std::string & select )
{
  typedef THistogramGenerator                         HistogramGeneratorType;
  typedef typename 
    HistogramGeneratorType::HistogramType             HistogramType;
  typedef typename HistogramGeneratorType::RealPixelType   RealPixelType;
  typedef TStatisticsFilter                           StatisticsFilterType;
  typedef typename StatisticsFilterType::PixelType    PixelType;
  typedef TInputImage                                 InputImageType;
  typedef itk::LogImageFilter<
    InputImageType, InputImageType>                   LogFilterType;
  
  /** Arithmetic mean */
  PixelType maxPixelValue, minPixelValue;
  if ( select == "arithmetic" || select == "" || select == "histogram" )
  {
    statistics->SetInput( inputImage );
    std::cout << "Computing arithmetic statistics ..." << std::endl;
    statistics->Update();
    std::cout << "Done computing arithmetic statistics." << std::endl;

    /** Only print if not histogram selected. */
    if ( select != "histogram" )
    {
      PrintStatistics<StatisticsFilterType>( statistics );
    }
    if ( select == "arithmetic" ) return;

    /** Save for later use for the histogram bin size. */
    maxPixelValue = statistics->GetMaximum();
    minPixelValue = statistics->GetMinimum();
  }

  /** Geometric mean/std: */
  if ( select == "geometric" || select == "" )
  {
    typename LogFilterType::Pointer logger = LogFilterType::New();
    logger->SetInput(inputImage);
    statistics->SetInput( logger->GetOutput() );
    std::cout << "Computing geometric statistics..." << std::endl;
    statistics->Update();
    std::cout << "Done computing geometric statistics." << std::endl;
    PrintGeometricStatistics<StatisticsFilterType>( statistics );

    if ( select == "geometric" ) return;
  }

  /** Prepare for the histogram. */
  if ( select == "histogram" || select == "" )
  {
    maskerOrCopier->SetInput( inputImage );
    std::string maskerOrCopierName = maskerOrCopier->GetNameOfClass();
    if ( maskerOrCopierName == "MaskImageFilter" )
    {
      std::cout << "Replacing all pixels outside the mask by -infinity, ";
      std::cout << "to make sure they are not included in the histogram..."
        << std::endl;
    }
    maskerOrCopier->Update();
    if ( maskerOrCopierName == "MaskImageFilter" )
    {
      std::cout << "Done replacing all pixels outside the mask by -infinity." << std::endl;
    }
  
    /** This code is copied from the ListSampleToHistogramGenerator->GenerateData()
     * and adapted.
     * It makes sure that the maximum values are also included in the histogram.
     */
    PixelType histogramMax;
    if ( !itk::NumericTraits< PixelType >::is_integer )
    {
      /** floating pixeltype */

      /** if the maximum (almost) equals the minimum, we have to make sure that 
       * everything still works. 
       * 4 conditions:
       * - The binsize must be greater than epsilon
       * - The uppermargin must be greater than epsilon
       * - the histogramMax must be at least statistics->GetMaximum() + uppermargin
       * - the histogramMax must be at least numberOfBins * binsize
       * epsilon is chosen a little larger than the computer indicates,
       * to be on the safe side. The factor of 100.0 is determined empirically
       * to still give good results.
       */
      double marginalScale = 100.0;
      double epsilon = itk::NumericTraits<PixelType>::epsilon() * 100.0;
      double binsize = static_cast<double>( maxPixelValue - minPixelValue )
        / static_cast<double>( numberOfBins );
      binsize = vnl_math_max( binsize, epsilon );
      double uppermargin = vnl_math_max( epsilon, binsize / marginalScale );
      histogramMax = static_cast<RealPixelType>(
        vnl_math_max( binsize * static_cast<double>( numberOfBins ) + minPixelValue,
        maxPixelValue + uppermargin ) );
    }
    else
    {
      /** Integer pixeltypes. in principle this function will never be called
       * with an integer pixeltype, but just in case this is changed in the
       * future ...
       */
      PixelType uppermargin = itk::NumericTraits<PixelType>::One;
      histogramMax = static_cast<RealPixelType>( maxPixelValue + uppermargin );
    }
    if ( histogramMax <= maxPixelValue )
    {
      /** Overflow occurred; maximum was already maximum of pixeltype;
       * We could solve this somehow (by adding a ClipBinsAtUpperBound(bool)
       * function to the itkScalarImageToHistogramGenerator2, and calling it
       * with argument 'false'), but the situation is quite unlikely; anyway,
       * mostly something is going wrong when a float image has value
       * infinity somewhere.
       */
      std::cerr << "Error during histogram computation!" << std::endl;
      std::cerr << "The maximum of the image is equal to the maximum of its pixeltype." << std::endl;
      std::cerr << "Histogram computation cannot be reliably performed now.";
      std::cerr << " pxstatisticsonimage cannot handle this situation." << std::endl;
      itkGenericExceptionMacro( << "Histogram cannot be computed." );
    }
          
    histogramGenerator->SetAutoMinMax(false);
    histogramGenerator->SetNumberOfBins(numberOfBins);
    histogramGenerator->SetHistogramMin( static_cast<RealPixelType>(minPixelValue) );
    histogramGenerator->SetHistogramMax( histogramMax );    
    histogramGenerator->SetInput( maskerOrCopier->GetOutput() );
    std::cout << "Computing histogram..." << std::endl;
    histogramGenerator->Compute();
    std::cout << "Done computing histogram." << std::endl;
    PrintHistogramStatistics<HistogramType>(
      histogramGenerator->GetOutput(), histogramOutputFileName );
  }

} // end ComputeStatistics()


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
  unsigned int numberOfBins,
  const std::string & select )
{
  /** Typedefs. */
  typedef ComponentType ScalarPixelType;
  typedef double InternalPixelType;
  typedef unsigned char MaskPixelType;
  typedef itk::Vector<ComponentType, NumberOfComponents>  VectorPixelType;
  typedef itk::Image<ScalarPixelType, Dimension>          ScalarImageType;
  typedef itk::Image<VectorPixelType, Dimension>          VectorImageType;
  
  typedef itk::Image<InternalPixelType, Dimension>    InternalImageType;
  typedef itk::Image<MaskPixelType, Dimension>        MaskImageType;
  typedef itk::ImageMaskSpatialObject< Dimension>     MaskSpatialObjectType;
  
  typedef itk::ImageToImageFilter<
    InternalImageType, InternalImageType>             BaseFilterType;
  typedef itk::ImageFileReader< ScalarImageType >     ScalarReaderType;
  typedef itk::ImageFileReader< VectorImageType >     VectorReaderType;
  typedef itk::ImageFileReader< MaskImageType >       MaskReaderType;
  typedef itk::CastImageFilter<
    ScalarImageType, InternalImageType >              CasterType;
  typedef itk::CastImageFilter<
    InternalImageType, InternalImageType>             CopierType;
  typedef itk::GradientToMagnitudeImageFilter<
    VectorImageType, InternalImageType >              MagnitudeFilterType;
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
  typename BaseFilterType::Pointer maskerOrCopier = (CopierType::New()).GetPointer();
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
    maskerOrCopier = maskFilter.GetPointer();
  }
  
  /** Create StatisticsFilter */
  typename StatisticsFilterType::Pointer statistics = 
    StatisticsFilterType::New();
  statistics->SetMask(mask);
  /** vnl_svd is used by this class, which is not thread safe */
  statistics->SetNumberOfThreads(1);

  typename HistogramGeneratorType::Pointer histogramGenerator = 
    HistogramGeneratorType::New();

  if ( NumberOfComponents == 1 )
  {
    typename ScalarReaderType::Pointer reader = ScalarReaderType::New();
    typename CasterType::Pointer caster = CasterType::New();

    std::cout << "InputImage is a scalar image." << std::endl;
    std::cout << "Statistics are computed on the gray values." << std::endl;

    reader->SetFileName( inputFileName.c_str() );
    std::cout << "Reading input image..." << std::endl;
    reader->Update();
    std::cout << "Done reading input image." << std::endl;
    
    caster->SetInput( reader->GetOutput() );
    std::cout << "Casting input image to float..." << std::endl;
    caster->Update();
    std::cout << "Done casting input image to float." << std::endl;

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
        histogramOutputFileName,
        select );
        
  } // end scalar images
  else
  {
    typename VectorReaderType::Pointer reader = VectorReaderType::New();
    
    std::cout << "InputImage is a vector image." << std::endl;
    std::cout << "Statistics are computed on the magnitude of the vectors." << std::endl;
    
    reader->SetFileName( inputFileName.c_str() );
    std::cout << "Reading input image..." << std::endl;
    reader->Update();
    std::cout << "Done reading input image." << std::endl;

    typename MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
    
    magnitudeFilter->SetInput( reader->GetOutput() );
    std::cout << "Computing magnitude image..." << std::endl;
    magnitudeFilter->Update();
    std::cout << "Done computing magnitude image." << std::endl;
    
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
        histogramOutputFileName,
        select );

  } // end vector images

} // end StatisticsOnImage


#endif // #ifndef __statisticsonimage_h

