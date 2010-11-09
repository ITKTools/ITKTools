# This project is intended to be built outside the Insight source tree
PROJECT(pca)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxpca
 pca.cxx
 itkPCAImageToImageFilter.h
 itkPCAImageToImageFilter.txx )

TARGET_LINK_LIBRARIES(pxpca pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxpca
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

