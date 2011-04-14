# This project is intended to be built outside the Insight source tree
PROJECT(common)

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_LIBRARY(pxcommon
 itkCommandLineArgumentParser.cxx
 itkCommandLineArgumentParser.h
 CommandLineArgumentHelper.cxx
 CommandLineArgumentHelper.h
)

TARGET_LINK_LIBRARIES(pxcommon ${ITK_LIBRARIES})

