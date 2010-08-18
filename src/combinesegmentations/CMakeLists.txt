# This project is intended to be built outside the Insight source tree
PROJECT(combinesegmentations)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

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

INSTALL_TARGETS(/. pxcombinesegmentations)

TARGET_LINK_LIBRARIES(pxcombinesegmentations pxcommon ITKBasicFilters ITKIO ITKCommon ITKNumerics)

