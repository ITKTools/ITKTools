
PROJECT(scripts)

# install all files in this directory except the CMakeLists.txt
FILE(GLOB allscripts *)
FOREACH(script ${allscripts} )
  GET_FILENAME_COMPONENT(scriptname ${script} NAME)
  STRING(COMPARE NOTEQUAL ${scriptname} "CMakeLists.txt" testvar)
  IF( ${testvar} )
    INSTALL_PROGRAMS(/. ${scriptname} )
  ENDIF( ${testvar} )
ENDFOREACH(script)



