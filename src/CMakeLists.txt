# This project is intended to be built outside the Insight source tree
PROJECT(praxix)

SUBDIRS(
 averagevectormagnitude
 castconvert
 common
 computedifferenceimage
 computedifferenceimageBIG
 computedistances
 computeoverlap
 contrastenhanceimage
 countnonzerovoxels
 createbox
 creategridimage
 createpointsinimage
 createrandomimage
 createsphere
 cropimage
 deformationfieldoperator
 detectgoldmarkers
 dilateimage
 erodeimage
 extracteveryotherslice
 extractindexfromvectorimage
 extractslice
 getDICOMseriesUIDs
 getpointsinimage
 histogramequalizeimage
 imagecompare
 intensitywindowing
 invertintensityimagefilter
 logicalimageoperator
 maximumimagefilter
 minimumimagefilter
 morphologicalclosingimage
 morphologicalopeningimage
 multiplyimages
 rescaleintensityimagefilter
 resizeimage
 scripts
 statisticsonimage
 tileimages
 thresholdimage
 unaryimageoperator)


#---------------------------------------------------------------------
# Output directories.

SET (LIBRARY_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(praxix_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(praxix_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")
INCLUDE_DIRECTORIES(${praxix_SOURCE_DIR}/common)


