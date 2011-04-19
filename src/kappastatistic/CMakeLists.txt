# This project is intended to be built outside the Insight source tree
PROJECT( kappastatistic )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxkappastatistic
  kappastatistic.cxx
  KappaStatisticMainHelper.cxx
  itkKappaStatisticBase.cxx
  itkFleissKappaStatistic.cxx
  itkCohenWeightedKappaStatistic.cxx )

TARGET_LINK_LIBRARIES( pxkappastatistic pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxkappastatistic
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

