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
  std::string inputDirectoryName = "";
  bool retin = parser->GetCommandLineArgument( "-in", inputDirectoryName );

  std::string seriesNumber = "";
  bool rets = parser->GetCommandLineArgument( "-s", seriesNumber );

  /** Check arguments. */
  if ( argc < 3 || argc > 5 )
  {
    std::cout << "Usage:" << std::endl << "pxgetdicominformation" << std::endl;
    std::cout << "\t-in\tinputDirectoryName" << std::endl;
    std::cout << "\t[-s]\tseriesUID" << std::endl;
    std::cout << "By default the first series encountered is used." << std::endl;
    return 1;
  }

  if ( !retin )
  {
    std::cerr << "ERROR: You should specify \"-in\"." << std::endl;
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
  GDCMImageIOType::Pointer gdcmIO = GDCMImageIOType::New();
  testReader->SetImageIO( gdcmIO );
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
  unsigned int sizeX = gdcmIO->GetDimensions( 0 );
  unsigned int sizeY = gdcmIO->GetDimensions( 1 );
  unsigned int sizeZ = gdcmIO->GetDimensions( 2 );
  double spacingX = gdcmIO->GetSpacing( 0 );
  double spacingY = gdcmIO->GetSpacing( 1 );
  double spacingZ = gdcmIO->GetSpacing( 2 );
  double originX = gdcmIO->GetOrigin( 0 );
  double originY = gdcmIO->GetOrigin( 1 );
  double originZ = gdcmIO->GetOrigin( 2 );
  std::string orientation = "";
  gdcmIO->GetValueFromTag( "0020|0037", orientation );

  /** Print the general image information. */
  std::cout << "General image information:" << std::endl;
  std::cout << "dimension:        " << gdcmIO->GetNumberOfDimensions() << std::endl;
  std::cout << "# components:     " << gdcmIO->GetNumberOfComponents() << std::endl;
  std::cout << "pixel type:       "
    << gdcmIO->GetPixelTypeAsString( gdcmIO->GetPixelType() )
    << ", "
    << gdcmIO->GetComponentTypeAsString( gdcmIO->GetComponentType() )
    << std::endl;
  std::cout << "size:             " << sizeX << " " << sizeY << " " << sizeZ << std::endl;
  std::cout << "spacing:          " << spacingX << " " << spacingY << " " << spacingZ << std::endl;
  std::cout << "origin:           " << originX << " " << originY << " " << originZ << std::endl;
  std::cout << "image orientation:" << orientation << std::endl;
  std::cout << "rescale intercept:" << gdcmIO->GetRescaleIntercept() << std::endl;
  std::cout << "rescale slope:    " << gdcmIO->GetRescaleSlope() << std::endl;
  std::cout << "use compression:  " << gdcmIO->GetUseCompression() << std::endl;

  /** Get patient information from the dicomIO. */
  const unsigned int maxSize = 255;
  char patientName[maxSize];
  gdcmIO->GetPatientName( patientName );
  char patientAge[maxSize];
  gdcmIO->GetPatientAge( patientAge );
  char patientSex[maxSize];
  gdcmIO->GetPatientSex( patientSex );
  char patientDOB[maxSize];
  gdcmIO->GetPatientDOB( patientDOB );
  char patientID[maxSize];
  gdcmIO->GetPatientID( patientID );
  char bodypart[maxSize];
  gdcmIO->GetBodyPart( bodypart );
  std::string position = "";
  gdcmIO->GetValueFromTag( "0018|5100", position );
  std::string viewPosition = "";
  gdcmIO->GetValueFromTag( "0018|5101", viewPosition );

  /** Print patient information. */
  std::cout << std::endl;
  std::cout << "Patient information:" << std::endl;
  std::cout << "patient name:     " << patientName << std::endl;
  std::cout << "age:              " << patientAge << std::endl;
  std::cout << "sex:              " << patientSex << std::endl;
  std::cout << "DOB:              " << patientDOB << std::endl;
  std::cout << "ID:               " << patientID << std::endl;
  std::cout << "body part:        " << bodypart << std::endl;
  std::cout << "position:         " << position << std::endl;

  /** Get study information from the dicomIO. */
  char noSeries[maxSize];
  gdcmIO->GetNumberOfSeriesInStudy( noSeries );
  char noRelatedSeries[maxSize];
  gdcmIO->GetNumberOfStudyRelatedSeries( noRelatedSeries );
  char studyDate[maxSize];
  gdcmIO->GetStudyDate( studyDate );
  char studyDesc[maxSize];
  gdcmIO->GetStudyDescription( studyDesc );
  char studyID[maxSize];
  gdcmIO->GetStudyID( studyID );
  std::string protocolvalue = "";
  gdcmIO->GetValueFromTag( "0018|1030", protocolvalue );

  /** Print study information. */
  std::cout << std::endl;
  std::cout << "Study information:" << std::endl;
  std::cout << "study UID:        " << gdcmIO->GetStudyInstanceUID() << std::endl;
  std::cout << "UID prefix:       " << gdcmIO->GetUIDPrefix() << std::endl;
  std::cout << "series UID:       " << gdcmIO->GetSeriesInstanceUID() << std::endl;
  std::cout << "# series:         " << noSeries << std::endl;
  std::cout << "# related series: " << noSeries << std::endl;
  std::cout << "date:             " << studyDate << std::endl;
  std::cout << "description:      " << studyDesc << std::endl;
  std::cout << "ID:               " << studyID << std::endl;
  std::cout << "protocol name:    " << protocolvalue << std::endl;

  /** Get scanner information from the dicomIO. */
  char modality[maxSize];
  gdcmIO->GetModality( modality );
  char manufacturer[maxSize];
  gdcmIO->GetManufacturer( manufacturer );
  char model[maxSize];
  gdcmIO->GetModel( model );
  char scanOptions[maxSize];
  gdcmIO->GetScanOptions( scanOptions );
  char institution[maxSize];
  gdcmIO->GetInstitution( institution );
 
  /** Print scanner information. */
  std::cout << std::endl;
  std::cout << "Scanner information:" << std::endl;
  std::cout << "modality:         " << modality << std::endl;
  std::cout << "manufacturer:     " << manufacturer << std::endl;
  std::cout << "model:            " << model << std::endl;
  std::cout << "scan options:     " << scanOptions << std::endl;
  std::cout << "institution:      " << institution << std::endl;

//  GetLabelFromTag(const std::string &tag, std::string &labelId)
  //  GetValueFromTag(const std::string &tag, std::string &value)

  /** End  program. Return success. */
  return 0;

}  // end main

