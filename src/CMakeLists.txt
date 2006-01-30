# This project is intended to be built outside the Insight source tree
PROJECT(praxix)

SUBDIRS(andimagefilter andnotimagefilter castconvert computedifferenceimage computedifferenceimageBIG computedistances computeoverlap convert countnonzerovoxels createpointsinimage createsphere dilateimage erodeimage extractindexfromvectorimage extractslice imagecompare resize scripts)

#---------------------------------------------------------------------
# Output directories.

SET (LIBRARY_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${praxix_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(praxix_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(praxix_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")


