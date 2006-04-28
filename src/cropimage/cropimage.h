#ifndef __cropimage_h
#define __cropimage_h

#include <iostream>
#include <string>
#include <map>
#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"

typedef std::map<std::string, std::string> ArgMapType;

void PrintUsageString(void)
{
	std::cerr 
		<< "\nThis program crops an image.\n"
		<< "The region of interest is defined by point A and B.\n"
		<< "In the output image point A and B are the corners.\n"
		<< "Usage:\n"
		<< "pxcropimage\n"
		<< "\t-in    \tInputImageFileName\t\n"
		<< "\t-out   \tOutputImageFileName\n"
		<< "\t-pt    \tPixelType <FLOAT, SHORT, USHORT, INT, UINT, CHAR, UCHAR>\n"
		<< "\t\tCurrently only char, uchar and short are supported.\n"
		<< "\t-id    \tImageDimension <2,3>\n"
		<< "\t-pA0  \tIndex 0 of pointA\n"
		<< "\t-pA1  \tIndex 1 of pointA\n"
		<< "\t[-pA2]\tIndex 2 of pointA\n"
		<< "\t-pB0  \tIndex 0 of pointB\n"
		<< "\t-pB1  \tIndex 1 of pointB\n"
		<< "\t[-pB2]\tIndex 2 of pointB\n" 
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
		typedef itk::RegionOfInterestImageFilter<
			ImageType, ImageType>												CropFilterType;
		typedef typename CropFilterType::Pointer			CropFilterPointer;
		

		/** vars */
		std::string inputImageFileName("");
		std::string outputImageFileName("");
		int returndummy = 0;
		SizeType outputSize;
		RegionType outputRegion;
		IndexType outputIndex;
		WriterPointer writer = WriterType::New();
		CropFilterPointer cropper = CropFilterType::New();
		IndexType indexA;
		IndexType indexB;
		
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
		
						
		/** read point A and B from the commandline,
		 * and compute the output index and size.
		 */
		
		for (unsigned int i=0; i< ImageDimension ; i++)
		{
			std::ostringstream makeStringA("");
			std::ostringstream makeStringB("");
			makeStringA << "-pA" << i;	
			makeStringB << "-pB" << i;	
			std::string tempstringA("");
			std::string tempstringB("");
			returndummy |= ReadArgument(argmap, makeStringA.str(), tempstringA, false);
			returndummy |= ReadArgument(argmap, makeStringB.str(), tempstringB, false);
			if (returndummy ==0)
			{
				indexA[i] = atoi( tempstringA.c_str() );
				indexB[i] = atoi( tempstringB.c_str() );
				outputIndex[i] = vnl_math_min(indexA[i], indexB[i]);
				outputSize[i] = vnl_math_max(indexA[i], indexB[i]) - outputIndex[i] +1;
			}
		}
		if ( returndummy !=0 )
		{
			return returndummy;
		}
		
		/** Setup pipeline and configure its components */
		
		outputRegion.SetSize(outputSize);
		outputRegion.SetIndex(outputIndex);
		cropper->SetRegionOfInterest(outputRegion);
		cropper->SetInput( reader->GetOutput() );
		writer->SetInput( cropper->GetOutput() );
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



#endif // #ifndef __cropimage_h

