# This project is intended to be built outside the Insight source tree
PROJECT( createsimplebox )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxcreatesimplebox
  createsimplebox.cxx
  createsimplebox.h
  itkSimpleBoxSpatialFunction.txx
  itkSimpleBoxSpatialFunction.h )

TARGET_LINK_LIBRARIES( pxcreatesimplebox ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS pxcreatesimplebox
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

