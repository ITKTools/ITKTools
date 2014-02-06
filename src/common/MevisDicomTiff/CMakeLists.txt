# include this file in your CMakeLists.txt file using the add_subdirectory command.
PROJECT( MevisDicomTiff )

IF( ITKTOOLS_USE_MEVISDICOMTIFF )
  ADD_DEFINITIONS( -D_ITKTOOLS_USE_MEVISDICOMTIFF )
  
  ADD_LIBRARY( mevisdcmtiff
    itkMevisDicomTiffImageIO.cxx
    itkMevisDicomTiffImageIOFactory.cxx
    itkUseMevisDicomTiff.cxx
  )
ELSE()
  # avoid the dependencies on the tiff library.
  ADD_LIBRARY( mevisdcmtiff
    itkUseMevisDicomTiff.cxx
  )
ENDIF()

TARGET_LINK_LIBRARIES( mevisdcmtiff ${ITK_LIBRARIES} )

# add mevisdcmtiff to the list of target_link_libraries in your cmakelists.txt file.
# Include the itkUseMevisDicomTiff.h file in your code.
# At the beginning of your executable, call:
# RegisterMevisDicomTiff();

