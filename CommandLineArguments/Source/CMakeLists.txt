CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

INCLUDE( ${CMAKE_SOURCE_DIR}/IJMacros.txt )

#Change PROJECT_NAME to the name of your project
PROJECT( IJ-CommandLineArgumentParser )

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
SET( Required_Packages ITK )

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH( Package ${Required_Packages} )
  LOADPACKAGE( ${Package} )
ENDFOREACH()

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET( Libraries ${ITK_LIBRARIES} )

SET( CommandLineArgumentParserCode
  itkCommandLineArgumentParser.h
  itkCommandLineArgumentParser.cxx
)

# Build an executable
SET( CurrentExe "TestCommandLineArgumentParser" )
ADD_EXECUTABLE( ${CurrentExe}
  TestCommandLineArgumentParser.cxx ${CommandLineArgumentParserCode} )
TARGET_LINK_LIBRARIES( ${CurrentExe} ${Libraries} )

# Testing
ADD_TEST( TestCommandLine1 TestCommandLineArgumentParser -in in1.mhd in2.mhd -z -out -pA  4 7 9 0 -pB 2 )

