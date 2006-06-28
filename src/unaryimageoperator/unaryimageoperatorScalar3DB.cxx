#include "UnaryImageOperatorHelper.h"

/** Implementation of UnaryImageOperatorScalar3DB(). */
int UnaryImageOperatorScalar3DB( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut )
{
  enum { ImageDims = 3 };

  if ( inputDimension == ImageDims )
  {
    run(UnaryImageOperator,double,unsigned char,3);
    run(UnaryImageOperator,double,char,3);
    run(UnaryImageOperator,double,unsigned short,3);
    run(UnaryImageOperator,double,short,3);
    run(UnaryImageOperator,double,unsigned int,3);
    run(UnaryImageOperator,double,int,3);
    run(UnaryImageOperator,double,float,3);
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

