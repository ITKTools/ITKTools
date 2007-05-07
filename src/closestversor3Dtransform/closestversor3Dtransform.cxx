#include "itkCommandLineArgumentParser.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkVersorRigid3DTransform.h"

#include <iostream>
#include <fstream>


//-------------------------------------------------------------------------------------

void PrintHelp()
{
  std::cout << "Calculates the closest rigid transform (VersorRigid3D) between" << std::endl;
  std::cout << "two sets of landmarks. The two sets should be of equal size." << std::endl;
	std::cout << "Usage:" << std::endl << "pxclosestversor3Dtransform" << std::endl;
	std::cout << "  -f       the file containing the fixed landmarks" << std::endl;
  std::cout << "  -m       the file containing the moving landmarks" << std::endl;
  std::cout << "  [-c]     the center of rotation, default (0,0,0)" << std::endl;

} // end PrintHelp

//-------------------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
  /** Check arguments for help. */
	if ( argc < 5 || argc > 9 )
	{
		PrintHelp();
		return 1;
	}

  /** Create a command line argument parser. */
	itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
	parser->SetCommandLineArguments( argc, argv );

  /** Get arguments. */
	std::string fixedLandmarkFileName = "";
	bool retf = parser->GetCommandLineArgument( "-f", fixedLandmarkFileName );
  std::string movingLandmarkFileName = "";
	bool retm = parser->GetCommandLineArgument( "-m", movingLandmarkFileName );
  std::vector<double> centerOfRotation( 3, 0.0 );
	bool retc = parser->GetCommandLineArgument( "-c", centerOfRotation );

  /** Check if the required arguments are given. */
  if ( !retf )
	{
		std::cerr << "ERROR: You should specify \"-f\"." << std::endl;
		return 1;
	}

  if ( !retm )
	{
		std::cerr << "ERROR: You should specify \"-m\"." << std::endl;
		return 1;
	}

	/** Some consts. */
	const unsigned int	Dimension = 3;
	typedef short	PixelType;

	/** Typedefs. */
	typedef itk::Image< PixelType, Dimension >					ImageType;
  typedef itk::VersorRigid3DTransform< double >       TransformType;
  typedef TransformType::ParametersType               ParametersType;
  typedef itk::LandmarkBasedTransformInitializer<
    TransformType, ImageType, ImageType >             EstimatorType;
  typedef EstimatorType::LandmarkPointType            LandmarkType;
  typedef EstimatorType::LandmarkPointContainer       LandmarkContainer;

  /** Read the fixed landmark points. */
  std::ifstream fixedLandmarkFile( fixedLandmarkFileName.c_str() );
  LandmarkContainer fixedLandmarkContainer;
  if ( fixedLandmarkFile.is_open() )
  {
    LandmarkType landmark;
    while ( !fixedLandmarkFile.eof() )
    {
      for ( unsigned int i = 0; i < Dimension; i++ )
      {
        fixedLandmarkFile >> landmark[ i ];
      }
      fixedLandmarkContainer.push_back( landmark );
    }
  }
  fixedLandmarkFile.close();
  fixedLandmarkContainer.pop_back();

  /** Read the moving landmark points. */
  std::ifstream movingLandmarkFile( movingLandmarkFileName.c_str() );
  LandmarkContainer movingLandmarkContainer;
  if ( movingLandmarkFile.is_open() )
  {
    LandmarkType landmark;
    while ( !movingLandmarkFile.eof() )
    {
      for ( unsigned int i = 0; i < Dimension; i++ )
      {
        movingLandmarkFile >> landmark[ i ];
      }
      movingLandmarkContainer.push_back( landmark );
    }
  }
  movingLandmarkFile.close();
  movingLandmarkContainer.pop_back();

  /** Check the sizes. */
  if ( fixedLandmarkContainer.size() != movingLandmarkContainer.size() )
  {
    std::cerr << "ERROR: the two sets of landmarks are not of the same size." << std::endl;
    return 1;
  }

  /** Get the center of rotation. */
  LandmarkType center;
  if ( centerOfRotation.size() != Dimension )
  {
    std::cerr << "ERROR: the center of rotation consists of "
      << Dimension << " values." << std::endl;
    return 1;
  }
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    center[ i ] = centerOfRotation[ i ];
  }

  /** Create transform. */
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();
  transform->SetCenter( center );

  /** Create estimator. */
  EstimatorType::Pointer estimator = EstimatorType::New();
  estimator->SetTransform( transform );
  estimator->SetFixedLandmarks(  fixedLandmarkContainer );
  estimator->SetMovingLandmarks( movingLandmarkContainer );

  /** Run. */
  try
	{
    estimator->InitializeTransform();
  }
  catch( itk::ExceptionObject &excp )
	{
		std::cerr << "Caught ITK exception: " << excp << std::endl;
		return 1;
	}
  
  /** Print. */
  ParametersType parameters = transform->GetParameters();
  unsigned int nop = transform->GetNumberOfParameters();
  for ( unsigned int i = 0; i < nop - 1; i++ )
  {
    std::cout << parameters[ i ] << " ";
  }
  std::cout << parameters[ nop - 1 ] << std::endl;

	/** End program. */
	return 0;

} // end main

