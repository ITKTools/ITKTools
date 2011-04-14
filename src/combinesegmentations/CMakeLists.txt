# This project is intended to be built outside the Insight source tree
PROJECT(combinesegmentations)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxcombinesegmentations
 combinesegmentations.cxx
 itkLabelVoting2ImageFilter.h
 itkLabelVoting2ImageFilter.txx
 itkMultiLabelSTAPLEImageFilter.h
 itkMultiLabelSTAPLEImageFilter.txx
 itkMultiLabelSTAPLE2ImageFilter.h
 itkMultiLabelSTAPLE2ImageFilter.txx
 itkNaryUnequalityTestImageFilter.h
)

TARGET_LINK_LIBRARIES(pxcombinesegmentations pxcommon ${ITK_LIBRARIES})

INSTALL( TARGETS pxcombinesegmentations
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

