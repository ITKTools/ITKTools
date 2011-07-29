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
#ifndef __BinaryImageOperatorHelper_h
#define __BinaryImageOperatorHelper_h

#include "itkImage.h"
#include "itkBinaryFunctors.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <vector>
#include <itksys/SystemTools.hxx>

//-------------------------------------------------------------------------------------


/** ITKToolsBinaryImageOperator */
class ITKToolsBinaryImageOperatorBase : public itktools::ITKToolsBase
{
public:
  ITKToolsBinaryImageOperatorBase()
  {
    this->m_InputFileName1 = "";
    this->m_InputFileName2 = "";
    this->m_OutputFileName = "";
    this->m_Ops = "";
    this->m_UseCompression = false;
    this->m_Arg = "";
  }
  ~ITKToolsBinaryImageOperatorBase(){};

  /** Input parameters */
  std::string m_InputFileName1;
  std::string m_InputFileName2;
  std::string m_OutputFileName;
  std::string m_Ops;
  bool m_UseCompression;
  std::string m_Arg;

  virtual void Run( void ) = 0;

}; // end ITKToolsBinaryImageOperatorBase


template< class TComponentType1, class TComponentType2, class TComponentTypeOut, unsigned int VDimension >
class ITKToolsBinaryImageOperator : public ITKToolsBinaryImageOperatorBase
{
public:
  typedef ITKToolsBinaryImageOperator Self;

  ITKToolsBinaryImageOperator(){};
  ~ITKToolsBinaryImageOperator(){};

