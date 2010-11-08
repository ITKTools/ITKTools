# This project is intended to be built outside the Insight source tree
PROJECT(castconvert)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxcastconvert
 castconvert.cxx
 castconvertScalar.cxx
 castconvertDicomScalarA.cxx
 castconvertDicomScalarB.cxx
 castconvertScalar2DA.cxx
 castconvertScalar2DB.cxx
 castconvertScalar3DA.cxx
 castconvertScalar3DB.cxx
 castconvertScalar4DA.cxx
 castconvertScalar4DB.cxx
 castconvertMultiComponent.cxx
 castconvertMultiComponent2D2CA.cxx
 castconvertMultiComponent2D2CB.cxx
 castconvertMultiComponent2D3CA.cxx
 castconvertMultiComponent2D3CB.cxx
 castconvertMultiComponent2D4CA.cxx
 castconvertMultiComponent2D4CB.cxx
 castconvertMultiComponent3D2CA.cxx
 castconvertMultiComponent3D2CB.cxx
 castconvertMultiComponent3D3CA.cxx
 castconvertMultiComponent3D3CB.cxx
 castconvertMultiComponent3D4CA.cxx
 castconvertMultiComponent3D4CB.cxx
 castconvertMultiComponent4D2CA.cxx
 castconvertMultiComponent4D2CB.cxx
 castconvertMultiComponent4D3CA.cxx
 castconvertMultiComponent4D3CB.cxx
 castconvertMultiComponent4D4CA.cxx
 castconvertMultiComponent4D4CB.cxx
)

TARGET_LINK_LIBRARIES(pxcastconvert pxcommon ITKIO ITKCommon ITKBasicFilters ITKIOReview)


INSTALL( TARGETS pxcastconvert
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

