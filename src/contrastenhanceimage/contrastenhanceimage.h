#ifndef __contrastenhanceimage_h
#define __contrastenhanceimage_h

#include <iostream>
#include <string>
#include <map>
//#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"

#include "itkCommandLineArgumentParser.h"

typedef std::map<std::string, std::string> ArgMapType;

std::string GetHelpString(void)
{
  std::string helpString = "This program enhances an image. \
    alpha and beta control the exact behaviour of the filter. See the\n \
    ITK documentation of the AdaptiveHistogramEqualizationImageFilter\n \
    Usage:\n \
    pxcontrastenhanceimage\n \
    \t-in    \tInputImageFileName\t\n \
    \t-out   \tOutputImageFileName\n \
    \t-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>\n \
    \t\tCurrently only char, uchar and short are supported.\n \
    \t-id    \tImageDimension <2,3>\n \
    \t-alpha \t0.0 < alpha < 1.0\n \
    \t-beta  \t0.0 < beta < 1.0\n \
    \t-r0    \tInteger radius of window, dimension 0\n \
    \t-r1    \tInteger radius of window, dimension 1\n \
    \t[-r2]  \tInteger radius of window, dimension 2\n \
    \t[-LUT] \tUse Lookup-table <true, false>;\n \
    \t\tdefault = true; Faster, but requires more memory.";
  return helpString;
} // end PrintUsageString


//strange hack:
#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#  define CRI_STATIC_ENUM static enum
#else
#  define CRI_STATIC_ENUM enum
#endif

CRI_STATIC_ENUM enum_type { eFLOAT, eINT, eUINT, eSHORT, eUSHORT, eCHAR, eUCHAR, eUNKNOWN };

template < unsigned int NImageDimension, class TPixel>
class runwrap
{
  public:

  static int run_cri(itk::CommandLineArgumentParser::Pointer parser)
  {
    const unsigned int ImageDimension = NImageDimension;
    typedef TPixel                                PixelType;
    typedef itk::Image<PixelType, ImageDimension> ImageType;
    typedef typename ImageType::Pointer           ImagePointer;
    typedef typename ImageType::IndexType         IndexType;
    typedef typename ImageType::SizeType          SizeType;
    typedef typename ImageType::RegionType        RegionType;
    typedef typename ImageType::PointType         PointType;
    typedef itk::ImageFileReader<ImageType>       ReaderType;
    typedef itk::ImageFileWriter<ImageType>       WriterType;
    typedef typename ReaderType::Pointer          ReaderPointer;
    typedef typename WriterType::Pointer          WriterPointer;
    typedef itk::AdaptiveHistogramEqualizationImageFilter<
      ImageType>                                  EnhancerType;
    typedef typename EnhancerType::Pointer        EnhancerPointer;
    typedef typename EnhancerType::ImageSizeType  RadiusType;

    /** vars */
    WriterPointer writer = WriterType::New();
    EnhancerPointer enhancer = EnhancerType::New();
    RadiusType radius;

    std::string inputImageFileName = "";
    std::string outputImageFileName = "";

    parser->GetCommandLineArgument( "-in", inputImageFileName);
    parser->GetCommandLineArgument( "-out", outputImageFileName);

    /** Try to read input image */
    ReaderPointer reader = ReaderType::New();
    reader->SetFileName( inputImageFileName.c_str() );
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << "Error while reading input image." << std::endl;
      std::cerr << err << std::endl;
      return 2;
    }

    /** read alpha and beta from the commandline */

    float alpha = 0.0f;
    float beta = 0.0f;
    parser->GetCommandLineArgument( "-alpha", alpha);
    parser->GetCommandLineArgument( "-beta", beta);

    /** read LUT */
    bool lut = true;
    parser->GetCommandLineArgument( "-LUT", lut);

    /** read radius from the commandline. */

    for (unsigned int i=0; i< ImageDimension ; i++)
    {
      std::ostringstream key("");
      key << "-r" << i;
      unsigned int r = 0;
      parser->GetCommandLineArgument( key.str(), r);

      radius[i] = r;
    }

    /** Setup pipeline and configure its components */

    enhancer->SetUseLookupTable(lut);
    enhancer->SetAlpha(alpha);
    enhancer->SetBeta(beta);
    enhancer->SetRadius(radius);
    enhancer->SetInput( reader->GetOutput() );
    writer->SetInput( enhancer->GetOutput() );
    writer->SetFileName(outputImageFileName.c_str());

    /** do it. */
    std::cout
      << "Saving image to disk as \""
      << outputImageFileName
      << "\""
      << std::endl;
    try
    {
      writer->Update();
    }
    catch (itk::ExceptionObject & err)
    {
      std::cerr << err << std::endl;
      return 3;
    }

    return 0;

  } // end function run_cri

  /** Constructor and destructor */
  runwrap(){}
  ~runwrap(){}


}; //end class runwrap



template < unsigned int NImageDimension>
class ptswrap
{ public:

  static int PixelTypeSelector(itk::CommandLineArgumentParser::Pointer parser)
  {
    std::string pixelType("");
    parser->GetCommandLineArgument("-pt", pixelType);

    const unsigned int ImageDimension = NImageDimension;

    std::map<std::string, enum_type> typemap;
    typemap["FLOAT"] = eFLOAT;
    typemap["INT"] = eINT;
    typemap["UINT"] = eUINT;
    typemap["SHORT"] = eSHORT;
    typemap["USHORT"] = eUSHORT;
    typemap["CHAR"] = eCHAR;
    typemap["UCHAR"] = eUCHAR;

    enum_type pt = eUNKNOWN;
    if ( typemap.count(pixelType) )
    {
      pt = typemap[ pixelType ];
    }
    switch( pt )
    {
    case eSHORT :
      return  runwrap<ImageDimension, short>::run_cri(parser);
    case eCHAR :
      return  runwrap<ImageDimension, char>::run_cri(parser);
    case eUCHAR :
      return  runwrap<ImageDimension, unsigned char>::run_cri(parser);
    default :
      std::cerr << "ERROR: PixelType not supported" << std::endl;
      return 1;
    }

  }

  /** constructor and destructor */
  ptswrap(){}
  ~ptswrap(){}

}; //end class ptswrap



#endif // #ifndef __contrastenhanceimage_h

