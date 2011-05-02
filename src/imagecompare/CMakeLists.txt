# This project is intended to be built outside the Insight source tree
PROJECT( imagecompare )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pximagecompare ImageCompare.cxx )

TARGET_LINK_LIBRARIES( pximagecompare ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pximagecompare
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

