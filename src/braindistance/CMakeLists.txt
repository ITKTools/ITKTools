# This project is intended to be built outside the Insight source tree
PROJECT(braindistance)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxbraindistance
  braindistance.cxx
	itkDeformationFieldBendingEnergyFilter.h
	itkDeformationFieldBendingEnergyFilter.txx
)

INSTALL_TARGETS(/. pxbraindistance)

TARGET_LINK_LIBRARIES(pxbraindistance pxcommon ITKBasicFilters ITKIO ITKCommon ITKNumerics ITKStatistics)

