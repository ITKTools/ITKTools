# This project is intended to be built outside the Insight source tree
PROJECT( castconvert )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcastconvert
  castconvert.cxx
  castconvert2D.cxx
  castconvert3D.cxx
  castconvertDICOM.cxx
  castconverthelpers.h
  castconverthelpers2.h
)

TARGET_LINK_LIBRARIES( pxcastconvert ITKTools-Common ${ITK_LIBRARIES} )


INSTALL( TARGETS pxcastconvert
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

