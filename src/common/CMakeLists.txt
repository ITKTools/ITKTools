# This project is intended to be built outside the Insight source tree
PROJECT( common )

ADD_SUBDIRECTORY( MevisDicomTiff )

ADD_LIBRARY( ITKTools-Common
  itkCommandLineArgumentParser.h
  itkCommandLineArgumentParser.cxx
  CommandLineArgumentHelper.h
  CommandLineArgumentHelper.hxx
  ITKToolsHelpers.h
  ITKToolsHelpers.cxx
  ITKToolsImageProperties.h
  ITKToolsImageProperties.cxx
  ITKToolsBase.h
)


TARGET_LINK_LIBRARIES( ITKTools-Common ${ITK_LIBRARIES} mevisdcmtiff )

