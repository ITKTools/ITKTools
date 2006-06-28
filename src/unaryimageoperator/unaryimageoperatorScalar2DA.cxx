#include "UnaryImageOperatorHelper.h"

/** Implementation of UnaryImageOperatorScalar2DA(). */
int UnaryImageOperatorScalar2DA( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut )
{
  enum { ImageDims = 2 };

  if ( inputDimension == ImageDims )
  {
    run(UnaryImageOperator,int,unsigned char,2);
    run(UnaryImageOperator,int,char,2);
    run(UnaryImageOperator,int,unsigned short,2);
    run(UnaryImageOperator,int,short,2);
    run(UnaryImageOperator,int,unsigned int,2);
    run(UnaryImageOperator,int,int,2);
    run(UnaryImageOperator,int,float,2);
  }
  else
  {
    std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
    std::cerr << "Only "<< ImageDims << "D images are supported." << std::endl;
    return 1;
  } // end if over inputDimension

  /** Return a value. */
  return 0;

} // end support for SCALAR pixel type
