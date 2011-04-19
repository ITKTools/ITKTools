# This project is intended to be built outside the Insight source tree
PROJECT( texture )

# Increases address capacity
IF ( WIN32 )
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
ENDIF ( WIN32 )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxtexture
 texture.cxx
 itkTextureImageToImageFilter.h
 itkTextureImageToImageFilter.txx
 itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.h
 itkScalarImageToGrayLevelCooccurrenceMatrixGenerator.txx
 itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.h
 itkGrayLevelCooccurrenceMatrixTextureCoefficientsCalculator.txx )

TARGET_LINK_LIBRARIES( pxtexture ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxtexture
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

