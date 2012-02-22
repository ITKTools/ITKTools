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
 \brief Takes a directory and searches for dicom files, from which patient and study information is extracted.
 
 \verbinclude getdicominformation.help
 */

#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"

#include <itksys/SystemTools.hxx>
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "ITKTools v" << itktools::GetITKToolsVersion() << "\n"
    << "Usage:\n"
    << "pxgetdicominformation\n"
    << "  -in      inputDirectoryName\n"
    << "  [-s]     seriesUID\n"
    << "  [-r]     add restrictions to generate a unique seriesUID\n"
    << "           e.g. \"0020|0012\" to add a check for acquisition number.\n"
    << "By default the first series encountered is used.";

  return ss.str();

} // end GetHelpString()

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::string inputDirectoryName = "";
  parser->GetCommandLineArgument( "-in", inputDirectoryName );

  std::string seriesNumber = "";
  parser->GetCommandLineArgument( "-s", seriesNumber );

  std::vector<std::string> restrictions;
  parser->GetCommandLineArgument( "-r", restrictions );

  /** Make sure last character of inputDirectoryName != "/".
   * Otherwise FileIsDirectory() won't work.
   */
  if( inputDirectoryName.rfind( "/" ) == inputDirectoryName.size() - 1 )
  {
    inputDirectoryName.erase( inputDirectoryName.size() - 1, 1 );
  }

  /** Check if inputDirectoryName is a directory. */
  bool exists = itksys::SystemTools::FileExists( inputDirectoryName.c_str() );
  bool isDir = itksys::SystemTools::FileIsDirectory( inputDirectoryName.c_str() );
  isDir &= exists;
  if( !isDir )
  {
    std::cerr << "ERROR: " << inputDirectoryName
      << " does not exist or is no directory." << std::endl;
    return EXIT_FAILURE;
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
  for( unsigned int i = 0; i < restrictions.size(); ++i )
  {
    nameGenerator->AddSeriesRestriction( restrictions[ i ] );
  }
  nameGenerator->SetInputDirectory( inputDirectoryName.c_str() );

  /** Generate the file names corresponding to the series. */
  FileNamesContainerType fileNames;
  if( seriesNumber == "" )
  {
    fileNames = nameGenerator->GetInputFileNames();
  }
  else
  {
    fileNames = nameGenerator->GetFileNames( seriesNumber );
  }

  /** Check if there is at least one dicom file in the directory. */
  if( !fileNames.size() )
  {
    std::cerr << "ERROR: no DICOM series in directory "
      << inputDirectoryName << "." << std::endl;
    return EXIT_FAILURE;
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
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "ExceptionObject caught !"  << std::endl;
    std::cerr << excp <<  std::endl;
    return EXIT_FAILURE;
  }

  /** Get general image information from the dicomIO. */
  unsigned int sizeX = gdcmIO->GetDimensions( 0 );
  unsigned int sizeY = gdcmIO->GetDimensions( 1 );
  unsigned int sizeZ = fileNames.size();//gdcmIO->GetDimensions( 2 );
  double spacingX = gdcmIO->GetSpacing( 0 );
  double spacingY = gdcmIO->GetSpacing( 1 );
  double spacingZ = gdcmIO->GetSpacing( 2 );
  double originX = gdcmIO->GetOrigin( 0 );
  double originY = gdcmIO->GetOrigin( 1 );
  double originZ = gdcmIO->GetOrigin( 2 );
  std::string orientation = "";
  gdcmIO->GetValueFromTag( "0020|0037", orientation );

  /** Print the general image information. */
  std::cout << "General image information:\n";
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
  std::cout << "\nPatient information:\n";
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
  std::string studyDate = "";
  gdcmIO->GetValueFromTag( "0008|0020", studyDate );
  std::string studyTime = "";
  gdcmIO->GetValueFromTag( "0008|0030", studyTime );
  char studyDesc[maxSize];
  gdcmIO->GetStudyDescription( studyDesc );
  char studyID[maxSize];
  gdcmIO->GetStudyID( studyID );
  std::string protocolName = "";
  gdcmIO->GetValueFromTag( "0018|1030", protocolName );

  /** Print study information. */
  std::cout << "\nStudy information:\n";
  std::cout << "study UID:        " << gdcmIO->GetStudyInstanceUID() << std::endl;
  std::cout << "UID prefix:       " << gdcmIO->GetUIDPrefix() << std::endl;
  std::cout << "study date:       " << studyDate << std::endl;
  std::cout << "study time:       " << studyTime << std::endl;
  std::cout << "description:      " << studyDesc << std::endl;
  std::cout << "ID:               " << studyID << std::endl;
  std::cout << "protocol name:    " << protocolName << std::endl;

  /** Get series information. */
  std::string seriesDate = "";
  gdcmIO->GetValueFromTag( "0008|0021", seriesDate );
  std::string seriesTime = "";
  gdcmIO->GetValueFromTag( "0008|0031", seriesTime );

  /** Print series information. */
  std::cout << "\nSeries information:\n";
  std::cout << "series UID:       " << gdcmIO->GetSeriesInstanceUID() << std::endl;
  std::cout << "# series:         " << noSeries << std::endl;
  std::cout << "# related series: " << noRelatedSeries << std::endl;
  std::cout << "series date:      " << seriesDate << std::endl;
  std::cout << "series time:      " << seriesTime << std::endl;

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
  std::string convolutionKernel = "";
  gdcmIO->GetValueFromTag( "0018|1210", convolutionKernel );

  /** Print scanner information. */
  std::cout << "\nScanner information:\n";
  std::cout << "institution:      " << institution << std::endl;
  std::cout << "modality:         " << modality << std::endl;
  std::cout << "manufacturer:     " << manufacturer << std::endl;
  std::cout << "model:            " << model << std::endl;
  std::cout << "scan options:     " << scanOptions << std::endl;
  std::cout << "conv. kernel:     " << convolutionKernel << std::endl;

  /** Get acquisition information from the dicomIO. */
  std::string acquisitionDate = "";
  gdcmIO->GetValueFromTag( "0008|0022", acquisitionDate );
  std::string acquisitionTime = "";
  gdcmIO->GetValueFromTag( "0008|0032", acquisitionTime );
  std::string kvp = "";
  gdcmIO->GetValueFromTag( "0018|0060", kvp );
  std::string exposureTime = "";
  gdcmIO->GetValueFromTag( "0018|1150", exposureTime );
  std::string xRayTubeCurrent = "";
  gdcmIO->GetValueFromTag( "0018|1151", xRayTubeCurrent );
  std::string exposure = "";
  gdcmIO->GetValueFromTag( "0018|1152", exposure );

  /** Print acquisition information. */
  std::cout << "\nAcquisition information:\n";
  std::cout << "acquisition date: " << acquisitionDate << std::endl;
  std::cout << "acquisition time: " << acquisitionTime << std::endl;
  std::cout << "KVP:              " << kvp << std::endl;
  std::cout << "exposure time:    " << exposureTime << std::endl;
  std::cout << "XRayTubeCurrent:  " << xRayTubeCurrent << std::endl;
  std::cout << "exposure:         " << exposure << std::endl;

//  GetLabelFromTag(const std::string &tag, std::string &labelId)
  //  GetValueFromTag(const std::string &tag, std::string &value)

  /** End  program. Return success. */
  return EXIT_SUCCESS;

}  // end main
