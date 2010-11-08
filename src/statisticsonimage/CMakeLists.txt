# This project is intended to be built outside the Insight source tree
PROJECT(statisticsonimage)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxstatisticsonimage
  statisticsonimage.cxx
  statisticsonimage.h
  itkStatisticsImageFilterWithMask.h
  itkStatisticsImageFilterWithMask.txx
  itkScalarImageToHistogramGenerator2.h
  itkScalarImageToHistogramGenerator2.txx
  statisticsprinters.h)

TARGET_LINK_LIBRARIES(pxstatisticsonimage pxcommon ITKIO ITKBasicFilters ITKCommon ITKStatistics)

INSTALL( TARGETS pxstatisticsonimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

