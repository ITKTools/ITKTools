# Add the tool
ADD_ITKTOOL( distancetransform )

# Depends on some filters in the directory morphology
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/morphology )
