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
/** \file
 \brief Create a random image.
 
 \verbinclude createrandomimage.help
 */
#ifndef __createrandomimage_cxx
#define __createrandomimage_cxx

#include "createrandomimage.h"

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageToVectorImageFilter.h"

std::string GetHelpString(void)
{
  std::stringstream ss;
  ss << "This program creates a random image." << std::endl
    << "Usage:" << std::endl
    << "pxcreaterandomimage" << std::endl
    << "-out   \tOutputImageFileName" << std::endl
    << "-pt    \tPixelType <SHORT, USHORT, INT, UINT, CHAR, UCHAR, FLOAT>" << std::endl
    << "-id    \tImageDimension <2,3>" << std::endl
    << "[-sd]  \tSpaceDimension (the number of channels) <1,2,3>" << std::endl
    << "-d0    \tSize of dimension 0" << std::endl
    << "-d1    \tSize of dimension 1" << std::endl
    << "[-d2]  \tSize of dimension 2" << std::endl
    << "[-r]   \tThe resolution of the random image <unsigned long>." << std::endl
    << "This determines the number of voxels set to a random value before blurring." << std::endl
    << "If set to 0, all voxels are set to a random value" << std::endl
    << "[-sigma]\tThe standard deviation of the blurring filter" << std::endl
    << "[-min] \tMinimum pixel value" << std::endl
    << "[-max] \tMaximum pixel value" << std::endl
    << "[-seed]\tThe random seed <int>";
  //<< "\t[-d3]  \tSize of dimension 3\n"
  //<< "\t[-d4]  \tSize of dimension 4\n"
  return ss.str();
} // end GetHelpString


/** CreateRandomImage */

class CreateRandomImageBase : public itktools::ITKToolsBase
{ 
public:
  CreateRandomImageBase(){};
  ~CreateRandomImageBase(){};

  /** Input parameters */
  std::string m_OutputFileName;
  itk::Array<unsigned int> m_Sizes;
  double m_Min_value;
  double m_Max_value;
  unsigned long m_Resolution;
  double m_Sigma;
  int m_Rand_seed;
  unsigned int m_SpaceDimension;
    
}; // end CreateGridImageBase


template< unsigned int VImageDimension, class TValue >
class CreateRandomImage : public CreateRandomImageBase
{
public:
  typedef CreateRandomImage Self;

  CreateRandomImage(){};
  ~CreateRandomImage(){};

  static Self * New( unsigned int imageDimension, itktools::ComponentType componentType )
  {
    if ( VImageDimension == imageDimension && itktools::IsType<TValue>( componentType ) )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedefs */

    /** Dimensions */
    const unsigned int ImageDimension = VImageDimension;

    /** PixelTypes */
    typedef TValue    ValueType;
    typedef ValueType ScalarPixelType;
    typedef float InternalValueType;
    //typedef itk::VariableLengthVector<ValueType> VectorPixelType;

    /** ImageTypes */
    typedef itk::Image<ValueType, ImageDimension> ImageType;
    typedef itk::Image<InternalValueType, ImageDimension> InternalImageType;
    typedef itk::Image<ScalarPixelType, ImageDimension> ScalarOutputImageType;
    typedef itk::VectorImage<ScalarPixelType, ImageDimension> VectorOutputImageType;
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

    typedef std::vector<InternalImagePointer> SetOfChannelsType;

    /** Iterator */
    typedef itk::ImageRandomIteratorWithIndex<InternalImageType>  RandomIteratorType;
    typedef itk::ImageRegionIterator<InternalImageType> RegionIteratorType;

    /** Blurring filter */
    //typedef itk::DiscreteGaussianImageFilter<
    // InternalImageType, InternalImageType>              BlurFilterType;
    typedef itk::SmoothingRecursiveGaussianImageFilter<
      InternalImageType, InternalImageType>               BlurFilterType;
    typedef typename BlurFilterType::Pointer            BlurFilterPointer;
    
    typedef std::vector<BlurFilterPointer> SetOfBlurrersType;

    typedef itk::CastImageFilter<InternalImageType, ImageType> CastFilterType;
    typedef typename CastFilterType::Pointer CastFilterPointer;
    typedef std::vector<CastFilterPointer> SetOfCastersType;

    typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractFilterType;
    typedef typename ExtractFilterType::Pointer ExtractFilterPointer;
    typedef std::vector<ExtractFilterPointer> SetOfExtractersType;

    /** ImageWriters */
    typedef itk::ImageFileWriter<VectorOutputImageType> VectorWriterType;
    typedef typename VectorWriterType::Pointer VectorWriterPointer;

    /** RandomGenerator */
    typedef itk::Statistics::MersenneTwisterRandomVariateGenerator RandomGeneratorType;
    typedef RandomGeneratorType::Pointer RandomGeneratorPointer;


    /** Create variables */
    VectorWriterPointer vectorWriter = 0;
    
    SetOfChannelsType setOfChannels(m_SpaceDimension);
    
    SetOfBlurrersType setOfBlurrers(m_SpaceDimension);
    
    SetOfCastersType setOfCasters(m_SpaceDimension);
    
    SetOfExtractersType setOfExtracters(m_SpaceDimension);
    
    RandomGeneratorPointer randomGenerator = RandomGeneratorType::New();
    bool randomiterating = true;

    /** Set the random seed */
    randomGenerator->SetSeed(m_Rand_seed);

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
      internalimagesize.SetElement(i, m_Sizes[i]); //will be changed later
      imagesize.SetElement(i, m_Sizes[i]);
      imageindex.SetElement(i, 0);
      imageorigin.SetElement(i,0.0);
      nrOfPixels *= m_Sizes[i];
    }

