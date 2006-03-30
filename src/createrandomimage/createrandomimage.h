#ifndef __createrandomimage_h
#define __createrandomimage_h


#include "itkImageFileWriter.h"
#include "itkArray.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageRandomIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
//#include "itkDiscreteGaussianImageFilter.h"
//#include "itkRecursiveGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkCompose3DVectorImageFilter.h"
#include "itkExceptionObject.h"
#include "itkNumericTraits.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include <iostream>
#include <string>
#include <math.h>
#include <map>

//strange hack:
#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#  define CRI_STATIC_ENUM static enum
#else 
#  define CRI_STATIC_ENUM enum
#endif

CRI_STATIC_ENUM enum_type { eFLOAT, eINT, eUINT, eSHORT, eUSHORT, eCHAR, eUCHAR, eUNKNOWN };




template < unsigned int NImageDimension,
	unsigned int NSpaceDimension,
	class TValue >
class runwrap
{
	public:

	static int run_cri(const char * outputImageFileName,
									const itk::Array<unsigned int> & sizes,
									double min_value, double max_value,
									unsigned long resolution, double sigma,
									int rand_seed )
{
	/** Typedefs */
	
	/** Dimensions */
	const unsigned int ImageDimension = NImageDimension;
	const unsigned int SpaceDimension = NSpaceDimension;
		
	/** PixelTypes */
	typedef TValue 		ValueType;
	typedef ValueType ScalarPixelType;
	typedef float InternalValueType;
	typedef itk::Vector<ValueType, SpaceDimension> VectorPixelType;
	
	/** ImageTypes */
	typedef itk::Image<ValueType, ImageDimension> ImageType;
	typedef itk::Image<InternalValueType, ImageDimension> InternalImageType;
	typedef itk::Image<ScalarPixelType, ImageDimension> ScalarOutputImageType;
	typedef itk::Image<VectorPixelType, ImageDimension>	VectorOutputImageType;
	typedef typename ImageType::Pointer ImagePointer;
	typedef typename InternalImageType::Pointer InternalImagePointer;
	
	typedef typename ImageType::SizeType SizeType;
	typedef typename ImageType::IndexType IndexType;
  typedef typename ImageType::PointType OriginType;
	typedef typename ImageType::RegionType RegionType;
	
	typedef typename InternalImageType::SizeType InternalSizeType;
	typedef typename InternalImageType::IndexType InternalIndexType;
	typedef typename InternalImageType::PointType InternalOriginType;
	typedef typename InternalImageType::RegionType InternalRegionType;
	
	typedef itk::FixedArray<InternalImagePointer, SpaceDimension> SetOfChannelsType;
		
	/** Iterator */
	typedef itk::ImageRandomIteratorWithIndex<InternalImageType>	RandomIteratorType;
	typedef itk::ImageRegionIterator<InternalImageType>	RegionIteratorType;
	
	/** Blurring filter */
  //typedef itk::DiscreteGaussianImageFilter<
  // InternalImageType, InternalImageType>							BlurFilterType;
  typedef itk::SmoothingRecursiveGaussianImageFilter<
    InternalImageType, InternalImageType>								BlurFilterType;
  typedef typename BlurFilterType::Pointer						BlurFilterPointer;
  typedef itk::FixedArray<BlurFilterPointer, SpaceDimension> SetOfBlurrersType;	
    	
	typedef itk::CastImageFilter<InternalImageType, ImageType> CastFilterType;
	typedef typename CastFilterType::Pointer CastFilterPointer;
	typedef itk::FixedArray<CastFilterPointer, SpaceDimension> SetOfCastersType;

	typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractFilterType;
	typedef typename ExtractFilterType::Pointer ExtractFilterPointer;
	typedef itk::FixedArray<ExtractFilterPointer, SpaceDimension> SetOfExtractersType;
	

	/** For different space dimensions different types: */
	/** Combine channels into one vector image */
	typedef itk::Compose2DVectorImageFilter<ImageType>	Composer2DType;
	typedef itk::Compose3DVectorImageFilter<ImageType>	Composer3DType;
	typedef typename Composer2DType::Pointer Composer2DPointer;
	typedef typename Composer3DType::Pointer Composer3DPointer;
	
	/** ImageWriters */
	typedef itk::ImageFileWriter<ScalarOutputImageType>	ScalarWriterType;
	typedef itk::ImageFileWriter<VectorOutputImageType>	VectorWriterType;
	typedef typename ScalarWriterType::Pointer ScalarWriterPointer;
	typedef typename VectorWriterType::Pointer VectorWriterPointer;

	/** RandomGenerator */
	typedef itk::Statistics::MersenneTwisterRandomVariateGenerator RandomGeneratorType;
	typedef RandomGeneratorType::Pointer RandomGeneratorPointer;
	
	
	/** Create variables */
	 
	ScalarWriterPointer scalarWriter = 0;
	VectorWriterPointer vectorWriter = 0;
	Composer2DPointer composer2D = 0;
	Composer3DPointer composer3D = 0;
	SetOfChannelsType setOfChannels;
	SetOfBlurrersType setOfBlurrers;
	SetOfCastersType setOfCasters;
	SetOfExtractersType setOfExtracters;
	RandomGeneratorPointer randomGenerator = RandomGeneratorType::New();
	bool randomiterating = true;
  

	/** Set the random seed */
	randomGenerator->SetSeed(rand_seed);


  	
	/** Convert the itkArray to an itkSizeType and calculate nrOfPixels */
	InternalSizeType internalimagesize;
	InternalIndexType internalimageindex;
  InternalOriginType internalimageorigin;
	InternalRegionType internalimageregion;
	SizeType imagesize;
	IndexType imageindex;
  OriginType imageorigin;
	RegionType imageregion;
	unsigned long nrOfPixels = 1;
	for (unsigned int i = 0; i< ImageDimension; i++)
	{
		internalimagesize.SetElement(i, sizes[i]); //will be changed later
		imagesize.SetElement(i, sizes[i]);
		imageindex.SetElement(i, 0);
    imageorigin.SetElement(i,0.0);
		nrOfPixels *= sizes[i];
	}
	
	/** Compute the standard deviation of the Gaussian used for blurring
	 * the random images */
	if ( sigma < 0 )
	{
		sigma = static_cast<double>(
			static_cast<double>(nrOfPixels) /	
			static_cast<double>(resolution) / 
			pow( 2.0, static_cast<double>(ImageDimension) )
			);
	}

  int paddingSize = static_cast<int>(2.0*sigma);
  int maximumKernelWidth = 64;

	for (unsigned int i = 0; i< ImageDimension; i++)
	{
		internalimagesize[i] += static_cast<unsigned long>( 2*paddingSize );
    internalimageindex[i] = static_cast<long>(-paddingSize);
		internalimageorigin[i] =0;// static_cast<double> ( -paddingSize );		
	}
	
	internalimageregion.SetSize(internalimagesize);
  internalimageregion.SetIndex(internalimageindex);

	imageregion.SetSize(imagesize);
	imageregion.SetIndex(imageindex);

	/** Check whether a random iterator should be used or a region */
	if ( resolution ==0 )
	{
		randomiterating = false;
	}
  
	/** Create the images */
	for (unsigned int i = 0; i< SpaceDimension; i++)
	{
		
		setOfChannels[i] = InternalImageType::New();
		setOfChannels[i]->SetRegions( internalimageregion );
    setOfChannels[i]->SetOrigin( internalimageorigin );
		setOfChannels[i]->SetRequestedRegion( imageregion );
		setOfChannels[i]->Allocate();
		setOfChannels[i]->FillBuffer( itk::NumericTraits<InternalValueType>::Zero );
		
		
		/** Setting random values to random points */

		double range = max_value-min_value;
		if (randomiterating)
		{
			std::cout
				<< "Channel"
				<< i
				<< ": Setting random values to "
				<< resolution
				<< " random points."
				<< std::endl;
			RandomIteratorType iterator = 
    		RandomIteratorType( setOfChannels[i], setOfChannels[i]->GetLargestPossibleRegion() );
			iterator.SetNumberOfSamples(resolution);
			iterator.GoToBegin();
			while( !iterator.IsAtEnd() )
			{
				/** Set a random value to a random point */
			 	iterator.Set( static_cast<InternalValueType>( 
					randomGenerator->GetUniformVariate(min_value, max_value) ) );
				++iterator;  
			} 
		}
		else
		{
			std::cout
				<< "Channel"
				<< i
				<< ": Setting random values to "
				<< "all voxels in the image."
				<< std::endl;
			RegionIteratorType iterator = 
    		RegionIteratorType( setOfChannels[i], setOfChannels[i]->GetLargestPossibleRegion() );
			iterator.GoToBegin();
			while( !iterator.IsAtEnd() )
			{
				/** Set a random value to a point */
				iterator.Set( static_cast<InternalValueType>(
					randomGenerator->GetUniformVariate(min_value,max_value) ) );
				++iterator;  
			} 

		}
		
		/** The random image is blurred */
		std::cout 
			<< "Channel"
			<< i
			<< ": Blurring with standard deviation "
			<< sigma 
			<< "."
			<< std::endl;
		
		setOfBlurrers[i] = BlurFilterType::New();
    setOfBlurrers[i]->SetSigma(sigma);
    
		//setOfBlurrers[i]->SetVariance( sigma*sigma );
		//setOfBlurrers[i]->SetUseImageSpacingOff();
		//setOfBlurrers[i]->SetMaximumError(0.01);
		//setOfBlurrers[i]->SetMaximumKernelWidth(maximumKernelWidth);
		setOfBlurrers[i]->SetInput( setOfChannels[i] );
		
		setOfCasters[i] = CastFilterType::New();
		setOfCasters[i]->SetInput( setOfBlurrers[i]->GetOutput() );
		//setOfCasters[i]->UpdateLargestPossibleRegion();

		setOfExtracters[i] = ExtractFilterType::New();
		setOfExtracters[i]->SetInput( setOfCasters[i]->GetOutput() );
		setOfExtracters[i]->SetExtractionRegion(imageregion);

    // suppress warnings about exceeding the maximum kernel width,
    // generated by the blurrers.
	//	itk::Object::GlobalWarningDisplayOff();
		try 
		{
			setOfExtracters[i]->Update();
		}
		catch (itk::ExceptionObject & err)
		{

		//	itk::Object::GlobalWarningDisplayOn();
			std::cerr << "ERROR:" << std::endl;
			std::cerr << err << std::endl;
			return 3;
		}
	
//  itk::Object::GlobalWarningDisplayOn();
  }
	
	/** Combine the channels into the final vector image (if sDim > 1)
	 * and setup the Writer */
	if ( SpaceDimension == 1 )
	{
		scalarWriter = ScalarWriterType::New();
		scalarWriter->SetFileName(outputImageFileName);
		scalarWriter->SetInput( setOfExtracters[0]->GetOutput() );
	}
	else
	{
		vectorWriter = VectorWriterType::New();
		vectorWriter->SetFileName(outputImageFileName);
		if ( SpaceDimension == 2 )
		{
			composer2D = Composer2DType::New();
			composer2D->SetInput1( setOfExtracters[0]->GetOutput() );
			composer2D->SetInput2( setOfExtracters[1]->GetOutput() );
			vectorWriter->SetInput( dynamic_cast<VectorOutputImageType *>( composer2D->GetOutput() )   );
		}
		else if ( SpaceDimension == 3 ) 
		{
			composer3D = Composer3DType::New();
			composer3D->SetInput1( setOfExtracters[0]->GetOutput() );
			composer3D->SetInput2( setOfExtracters[1]->GetOutput() );
			composer3D->SetInput3( setOfExtracters[2]->GetOutput() );
			vectorWriter->SetInput( dynamic_cast<VectorOutputImageType *>( composer3D->GetOutput() )   );
		}
	}
	
	/** do it. */
	std::cout 
		<< "Saving image to disk as \""
		<< outputImageFileName
		<< "\""
		<< std::endl;
	try
	{
		if (scalarWriter)
		{
			scalarWriter->Update();
		}
		else if (vectorWriter)
		{
			vectorWriter->Update();
		}
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << err << std::endl;
		return 2;
	}
	
	return 0;
	
} // end function run_cri

