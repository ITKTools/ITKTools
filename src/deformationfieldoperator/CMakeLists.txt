# This project is intended to be built outside the Insight source tree
PROJECT(deformationfieldoperator)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE(pxdeformationfieldoperator deformationfieldoperator.cxx deformationfieldoperator.h )

TARGET_LINK_LIBRARIES(pxdeformationfieldoperator pxcommon ${ITK_LIBRARIES})

INSTALL( TARGETS pxdeformationfieldoperator
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

