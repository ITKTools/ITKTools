#ifndef __CalculateOverlap_CXX__
#define __CalculateOverlap_CXX__

#include "itkImageFileReader.h"
#include "itkImage.h"

#include "itkImageRegionConstIterator.h"
#include "itkListSample.h"
#include "itkListSampleToHistogramGenerator.h"
#include "itkHistogramToEntropyImageFilter.h"


//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	/** Declare stuff. */
	std::string imageFileName = "";
	std::string maskFileName = "";
	bool useInsideMask = true;
	unsigned int numberOfBins = 0;
	double sumOfSquaredVoxels = 0.0;
	unsigned int numberOfVoxels = 0;

	/** Get and check the arguments. */
	if ( argc == 3 )
	{
		imageFileName = argv[ 1 ];
		numberOfBins = atoi( argv[ 2 ] );
	}
	else if ( argc == 4 )
	{
		imageFileName = argv[ 1 ];
		maskFileName = argv[ 2 ];
		numberOfBins = atoi( argv[ 3 ] );
	}
	else if ( argc == 5 )
	{
		imageFileName = argv[ 1 ];
		maskFileName = argv[ 2 ];
		if ( atoi( argv[ 3 ] ) == 0 ) useInsideMask = false;
		else useInsideMask = true;
		numberOfBins = atoi( argv[ 4 ] );
	}
	else
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "pxcomputedistances image [mask] [useVoxelsInMask] numberOfBins" << std::endl;
		std::cout << "where useVoxelsInMask = 1, yields taking into account all voxels inside the mask," << std::endl;
		std::cout << "and where useVoxelsInMask = 0, yields taking into account all voxels outside the mask." << std::endl;
		std::cout << "Default is useVoxelsInMask = 1." << std::endl;
		return 1;
	}

	/** Image definitions and reader typedef. */
	const unsigned int	Dimension = 2;
	typedef short				PixelType;
	typedef itk::Image< PixelType, Dimension >					ImageType;
	typedef itk::ImageFileReader< ImageType >						ImageReaderType;

	/** Create readers. */
	ImageReaderType::Pointer imageReader = ImageReaderType::New();
	ImageReaderType::Pointer maskReader = 0;
	imageReader->SetFileName( imageFileName.c_str() );
	if ( argc > 3 )
	{
		maskReader = ImageReaderType::New();
		maskReader->SetFileName( maskFileName.c_str() );
	}

	/** Update reader pipeline. */
	try
	{
		imageReader->Update();
		if ( argc > 3 ) maskReader->Update();
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

	/** Some typedefs for */
	typedef itk::ImageRegionConstIterator< ImageType >	ConstIteratorType;
	typedef float					MeasurementValueType;
	const unsigned int MeasurementVectorLength = 1;
	typedef	itk::Vector< MeasurementValueType, MeasurementVectorLength >			MeasurementVectorType;
	typedef itk::Statistics::ListSample< MeasurementVectorType >			ListSampleType;

	/** Create iterators over image and mask. */
	ConstIteratorType it( imageReader->GetOutput(),
		imageReader->GetOutput()->GetLargestPossibleRegion() );
	it.GoToBegin();
	ConstIteratorType itMask;
	if ( argc > 3 )
	{
		itMask = ConstIteratorType( maskReader->GetOutput(),
			maskReader->GetOutput()->GetLargestPossibleRegion() );
		itMask.GoToBegin();
	}

	/** Create a list of samples. */
	ListSampleType::Pointer listOfSamples = ListSampleType::New();
	listOfSamples->SetMeasurementVectorSize( MeasurementVectorLength );
	MeasurementVectorType sample;

	/** Fill the list. */
	if ( argc == 3 )
	{
		while ( !it.IsAtEnd() )
		{
			/** Get the sample and put it in the list. */
			sample[ 0 ] = static_cast<MeasurementValueType>( it.Get() );
			listOfSamples->PushBack( sample );
			/** Update the sumOfSquaredVoxels. */
			sumOfSquaredVoxels += static_cast<double>( sample[ 0 ] * sample[ 0 ] );
			/** Increase iterators. */
			++it;
		}
	}
	else if ( argc > 3 && useInsideMask )
	{
		while ( !it.IsAtEnd() )
		{
			/** Get the sample and put it in the list. */
			if ( itMask.Value() )
			{
				sample[ 0 ] = static_cast<MeasurementValueType>( it.Get() );
				listOfSamples->PushBack( sample );
				/** Update the sumOfSquaredVoxels. */
				sumOfSquaredVoxels += static_cast<double>( sample[ 0 ] * sample[ 0 ] );
			}
			/** Increase iterators. */
			++it;
			++itMask;
		}
	}
	else if ( argc > 3 && !useInsideMask )
	{
		while ( !it.IsAtEnd() )
		{
			/** Get the sample and put it in the list. */
			if ( !itMask.Value() )
			{
				sample[ 0 ] = static_cast<MeasurementValueType>( it.Get() );
				listOfSamples->PushBack( sample );
				/** Update the sumOfSquaredVoxels. */
				sumOfSquaredVoxels += static_cast<double>( sample[ 0 ] * sample[ 0 ] );
			}
			/** Increase iterators. */
			++it;
			++itMask;
		}
	}

	/** Typedefs for the creation of a histogram and entropy calculator. */
	typedef itk::Statistics::ListSampleToHistogramGenerator<
		ListSampleType, MeasurementValueType >						HistogramGeneratorType;
	typedef HistogramGeneratorType::HistogramType				HistogramType;
	typedef HistogramType::SizeType											HistogramSizeType;
	typedef itk::HistogramToEntropyImageFilter<
		HistogramType >																		HistogramToEntropyType;
	typedef HistogramToEntropyType::OutputImageType			OutputImageType;

	/** Create numberOfBinS. */
	HistogramSizeType numberOfBinS;
	numberOfBinS.Fill( numberOfBins );

	/** Create histogram and entropy image. */
	HistogramGeneratorType::Pointer histogramGenerator
		= HistogramGeneratorType::New();
	HistogramToEntropyType::Pointer histogramToEntropy
		= HistogramToEntropyType::New();

	/** Setup and update the histogram generator. */
	histogramGenerator->SetNumberOfBins( numberOfBinS );
	histogramGenerator->SetMarginalScale( 10.0 );
	try
	{
		histogramGenerator->SetListSample( listOfSamples );
		histogramGenerator->Update();
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

	/** Connect histogram to entropy filter. */
	histogramToEntropy->SetInput( histogramGenerator->GetOutput() );

	/** Update pipeline. */
	try
	{
		histogramToEntropy->Update();
	}
	catch( itk::ExceptionObject &e )
	{
		std::cerr << "Caught ITK exception: " << e << std::endl;
		return 1;
	}

	/** Sum over the entropy image to get the total entropy. */
	typedef itk::ImageRegionConstIterator< OutputImageType >	ConstOutputIteratorType;
	ConstOutputIteratorType itE( histogramToEntropy->GetOutput(),
		histogramToEntropy->GetOutput()->GetLargestPossibleRegion() );
	itE.GoToBegin();

	double totalEntropy = 0.0;
	while ( !itE.IsAtEnd() )
	{
		totalEntropy += itE.Get();
		/** Update iterators. */
		++itE;
	} // end while

	/** Calculate stuff. */
	numberOfVoxels = listOfSamples->Size();
	double averageSD = 0.0;
	if ( numberOfVoxels != 0 )
	{
		averageSD = sumOfSquaredVoxels / numberOfVoxels;
	}

	/** Print stuff to screen. *
	std::cout << "The sum of squared voxels equals: " << sumOfSquaredVoxels << std::endl;
	std::cout << "This was calculated over " << numberOfVoxels
		<< " number of voxels, and therefore the average equals: ";
	if ( numberOfVoxels == 0 )
	{
		std::cout << "0" << std::endl;
	}
	else
	{
		std::cout << sumOfSquaredVoxels / numberOfVoxels << std::endl;
	}
	std::cout << "The entropy equals: " << totalEntropy << std::endl;

	/** Print stuff to screen. (SquaredDistance and Entropy). */
	std::cout << "NrOfVoxels:\t\t" << numberOfVoxels << std::endl;
	std::cout << "SquaredDistance:\t" << sumOfSquaredVoxels << std::endl;
	std::cout << "AverageSquaredDistance:\t" << averageSD << std::endl;
	std::cout << "Entropy:\t\t" << totalEntropy << std::endl;
	
	/** Return a value. */
	return 0;

} // end main


#endif // #ifndef __CalculateOverlap_CXX__
