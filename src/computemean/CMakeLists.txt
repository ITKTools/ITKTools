# This project is intended to be built outside the Insight source tree
PROJECT(computemean)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcomputemean computemean.cxx)

TARGET_LINK_LIBRARIES(pxcomputemean pxcommon)

INSTALL( TARGETS pxcomputemean
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

