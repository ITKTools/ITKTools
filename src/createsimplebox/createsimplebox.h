/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef __createsimplebox_h_
#define __createsimplebox_h_

#include "ITKToolsBase.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleBoxSpatialFunction.h"
#include "itkSpatialFunctionImageEvaluatorFilter.h"


/** \class ITKToolsCreateSimpleBoxBase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsCreateSimpleBoxBase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsCreateSimpleBoxBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
  };
  /** Destructor. */
  ~ITKToolsCreateSimpleBoxBase(){};

  /** Input member parameters. */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_BoxSize;
  std::vector<unsigned int> m_IndexA;
  std::vector<unsigned int> m_IndexB;

}; // end class ITKToolsCreateSimpleBoxBase


/** \class ITKToolsCreateSimpleBox
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsCreateSimpleBox : public ITKToolsCreateSimpleBoxBase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsCreateSimpleBox Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsCreateSimpleBox(){};
  ~ITKToolsCreateSimpleBox(){};

  /** Run function. */
  void Run( void )
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
    if( inputImageFileName == "" )
    {
      /** read the dimension from the commandline.*/
      for( unsigned int i = 0; i < VDimension ; i++ )
      {
        sizes[ i ] = this->m_BoxSize[ i ];
      }

      /** make some assumptions */
      origin.Fill(0.0);
      spacing.Fill(1.0);
    }
    else
    {
      /** Take dimension, origin and spacing from the inputfile.*/
      ReaderPointer reader = ReaderType::New();
      reader->SetFileName( this->m_InputFileName.c_str() );
      reader->Update();

      ImagePointer inputImage = reader->GetOutput();
      sizes = inputImage->GetLargestPossibleRegion().GetSize();
      origin = inputImage->GetOrigin();
      spacing = inputImage->GetSpacing();
    }

    /** Setup pipeline and configure its components */
    tempImage->SetRegions( sizes );
    tempImage->SetOrigin( origin );
    tempImage->SetSpacing( spacing );

    // Convert the indices to the necessary ITK type
    IndexType indexA;
    IndexType indexB;

    for( unsigned int i = 0; i < VDimension; ++i )
    {
      indexA[ i ] = this->m_IndexA[ i ];
      indexB[ i ] = this->m_IndexB[ i ];
    }
    tempImage->TransformIndexToPhysicalPoint( indexA, pointA );
    tempImage->TransformIndexToPhysicalPoint( indexB, pointB );

    /** Enlarge the box a little, to make sure that pointA and B
     * fall within the box. */
    const double small_factor = 0.1;
    const double small_number = 1e-14;
    for( unsigned int i = 0; i < VDimension ; i++ )
    {
      const double distance = pointB[ i ] - pointA[ i ];
      double sign = 1.0;
      if( vcl_abs(distance) > small_number )
      {
        sign = distance / vcl_abs( distance );
      }
      pointA[ i ] -= small_factor * sign * spacing[ i ];
      pointB[ i ] += small_factor * sign * spacing[ i ];
    }

    boxfunc->SetPointA(pointA);
    boxfunc->SetPointB(pointB);

    boxGenerator->SetFunction(boxfunc);
    boxGenerator->SetInput(tempImage);

    writer->SetInput( boxGenerator->GetOutput() );
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->Update();

  } // end Run()

}; // end class ITKToolsCreateSimpleBox


#endif // #ifndef __createsimplebox_h_
