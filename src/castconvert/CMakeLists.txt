# This project is intended to be built outside the Insight source tree
PROJECT(castconvert)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(castconvert castconvert.cxx  castconvertDicomScalar.cxx castconvertScalar.cxx  castconvertScalar2D.cxx castconvertScalar3D.cxx castconvertScalar4D.cxx)
ADD_EXECUTABLE(ImageCompare ImageCompare.cxx)

TARGET_LINK_LIBRARIES(castconvert  ITKIO ITKCommon)
TARGET_LINK_LIBRARIES(ImageCompare ITKIO ITKCommon)


# We add some tests
ENABLE_TESTING()

# Set the tool used for comparing images
SET(IMAGE_COMPARE_TOOL ${EXECUTABLE_OUTPUT_PATH}/ImageCompare)


# Set the path to the images
SET(IMAGE_PATH ${castconvert_SOURCE_DIR}/Data)
SET(IMAGE_PATH_OUTPUTS ${castconvert_BINARY_DIR})

# Convert from grayscale mhd to other image file formats
ADD_TEST(ccvnt_to_png                   ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.png)
ADD_TEST(ccvnt_to_jpg_unsigned_int      ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.jpg unsigned_int)
ADD_TEST(ccvnt_to_tiff                  ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.tiff)
ADD_TEST(ccvnt_to_analyze75             ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.img)
ADD_TEST(ccvnt_to_bmp_unsigned_char     ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.bmp unsigned_char)
ADD_TEST(ccvnt_to_nrrd_float            ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.nrrd float)
ADD_TEST(ccvnt_to_vtk_long              ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.vtk long)
ADD_TEST(ccvnt_to_gipl                  ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.gipl)
#only 3D currently ADD_TEST(ccvnt_to_nifti_test                 ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_out.nii.gz)

# Convert back to mhd
ADD_TEST(ccvnt_from_png                 ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.png    ${IMAGE_PATH_OUTPUTS}/image_in1.mhd)
ADD_TEST(ccvnt_from_jpg_unsigned_int    ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.jpg    ${IMAGE_PATH_OUTPUTS}/image_in2.mhd unsigned_short)
ADD_TEST(ccvnt_from_tiff                ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.tiff   ${IMAGE_PATH_OUTPUTS}/image_in3.mhd)
ADD_TEST(ccvnt_from_analyze75           ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.hdr    ${IMAGE_PATH_OUTPUTS}/image_in4.mhd)
ADD_TEST(ccvnt_from_bmp_unsigned_char   ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.bmp    ${IMAGE_PATH_OUTPUTS}/image_in5.mhd unsigned_short)
ADD_TEST(ccvnt_from_nrrd_float          ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.nrrd   ${IMAGE_PATH_OUTPUTS}/image_in6.mhd unsigned_short)
ADD_TEST(ccvnt_from_vtk_long            ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.vtk    ${IMAGE_PATH_OUTPUTS}/image_in7.mhd unsigned_short)
ADD_TEST(ccvnt_from_gipl                ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.gipl   ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
#only 3D currently ADD_TEST(ccvnt_from_nifti               ${EXECUTABLE_OUTPUT_PATH}/castconvert ${IMAGE_PATH_OUTPUTS}/image_out.nii.gz ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)

# Compare with original png
ADD_TEST(compare_png              ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in1.mhd)
ADD_TEST(compare_jpg_unsigned_int ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in2.mhd)
ADD_TEST(compare_tiff             ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in3.mhd)
ADD_TEST(compare_analyze75        ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in4.mhd)
ADD_TEST(compare_bmp_unsigned_char ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in5.mhd)
ADD_TEST(compare_nrrd_float       ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in6.mhd)
ADD_TEST(compare_vtk_long         ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in7.mhd)
ADD_TEST(compare_gipl             ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)
#only 3D currently ADD_TEST(ccvnt_compare_nifti            ${IMAGE_COMPARE_TOOL} ${IMAGE_PATH}/image_in.mhd ${IMAGE_PATH_OUTPUTS}/image_in8.mhd)

INSTALL_TARGETS(/. castconvert)
INSTALL_TARGETS(/. ImageCompare)
