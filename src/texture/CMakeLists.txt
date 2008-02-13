# This project is intended to be built outside the Insight source tree
PROJECT(texture)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxtexture
 texture.cxx
 itkTextureImageToImageFilter.h
 itkTextureImageToImageFilter.txx
 itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.h
 itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.txx
 itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.h
 itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.txx )

INSTALL_TARGETS(/. pxtexture)

TARGET_LINK_LIBRARIES(pxtexture pxcommon ITKBasicFilters ITKIO ITKCommon ITKStatistics)

