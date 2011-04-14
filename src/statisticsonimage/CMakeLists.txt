# This project is intended to be built outside the Insight source tree
PROJECT(statisticsonimage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxstatisticsonimage
  statisticsonimage.cxx
  statisticsonimage.h
  itkStatisticsImageFilterWithMask.h
  itkStatisticsImageFilterWithMask.txx
  itkScalarImageToHistogramGenerator2.h
  itkScalarImageToHistogramGenerator2.txx
  statisticsprinters.h)

TARGET_LINK_LIBRARIES(pxstatisticsonimage pxcommon ${ITK_LIBRARIES})

INSTALL( TARGETS pxstatisticsonimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

