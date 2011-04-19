# This project is intended to be built outside the Insight source tree
PROJECT( ComputeOverlap )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcomputeoverlap
  computeoverlap.cxx
  itkDiceOverlapImageFilter.h
  itkDiceOverlapImageFilter.txx )

TARGET_LINK_LIBRARIES( pxcomputeoverlap
  ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcomputeoverlap
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

