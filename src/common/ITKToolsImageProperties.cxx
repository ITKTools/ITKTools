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
#include "ITKToolsImageProperties.h"

#include "itkImage.h"
#include "itkImageFileReader.h"

namespace itktools
{

/**
 * ***************** GetImagePixelType ************************
 */

bool GetImagePixelType(
  const std::string & filename,
  std::string & pixelType )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase( filename, imageIOBase );

  pixelType = imageIOBase->GetPixelTypeAsString(
    imageIOBase->GetPixelType() );

  return true;

} // end GetImagePixelType()


/**
 * ***************** GetImageComponentType ************************
 */

bool GetImageComponentType(
  const std::string & filename,
  itk::ImageIOBase::IOComponentType & componentType )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase( filename, imageIOBase );

  componentType = imageIOBase->GetComponentType();

  return true;

} // end GetImageComponentType()


/**
 * ***************** GetImageComponentType ************************
 */

itk::ImageIOBase::IOComponentType GetImageComponentType( const std::string & filename )
{
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(
    filename.c_str(), itk::ImageIOFactory::ReadMode);
  if( imageIO.IsNull() )
  {
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE; // complain
  }
  imageIO->SetFileName( filename.c_str() );
  imageIO->ReadImageInformation();

  return imageIO->GetComponentType();

} // end GetImageComponentType()


/**
 * ***************** GetImageNumberOfComponents ************************
 */

bool GetImageNumberOfComponents(
  const std::string & filename,
  unsigned int & numberOfComponents )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase( filename, imageIOBase );

  numberOfComponents = imageIOBase->GetNumberOfComponents();

  return true;

} // end GetImageNumberOfComponents()


/**
 * ***************** GetImageDimension ************************
 */

bool GetImageDimension(
  const std::string & filename,
  unsigned int & dimension )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase( filename, imageIOBase );

  dimension = imageIOBase->GetNumberOfDimensions();

  return true;

} // end GetImageDimension()


/**
 * ***************** GetImageSize ************************
 */

bool GetImageSize(
  const std::string & filename,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase( filename, imageIOBase );

  GetImageSize( imageIOBase, imageSize );

  return true;

} // end GetImageSize()


/**
 * ***************** GetImageSize ************************
 */

bool GetImageSize(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<unsigned int> & imageSize )
{
  const unsigned int dimension = imageIOBase->GetNumberOfDimensions();
  imageSize.resize( dimension );

  for( unsigned int i = 0; i < dimension; i++ )
  {
    imageSize [ i ] = imageIOBase->GetDimensions( i );
  }

  return true;

} // end GetImageSize()


/**
 * ***************** GetImageOrigin ************************
 */

bool GetImageOrigin(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<double> & imageOrigin )
{
  const unsigned int dimension = imageIOBase->GetNumberOfDimensions();
  imageOrigin.resize( dimension );
  for( unsigned int i = 0; i < dimension; i++ )
  {
    imageOrigin[ i ] = imageIOBase->GetOrigin( i );
  }

  return true;

} // end GetImageOrigin()


/**
 * ***************** GetImageSpacing ************************
 */
bool GetImageSpacing(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<double> & imageSpacing )
{
  const unsigned int dimension = imageIOBase->GetNumberOfDimensions();
  imageSpacing.resize( dimension );
  for( unsigned int i = 0; i < dimension; i++ )
  {
    imageSpacing[ i ] = imageIOBase->GetSpacing( i );
  }

  return true;

} // end GetImageSpacing()


/**
 * ***************** GetImageDirection ************************
 */

bool GetImageDirection(
  itk::ImageIOBase::Pointer imageIOBase,
  std::vector<double> & imageDirection )
{
  const unsigned int dimension = imageIOBase->GetNumberOfDimensions();
  imageDirection.resize( dimension * dimension );
  for( unsigned int i = 0; i < dimension; i++ )
  {
    std::vector<double> tmpDirectionVec = imageIOBase->GetDirection( i );
    for( unsigned int j = 0; j < dimension; j++ )
    {
      imageDirection[ i * dimension + j ] = tmpDirectionVec[ j ];
    }
  }

  return true;

} // end GetImageDirection()


/**
 * ***************** GetImageProperties ************************
 */

