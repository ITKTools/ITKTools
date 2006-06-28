#include "UnaryImageOperatorHelper.h"

/** Implementation of UnaryImageOperatorScalar2DB(). */
int UnaryImageOperatorScalar2DB( const std::string & inputFileName,
  const std::string & outputFileName, const std::string & ops,
  const std::string & argument, const unsigned int inputDimension,
  const std::string & ComponentTypeIn, const std::string & ComponentTypeOut )
{
  enum { ImageDims = 2 };

  if ( inputDimension == ImageDims )
  {
    run(UnaryImageOperator,double,unsigned char,2);
    run(UnaryImageOperator,double,char,2);
    run(UnaryImageOperator,double,unsigned short,2);
    run(UnaryImageOperator,double,short,2);
    run(UnaryImageOperator,double,unsigned int,2);
    run(UnaryImageOperator,double,int,2);
    run(UnaryImageOperator,double,float,2);
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
