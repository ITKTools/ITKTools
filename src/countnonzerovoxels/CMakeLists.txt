# This project is intended to be built outside the Insight source tree
PROJECT(CountNonZeroVoxels)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcountnonzerovoxels countnonzerovoxels.cxx)

TARGET_LINK_LIBRARIES(pxcountnonzerovoxels ITKIO ITKCommon pxcommon )

INSTALL( TARGETS pxcountnonzerovoxels
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

