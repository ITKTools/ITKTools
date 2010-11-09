# This project is intended to be built outside the Insight source tree
PROJECT(segmentationdistance)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxsegmentationdistance segmentationdistance.cxx
 itkCartesianToSphericalCoordinateImageFilter.h
 itkCartesianToSphericalCoordinateImageFilter.txx
 )

TARGET_LINK_LIBRARIES(pxsegmentationdistance pxcommon ITKBasicFilters ITKIO ITKCommon ITKStatistics)

INSTALL( TARGETS pxsegmentationdistance
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

