# This project is intended to be built outside the Insight source tree
PROJECT( common )

ADD_LIBRARY( ITKTools-Common
  itkCommandLineArgumentParser.cxx
  itkCommandLineArgumentParser.h
  CommandLineArgumentHelper.cxx
  CommandLineArgumentHelper.txx
  CommandLineArgumentHelper.h
  ITKToolsHelpers.h
  ITKToolsHelpers.cxx
  ITKToolsImageProperties.h
  ITKToolsImageProperties.cxx
  ITKToolsBase.h )

TARGET_LINK_LIBRARIES( ITKTools-Common ${ITK_LIBRARIES} )

