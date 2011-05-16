#ifndef __createbox_h
#define __createbox_h

#include <iostream>
#include <string>
#include <map>
//#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleBoxSpatialFunction.h"
#include "itkSpatialFunctionImageEvaluatorFilter.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsBase.h"

/** CreateSimpleBox */

class CreateSimpleBoxBase : public itktools::ITKToolsBase
{ 
public:
  CreateSimpleBoxBase(){};
  ~CreateSimpleBoxBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_BoxSize;
  std::vector<unsigned int> m_IndexA;
  std::vector<unsigned int> m_IndexB;
    
}; // end CreateSimpleBoxBase


template< class TComponentType, unsigned int VDimension >
class CreateSimpleBox : public CreateSimpleBoxBase
{
public:
  typedef CreateSimpleBox Self;

  CreateSimpleBox(){};
  ~CreateSimpleBox(){};

  static Self * New( itktools::EnumComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    typedef TComponentType                        PixelType;
    typedef itk::Image<PixelType, VDimension>     ImageType;
    typedef typename ImageType::Pointer           ImagePointer;
    typedef typename ImageType::IndexType         IndexType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::PointType         OriginType;
    typedef typename ImageType::SpacingType       SpacingType;
    typedef typename ImageType::PointType         PointType;
    typedef itk::ImageFileReader<ImageType>       ReaderType;
    typedef itk::ImageFileWriter<ImageType>       WriterType;
    typedef typename ReaderType::Pointer          ReaderPointer;
    typedef typename WriterType::Pointer          WriterPointer;
    typedef itk::SimpleBoxSpatialFunction<
      VDimension, PointType>                  BoxFunctionType;
    typedef typename BoxFunctionType::Pointer     BoxFunctionPointer;
    typedef itk::SpatialFunctionImageEvaluatorFilter<
      BoxFunctionType, ImageType, ImageType>      FunctionEvaluatorType;
    typedef typename FunctionEvaluatorType::Pointer FunctionEvaluatorPointer;


    /** vars */
    std::string inputImageFileName("");
    std::string outputImageFileName(inputImageFileName + "Output.mhd");
    SizeType sizes;
    OriginType origin;
    SpacingType spacing;
    WriterPointer writer = WriterType::New();
    ImagePointer tempImage = ImageType::New();
    BoxFunctionPointer boxfunc = BoxFunctionType::New();
    PointType pointA;
    PointType pointB;
    FunctionEvaluatorPointer boxGenerator = FunctionEvaluatorType::New();

    /** Determine size, origin and spacing */
    if (inputImageFileName == "")
    {
      /** read the dimension from the commandline.*/
      for (unsigned int i=0; i< VDimension ; i++)
      {
        sizes[i] = m_BoxSize[i];
      }

      /** make some assumptions */
      origin.Fill(0.0);
      spacing.Fill(1.0);
    }
    else
    {
      /** Take dimension, origin and spacing from the inputfile.*/
      ReaderPointer reader = ReaderType::New();
      reader->SetFileName( m_InputFileName.c_str() );
      try
      {
        reader->Update();
      }
      catch (itk::ExceptionObject & err)
      {
        std::cerr << "Error while reading input image." << std::endl;
        std::cerr << err << std::endl;
      }
      ImagePointer inputImage = reader->GetOutput();
      sizes = inputImage->GetLargestPossibleRegion().GetSize();
      origin = inputImage->GetOrigin();
      spacing = inputImage->GetSpacing();
    }

    /** Setup pipeline and configure its components */

    tempImage->SetRegions(sizes);
    tempImage->SetOrigin(origin);
    tempImage->SetSpacing(spacing);
    
    // Convert the indices to the necessary ITK type
    IndexType indexA;
    IndexType indexB;
    
    for(unsigned int i = 0; i < VDimension; ++i)
    {
      indexA[i] = m_IndexA[i];
      indexB[i] = m_IndexB[i];
    }
    tempImage->TransformIndexToPhysicalPoint(indexA, pointA);
    tempImage->TransformIndexToPhysicalPoint(indexB, pointB);

    /** Enlarge the box a little, to make sure that pointA and B
     * fall within the box. */
    const double small_factor = 0.1;
    const double small_number = 1e-14;
    for (unsigned int i=0; i< VDimension ; i++)
    {
      const double distance = pointB[i]-pointA[i];
      double sign = 1.0;
      if ( vcl_abs(distance) > small_number )
      {
        sign = distance / vcl_abs( distance );
      }
      pointA[i] -= small_factor * sign * spacing[i];
      pointB[i] += small_factor * sign * spacing[i];
    }

    boxfunc->SetPointA(pointA);
    boxfunc->SetPointB(pointB);

    boxGenerator->SetFunction(boxfunc);
    boxGenerator->SetInput(tempImage);

    writer->SetInput( boxGenerator->GetOutput() );
    writer->SetFileName( m_OutputFileName.c_str() );

    std::cout
      << "Saving image to disk as \""
      << outputImageFileName
      << "\""
      << std::endl;
    try
    {
      writer->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << err << std::endl;
      return;
    }

  }

}; // end CreateSimpleBox


#endif // #ifndef __createbox_h
