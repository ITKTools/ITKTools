# This project is intended to be built outside the Insight source tree
PROJECT( braindistance )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( pxbraindistance
  braindistance.cxx
	itkDeformationFieldBendingEnergyFilter.h
	itkDeformationFieldBendingEnergyFilter.txx
 )

TARGET_LINK_LIBRARIES( pxbraindistance pxcommon ${ITK_LIBRARIES} )

INSTALL( TARGETS pxbraindistance
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

