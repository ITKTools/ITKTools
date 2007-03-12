# This project is intended to be built outside the Insight source tree
PROJECT(computeadditionimage)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcomputeadditionimage computeadditionimage.cxx)

TARGET_LINK_LIBRARIES(pxcomputeadditionimage pxcommon ITKIO ITKCommon ITKBasicFilters)

INSTALL_TARGETS(/. pxcomputeadditionimage)

