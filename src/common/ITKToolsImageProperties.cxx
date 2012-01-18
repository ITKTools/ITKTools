#include "ITKToolsImageProperties.h"
#include "ITKToolsHelpers.h"

#include "itkImage.h"
#include "itkImageFileReader.h"

namespace itktools
{

/**
 * ***************** GetImageProperties ************************
 */

int GetImageProperties(
  const std::string & filename,
  std::string & pixelTypeAsString,
  std::string & componentTypeAsString,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & size )
{
  itk::ImageIOBase::Pointer testImageIOBase;
  GetImageIOBase(filename, testImageIOBase);

  /** Extract information from the ImageIOBase. */
  std::vector<double> dummySpacing, dummyOrigin, dummyDirection;
  if ( !GetImageInformationFromImageIOBase( testImageIOBase,
    pixelTypeAsString, componentTypeAsString, dimension, numberOfComponents,
    size, dummySpacing, dummyOrigin, dummyDirection ) )
  {
    return 1;
  }

  return 0;

} // end GetImageProperties()

/**
 * ***************** GetImageProperties ************************
 */
int GetImageProperties(
  const std::string & filename,
  ComponentType & componentType,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  componentType = imageIOBase->GetComponentType();

  dimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();
  GetImageSize(imageIOBase, imageSize);
  return 0;
}

/**
 * ***************** GetImageProperties ************************
 */
int GetImageProperties(
  const std::string & fileName,
  itk::ImageIOBase::IOPixelType & pixelType,
  ComponentType & componentType,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(fileName, imageIOBase);

  componentType = imageIOBase->GetComponentType();
  pixelType = imageIOBase->GetPixelType();

  dimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();
  GetImageSize(imageIOBase, imageSize);
  return 0;
}


/**
 * ***************** GetImageProperties ************************
 */

int GetImageProperties(
  const std::string & filename,
  std::string & pixelTypeAsString,
  std::string & componentTypeAsString,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & size,
  std::vector<double> & spacing,
  std::vector<double> & origin,
  std::vector<double> & direction )
{
  itk::ImageIOBase::Pointer testImageIOBase;
  GetImageIOBase( filename, testImageIOBase );

  /** Extract information from the ImageIOBase. */
  if ( !GetImageInformationFromImageIOBase( testImageIOBase,
    pixelTypeAsString, componentTypeAsString, dimension, numberOfComponents,
    size, spacing, origin, direction ) )
  {
    return 1;
  }

  return 0;

} // end GetImageProperties()


/**
 * ***************** GetImageProperties ************************
 */

bool GetImageIOBase(
  const std::string & filename,
  itk::ImageIOBase::Pointer & testImageIOBase )
{
  /** Dummy image type. */
  const unsigned int DummyDimension = 3;
  typedef short      DummyPixelType;
  typedef itk::Image< DummyPixelType, DummyDimension >   DummyImageType;

  /** Create a testReader. */
  typedef itk::ImageFileReader< DummyImageType >     ReaderType;
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( filename.c_str() );

  /** Generate all information. */
  try
  {
    testReader->GenerateOutputInformation();
  }
  catch ( itk::ExceptionObject & e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return false;
  }

  /** Extract the ImageIO from the testReader. */
  testImageIOBase = testReader->GetImageIO();

  return true;

} // end GetImageProperties()

/**
 * ***************** GetImagePixelType ************************
 */
/** Determine pixeltype (scalar/vector) of an image */
bool GetImagePixelType(
  const std::string & filename,
  std::string & pixelType)
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  pixelType = imageIOBase->GetPixelTypeAsString(
    imageIOBase->GetPixelType() );

  return true;
}

/**
 * ***************** GetImageComponentType ************************
 */
/** Determine componenttype (short, float etc) of an image */

bool GetImageComponentType(
  const std::string & filename,
  itktools::ComponentType & componentType)
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  componentType = imageIOBase->GetComponentType();

  return true;
}

/**
 * ***************** GetImageDimension ************************
 */
/** Determine dimension of an image */
bool GetImageDimension(
  const std::string & filename,
  unsigned int & dimension)
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  dimension = imageIOBase->GetNumberOfDimensions();

  return true;
}

/**
 * ***************** GetImageNumberOfComponents ************************
 */
/** Determine the number of components of each pixel in an image. */
bool GetImageNumberOfComponents(
  const std::string & filename,
  unsigned int & numberOfComponents)
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  numberOfComponents = imageIOBase->GetNumberOfComponents();

  return true;
}

