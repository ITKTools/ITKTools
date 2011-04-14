# This project is intended to be built outside the Insight source tree
PROJECT(histogramequalizeimage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxhistogramequalizeimage histogramequalizeimage.cxx histogramequalizeimage.h itkHistogramEqualizationImageFilter.h itkHistogramEqualizationImageFilter.hxx)

TARGET_LINK_LIBRARIES(pxhistogramequalizeimage ${ITK_LIBRARIES})

INSTALL( TARGETS pxhistogramequalizeimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

