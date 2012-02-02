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
#ifndef __meanstdimage_hxx
#define __meanstdimage_hxx

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

template< class TComponentType, unsigned int VDimension >
void
ITKToolsMeanStdImage< TComponentType, VDimension >
::MeanStdImage(
  const std::vector<std::string> & inputFileNames,
  const bool calc_mean,
  const std::string & outputFileNameMean,
  const bool calc_std,
  const std::string & outputFileNameStd )
{
  /** TYPEDEF's. */
  typedef typename InputImageType::Pointer		ImagePointer;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileWriter< InputImageType >        WriterType;
  typedef typename InputImageType::PixelType            PixelType;
  typedef typename ReaderType::Pointer                  ReaderPointer;
  typedef typename WriterType::Pointer                  WriterPointer;
    
  /** DECLARATION'S. */
  unsigned int nrInputs = inputFileNames.size();
  ReaderPointer inReader;
  WriterPointer writer_mean = WriterType::New();
  WriterPointer writer_std = WriterType::New();

  ImagePointer mean = InputImageType::New();
  ImagePointer sq_mean = InputImageType::New();
  ImagePointer std = InputImageType::New();

  itk::ImageRegionConstIterator<InputImageType> input_iterator;
  itk::ImageRegionIterator<InputImageType> mean_iterator;
  itk::ImageRegionIterator<InputImageType> sq_mean_iterator;
  itk::ImageRegionIterator<InputImageType> std_iterator;

  /** Create temporary & output images */
  inReader = ReaderType::New();
  inReader->SetFileName( inputFileNames[0].c_str() );
  inReader->Update();

  mean->CopyInformation( inReader->GetOutput() );
  sq_mean->CopyInformation( inReader->GetOutput() );
  std->CopyInformation( inReader->GetOutput() );

  mean->SetRegions( inReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  sq_mean->SetRegions( inReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  std->SetRegions( inReader->GetOutput()->GetLargestPossibleRegion().GetSize() );

  mean->Allocate();
  sq_mean->Allocate();
  std->Allocate();

  mean->FillBuffer( 0.0 );
  sq_mean->FillBuffer( 0.0 );
  std->FillBuffer( 0.0 );

  mean_iterator = itk::ImageRegionIterator<InputImageType>( mean, mean->GetRequestedRegion() );
  sq_mean_iterator = itk::ImageRegionIterator<InputImageType>( sq_mean, sq_mean->GetRequestedRegion() );
  std_iterator = itk::ImageRegionIterator<InputImageType>( std, std->GetRequestedRegion() );


  /** Loop over all images and create sum(X) and sum(X^2) which is required for E(X) and E(X^2) */
  for ( unsigned int i = 0; i < nrInputs; ++i )
  {
    inReader = ReaderType::New();
    inReader->SetFileName( inputFileNames[i].c_str() );
    inReader->Update();

    input_iterator = itk::ImageRegionConstIterator<InputImageType>(inReader->GetOutput(), inReader->GetOutput()->GetRequestedRegion());

    input_iterator.GoToBegin();
    mean_iterator.GoToBegin();
    sq_mean_iterator.GoToBegin();
    for (; !mean_iterator.IsAtEnd(); ++mean_iterator, ++sq_mean_iterator,  ++input_iterator )
    {
      mean_iterator.Set( mean_iterator.Get() + input_iterator.Get() );
      if (calc_std)
        sq_mean_iterator.Set( sq_mean_iterator.Get() + (input_iterator.Get() * input_iterator.Get()) );
    }
  }

  /** Divide images by N to get E(X) and E(X^2)*/
  mean_iterator.GoToBegin();
  sq_mean_iterator.GoToBegin();
  float denominator( 1.0f / nrInputs );
  for (; !mean_iterator.IsAtEnd(); ++mean_iterator, ++sq_mean_iterator )
  {
    mean_iterator.Set( mean_iterator.Get() * denominator );
    if (calc_std)
      sq_mean_iterator.Set( sq_mean_iterator.Get() * denominator );

	bool nothing = false;
  }

  /** Calculate standard deviation: sqrt( E(X^2) - (E(X))^2 ) */
  if (calc_std)
  {
    mean_iterator.GoToBegin();
    sq_mean_iterator.GoToBegin();
    std_iterator.GoToBegin();

    for (; !mean_iterator.IsAtEnd(); ++mean_iterator, ++sq_mean_iterator, ++std_iterator )
    {
		std_iterator.Set( std::sqrt( (float) std::abs( sq_mean_iterator.Get() - (mean_iterator.Get() * mean_iterator.Get() ) ) ) );
    }
  }
  
  /** Write the output images */
  if (calc_mean)
  {
    writer_mean->SetFileName( outputFileNameMean.c_str() );
    writer_mean->SetInput( mean );
    writer_mean->Update();
  }

  if (calc_std) 
  {
    writer_std->SetFileName( outputFileNameStd.c_str() );
    writer_std->SetInput( std );
    writer_std->Update();
  }
} // end MeanStdImage()

#endif // end #ifndef __meanstdimage_hxx
