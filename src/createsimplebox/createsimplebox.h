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

typedef std::map<std::string, std::string> ArgMapType;

std::string GetHelpString(void)
{
  std::stringstream ss;
  ss << "This program creates an image containing a white box, defined by point A and B." << std::endl
    << "Usage:" << std::endl
    << "pxcreatesimplebox" << std::endl
    << "[-in]  InputImageFileName" << std::endl
    << "Size, origin, and spacing for the output image will be taken" << std::endl
    << "from this image. NB: not the dimension and the pixeltype;" << std::endl
    << "you must set them anyway!" << std::endl
    << "-out   OutputImageFileName" << std::endl
    << "-pt    PixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>" << std::endl
    << "Currently only char, uchar and short are supported." << std::endl
    << "-id    ImageDimension <2,3>" << std::endl
    << "[-d0]  Size of dimension 0" << std::endl
    << "[-d1]  Size of dimension 1" << std::endl
    << "[-d2]  Size of dimension 2" << std::endl
    << "-pA0  Index 0 of pointA" << std::endl
    << "-pA1  Index 1 of pointA" << std::endl
    << "[-pA2]Index 2 of pointA" << std::endl
    << "-pB0  Index 0 of pointB" << std::endl
    << "-pB1  Index 1 of pointB" << std::endl
    << "[-pB2]Index 2 of pointB";
  return ss.str();
} // end GetHelpString



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