    /** Compute the standard deviation of the Gaussian used for blurring
    * the random images */
    if ( m_Sigma < 0 )
    {
      m_Sigma = static_cast<double>(
	static_cast<double>(nrOfPixels) /
	static_cast<double>(m_Resolution) /
	pow( 2.0, static_cast<double>(ImageDimension) )
	);
    }

    int paddingSize = static_cast<int>(2.0*m_Sigma);
    

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
    if ( m_Resolution ==0 )
    {
      randomiterating = false;
    }

    /** Create the images */
    for (unsigned int i = 0; i< m_SpaceDimension; i++)
    {

      setOfChannels[i] = InternalImageType::New();
      setOfChannels[i]->SetRegions( internalimageregion );
      setOfChannels[i]->SetOrigin( internalimageorigin );
      setOfChannels[i]->SetRequestedRegion( imageregion );
      setOfChannels[i]->Allocate();
      setOfChannels[i]->FillBuffer( itk::NumericTraits<InternalValueType>::Zero );


      /** Setting random values to random points */

      if (randomiterating)
      {
	std::cout
	  << "Channel"
	  << i
	  << ": Setting random values to "
	  << m_Resolution
	  << " random points."
	  << std::endl;
	RandomIteratorType iterator =
	  RandomIteratorType( setOfChannels[i], setOfChannels[i]->GetLargestPossibleRegion() );
	iterator.SetNumberOfSamples(m_Resolution);
	iterator.GoToBegin();
	while( !iterator.IsAtEnd() )
	{
	  /** Set a random value to a random point */
	  iterator.Set( static_cast<InternalValueType>(
	    randomGenerator->GetUniformVariate(m_Min_value, m_Max_value) ) );
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
	    randomGenerator->GetUniformVariate(m_Min_value,m_Max_value) ) );
	  ++iterator;
	}

      }

      /** The random image is blurred */
      std::cout
	<< "Channel"
	<< i
	<< ": Blurring with standard deviation "
	<< m_Sigma
	<< "."
	<< std::endl;

      setOfBlurrers[i] = BlurFilterType::New();
      setOfBlurrers[i]->SetSigma(m_Sigma);

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
    //  itk::Object::GlobalWarningDisplayOff();
      try
      {
	setOfExtracters[i]->Update();
      }
      catch (itk::ExceptionObject & err)
      {

      //  itk::Object::GlobalWarningDisplayOn();
	std::cerr << "ERROR:" << std::endl;
	std::cerr << err << std::endl;
      }

  //  itk::Object::GlobalWarningDisplayOn();
    }

    typedef itk::ImageToVectorImageFilter<ScalarOutputImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
    for(unsigned int spaceDimensionIndex = 0; spaceDimensionIndex < m_SpaceDimension; ++spaceDimensionIndex)
    {
      imageToVectorImageFilter->SetNthInput(spaceDimensionIndex, setOfExtracters[spaceDimensionIndex]->GetOutput());
    }
    imageToVectorImageFilter->Update();
    
    std::cout
      << "Saving image to disk as \""
      << m_OutputFileName
      << "\""
      << std::endl;
    
    vectorWriter = VectorWriterType::New();
    vectorWriter->SetInput(imageToVectorImageFilter->GetOutput());
    vectorWriter->Update();
    
  }

}; // end CreateRandomImage

