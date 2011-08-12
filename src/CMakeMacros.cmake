# Include this file from the main ITKTools CMakeLists.txt.
# It is also included by UseITKTools.cmake, so that external programs 
# can also use this macro outside the itktools directory.

#---------------------------------------------------------------------
# Macro to simplify the addition of an ITKTool
macro( ADD_ITKTOOL name )

  # Define the project
  # Only define project name when it wasn't done yet.
  if ( NOT ( "${CMAKE_CURRENT_BINARY_DIR}" STREQUAL
             "${PROJECT_BINARY_DIR}" ) )
    project( name )
  endif()

  # Get the list of files in this directory
  file( GLOB filelist RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * )
  list( REMOVE_ITEM filelist "CMakeLists.txt" )

  # Create the executable
  add_executable( px${name} ${filelist} )

  # Link
  target_link_libraries( px${name} ${ITKTOOLS_LIBRARIES} ${ITK_LIBRARIES} )

  # Install
  install( TARGETS px${name}
    RUNTIME DESTINATION ${ITKTOOLS_INSTALL_DIR} )
endmacro()
