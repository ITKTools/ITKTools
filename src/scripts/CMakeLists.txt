
PROJECT( scripts )

# install all files in this directory except the CMakeLists.txt
FILE( GLOB allscripts * )
FOREACH( script ${allscripts} )
  GET_FILENAME_COMPONENT( scriptname ${script} NAME )
  STRING( COMPARE NOTEQUAL ${scriptname} "CMakeLists.txt" testvar1 )
  STRING( COMPARE NOTEQUAL ${scriptname} "CVS" testvar2 )
  STRING( COMPARE NOTEQUAL ${scriptname} ".svn" testvar3 )
  IF( ${testvar1} AND ${testvar2} AND ${testvar3} )
    INSTALL( PROGRAMS ${scriptname}
      DESTINATION ${ITKTOOLS_INSTALL_DIR} )
  ENDIF()
ENDFOREACH()

