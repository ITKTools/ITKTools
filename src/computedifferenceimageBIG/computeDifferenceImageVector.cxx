#include "computeDifferenceImageHelpers.h"

extern int ComputeVectorDifferenceImage2D( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension, int vectorDimension );
extern int ComputeVectorDifferenceImage3D( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension, int vectorDimension );
extern int ComputeVectorDifferenceImage4D( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension, int vectorDimension );

int ComputeVectorDifferenceImage( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension, int vectorDimension )
{
	/** Support for 2D images. */
	if ( inputDimension == 2 )
	{
		const int ret_value = ComputeVectorDifferenceImage2D(
			inputPixelComponentType1, inputPixelComponentType2,
			outputPixelComponentType,
			image1FileName, image2FileName, outputFileName,
			inputDimension, vectorDimension );
		if ( ret_value != 0 )
		{
			return ret_value;
		}
	}
	else if ( inputDimension == 3 )
	{
		const int ret_value = ComputeVectorDifferenceImage3D(
			inputPixelComponentType1, inputPixelComponentType2,
			outputPixelComponentType,
			image1FileName, image2FileName, outputFileName,
			inputDimension, vectorDimension );
		if ( ret_value != 0 )
		{
			return ret_value;
		}
	} // end support for 3D images
	else if ( inputDimension == 4 )
	{
		const int ret_value = ComputeVectorDifferenceImage4D(
			inputPixelComponentType1, inputPixelComponentType2,
			outputPixelComponentType,
			image1FileName, image2FileName, outputFileName,
			inputDimension, vectorDimension );
		if ( ret_value != 0 )
		{
			return ret_value;
		}
	} // end support for 4D images
	else
	{
		std::cerr << "Dimension equals " << inputDimension << ", which is not supported." << std::endl;
		std::cerr << "Only 2D, 3D, and 4D images are supported." << std::endl;
		return 1;
	} // end if over inputDimension

	/** Return a succes value. */
	return 0;

} // end support for VECTOR pixel type
