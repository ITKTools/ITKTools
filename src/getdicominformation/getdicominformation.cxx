/** This little program takes a directory and searches for dicom files,
 * from which patient and study information is extracted.
 */

#include "itkCommandLineArgumentParser.h"

#include <itksys/SystemTools.hxx>

#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

	/** Get arguments. */
	std::string	inputDirectoryName = "";
	bool retin = parser->GetCommandLineArgument( "-in", inputDirectoryName );

  std::string	seriesNumber = "";
	bool rets = parser->GetCommandLineArgument( "-s", seriesNumber );

  /** Check arguments. */
	if ( !retin )
	{
		std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
		return 1;
	}
	
	if ( argc < 3 || argc > 5 )
	{
    std::cout << "Usage:" << std::endl << "pxgetdicominformation" << std::endl;
    std::cout << "\t-in\tinputDirectoryName" << std::endl;
    std::cout << "\t[-s]\tseriesUID" << std::endl;
    std::cout << "By default the first series encountered is used." << std::endl;
		return 1;
	}

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

  /** Some typedefs. */
  typedef itk::Image< short, 3>               ImageType;
  typedef itk::ImageSeriesReader< ImageType > SeriesReaderType;
  typedef itk::GDCMImageIO                    GDCMImageIOType;
	typedef itk::GDCMSeriesFileNames            GDCMNamesGeneratorType;
  typedef std::vector< std::string >          FileNamesContainerType;

	/** Get the seriesUIDs from the DICOM directory.
   * With SetUseSeriesDetails( true ) series UIDs are generated that
   * are unique and therefore extra long.
   */
	GDCMNamesGeneratorType::Pointer nameGenerator = GDCMNamesGeneratorType::New();
	nameGenerator->SetUseSeriesDetails( true );
	nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );

  /** Generate the file names corresponding to the series. */
  FileNamesContainerType fileNames;
  if ( seriesNumber == "" )
  {
    fileNames = nameGenerator->GetInputFileNames();
  }
  else
  {
    fileNames = nameGenerator->GetFileNames( seriesNumber );
  }

 	/** Check if there is at least one dicom file in the directory. */
	if ( !fileNames.size() )
	{
		std::cerr << "ERROR: no DICOM series in directory "
			<< inputDirectoryName << "." << std::endl;
		return 1;
	}
  
  /** Create a test reader. */
  SeriesReaderType::Pointer testReader = SeriesReaderType::New();
  GDCMImageIOType::Pointer dicomIO = GDCMImageIOType::New();
  testReader->SetImageIO( dicomIO );
  testReader->SetFileNames( fileNames );
  
  /** Try reading image information. */
  try
	{
		testReader->GenerateOutputInformation();
	}
	catch( itk::ExceptionObject  &  err  )
  {
    std::cerr  << "ExceptionObject caught !"  << std::endl;
    std::cerr  << err <<  std::endl;
    return 1;
  }
  
  /** Get general image information from the dicomIO. */
  unsigned int inputDimension = dicomIO->GetNumberOfDimensions();
  unsigned int numberOfComponents = dicomIO->GetNumberOfComponents();
  std::string inputPixelComponentType = dicomIO->GetComponentTypeAsString(
    dicomIO->GetComponentType() );
  std::string pixelType = dicomIO->GetPixelTypeAsString(
    dicomIO->GetPixelType() );
  unsigned int sizeX = dicomIO->GetDimensions( 0 );
  unsigned int sizeY = dicomIO->GetDimensions( 1 );
  unsigned int sizeZ = dicomIO->GetDimensions( 2 );
  double spacingX = dicomIO->GetSpacing( 0 );
  double spacingY = dicomIO->GetSpacing( 1 );
  double spacingZ = dicomIO->GetSpacing( 2 );
  double originX = dicomIO->GetOrigin( 0 );
  double originY = dicomIO->GetOrigin( 1 );
  double originZ = dicomIO->GetOrigin( 2 );

  /** Print the general image information. */
  std::cout << "General image information:" << std::endl;
  std::cout << "dimension:        " << inputDimension << std::endl;
  std::cout << "# components:     " << numberOfComponents << std::endl;
  std::cout << "pixel type:       " << pixelType << ", " << inputPixelComponentType << std::endl;
  std::cout << "size:             " << sizeX << " " << sizeY << " " << sizeZ << std::endl;
  std::cout << "spacing:          " << spacingX << " " << spacingY << " " << spacingZ << std::endl;
  std::cout << "origin:           " << originX << " " << originY << " " << originZ << std::endl;

  /** Get patient information from the dicomIO. */
  const unsigned int maxSize = 255;
  char patientName[maxSize];
  dicomIO->GetPatientName( patientName );
  char patientAge[maxSize];
  dicomIO->GetPatientAge( patientAge );
  char patientSex[maxSize];
  dicomIO->GetPatientSex( patientSex );
  char patientDOB[maxSize];
  dicomIO->GetPatientDOB( patientDOB );
  char patientID[maxSize];
  dicomIO->GetPatientID( patientID );
  char bodypart[maxSize];
  dicomIO->GetBodyPart( bodypart );

  /** Get study information from the dicomIO. */
  char noSeries[maxSize];
  dicomIO->GetNumberOfSeriesInStudy( noSeries );
  char noRelatedSeries[maxSize];
  dicomIO->GetNumberOfStudyRelatedSeries( noRelatedSeries );
  char studyDate[maxSize];
  dicomIO->GetStudyDate( studyDate );
  char studyDesc[maxSize];
  dicomIO->GetStudyDescription( studyDesc );
  char studyID[maxSize];
  dicomIO->GetStudyID( studyID );

  /** Get scanner information from the dicomIO. */
  char modality[maxSize];
  dicomIO->GetModality( modality );
  char manufacturer[maxSize];
  dicomIO->GetManufacturer( manufacturer );
  char model[maxSize];
  dicomIO->GetModel( model );
  char scanOptions[maxSize];
  dicomIO->GetScanOptions( scanOptions );
  char institution[maxSize];
  dicomIO->GetInstitution( institution );

  /** Print patient and study information. */
  std::cout << std::endl;
  std::cout << "Patient information:" << std::endl;
  std::cout << "name:             " << patientName << std::endl;
  std::cout << "age:              " << patientAge << std::endl;
  std::cout << "sex:              " << patientSex << std::endl;
  std::cout << "DOB:              " << patientDOB << std::endl;
  std::cout << "ID:               " << patientID << std::endl;
  std::cout << "body part:        " << bodypart << std::endl;

  /** Print study information. */
  std::cout << std::endl;
  std::cout << "Study information:" << std::endl;
  std::cout << "# series:         " << noSeries << std::endl;
  std::cout << "# related series: " << noSeries << std::endl;
  std::cout << "date:             " << studyDate << std::endl;
  std::cout << "description:      " << studyDesc << std::endl;
  std::cout << "ID:               " << studyID << std::endl;

  /** Print scanner information. */
  std::cout << std::endl;
  std::cout << "Scanner information:" << std::endl;
  std::cout << "modality:         " << modality << std::endl;
  std::cout << "manufacturer:     " << manufacturer << std::endl;
  std::cout << "model:            " << model << std::endl;
  std::cout << "scan options:     " << scanOptions << std::endl;
  std::cout << "institution:      " << institution << std::endl;

  /** End  program. Return succes. */
  return 0;

}  // end main

