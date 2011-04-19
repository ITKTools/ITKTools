# This project is intended to be built outside the Insight source tree
PROJECT( reshape )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxreshape
 reshape.cxx
 itkReshapeImageToImageFilter.h
 itkReshapeImageToImageFilter.txx )

TARGET_LINK_LIBRARIES( pxreshape ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxreshape
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

