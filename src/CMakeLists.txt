# This project is intended to be built outside the Insight source tree
PROJECT(praxix)

SUBDIRS(
 addtopixelimagefilter
 andimagefilter
 andnotimagefilter
 averagevectormagnitude
 castconvert
 common
 computedifferenceimage
 computedifferenceimageBIG
 computedistances
 computeoverlap
 contrastenhanceimage
 convert
 countnonzerovoxels
 createbox
 creategridimage
 createpointsinimage
 createrandomimage
 createsphere
 cropimage
 detectgoldmarkers
 dilateimage
 erodeimage
 extracteveryotherslice
 extractindexfromvectorimage
 extractslice
 getpointsinimage
 histogramequalizeimage
 imagecompare
 invertintensityimagefilter
 maximumimagefilter
 minimumimagefilter
 multiplypixelimagefilter
 rescaleintensityimagefilter
 resizeimage
 scripts
 statisticsonimage
 tileimages)


#---------------------------------------------------------------------
# Output directories.

SET (LIBRARY_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(praxix_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(praxix_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")
INCLUDE_DIRECTORIES(${praxix_SOURCE_DIR}/common)


