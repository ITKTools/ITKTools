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
#ifndef __meanstdimage_hxx_
#define __meanstdimage_hxx_

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

template< unsigned int VDimension, class TComponentType >
void
ITKToolsMeanStdImage< VDimension, TComponentType >
::MeanStdImage(
  const std::vector<std::string> & inputFileNames,
  const std::vector<std::string> & inputMaskFileNames,
  const bool calc_mean,
  const std::string & outputFileNameMean,
  const bool calc_std,
  const std::string & outputFileNameStd,
  const bool population_std,
  const bool use_compression)
{
  /** TYPEDEF's. */
  typedef typename InputImageType::Pointer              ImagePointer;
  typedef typename OutputImageType::Pointer             OutImagePointer;
  typedef itk::ImageFileReader< InputImageType >        ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >       WriterType;
  typedef typename InputImageType::PixelType            PixelType;
  typedef typename ReaderType::Pointer                  ReaderPointer;
  typedef typename WriterType::Pointer                  WriterPointer;

  /** DECLARATION'S. */
  const unsigned int nrInputs = inputFileNames.size();
  const unsigned int nrMasks = inputMaskFileNames.size();
  
  ReaderPointer inReader;
  ReaderPointer inMaskReader;
  WriterPointer writer_mean = WriterType::New();
  WriterPointer writer_std = WriterType::New();
  
  OutImagePointer mean = OutputImageType::New();
  OutImagePointer sq_mean = OutputImageType::New();
  OutImagePointer std = OutputImageType::New();
  OutImagePointer nr_images = OutputImageType::New();
  
  itk::ImageRegionConstIterator<InputImageType> input_iterator;
  itk::ImageRegionConstIterator<InputImageType> mask_iterator;
  itk::ImageRegionIterator<OutputImageType> mean_iterator;
  itk::ImageRegionIterator<OutputImageType> sq_mean_iterator;
  itk::ImageRegionIterator<OutputImageType> std_iterator;
  itk::ImageRegionIterator<OutputImageType> nr_images_iterator;

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
  
  mean_iterator = itk::ImageRegionIterator<OutputImageType>( mean, mean->GetRequestedRegion() );
  sq_mean_iterator = itk::ImageRegionIterator<OutputImageType>( sq_mean, sq_mean->GetRequestedRegion() );
  std_iterator = itk::ImageRegionIterator<OutputImageType>( std, std->GetRequestedRegion() );
  
  /** Checking if there are masks and initialising the iterator*/
  if (nrMasks != 0 )
  {
    inMaskReader = ReaderType::New();
    inMaskReader->SetFileName( inputMaskFileNames[0].c_str() );
    inMaskReader->Update();
	
    nr_images->CopyInformation( inReader->GetOutput() );
    nr_images->SetRegions( inReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
    nr_images->Allocate();
    nr_images->FillBuffer(0);
	nr_images_iterator = itk::ImageRegionIterator<OutputImageType>( nr_images, nr_images->GetRequestedRegion() );
  }
  /** Loop over all images and create sum(X) and sum(X^2) which is required for E(X) and E(X^2) */
  for( unsigned int i = 0; i < nrInputs; ++i )
  {
    std::cout << "Reading image " << inputFileNames[ i ].c_str() << std::endl;
    inReader = ReaderType::New();
    inReader->SetFileName( inputFileNames[ i ].c_str() );
    inReader->Update();
	
    input_iterator = itk::ImageRegionConstIterator<InputImageType>(
      inReader->GetOutput(), inReader->GetOutput()->GetRequestedRegion() );	
	  
    input_iterator.GoToBegin();
    mean_iterator.GoToBegin();
    sq_mean_iterator.GoToBegin();
	
	/** Reading the masks if needed*/
	if (nrMasks != 0 )
	{
	  std::cout << "Reading mask " << inputMaskFileNames[ i ].c_str() << std::endl;
	  inMaskReader = ReaderType::New();
      inMaskReader->SetFileName( inputMaskFileNames[ i ].c_str() );
      inMaskReader->Update();
	  

	  mask_iterator = itk::ImageRegionConstIterator<InputImageType>(
        inMaskReader->GetOutput(), inMaskReader->GetOutput()->GetRequestedRegion() );

	  mask_iterator.GoToBegin();
	  nr_images_iterator.GoToBegin();
	}
	/** Create two maps for calculating the mean and std: sum(X) and sum(X^2) */
    for (; !mean_iterator.IsAtEnd(); ++mean_iterator, ++sq_mean_iterator, ++input_iterator)
    {	
      /** Calculating the mean if there are no masks or if the mask is not zero in current voxel*/  	
	  if (nrMasks == 0 || mask_iterator.Get() !=0)
	  {
	    mean_iterator.Set( mean_iterator.Get() + input_iterator.Get() );
	    if( calc_std )
  	      sq_mean_iterator.Set( sq_mean_iterator.Get() + (input_iterator.Get() * input_iterator.Get()) );

	  }
	  if (nrMasks != 0)
	  {
	    /** If masks are used, the number of the images used for the mean for each voxel is updated*/
	    if (mask_iterator.Get() != 0 )
		{
		  nr_images_iterator.Set( nr_images_iterator.Get() + 1);
		}  
		/** Update iterators if we are in fact using masks*/
		++mask_iterator;
		++nr_images_iterator;
	  }
	}
  }

  /** Calculate mean and standard deviation using:
      mean = ( SUM(X) / N )
      std  = sqrt( E(X^2) - (E(X))^2 ) for population standard deviation
      std  = sqrt(N / (N-1)) * sqrt( E(X^2) - (E(X))^2 ) for sample standard deviation
  */
  mean_iterator.GoToBegin();
  sq_mean_iterator.GoToBegin();
  std_iterator.GoToBegin();
  nr_images_iterator.GoToBegin();
  
  /** Denominator for the 1/N calculations and sample_std_factor N/(N-1) to get sample std from population std */
  float denominator( 1.0f / nrInputs );
  float sample_std_factor = std::sqrt(((float) nrInputs) / ((float) nrInputs - 1));

  for (; !mean_iterator.IsAtEnd(); ++mean_iterator, ++sq_mean_iterator, ++std_iterator)
  {
    if (nrMasks != 0)
	{
	  if (nr_images_iterator.Get() > 1)
	  {
	    denominator = 1 / nr_images_iterator.Get();
	    sample_std_factor = std::sqrt(((float) nr_images_iterator.Get()) / ((float) nr_images_iterator.Get() - 1));
	  }
	  else
	  {
	    denominator = nr_images_iterator.Get();
	    sample_std_factor = 0;
	  }
	  ++nr_images_iterator;
	}
	
	/** Calculate mean and mean of squares */
	mean_iterator.Set( denominator * mean_iterator.Get() );
	sq_mean_iterator.Set( denominator * sq_mean_iterator.Get() );
	
	/** Calculate standard deviation */
	
	if (calc_std)
	{
	  if (population_std) // Calculate either sample or population standard deviation
	  { 
	    std_iterator.Set( std::sqrt(
	      (float) std::abs( sq_mean_iterator.Get() - (mean_iterator.Get() * mean_iterator.Get() ) ) ) );
	  } 
	  else
	  {
	    std_iterator.Set( sample_std_factor * std::sqrt(
	      (float) std::abs( sq_mean_iterator.Get() - (mean_iterator.Get() * mean_iterator.Get() ) ) ) );
	  }
	}
  }
  
  /** Write the output images */
  if( calc_mean )
  {
    writer_mean->SetFileName( outputFileNameMean.c_str() );
    writer_mean->SetInput( mean );
	writer_mean->SetUseCompression( use_compression );
    writer_mean->Update();
  }

  if( calc_std )
  {
    writer_std->SetFileName( outputFileNameStd.c_str() );
    writer_std->SetInput( std );
	writer_std->SetUseCompression( use_compression );
    writer_std->Update();
  }

} // end MeanStdImage()

#endif // end #ifndef __meanstdimage_hxx_
