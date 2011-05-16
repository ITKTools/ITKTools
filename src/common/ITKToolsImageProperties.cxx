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
  GetImageIOBase(filename, testImageIOBase);

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
 */
/** Determine the size of an image.
 */
bool GetImageSize(
  const std::string & filename,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase(filename, imageIOBase);

  unsigned int dimension;
  GetImageDimension(filename,dimension);
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
  itk::ImageIOBase::IOPixelType pixelType;
  if ( pixelTypeAsString == "scalar" )
  {
    pixelType = itk::ImageIOBase::SCALAR;
  }
  else if ( pixelTypeAsString == "vector" )
  {
    pixelType = itk::ImageIOBase::VECTOR;
  }
  else if ( pixelTypeAsString == "covariant_vector" )
  {
    pixelType = itk::ImageIOBase::COVARIANTVECTOR;
  }
  else if ( pixelTypeAsString == "point" )
  {
    pixelType = itk::ImageIOBase::POINT;
  }
  else if ( pixelTypeAsString == "offset" )
  {
    pixelType = itk::ImageIOBase::OFFSET;
  }
  else if ( pixelTypeAsString == "rgb" )
  {
    pixelType = itk::ImageIOBase::RGB;
  }
  else if ( pixelTypeAsString == "rgba" )
  {
    pixelType = itk::ImageIOBase::RGBA;
  }
  else if ( pixelTypeAsString == "symmetric_second_rank_tensor" )
  {
    pixelType = itk::ImageIOBase::SYMMETRICSECONDRANKTENSOR;
  }
  else if ( pixelTypeAsString == "diffusion_tensor_3D" )
  {
    pixelType = itk::ImageIOBase::DIFFUSIONTENSOR3D;
  }
  else if ( pixelTypeAsString == "complex" )
  {
    pixelType = itk::ImageIOBase::COMPLEX;
  }
  else
  {
    pixelType = itk::ImageIOBase::UNKNOWNPIXELTYPE;
  }
  imageIOBase->SetPixelType( pixelType );

  /** Set component type. */
  itk::ImageIOBase::IOComponentType componentType;
  if ( componentTypeAsString == "unsigned_char" )
  {
    componentType = itk::ImageIOBase::UCHAR;
  }
  else if ( componentTypeAsString == "char" )
  {
    componentType = itk::ImageIOBase::CHAR;
  }
  else if ( componentTypeAsString == "unsigned_short" )
  {
    componentType = itk::ImageIOBase::USHORT;
  }
  else if ( componentTypeAsString == "short" )
  {
    componentType = itk::ImageIOBase::SHORT;
  }
  else if ( componentTypeAsString == "unsigned_int" )
  {
    componentType = itk::ImageIOBase::UINT;
  }
  else if ( componentTypeAsString == "int" )
  {
    componentType = itk::ImageIOBase::INT;
  }
  else if ( componentTypeAsString == "unsigned_long" )
  {
    componentType = itk::ImageIOBase::ULONG;
  }
  else if ( componentTypeAsString == "long" )
  {
    componentType = itk::ImageIOBase::LONG;
  }
  else if ( componentTypeAsString == "float" )
  {
    componentType = itk::ImageIOBase::FLOAT;
  }
  else if ( componentTypeAsString == "double" )
  {
    componentType = itk::ImageIOBase::DOUBLE;
  }
  else
  {
    componentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  }
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
  itktools::ReplaceUnderscoreWithSpace( componentTypeAsString );
  pixelTypeAsString = imageIOBase->GetPixelTypeAsString(
    imageIOBase->GetPixelType() );

  /** Get the image size, spacing, origin. */
  size.resize( imageDimension );
  spacing.resize( imageDimension );
  origin.resize( imageDimension );
  for ( unsigned int i = 0; i < imageDimension; i++ )
  {
    size[ i ] = imageIOBase->GetDimensions( i );
    spacing[ i ] = imageIOBase->GetSpacing( i );
    origin[ i ] = imageIOBase->GetOrigin( i );
  }

  /** Check inputPixelType. */
  if ( componentTypeAsString != "unsigned char"
    && componentTypeAsString != "char"
    && componentTypeAsString != "unsigned short"
    && componentTypeAsString != "short"
    && componentTypeAsString != "unsigned int"
    && componentTypeAsString != "int"
    && componentTypeAsString != "unsigned long"
    && componentTypeAsString != "long"
    && componentTypeAsString != "float"
    && componentTypeAsString != "double" )
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