bool GetImageProperties(
  const std::string & fileName,
  itk::ImageIOBase::IOPixelType & pixelType,
  itk::ImageIOBase::IOComponentType & componentType,
  unsigned int & dimension,
  unsigned int & numberOfComponents )
{
  itk::ImageIOBase::Pointer imageIOBase;
  bool success = GetImageIOBase( fileName, imageIOBase );

  pixelType = imageIOBase->GetPixelType();
  componentType = imageIOBase->GetComponentType();
  dimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();

  return success;

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
  std::vector<unsigned int> & size )
{
  itk::ImageIOBase::Pointer testImageIOBase;
  GetImageIOBase(filename, testImageIOBase);

  /** Extract information from the ImageIOBase. */
  std::vector<double> dummySpacing, dummyOrigin, dummyDirection;
  if( !GetImageInformationFromImageIOBase( testImageIOBase,
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
  itk::ImageIOBase::IOComponentType & componentType,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  GetImageIOBase( filename, imageIOBase );

  componentType = imageIOBase->GetComponentType();

  dimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();
  GetImageSize( imageIOBase, imageSize );

  return 0;

} // end GetImageProperties()


/**
 * ***************** GetImageProperties ************************
 */

bool GetImageProperties(
  const std::string & fileName,
  itk::ImageIOBase::IOPixelType & pixelType,
  itk::ImageIOBase::IOComponentType & componentType,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & imageSize )
{
  itk::ImageIOBase::Pointer imageIOBase;
  bool success = GetImageIOBase( fileName, imageIOBase );

  componentType = imageIOBase->GetComponentType();
  pixelType = imageIOBase->GetPixelType();

  dimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();
  GetImageSize( imageIOBase, imageSize );

  return success;

} // end GetImageProperties()


/**
 * ***************** GetImageProperties ************************
 */

bool GetImageProperties(
  const std::string & fileName,
  itk::ImageIOBase::IOPixelType & pixelType,
  itk::ImageIOBase::IOComponentType & componentType,
  unsigned int & dimension,
  unsigned int & numberOfComponents,
  std::vector<unsigned int> & imageSize,
  std::vector<double> & imageSpacing,
  std::vector<double> & imageOrigin,
  std::vector<double> & imageDirection )
{
  itk::ImageIOBase::Pointer imageIOBase;
  bool success = GetImageIOBase( fileName, imageIOBase );

  componentType = imageIOBase->GetComponentType();
  pixelType = imageIOBase->GetPixelType();

  dimension = imageIOBase->GetNumberOfDimensions();
  numberOfComponents = imageIOBase->GetNumberOfComponents();
  GetImageSize( imageIOBase, imageSize );
  GetImageSpacing( imageIOBase, imageSpacing );
  GetImageOrigin( imageIOBase, imageOrigin );
  GetImageDirection( imageIOBase, imageDirection );

  return success;

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
  GetImageIOBase( filename, testImageIOBase );

  /** Extract information from the ImageIOBase. */
  if( !GetImageInformationFromImageIOBase( testImageIOBase,
    pixelTypeAsString, componentTypeAsString, dimension, numberOfComponents,
    size, spacing, origin, direction ) )
  {
    return 1;
  }

  return 0;

} // end GetImageProperties()


/**
 * ***************** GetImageIOBase ************************
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
    testReader->UpdateOutputInformation();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ERROR: Caught ITK exception: " << excp << std::endl;
    return false;
  }

  /** Extract the ImageIO from the testReader. */
  testImageIOBase = testReader->GetImageIO();

  return true;

} // end GetImageProperties()


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
  for( unsigned int i = 0; i < imageDimension; i++ )
  {
    imageIOBase->SetDimensions( i, size[ i ] );
    imageIOBase->SetSpacing( i, spacing[ i ] );
    imageIOBase->SetOrigin( i, origin[ i ] );
  }

  /** Set image direction / orientation. */
  for( unsigned int i = 0; i < imageDimension; i++ )
  {
    std::vector< double > subDirection( imageDimension );
    for( unsigned int j = 0; j < imageDimension; j++ )
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
  pixelTypeAsString = imageIOBase->GetPixelTypeAsString(
    imageIOBase->GetPixelType() );

  /** Get the image size, spacing, origin, direction. */
  size.resize( imageDimension );
  spacing.resize( imageDimension );
  origin.resize( imageDimension );
  direction.resize( imageDimension * imageDimension );
  for( unsigned int i = 0; i < imageDimension; i++ )
  {
    size[ i ] = imageIOBase->GetDimensions( i );
    spacing[ i ] = imageIOBase->GetSpacing( i );
    origin[ i ] = imageIOBase->GetOrigin( i );
    std::vector<double> tmpDirectionVec = imageIOBase->GetDirection( i );
    for( unsigned int j = 0; j < imageDimension; j++ )
    {
      direction[ i * imageDimension + j ] = tmpDirectionVec[ j ];
    }
  }

  /** Check inputPixelType. */
  if( componentTypeAsString == "unknown" )
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