  static Self * New( itktools::ComponentType componentType1,
    itktools::ComponentType componentType2,
    itktools::ComponentType componentTypeOut, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType1>( componentType1 ) &&
      itktools::IsType<TComponentType2>( componentType2 ) &&
      itktools::IsType<TComponentTypeOut>( componentTypeOut ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType1, VDimension>     InputImage1Type;
    typedef itk::Image<TComponentType2, VDimension>     InputImage2Type;
    typedef itk::Image<TComponentTypeOut, VDimension>   OutputImageType;

    typedef typename InputImage1Type::PixelType         InputPixel1Type;
    typedef typename InputImage2Type::PixelType         InputPixel2Type;
    typedef typename OutputImageType::PixelType         OutputPixelType;
    typedef itk::ImageToImageFilter<InputImage1Type, OutputImageType> BaseFilterType;
    typedef itk::ImageFileReader< InputImage1Type >     Reader1Type;
    typedef itk::ImageFileReader< InputImage2Type >     Reader2Type;
    typedef itk::ImageFileWriter< OutputImageType >     WriterType;

    /** Typedef's for the binary functor filters. */
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::ADDITION<InputPixel1Type, InputPixel2Type, OutputPixelType> > ADDITIONFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::MINUS<InputPixel1Type, InputPixel2Type, OutputPixelType> > MINUSFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::TIMES<InputPixel1Type, InputPixel2Type, OutputPixelType> > TIMESFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::DIVIDE<InputPixel1Type, InputPixel2Type, OutputPixelType> > DIVIDEFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::POWER<InputPixel1Type, InputPixel2Type, OutputPixelType> > POWERFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::MAXIMUM<InputPixel1Type, InputPixel2Type, OutputPixelType> > MAXIMUMFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::MINIMUM<InputPixel1Type, InputPixel2Type, OutputPixelType> > MINIMUMFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::ABSOLUTEDIFFERENCE<InputPixel1Type, InputPixel2Type, OutputPixelType> > ABSOLUTEDIFFERENCEFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::SQUAREDDIFFERENCE<InputPixel1Type, InputPixel2Type, OutputPixelType> > SQUAREDDIFFERENCEFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::BINARYMAGNITUDE<InputPixel1Type, InputPixel2Type, OutputPixelType> > BINARYMAGNITUDEFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::LOG<InputPixel1Type, InputPixel2Type, OutputPixelType> > LOGFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::WEIGHTEDADDITION<InputPixel1Type, InputPixel2Type, OutputPixelType> > WEIGHTEDADDITIONFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::MASK<InputPixel1Type, InputPixel2Type, OutputPixelType> > MASKFilterType;
    typedef itk::BinaryFunctorImageFilter<
      InputImage1Type, InputImage2Type, OutputImageType,
      itk::Functor::MASKNEGATED<InputPixel1Type, InputPixel2Type, OutputPixelType> > MASKNEGATEDFilterType;

    /** Read the input images. */
    typename Reader1Type::Pointer reader1 = Reader1Type::New();
    reader1->SetFileName( this->m_InputFileName1.c_str() );
    typename Reader2Type::Pointer reader2 = Reader2Type::New();
    reader2->SetFileName( this->m_InputFileName2.c_str() );

    /** Get the argument. */
    double argument = atof( this->m_Arg.c_str() );

    /** Get the binaryOperatorName. */
    std::string binaryOperatorName = this->m_Ops;

    /** Set up the binaryFilter. */
    typename BaseFilterType::Pointer binaryFilter = 0;
    if ( binaryOperatorName == "ADDITION" )
    {
      typename ADDITIONFilterType::Pointer tempBinaryFilter = ADDITIONFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "MINUS" )
    {
      typename MINUSFilterType::Pointer tempBinaryFilter = MINUSFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "TIMES" )
    {
      typename TIMESFilterType::Pointer tempBinaryFilter = TIMESFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "DIVIDE" )
    {
      typename DIVIDEFilterType::Pointer tempBinaryFilter = DIVIDEFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "POWER" )
    {
      typename POWERFilterType::Pointer tempBinaryFilter = POWERFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "MAXIMUM" )
    {
      typename MAXIMUMFilterType::Pointer tempBinaryFilter = MAXIMUMFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "MINIMUM" )
    {
      typename MINIMUMFilterType::Pointer tempBinaryFilter = MINIMUMFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "ABSOLUTEDIFFERENCE" )
    {
      typename ABSOLUTEDIFFERENCEFilterType::Pointer tempBinaryFilter = ABSOLUTEDIFFERENCEFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "SQUAREDDIFFERENCE" )
    {
      typename SQUAREDDIFFERENCEFilterType::Pointer tempBinaryFilter = SQUAREDDIFFERENCEFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "BINARYMAGNITUDE" )
    {
      typename BINARYMAGNITUDEFilterType::Pointer tempBinaryFilter = BINARYMAGNITUDEFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "LOG" )
    {
      typename LOGFilterType::Pointer tempBinaryFilter = LOGFilterType::New();
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "WEIGHTEDADDITION" )
    {
      typename WEIGHTEDADDITIONFilterType::Pointer tempBinaryFilter = WEIGHTEDADDITIONFilterType::New();
      tempBinaryFilter->GetFunctor().SetArgument( argument );
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "MASK" )
    {
      typename MASKFilterType::Pointer tempBinaryFilter = MASKFilterType::New();
      tempBinaryFilter->GetFunctor().SetArgument( argument );
      binaryFilter = tempBinaryFilter.GetPointer();
    }
    else if ( binaryOperatorName == "MASKNEGATED" )
    {
      typename MASKNEGATEDFilterType::Pointer tempBinaryFilter = MASKNEGATEDFilterType::New();
      tempBinaryFilter->GetFunctor().SetArgument( argument );
      binaryFilter = tempBinaryFilter.GetPointer();
    }

    /** Connect the pipeline. */
    binaryFilter->SetInput( 0, reader1->GetOutput() );
    binaryFilter->SetInput( 1, reader2->GetOutput() );

    /** Write the image to disk */
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( binaryFilter->GetOutput() );
    writer->SetUseCompression( this->m_UseCompression );
    writer->Update();
  }

}; // end ITKToolsBinaryImageOperator


#endif //#ifndef __BinaryImageOperatorHelper_h
