# This project is intended to be built outside the Insight source tree
PROJECT(computeDifferenceImageBIG)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcomputedifferenceimageBIG computeDifferenceImage.cxx computeDifferenceImageScalar.cxx computeDifferenceImageScalar2D.cxx computeDifferenceImageScalar3D.cxx computeDifferenceImageScalar4D.cxx computeDifferenceImageVector.cxx computeDifferenceImageVector2D.cxx computeDifferenceImageVector3D.cxx computeDifferenceImageVector4D.cxx)

TARGET_LINK_LIBRARIES(pxcomputedifferenceimageBIG ITKIO ITKCommon)

INSTALL_TARGETS(/. pxcomputedifferenceimageBIG)

