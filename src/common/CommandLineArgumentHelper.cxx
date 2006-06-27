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
	
} // end ReplaceUnderscoreWithSpace


/** 
 * ***************** GetImageProperties ************************
 */

int GetImageProperties(
  const std::string & filename,
  std::string & pixeltype,
  std::string & componenttype,
  unsigned int & dimension,
  unsigned int & numberofcomponents)
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
  ReplaceUnderscoreWithSpace(componenttype);
  pixeltype = testImageIOBase->GetPixelTypeAsString(
    testImageIOBase->GetPixelType() );

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
    /** In this case an illegal pixeltype  is found. */
    std::cerr 
      << "ERROR while determining image properties!"
      << "The found componenttype is \""
      << componenttype
      << "\", which is not supported." 
      << std::endl;
    return 1;
  }

  return 0;
  
} // end GetImageProperties


#endif // end #ifndef __CommandLineArgumentHelper_h

