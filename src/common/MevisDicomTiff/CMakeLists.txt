# include this file in your CMakeLists.txt file using the add_subdirectory command.
PROJECT( MevisDicomTiff )

IF( ITKTOOLS_USE_MEVISDICOMTIFF )
  ADD_DEFINITIONS( -D_ITKTOOLS_USE_MEVISDICOMTIFF )
ENDIF()

ADD_LIBRARY( mevisdcmtiff
  itkMevisDicomTiffImageIO.cxx
  itkMevisDicomTiffImageIOFactory.cxx
  itkUseMevisDicomTiff.cxx
)

TARGET_LINK_LIBRARIES( mevisdcmtiff ${ITK_LIBRARIES} )

# add mevisdcmtiff to the list of target_link_libraries in your cmakelists.txt file.
# Include the itkUseMevisDicomTiff.h file in your code.
# At the beginning of your executable, call:
# RegisterMevisDicomTiff();

