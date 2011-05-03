CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

# This project is intended to be built outside the Insight source tree
PROJECT( ITKTools )
ENABLE_TESTING()

# Define where to install ITKTools
IF( WIN32 )
  SET( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
ELSE()
  SET( ITKTools_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
ENDIF()

# Include all subdirs
file( GLOB listOfTools RELATIVE ${CMAKE_SOURCE_DIR} "*/CMakeLists.txt" )
foreach( tool ${listOfTools} )
  get_filename_component( path_to_tool ${tool} PATH )
  subdirs( ${path_to_tool} )
  #add_subdirectory( ${path_to_tool} )
endforeach()

#---------------------------------------------------------------------
# Kill the anoying MS VS warning about non-safe functions.
# They hide real warnings.
IF( MSVC )
  ADD_DEFINITIONS(
    /D_SCL_SECURE_NO_DEPRECATE
    /D_CRT_SECURE_NO_DEPRECATE
    /D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
    )
ENDIF( MSVC )

#---------------------------------------------------------------------
# Output directories.

SET( LIBRARY_OUTPUT_PATH ${ITKTools_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all libraries." )
SET( EXECUTABLE_OUTPUT_PATH ${ITKTools_BINARY_DIR}/bin
  CACHE INTERNAL "Single output directory for building all executables." )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )
SET( ITKTools_LIBRARY_PATH    "${LIBRARY_OUTPUT_PATH}" )
SET( ITKTools_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}" )
INCLUDE_DIRECTORIES( ${ITKTools_SOURCE_DIR}/common )

#---------------------------------------------------------------------
# Tests
# Every exectuable has at least one test. Each tests is composed of two add_test commands.
# The first is [name]Output. This runs the executable with the specified parameters to produce an output.
# The second is [name]Test. This runs the imagecompare program to compaer the baseline for the given test
# to the output of [name]Output.
# Additionally, a _[something] after the name means this is a specific type of test for this program.
# E.g. BinaryImageOperatorOutput_Add means that this test tests the ADD functionality of BinaryImageOperator.

set(BaselineDir ${ITKTools_SOURCE_DIR}/Testing/Baselines/)
set(DataDir ${ITKTools_SOURCE_DIR}/Testing/Data/)
#########
# This tests the magnitude image. Also should test the average vector magnitude (not an image)
# add_test(NAME AverageVectorMagnitudeOutput COMMAND averagevectormagnitude -in ${DataDir}/BlackSquare.png -out )
# add_test(NAME AverageVectorMagnitudeTest COMMAND imagecompare DEPENDS AverageVectorMagnitudeOutput)

#########
# Add images
add_test(NAME BinaryImageOperatorOutput_Add COMMAND binaryimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops ADDITION -out binaryimageoperator_add.png)
add_test(NAME BinaryImageOperatorTest_Add COMMAND imagecompare -base ${BaselineDir}/BinaryImageOperator_Add.png -test binaryimageoperator_add.png DEPENDS BinaryImageOperatorOutput_Add)

#########
# add_test(NAME BinaryThinningOutput COMMAND binarythinning )
# add_test(NAME BinaryThinningTest COMMAND imagecompare DEPENDS BinaryThinningOutput)
# 
# add_test(NAME BrainDistanceOutput COMMAND braindistance )
# add_test(NAME BrainDistanceTest COMMAND imagecompare DEPENDS BrainDistanceOutput)

add_test(NAME CastConvertOutput COMMAND castconvert -in ${DataDir}/WhiteSquare.png -out castconvert.mhd)
add_test(NAME CastConvertTest COMMAND imagecompare -base ${BaselineDir}/CastConvert.mhd -test castconvert.mhd DEPENDS CastConvertOutput)
# 
# add_test(NAME ClosestVersor3DTransformOutput COMMAND closestversor3Dtransform )
# add_test(NAME ClosestVersor3DTransformTest COMMAND imagecompare DEPENDS ClosestVersor3DTransformOutput)
# 
# add_test(NAME CombineSegmentationsOutput COMMAND combinesegmentations )
# add_test(NAME CombineSegmentationsTest COMMAND imagecompare DEPENDS CombineSegmentationsOutput)
# 
# add_test(NAME ComputeBoundingBoxOutput COMMAND computeboundingbox )
# add_test(NAME ComputeBoundingBoxTest COMMAND imagecompare DEPENDS ComputeBoundingBoxOutput)
# 
# add_test(NAME ComputeDifferenceImageBIGOutput COMMAND computedifferenceimageBIG )
# add_test(NAME ComputeDifferenceImageBIGTest COMMAND imagecompare DEPENDS ComputeDifferenceImageBIGOutput)
# 
# add_test(NAME ComputeMeanOutput COMMAND computemean )
# add_test(NAME ComputeMeanTest COMMAND imagecompare DEPENDS ComputeMeanOutput)
# 
# add_test(NAME ComputeOverlapOutput COMMAND computeoverlap )
# add_test(NAME ComputeOverlapTest COMMAND imagecompare DEPENDS ComputeOverlapOutput)
#  
# add_test(NAME ContrastEnhanceImageOutput COMMAND contrastenhanceimage )
# add_test(NAME ContrastEnhanceImageTest COMMAND imagecompare DEPENDS ContrastEnhanceImageOutput)
#  
# add_test(NAME CountNonZeroVoxelsOutput COMMAND countnonzerovoxels )
# add_test(NAME CountNonZeroVoxelsTest COMMAND imagecompare DEPENDS CountNonZeroVoxelsOutput)
#  
# add_test(NAME CreateBoxOutput COMMAND createbox )
# add_test(NAME CreateBoxTest COMMAND imagecompare DEPENDS CreateBoxOutput)
#  
# add_test(NAME CreateCylinderOutput COMMAND createcylinder )
# add_test(NAME CreateCylinderTest COMMAND imagecompare DEPENDS CreateCylinderOutput)
#  
# add_test(NAME CreateEllipsoidOutput COMMAND createellipsoid )
# add_test(NAME CreateEllipsoidTest COMMAND imagecompare DEPENDS CreateEllipsoidOutput)
#  
# add_test(NAME CreateGridImageOutput COMMAND creategridimage )
# add_test(NAME CreateGridImageTest COMMAND imagecompare DEPENDS CreateGridImageOutput)
#  
# add_test(NAME CreatePointsInImageOutput COMMAND createpointsinimage )
# add_test(NAME CreatePointsInImageTest COMMAND imagecompare DEPENDS CreatePointsInImageOutput)
# 
# add_test(NAME CreateRandomImageOutput COMMAND createrandomimage )
# add_test(NAME CreateRandomImageTest COMMAND imagecompare DEPENDS CreateRandomImageOutput)
# 
# add_test(NAME CreateSimpleBoxOutput COMMAND createsimplebox )
# add_test(NAME CreateSimpleBoxTest COMMAND imagecompare DEPENDS CreateSimpleBoxOutput)
# 
# add_test(NAME CreateSphereOutput COMMAND createsphere )
# add_test(NAME CreateSphereTest COMMAND imagecompare DEPENDS CreateSphereOutput)
# 
# add_test(NAME CreateZeroImageOutput COMMAND createzeroimage )
# add_test(NAME CreateZeroImageTest COMMAND imagecompare DEPENDS CreateZeroImageOutput)
# 
# add_test(NAME CropImageOutput COMMAND cropimage )
# add_test(NAME CropImageTest COMMAND imagecompare DEPENDS CropImageOutput)
# 
# add_test(NAME DeformationFieldGeneratorOutput COMMAND deformationfieldgenerator )
# add_test(NAME DeformationFieldGeneratorTest COMMAND imagecompare DEPENDS DeformationFieldGeneratorOutput)
#  
# add_test(NAME DetectGoldMarkersOutput COMMAND detectgoldmarkers )
# add_test(NAME DetectGoldMarkersTest COMMAND imagecompare DEPENDS DetectGoldMarkersOutput)
#  
# add_test(NAME DistanceTransformOutput COMMAND distancetransform )
# add_test(NAME DistanceTransformTest COMMAND imagecompare DEPENDS DistanceTransformOutput)
#  
# add_test(NAME ExtractEveryOtherSliceOutput COMMAND extracteveryotherslice )
# add_test(NAME ExtractEveryOtherSliceTest COMMAND imagecompare DEPENDS ExtractEveryOtherSliceOutput)
#  
# add_test(NAME ExtractIndexFromVectorImageOutput COMMAND extractindexfromvectorimage )
# add_test(NAME ExtractIndexFromVectorImageTest COMMAND imagecompare DEPENDS ExtractIndexFromVectorImageOutput)
#  
# add_test(NAME ExtractSliceOutput COMMAND extractslice )
# add_test(NAME ExtractSliceTest COMMAND imagecompare DEPENDS ExtractSliceOutput)
# 
# add_test(NAME FFTImageOutput COMMAND fftimage )
# add_test(NAME FFTImageTest COMMAND imagecompare DEPENDS FFTImageOutput)
# 
# add_test(NAME GaussianImageFilterOutput COMMAND gaussianimagefilter )
# add_test(NAME GaussianImageFilterTest COMMAND imagecompare DEPENDS GaussianImageFilterOutput)
# 
# add_test(NAME GetDICOMInformationOutput COMMAND getdicominformation )
# add_test(NAME GetDICOMInformationTest COMMAND imagecompare DEPENDS GetDICOMInformationOutput)
# 
# add_test(NAME GetDICOMSeriesUIDsOutput COMMAND getDICOMseriesUIDs )
# add_test(NAME GetDICOMSeriesUIDsTest COMMAND imagecompare DEPENDS GetDICOMSeriesUIDsOutput)
# 
# add_test(NAME GetImageInformationOutput COMMAND getimageinformation )
# add_test(NAME GetImageInformationTest COMMAND imagecompare DEPENDS GetImageInformationOutput)
# 
# add_test(NAME GetPointsInImageOutput COMMAND getpointsinimage )
# add_test(NAME GetPointsInImageTest COMMAND imagecompare DEPENDS GetPointsInImageOutput)
# 
# add_test(NAME GIPLConvertOutput COMMAND giplconvert )
# add_test(NAME GIPLConvertTest COMMAND imagecompare DEPENDS GIPLConvertOutput)
#  
# add_test(NAME HistogramEqualizeImageOutput COMMAND histogramequalizeimage )
# add_test(NAME HistogramEqualizeImageTest COMMAND imagecompare DEPENDS HistogramEqualizeImageOutput)
# 
# add_test(NAME ImageCompareOutput COMMAND imagecompare )
# add_test(NAME ImageCompareTest COMMAND imagecompare DEPENDS ImageCompareOutput)
#  
add_test(NAME ImagesToVectorImageOutput COMMAND imagestovectorimage -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -out imagestovectorimage.mhd)
add_test(NAME ImagesToVectorImageTest COMMAND imagecompare -base ${BaselineDir}/ImagesToVectorImage.mhd -test imagestovectorimage.mhd DEPENDS ImagesToVectorImageOutput)
#  
# add_test(NAME IntensityReplaceOutput COMMAND intensityreplace )
# add_test(NAME IntensityReplaceTest COMMAND imagecompare DEPENDS IntensityReplaceOutput)
#  
# add_test(NAME IntensityWindowingOutput COMMAND intensitywindowing )
# add_test(NAME IntensityWindowingTest COMMAND imagecompare DEPENDS IntensityWindowingOutput)
#  
# add_test(NAME InvertIntensityImageFilterOutput COMMAND invertintensityimagefilter )
# add_test(NAME InvertIntensityImageFilterTest COMMAND imagecompare DEPENDS InvertIntensityImageFilterOutput)
#  
# add_test(NAME KappaStatisticOutput COMMAND kappastatistic )
# add_test(NAME KappaStatisticTest COMMAND imagecompare DEPENDS KappaStatisticOutput)
#  
#########
add_test(NAME LogicalImageOperatorOutput_AND COMMAND logicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops AND -out logicalimageoperator_and.png)
add_test(NAME LogicalImageOperatorTest_AND COMMAND imagecompare -base ${BaselineDir}/LogicalImageOperator_And.png -test logicalimageoperator_and.png DEPENDS LogicalImageOperatorOutput_AND)

add_test(NAME LogicalImageOperatorOutput_OR COMMAND logicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops OR -out logicalimageoperator_or.png)
add_test(NAME LogicalImageOperatorTest_OR COMMAND imagecompare -base ${BaselineDir}/LogicalImageOperator_Or.png -test logicalimageoperator_or.png DEPENDS LogicalImageOperatorOutput_OR)

#########
# add_test(NAME MorphologyOutput COMMAND morphology )
# add_test(NAME MorphologyTest COMMAND imagecompare DEPENDS MorphologyOutput)
#  
# add_test(NAME NaryImageOperatorOutput COMMAND naryimageoperator )
# add_test(NAME NaryImageOperatorTest COMMAND imagecompare DEPENDS NaryImageOperatorOutput)
#  
# add_test(NAME PCAOutput COMMAND pca )
# add_test(NAME PCATest COMMAND imagecompare DEPENDS PCAOutput)
#  
# add_test(NAME ReflectOutput COMMAND reflect )
# add_test(NAME ReflectTest COMMAND imagecompare DEPENDS ReflectOutput)
#  
# add_test(NAME ReplaceVoxelOutput COMMAND replacevoxel )
# add_test(NAME ReplaceVoxelTest COMMAND imagecompare DEPENDS ReplaceVoxelOutput)
#  
# add_test(NAME RescaleIntensityImageFilterOutput COMMAND rescaleintensityimagefilter )
# add_test(NAME RescaleIntensityImageFilterTest COMMAND imagecompare DEPENDS RescaleIntensityImageFilterOutput)
# 
# add_test(NAME ReshapeOutput COMMAND reshape )
# add_test(NAME ReshapeTest COMMAND imagecompare DEPENDS ReshapeOutput)
#  
# add_test(NAME ResizeImageOutput COMMAND resizeimage )
# add_test(NAME ResizeImageTest COMMAND imagecompare DEPENDS ResizeImageOutput)
#  
# add_test(NAME SegmentationDistanceOutput COMMAND segmentationdistance )
# add_test(NAME SegmentationDistanceTest COMMAND imagecompare DEPENDS SegmentationDistanceOutput)
#  
# add_test(NAME StatisticsOnImageOutput COMMAND statisticsonimage )
# add_test(NAME StatisticsOnImageTest COMMAND imagecompare DEPENDS StatisticsOnImageOutput)
#  
# add_test(NAME TextureOutput COMMAND texture )
# add_test(NAME TextureTest COMMAND imagecompare DEPENDS TextureOutput)
#  
# add_test(NAME TileImagesOutput COMMAND tileimages )
# add_test(NAME TileImagesTest COMMAND imagecompare DEPENDS TileImagesOutput)
#  
# add_test(NAME TTestOutput COMMAND ttest )
# add_test(NAME TTestTest COMMAND imagecompare DEPENDS TTestOutput)
#  
# add_test(NAME ThresholdImageOutput COMMAND thresholdimage )
# add_test(NAME ThresholdImageTest COMMAND imagecompare DEPENDS ThresholdImageOutput)
#  
# add_test(NAME UnaryImageOperatorOutput COMMAND unaryimageoperator )
# add_test(NAME UnaryImageOperatorTest COMMAND imagecompare DEPENDS UnaryImageOperatorOutput)
#  
# add_test(NAME WeightedAdditionOutput COMMAND weightedaddition )
# add_test(NAME WeightedAdditionTest COMMAND imagecompare DEPENDS WeightedAdditionOutput)
