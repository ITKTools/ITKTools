/** This little program takes a directory and searches for dicom files,
 * from which the seriesUID's are extracted.
 */

#include <iostream>
#include <itksys/SystemTools.hxx>
#include "itkGDCMSeriesFileNames.h"

int main( int argc, char **argv )
{
	std::string firstarg = "";
	if ( argc > 1 ) firstarg = argv[ 1 ];
	
	/** Check arguments. */
	if ( argc != 2 || firstarg == "--help" )
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "getseriesUIDs inputDirectoryName" << std::endl;
		return 1;
	}
	std::string inputDirectoryName = argv[ 1 ];

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

  /** End  program. Return succes. */
  return 0;

}  // end main

