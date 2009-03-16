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
  std::string & pixeltype,
  std::string & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & imagesize )
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

  /** Get the component type, number of components, dimension and pixel type. */
  dimension = testImageIOBase->GetNumberOfDimensions();
  numberofcomponents = testImageIOBase->GetNumberOfComponents();
  componenttype = testImageIOBase->GetComponentTypeAsString(
    testImageIOBase->GetComponentType() );
  ReplaceUnderscoreWithSpace( componenttype );
  pixeltype = testImageIOBase->GetPixelTypeAsString(
    testImageIOBase->GetPixelType() );

  /** Get the image size. */
  imagesize.resize( dimension );
  for ( unsigned int i = 0; i < dimension; i++ )
  {
    imagesize[ i ] = testImageIOBase->GetDimensions( i );
  }

  /** Check inputPixelType. */
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
    /** In this case an illegal pixeltype is found. */
    std::cerr 
      << "ERROR while determining image properties!"
      << "The found componenttype is \""
      << componenttype
      << "\", which is not supported." 
      << std::endl;
    return 1;
  }

  return 0;
  
} // end GetImageProperties()


/** 
 * ***************** GetImageProperties ************************
 */

int GetImageProperties(
  const std::string & filename,
  std::string & pixeltype,
  std::string & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents,
  std::vector<unsigned int> & imagesize,
  std::vector<double> & imagespacing,
  std::vector<double> & imageoffset )
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

  /** Get the component type, number of components, dimension and pixel type. */
  dimension = testImageIOBase->GetNumberOfDimensions();
  numberofcomponents = testImageIOBase->GetNumberOfComponents();
  componenttype = testImageIOBase->GetComponentTypeAsString(
    testImageIOBase->GetComponentType() );
  ReplaceUnderscoreWithSpace( componenttype );
  pixeltype = testImageIOBase->GetPixelTypeAsString(
    testImageIOBase->GetPixelType() );

  /** Get the image size. */
  imagesize.resize( dimension );
  imagespacing.resize( dimension );
  imageoffset.resize( dimension );
  for ( unsigned int i = 0; i < dimension; i++ )
  {
    imagesize[ i ] = testImageIOBase->GetDimensions( i );
    imagespacing[ i ] = testImageIOBase->GetSpacing( i );
    imageoffset[ i ] = testImageIOBase->GetOrigin( i );
  }

  /** Check inputPixelType. */
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
    /** In this case an illegal pixeltype is found. */
    std::cerr 
      << "ERROR while determining image properties!"
      << "The found componenttype is \""
      << componenttype
      << "\", which is not supported." 
      << std::endl;
    return 1;
  }

  return 0;

}


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
    output = ranking[ Type1 ] > ranking[ Type2 ]
    ? type1 : type2;
  }

  /** Return a value. */
  return output;

} // end GetTheLargestComponentType()


#endif // end #ifndef __CommandLineArgumentHelper_h

