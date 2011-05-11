#ifndef __CommandLineArgumentHelper_cxx
#define __CommandLineArgumentHelper_cxx

#include "CommandLineArgumentHelper.h"
#include "itkImageFileReader.h"


/**
 * *************** ReplaceUnderscoreWithSpace ***********************
 */

void ReplaceUnderscoreWithSpace( std::string & arg )
{
  /** Get rid of the possible "_" in arg. */
  std::basic_string<char>::size_type pos = arg.find( "_" );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos != npos )
  {
    arg.replace( pos, 1, " " );
  }

} // end ReplaceUnderscoreWithSpace()


/**
 * *************** ReplaceSpaceWithUnderscore ***********************
 */

void ReplaceSpaceWithUnderscore( std::string & arg )
{
  /** Get rid of the possible " " in arg. */
  std::basic_string<char>::size_type pos = arg.find( " " );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos != npos )
  {
    arg.replace( pos, 1, "_" );
  }

} // end ReplaceSpaceWithUnderscore()


/**
 * *************** RemoveUnsignedFromString ***********************
 */

void RemoveUnsignedFromString( std::string & arg )
{
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  std::basic_string<char>::size_type pos = arg.find( "unsigned " );
  if ( pos != npos ) arg = arg.substr( pos + 9 );
  pos = arg.find( "unsigned_" );
  if ( pos != npos ) arg = arg.substr( pos + 9 );

} // end RemoveUnsignedFromString()


/**
 * *************** CheckForValidComponentType ***********************
 */

bool CheckForValidComponentType( const std::string & arg )
{
  std::string componenttype = arg;
  ReplaceUnderscoreWithSpace( componenttype );

  /** Check argument. */
  bool out = true;
  if ( componenttype != "unsigned char"
    && componenttype != "char"
    && componenttype != "unsigned short"
    && componenttype != "short"
    && componenttype != "unsigned int"
    && componenttype != "int"
    && componenttype != "unsigned long"
    && componenttype != "long"
    && componenttype != "float"
    && componenttype != "double" )
  {
    out = false;
  }

  return out;

} // end CheckForValidComponentType()


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
  /** Dummy image type. */
  const unsigned int DummyDimension = 3;
  typedef short      DummyPixelType;
  typedef itk::Image< DummyPixelType, DummyDimension >   DummyImageType;

  /** Test reader */
  typedef itk::ImageFileReader< DummyImageType >     ReaderType;

  /** Image header information class */
  typedef itk::ImageIOBase                           ImageIOBaseType;

   /** Create a testReader. */
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( filename.c_str() );

  /** Generate all information. */
  try
  {
    testReader->GenerateOutputInformation();
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  /** Extract the ImageIO from the testReader. */
  ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();

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
  /** Dummy image type. */
  const unsigned int DummyDimension = 3;
  typedef short      DummyPixelType;
  typedef itk::Image< DummyPixelType, DummyDimension >   DummyImageType;

  /** Test reader */
  typedef itk::ImageFileReader< DummyImageType >     ReaderType;

  /** Image header information class */
  typedef itk::ImageIOBase                           ImageIOBaseType;

  /** Create a testReader. */
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName( filename.c_str() );

  /** Generate all information. */
  try
  {
    testReader->GenerateOutputInformation();
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    return 1;
  }

  /** Extract the ImageIO from the testReader. */
  ImageIOBaseType::Pointer testImageIOBase = testReader->GetImageIO();

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

bool GetImageProperties(
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
  ReplaceUnderscoreWithSpace( componentTypeAsString );
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


/**
 * *************** ConvertImageInformationToITKTypes ***********************
 */

template<unsigned int Dimension>
void ConvertImageInformationToITKTypes(
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::SizeType      & sizeITK,
  typename itk::ImageBase<Dimension>::SpacingType   & spacingITK,
  typename itk::ImageBase<Dimension>::PointType     & originITK,
  typename itk::ImageBase<Dimension>::DirectionType & directionITK )
{
  typedef itk::ImageBase<Dimension> ImageBaseType;
  typedef typename ImageBaseType::SizeValueType      SizeValueType;

  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    sizeITK[ i ] = static_cast<SizeValueType>( size[ i ] );
    spacingITK[ i ] = spacing[ i ];
    originITK[ i ] = origin[ i ];
  }

  /** Example: [a b c d] becomes:
   *  a b
   *  c d
   */
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    for ( unsigned int j = 0; j < Dimension; j++ )
    {
      directionITK[ i ][ j ] = direction[ j + i * Dimension ];
    }
  }

} // end ConvertImageInformationToITKTypes()

/** Instantiate for dimension 2 and 3. */
template void ConvertImageInformationToITKTypes<2>(
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction,
  itk::ImageBase<2>::SizeType       & sizeITK,
  itk::ImageBase<2>::SpacingType    & spacingITK,
  itk::ImageBase<2>::PointType      & originITK,
  itk::ImageBase<2>::DirectionType  & directionITK );

template void ConvertImageInformationToITKTypes<3>(
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction,
  itk::ImageBase<3>::SizeType       & sizeITK,
  itk::ImageBase<3>::SpacingType    & spacingITK,
  itk::ImageBase<3>::PointType      & originITK,
  itk::ImageBase<3>::DirectionType  & directionITK );


/**
 * *************** GetLargestComponentType ***********************
 */

std::string GetLargestComponentType(
  const std::string & type1, const std::string & type2 )
{
  /** Typedef's. */
  typedef std::map< std::string, unsigned int > RankingType;
  typedef RankingType::value_type               EntryType;

  /** Define the ranking. */
  RankingType ranking;
  ranking.insert( EntryType( "char",   1 ) );
  ranking.insert( EntryType( "short",  2 ) );
  ranking.insert( EntryType( "int",    3 ) );
  ranking.insert( EntryType( "long",   4 ) );
  ranking.insert( EntryType( "float",  5 ) );
  ranking.insert( EntryType( "double", 6 ) );

  /** Remove unsigned. */
  std::string Type1 = type1;
  std::string Type2 = type2;
  RemoveUnsignedFromString( Type1 );
  RemoveUnsignedFromString( Type2 );

  /** Determine which one is the largest. */
  std::string output = "";
  if ( Type1 == Type2 )
  {
    output = type1;
  }
  else
  {
    output = ranking[ Type1 ] > ranking[ Type2 ] ? type1 : type2;
  }

  /** Return a value. */
  return output;

} // end GetTheLargestComponentType()


#endif // end #ifndef __CommandLineArgumentHelper_h

