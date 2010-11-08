# This project is intended to be built outside the Insight source tree
PROJECT(deformationfieldgenerator)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxdeformationfieldgenerator
 deformationfieldgenerator.cxx
 itkMeshFileReaderBase.h
 itkMeshFileReaderBase.txx
 itkTransformixInputPointFileReader.h
 itkTransformixInputPointFileReader.hxx
)

TARGET_LINK_LIBRARIES(pxdeformationfieldgenerator pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxdeformationfieldgenerator
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

