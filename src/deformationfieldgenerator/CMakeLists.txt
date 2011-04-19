# This project is intended to be built outside the Insight source tree
PROJECT( deformationfieldgenerator )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxdeformationfieldgenerator
 deformationfieldgenerator.cxx
 itkMeshFileReaderBase.h
 itkMeshFileReaderBase.txx
 itkTransformixInputPointFileReader.h
 itkTransformixInputPointFileReader.hxx
 )

TARGET_LINK_LIBRARIES( pxdeformationfieldgenerator pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxdeformationfieldgenerator
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

