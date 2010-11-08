# This project is intended to be built outside the Insight source tree
PROJECT(unaryimageoperator)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(pxunaryimageoperator
 unaryimageoperator.cxx
 unaryimageoperatorScalar.cxx
 unaryimageoperatorScalar2DA.cxx
 unaryimageoperatorScalar2DB.cxx
 unaryimageoperatorScalar3DA.cxx
 unaryimageoperatorScalar3DB.cxx
 itkUnaryFunctors.h
 UnaryImageOperatorHelper.h
 UnaryImageOperatorMainHelper.h)

TARGET_LINK_LIBRARIES(pxunaryimageoperator pxcommon ITKBasicFilters ITKIO ITKCommon)

INSTALL( TARGETS pxunaryimageoperator
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