	runwrap(){}
	~runwrap(){}


}; //end runwrap



template < unsigned int NImageDimension, unsigned int NSpaceDimension >
class ptswrap
{ public:

static int PixelTypeSelector(const char * pixelType,
														 const char * outputImageFileName,
														 const itk::Array<unsigned int> & sizes,
														 double min_value, double max_value,
														 unsigned long resolution, double sigma,
														 int rand_seed )
{
	
	
		const unsigned int ImageDimension = NImageDimension; 
		const unsigned int SpaceDimension = NSpaceDimension; 		
		std::map<std::string, enum_type> typemap;
		typemap["FLOAT"] = eFLOAT;
		typemap["INT"] = eINT;
		typemap["UINT"] = eUINT;
		typemap["SHORT"] = eSHORT;
		typemap["USHORT"] = eUSHORT;
		typemap["CHAR"] = eCHAR;
		typemap["UCHAR"] = eUCHAR;
		
		enum_type pt = eUNKNOWN;
		if ( typemap.count(pixelType) )
		{
			pt = typemap[ pixelType ];
		}
		switch( pt )
		{
		case eFLOAT : 
			return  runwrap<ImageDimension, SpaceDimension, float>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed );
		case eSHORT : 
			return runwrap<ImageDimension, SpaceDimension, short>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed );
		case eUSHORT : 
			return runwrap<ImageDimension, SpaceDimension, unsigned short>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed );
		case eINT : 
			return runwrap<ImageDimension, SpaceDimension, int>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed );
		case eUINT : 
			return runwrap<ImageDimension, SpaceDimension, unsigned int>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed );
		case eCHAR : 
			return runwrap<ImageDimension, SpaceDimension, char>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed );
		case eUCHAR : 
			return runwrap<ImageDimension, SpaceDimension, unsigned char>::run_cri(
				outputImageFileName,
				sizes,
				min_value,
				max_value,
				resolution, sigma,
				rand_seed ); 
		default :
			std::cerr << "ERROR: PixelType not supported" << std::endl;
			return 1;
		}
		
}

ptswrap(){}
~ptswrap(){}

}; //end ptswrap



#endif // #ifndef __createrandomimage_h
