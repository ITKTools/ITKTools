/** This little program takes a directory and searches for dicom files,
 * from which the seriesUID's are extracted.
 */

#include "itkCommandLineArgumentParser.h"
#include <iostream>
#include <itksys/SystemTools.hxx>
#include "itkGDCMSeriesFileNames.h"

/** PrintHelp. */
void PrintHelp( void )
{
  std::cout << "Usage:\npxgetDICOMseriesUIDs\n";
  std::cout << "  -in      inputDirectoryName\n";
  std::cout << "  [-r]     add restrictions to generate a unique seriesUID\n";
  std::cout << "           e.g. \"0020|0012\" to add a check for acquisition "
    << "number." << std::endl;

} // end PrintHelp()



//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Check arguments for help. */
  if ( argc < 3 )
  {
    PrintHelp();
    return 1;
  }

  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
  std::string inputDirectoryName;
  bool retin = parser->GetCommandLineArgument( "-in", inputDirectoryName );

  std::vector<std::string> restrictions;
  bool retr = parser->GetCommandLineArgument( "-r", restrictions );

  /** Make sure last character of inputDirectoryName != "/".
   * Otherwise FileIsDirectory() won't work.
   */
  if ( inputDirectoryName.rfind( "/" ) == inputDirectoryName.size() - 1 )
  {
    inputDirectoryName.erase( inputDirectoryName.size() - 1, 1 );
  }

  /** Check if inputDirectoryName is a directory. */
  bool exists = itksys::SystemTools::FileExists( inputDirectoryName.c_str() );
  bool isDir = itksys::SystemTools::FileIsDirectory( inputDirectoryName.c_str() );
  isDir &= exists;
  if ( !isDir )
  {
    std::cerr << "ERROR: " << inputDirectoryName
      << " does not exist or is no directory." << std::endl;
    return 1;
  }

  typedef itk::GDCMSeriesFileNames                GDCMNamesGeneratorType;
  typedef std::vector< std::string >              FileNamesContainerType;

  /** Get the seriesUIDs from the DICOM directory. */
  GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails( true );
  for ( unsigned int i = 0; i < restrictions.size(); ++i )
  {
    nameGenerator->AddSeriesRestriction( restrictions[ i ] );
  }
  nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );
  FileNamesContainerType seriesNames = nameGenerator->GetSeriesUIDs();

  /** Check. */
  if ( !seriesNames.size() )
  {
    std::cerr << "ERROR: no DICOM series in directory "
      << inputDirectoryName << "." << std::endl;
    return 1;
  }
  
  /** Print series. */
  for ( unsigned int i = 0; i < seriesNames.size(); i++ )
  {
    std::cout << seriesNames[ i ] << std::endl;
  }

  /** End  program. Return success. */
  return 0;

}  // end main

