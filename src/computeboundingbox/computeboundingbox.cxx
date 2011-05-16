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
 \brief Compute the bounding box of an image.
 
 \verbinclude computeboundingbox.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "vnl/vnl_math.h"

//-------------------------------------------------------------------------------------


/**
  * ******************* GetHelpString *******************
  */
std::string GetHelpString()
{
  std::stringstream ss;
  ss << "This program computes the bounding box of an image." << std::endl
     << "Every pixel > 0 is considered to be within the bounding box." << std::endl
     << "Returns the minimum and maximum indices/points that lie within the bounding box." << std::endl
     << "Usage:" << std::endl
     << "pxcomputeboundingbox" << std::endl
     << "-in      inputFilename" << std::endl
     << "[-dim]   dimension, default 3" << std::endl
     << "[-pt]    pixelType, default short" << std::endl
     << "Supported: 2D, 3D, short. Images with PixelType other than short are automatically converted.";

  return ss.str();
} // end GetHelpString


/** ComputeBoundingBox */

class ComputeBoundingBoxBase : public itktools::ITKToolsBase
{ 
public:
  ComputeBoundingBoxBase()
  {
    m_InputFileName = "";
    m_OutputFileName = "";
  }
  ~ComputeBoundingBoxBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
    
}; // end ComputeBoundingBoxBase


template< class TComponentType, unsigned int VImageDimension >
class ComputeBoundingBox : public ComputeBoundingBoxBase
{
public:
  typedef ComputeBoundingBox Self;

  ComputeBoundingBox(){};
  ~ComputeBoundingBox(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VImageDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Typedefs. */
    typedef itk::Image<TComponentType, VImageDimension> InputImageType;
    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageRegionConstIteratorWithIndex<
      InputImageType>                                   IteratorType;
    typedef typename InputImageType::PixelType          PixelType;
    typedef typename InputImageType::IndexType          IndexType;
    typedef typename InputImageType::PointType          PointType;
    const unsigned int dimension = InputImageType::GetImageDimension();

    /** Declarations */
    typename ReaderType::Pointer reader = ReaderType::New();
    typename InputImageType::Pointer image;
    IndexType minIndex;
    IndexType maxIndex;

    /** Read input image */
    reader->SetFileName( m_InputFileName.c_str() );
    reader->Update();
    image = reader->GetOutput();

    /** Define iterator on input image */
    IteratorType iterator( image, image->GetLargestPossibleRegion() );

    /** Initialize the two corner points */
    iterator.GoToReverseBegin();
    minIndex = iterator.GetIndex();
    iterator.GoToBegin();
    maxIndex = iterator.GetIndex();
    PixelType zero = itk::NumericTraits< PixelType>::Zero;

    while ( ! iterator.IsAtEnd() )
    {
      if ( iterator.Get() > zero )
      {
	const IndexType & index = iterator.GetIndex();
	for ( unsigned int i = 0; i < dimension; ++i)
	{
	  minIndex[i] = vnl_math_min( index[i], minIndex[i] );
	  maxIndex[i] = vnl_math_max( index[i], maxIndex[i] );
	}
      }
      ++iterator;
    }

    PointType minPoint;
    PointType maxPoint;
    image->TransformIndexToPhysicalPoint(minIndex, minPoint);
    image->TransformIndexToPhysicalPoint(maxIndex, maxPoint);

    std::cout << "MinimumIndex = " << minIndex << "\n"
	      << "MaximumIndex = " << maxIndex << std::endl;
    std::cout << std::showpoint;
    std::cout << "MinimumPoint = " << minPoint << "\n"
	      << "MaximumPoint = " << maxPoint << std::endl;
  }

}; // end ComputeBoundingBox

//-------------------------------------------------------------------------------------

/** Declare other functions. */
std::string GetHelpString( void );


//-------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if(validateArguments == itk::CommandLineArgumentParser::FAILED)
  {
    return EXIT_FAILURE;
  }
  else if(validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED)
  {
    return EXIT_SUCCESS;
  }

  /** Get arguments. */
  std::string inputFileName = "";
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Determine image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 2;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );
  if ( retgip !=0 )
  {
    return 1;
  }
  std::cout << "The input image has the following properties:" << std::endl;
  /** Do not bother the user with the difference between pixeltype and componenttype:*/
  //std::cout << "\tPixelType:          " << PixelType << std::endl;
  std::cout << "\tPixelType:          " << ComponentType << std::endl;
  std::cout << "\tDimension:          " << Dimension << std::endl;
  std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;

  /** Let the user overrule this */
  bool retdim = parser->GetCommandLineArgument( "-dim", Dimension );
  bool retpt = parser->GetCommandLineArgument( "-pt", ComponentType );
  if (retdim | retpt)
  {
    std::cout << "The user has overruled this by specifying -pt and/or -dim:" << std::endl;
    std::cout << "\tPixelType:          " << ComponentType << std::endl;
    std::cout << "\tDimension:          " << Dimension << std::endl;
    std::cout << "\tNumberOfComponents: " << NumberOfComponents << std::endl;
  }

  if (NumberOfComponents > 1)
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get rid of the possible "_" in ComponentType. */
  itktools::ReplaceUnderscoreWithSpace( ComponentType );

  /** Overrule it, since only short will do something */
  if ( ComponentType != "short" )
  {
    /** Try short anyway, but warn user */
    ComponentType = "short";
    std::cout << "WARNING: the image will be converted to short!" << std::endl;
  }
  
  
  /** Class that does the work */
  ComputeBoundingBoxBase * computeBoundingBox = 0; 

  /** Short alias */
  unsigned int dim = Dimension;
 
  /** \todo some progs allow user to override the pixel type, 
   * so we need a method to convert string to EnumComponentType */
  itktools::ComponentType componentType = itktools::GetImageComponentType(inputFileName);
  
  std::cout << "Detected component type: " << 
    componentType << std::endl;

  try
  {    
    // now call all possible template combinations.
    if (!computeBoundingBox) computeBoundingBox = ComputeBoundingBox< short, 2 >::New( componentType, dim );
    
#ifdef ITKTOOLS_3D_SUPPORT
    if (!computeBoundingBox) computeBoundingBox = ComputeBoundingBox< short, 3 >::New( componentType, dim );    
#endif
    if (!computeBoundingBox) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << componentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    computeBoundingBox->m_InputFileName = inputFileName;

    computeBoundingBox->Run();
    
    delete computeBoundingBox;  
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete computeBoundingBox;
    return 1;
  }

  /** End program. */
  return 0;

} // end main
