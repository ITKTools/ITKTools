# This project is intended to be built outside the Insight source tree
PROJECT(kappastatistic)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxkappastatistic kappastatistic.cxx KappaStatisticMainHelper.cxx itkKappaStatisticBase.cxx itkFleissKappaStatistic.cxx itkCohenWeightedKappaStatistic.cxx)

TARGET_LINK_LIBRARIES(pxkappastatistic pxcommon ITKBasicFilters ITKCommon)

INSTALL_TARGETS(/. pxkappastatistic)

