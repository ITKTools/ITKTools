# This project is intended to be built outside the Insight source tree
PROJECT(createrandomimage)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

# Make sure the MS VC compiler has enough memory

IF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
 IF(CMAKE_CXX_FLAGS MATCHES "/Zm(0|1|2|3|4|5|6|7|8|9)*[^ ]")
   STRING(REGEX REPLACE "/Zm(0|1|2|3|4|5|6|7|8|9)*[^ ]" "/Zm1600 " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
 ELSE(CMAKE_CXX_FLAGS MATCHES "/Zm(0|1|2|3|4|5|6|7|8|9)*[^ ]")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm1600 ")
 ENDIF(CMAKE_CXX_FLAGS MATCHES "/Zm(0|1|2|3|4|5|6|7|8|9)*[^ ]")
ENDIF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")


ADD_EXECUTABLE(pxcreaterandomimage createrandomimage.cxx createrandomimage.h)

TARGET_LINK_LIBRARIES(pxcreaterandomimage ${ITK_LIBRARIES})

INSTALL( TARGETS pxcreaterandomimage
  RUNTIME DESTINATION ${PRAXIX_INSTALL_DIR} )

