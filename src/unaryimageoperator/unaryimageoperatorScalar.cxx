#include <string>
#include <iostream>

/** Make us aware of external functions. */
extern int UnaryImageOperatorScalar2DA( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut );
extern int UnaryImageOperatorScalar2DB( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut );
extern int UnaryImageOperatorScalar3DA( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut );
extern int UnaryImageOperatorScalar3DB( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut );

/** Implementation of UnaryImageOperatorScalar(). */
int UnaryImageOperatorScalar( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut )
{
  /** Support for 2D images. */
  if ( inputDimension == 2 )
  {
    const int ret_value = UnaryImageOperatorScalar2DA(
      inputFileName, outputFileName, ops, argument,
      inputDimension, ComponentTypeIn, ComponentTypeOut )
      ||                  UnaryImageOperatorScalar2DB(
      inputFileName, outputFileName, ops, argument,
      inputDimension, ComponentTypeIn, ComponentTypeOut );
    if ( ret_value != 0 ) return ret_value;
  }
  else if ( inputDimension == 3 )
  {
    const int ret_value = UnaryImageOperatorScalar3DA(
      inputFileName, outputFileName, ops, argument,
      inputDimension, ComponentTypeIn, ComponentTypeOut )
      ||                  UnaryImageOperatorScalar3DB(
      inputFileName, outputFileName, ops, argument,
      inputDimension, ComponentTypeIn, ComponentTypeOut );
    if ( ret_value != 0 ) return ret_value;
  } // end support for 3D images
  else
  {
    std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
    std::cerr << "Only 2D and 3D images are supported." << std::endl;
    return 1;
  } // end if over inputDimension

  /** Return a succes value. */
  return 0;

} // end support for SCALAR pixel type
