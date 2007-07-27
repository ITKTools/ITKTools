#ifndef __histogramequalizeimage_h
#define __histogramequalizeimage_h

#include <iostream>
#include <string>
#include <map>
#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramEqualizationImageFilter.h"

typedef std::map<std::string, std::string> ArgMapType;

void PrintUsageString(void)
{
	std::cerr 
		<< "\nThis program applies histogram equalization to an image.\n"
		<< "Works as described by Maintz, Introduction to Image Processing.\n"
		<< "Usage:\n"
		<< "pxhistogramequalizeimage\n"
		<< "\t-in    \tInputImageFileName\t\n"
		<< "\t-out   \tOutputImageFileName\n"
		<< "\t-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>\n"
		<< "\t\tCurrently only char, uchar, short, and ushort are supported.\n"
		<< "\t-id    \tImageDimension <2,3>\n"
		<< std::endl;
} // end PrintUsageString


int ReadArgument(const ArgMapType & argmap, const std::string & key, std::string & value, bool optional)
{

	if ( argmap.count(key) )
	{
		value = argmap.find(key)->second;
		return 0;
	}
	else
	{
		if (!optional)
		{
			std::cerr << "Not enough arguments\n";
			std::cerr << "Missing argument: " << key << std::endl;
			PrintUsageString();
			return 1;
		}
		else
		{
			return 0;
		}
	}

} // end ReadArgument


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

	static int run_cri(const ArgMapType & argmap )
	{
		const unsigned int ImageDimension = NImageDimension;
		typedef TPixel																PixelType;
		typedef itk::Image<PixelType, ImageDimension> ImageType;
		typedef typename ImageType::Pointer						ImagePointer;
		typedef typename ImageType::IndexType					IndexType;
		typedef typename ImageType::SizeType					SizeType;
		typedef typename ImageType::RegionType				RegionType;
		typedef typename ImageType::PointType					PointType;
		typedef itk::ImageFileReader<ImageType>				ReaderType;
		typedef itk::ImageFileWriter<ImageType>				WriterType;
		typedef typename ReaderType::Pointer					ReaderPointer;
		typedef typename WriterType::Pointer					WriterPointer;
		typedef itk::HistogramEqualizationImageFilter<
			ImageType>																	EnhancerType;
		typedef typename EnhancerType::Pointer				EnhancerPointer;
				
		/** vars */
		std::string inputImageFileName("");
		std::string outputImageFileName("");
		int returndummy = 0;
		WriterPointer writer = WriterType::New();
		EnhancerPointer enhancer = EnhancerType::New();
		
		/** Read filenames */
		returndummy |= ReadArgument(argmap, "-in", inputImageFileName, false);
		returndummy |= ReadArgument(argmap, "-out", outputImageFileName, false);
		if ( returndummy !=0 )
		{
			return returndummy;
		}

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
		
		/** Setup pipeline and configure its components */

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

	static int PixelTypeSelector(const ArgMapType & argmap )
	{
		const unsigned int ImageDimension = NImageDimension; 
		std::string pixelType("");
		int	returndummy = ReadArgument(argmap, "-pt", pixelType, false);
		if ( returndummy !=0 )
		{
			return returndummy;
		}
		
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
			return  runwrap<ImageDimension, short>::run_cri(argmap);
    case eUSHORT : 
			return  runwrap<ImageDimension, unsigned short>::run_cri(argmap);
		case eCHAR : 
			return  runwrap<ImageDimension, char>::run_cri(argmap);
		case eUCHAR : 
			return  runwrap<ImageDimension, unsigned char>::run_cri(argmap);
		default :
			std::cerr << "ERROR: PixelType not supported" << std::endl;
			return 1;
		}
		
	}

	/** constructor and destructor */
	ptswrap(){}
	~ptswrap(){}

}; //end class ptswrap



#endif // #ifndef __histogramequalizeimage_h