/**
 * ********************* main ***********************************
 */

int main(int argc, char** argv)
{

  std::string outputImageFileName("");
  std::string pixelType("");
  unsigned int imageDimension = 0;
  unsigned int spaceDimension = 1;
  double sigma = -1.0;

  std::ostringstream makeString("");
  itk::Array<unsigned int> sizes;
  unsigned int iDim = 0;

  double min_value = 0.0;
  double max_value = 0.0;
  int rand_seed = 0;

  unsigned long nrOfPixels = 1;

  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );
  
  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  
  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }
  
  parser->GetCommandLineArgument("-out", outputImageFileName);
  parser->GetCommandLineArgument("-pt", pixelType);
  parser->GetCommandLineArgument("-id", iDim);
  parser->GetCommandLineArgument("-sd", spaceDimension);
  parser->GetCommandLineArgument("-sigma", sigma);
  parser->GetCommandLineArgument("-min", min_value);
  parser->GetCommandLineArgument("-max", max_value);
  parser->GetCommandLineArgument("-seed", rand_seed);


  if (iDim ==0)
  {
    std::cerr << "ERROR: Image dimension cannot be 0" <<std::endl;
    return 1;
  }
  sizes.SetSize(iDim);
  for (unsigned int i=0; i< iDim ; i++)
  {
    makeString.str("");
    makeString << "-d" << i;
    unsigned int dimsize = 0;
    bool retdimsize = parser->GetCommandLineArgument(makeString.str(), dimsize);
    if (!retdimsize)
    {
      sizes[i] = dimsize;
      nrOfPixels *= sizes[i];
    }
  }

  unsigned long resolution = nrOfPixels/64;
  parser->GetCommandLineArgument("-r", resolution);
  
  /** Class that does the work */
  CreateRandomImageBase * createRandomImage = NULL; 
  itktools::ComponentType componentType = itktools::GetComponentTypeFromString(pixelType);

  try
  {
    // now call all possible template combinations.
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, float >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, short >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, unsigned short >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, int >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, unsigned int >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, char >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 2, unsigned char >::New( imageDimension, componentType );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, float >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, short >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, unsigned short >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, int >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, unsigned int >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, char >::New( imageDimension, componentType );
    if (!createRandomImage) createRandomImage = CreateRandomImage< 3, unsigned char >::New( imageDimension, componentType );

#endif
    if (!createRandomImage) 
    {
      std::cerr << "ERROR: this combination of pixeltype, image dimension, and space dimension is not supported!" << std::endl;
      std::cerr
        << " image dimension = " << imageDimension << std::endl
        << " space dimension = " << spaceDimension << std::endl
        << " pixel type = " << componentType << std::endl
        << std::endl;
      return 1;
    }

    createRandomImage->m_OutputFileName = outputImageFileName;
    createRandomImage->m_Sizes = sizes;
    createRandomImage->m_Min_value = min_value;
    createRandomImage->m_Max_value = max_value;
    createRandomImage->m_Resolution = resolution;
    createRandomImage->m_Sigma = sigma;
    createRandomImage->m_Rand_seed = rand_seed;
    createRandomImage->m_SpaceDimension = spaceDimension;
    
    createRandomImage->Run();
    
    delete createRandomImage;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete createRandomImage;
    return 1;
  }
  
  return 0;

} // end function main

#endif // #ifndef __createrandomimage_cxx