/**
 * ***************** GetImageSize ************************
 * Determine the size of an image.
 */
bool GetImageSize(
  const std::string & filename,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  GetImageSize(imageIOBase, imageSize);

  return true;
}

/**
 * ***************** GetImageSize ************************
 * Determine the size of an image.
 */
bool GetImageSize(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<unsigned int> & imageSize )
{
  unsigned int dimension = imageIOBase->GetNumberOfDimensions();
  imageSize .resize( dimension );

  for ( unsigned int i = 0; i < dimension; i++ )
  {
    imageSize [ i ] = imageIOBase->GetDimensions( i );
  }

  return true;
}


/**
 * ***************** FillImageIOBase ************************
 */

void FillImageIOBase( itk::ImageIOBase::Pointer & imageIOBase,
  const std::string & pixelTypeAsString,
  const std::string & componentTypeAsString,
  const unsigned int & imageDimension,
  const unsigned int & numberOfComponents,
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction )
{
  /** Set image dimensionality. */
  imageIOBase->SetNumberOfDimensions( imageDimension );
  imageIOBase->SetNumberOfComponents( numberOfComponents );

  /** Set pixel type. */
  itk::ImageIOBase::IOPixelType pixelType
    = itk::ImageIOBase::GetPixelTypeFromString( pixelTypeAsString );
  imageIOBase->SetPixelType( pixelType );

  /** Set component type. */
  itk::ImageIOBase::IOComponentType componentType
    = itk::ImageIOBase::GetComponentTypeFromString( componentTypeAsString );
  imageIOBase->SetComponentType( componentType );

  /** Set size, spacing, origin. */
  for ( unsigned int i = 0; i < imageDimension; i++ )
  {
    imageIOBase->SetDimensions( i, size[ i ] );
    imageIOBase->SetSpacing( i, spacing[ i ] );
    imageIOBase->SetOrigin( i, origin[ i ] );
  }

  /** Set image direction / orientation. */
  for ( unsigned int i = 0; i < imageDimension; i++ )
  {
    std::vector< double > subDirection( imageDimension );
    for ( unsigned int j = 0; j < imageDimension; j++ )
    {
      subDirection[ j ] = direction[ j + i * imageDimension ];
    }
    imageIOBase->SetDirection( i, subDirection );
  }

} // end FillImageIOBase()


/**
 * ***************** GetImageInformationFromImageIOBase ************************
 */

bool GetImageInformationFromImageIOBase(
  const itk::ImageIOBase::Pointer & imageIOBase,
  std::string & pixelTypeAsString,
  std::string & componentTypeAsString,
  unsigned int & imageDimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & size,
  std::vector<double> & spacing,
  std::vector<double> & origin,
  std::vector<double> & direction )
{
  /** Get the component type, number of components, dimension and pixel type. */
  imageDimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();
  componentTypeAsString = imageIOBase->GetComponentTypeAsString(
    imageIOBase->GetComponentType() );
  //itktools::ReplaceUnderscoreWithSpace( componentTypeAsString );
  pixelTypeAsString = imageIOBase->GetPixelTypeAsString(
    imageIOBase->GetPixelType() );

  /** Get the image size, spacing, origin, direction. */
  size.resize( imageDimension );
  spacing.resize( imageDimension );
  origin.resize( imageDimension );
  direction.resize( imageDimension * imageDimension );
  for ( unsigned int i = 0; i < imageDimension; i++ )
  {
    size[ i ] = imageIOBase->GetDimensions( i );
    spacing[ i ] = imageIOBase->GetSpacing( i );
    origin[ i ] = imageIOBase->GetOrigin( i );
    std::vector<double> tmpDirectionVec = imageIOBase->GetDirection( i );
    for ( unsigned int j = 0; j < imageDimension; j++ )
    {
      direction[ i * imageDimension + j ] = tmpDirectionVec[ j ];
    }
  }

  /** Check inputPixelType. */
  if ( componentTypeAsString == "unknown" )
  {
    /** In this case an illegal pixeltype is found. */
    std::cerr
      << "ERROR while determining image properties!"
      << "The found componenttype is \""
      << componentTypeAsString
      << "\", which is not supported."
      << std::endl;
    return false;
  }

  return true;

} // GetImageInformationFromImageIOBase()

} // end namespace itktools
