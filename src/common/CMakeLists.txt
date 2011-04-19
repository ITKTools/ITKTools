# This project is intended to be built outside the Insight source tree
PROJECT( common )

# Find ITK.
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_LIBRARY( ITKTools-Common
 itkCommandLineArgumentParser.cxx
 itkCommandLineArgumentParser.h
 CommandLineArgumentHelper.cxx
 CommandLineArgumentHelper.h
 )

TARGET_LINK_LIBRARIES( ITKTools-Common ${ITK_LIBRARIES} )

