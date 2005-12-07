#include "computeDifferenceImageHelpers.h"

extern int ComputeScalarDifferenceImage2D( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension );
extern int ComputeScalarDifferenceImage3D( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension );
extern int ComputeScalarDifferenceImage4D( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension );

int ComputeScalarDifferenceImage( const std::string &inputPixelComponentType1,
	const std::string &inputPixelComponentType2, const std::string &outputPixelComponentType,
	const std::string &image1FileName, const std::string &image2FileName,
	const std::string &outputFileName, int inputDimension )
{
	/** Support for 2D images. */
	if ( inputDimension == 2 )
	{
		const int ret_value = ComputeScalarDifferenceImage2D(
			inputPixelComponentType1, inputPixelComponentType2,
			outputPixelComponentType,
			image1FileName, image2FileName, outputFileName, inputDimension );
		if ( ret_value != 0 )
		{
			return ret_value;
		}
	}
	else if ( inputDimension == 3 )
	{
		const int ret_value = ComputeScalarDifferenceImage3D(
			inputPixelComponentType1, inputPixelComponentType2,
			outputPixelComponentType,
			image1FileName, image2FileName, outputFileName, inputDimension );
		if ( ret_value != 0 )
		{
			return ret_value;
		}
	} // end support for 3D images
	else if ( inputDimension == 4 )
	{
		const int ret_value = ComputeScalarDifferenceImage4D(
			inputPixelComponentType1, inputPixelComponentType2,
			outputPixelComponentType,
			image1FileName, image2FileName, outputFileName, inputDimension );
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

} // end support for SCALAR pixel type
