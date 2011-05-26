Project( ITKToolsTesting )

#---------------------------------------------------------------------
# Tests
# Every exectuable has at least one test. Each tests is composed of two add_test commands.
# The first is [name]Output. This runs the executable with the specified parameters to produce an output.
# The second is [name]Test. This runs the imagecompare program to compaer the baseline for the given test
# to the output of [name]Output.
# Additionally, a _[something] after the name means this is a specific type of test for this program.
# E.g. BinaryImageOperatorOutput_Add means that this test tests the ADD functionality of BinaryImageOperator.

set(BaselineDir ${ITKTools_SOURCE_DIR}/../Testing/Baselines/)
set(DataDir ${ITKTools_SOURCE_DIR}/../Testing/Data/)
set(ExeDir ${EXECUTABLE_OUTPUT_PATH})
set(OutDir ${ITKTools_BINARY_DIR}/Testing/)

#########
# This tests the magnitude image. Also should test the average vector magnitude (not an image)
# add_test(NAME AverageVectorMagnitudeOutput COMMAND averagevectormagnitude -in ${DataDir}/BlackSquare.png -out )
# add_test(NAME AverageVectorMagnitudeTest COMMAND imagecompare DEPENDS AverageVectorMagnitudeOutput)

#########
# Add images
add_test(NAME BinaryImageOperatorOutput_Add COMMAND ${ExeDir}/pxbinaryimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops ADDITION -out ${OutDir}/binaryimageoperator_add.png)
add_test(NAME BinaryImageOperatorTest_Add COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/BinaryImageOperator_Add.png -test ${OutDir}/binaryimageoperator_add.png DEPENDS BinaryImageOperatorOutput_Add)

#########
# add_test(NAME BinaryThinningOutput COMMAND ${ExeDir}/pxbinarythinning )
# add_test(NAME BinaryThinningTest COMMAND imagecompare DEPENDS BinaryThinningOutput)
# 
# add_test(NAME BrainDistanceOutput COMMAND ${ExeDir}/pxbraindistance )
# add_test(NAME BrainDistanceTest COMMAND imagecompare DEPENDS BrainDistanceOutput)

add_test(NAME CastConvertOutput COMMAND ${ExeDir}/pxcastconvert -in ${DataDir}/WhiteSquare.png -out ${OutDir}/castconvert.mhd)
add_test(NAME CastConvertTest COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/CastConvert.mhd -test ${OutDir}/castconvert.mhd DEPENDS CastConvertOutput)
# 
# add_test(NAME ClosestVersor3DTransformOutput COMMAND ${ExeDir}/pxclosestversor3Dtransform )
# add_test(NAME ClosestVersor3DTransformTest COMMAND ${ExeDir}/pximagecompare DEPENDS ClosestVersor3DTransformOutput)
# 
# add_test(NAME CombineSegmentationsOutput COMMAND ${ExeDir}/pxcombinesegmentations )
# add_test(NAME CombineSegmentationsTest COMMAND ${ExeDir}/pximagecompare DEPENDS CombineSegmentationsOutput)
# 
# add_test(NAME ComputeBoundingBoxOutput COMMAND ${ExeDir}/pxcomputeboundingbox )
# add_test(NAME ComputeBoundingBoxTest COMMAND ${ExeDir}/pximagecompare DEPENDS ComputeBoundingBoxOutput)
# 
# add_test(NAME ComputeDifferenceImageBIGOutput COMMAND ${ExeDir}/pxcomputedifferenceimageBIG )
# add_test(NAME ComputeDifferenceImageBIGTest COMMAND ${ExeDir}/pximagecompare DEPENDS ComputeDifferenceImageBIGOutput)
# 
# add_test(NAME ComputeMeanOutput COMMAND ${ExeDir}/pxcomputemean )
# add_test(NAME ComputeMeanTest COMMAND ${ExeDir}/pximagecompare DEPENDS ComputeMeanOutput)
# 
# add_test(NAME ComputeOverlapOutput COMMAND ${ExeDir}/pxcomputeoverlap )
# add_test(NAME ComputeOverlapTest COMMAND ${ExeDir}/pximagecompare DEPENDS ComputeOverlapOutput)
#  
# add_test(NAME ContrastEnhanceImageOutput COMMAND ${ExeDir}/pxcontrastenhanceimage )
# add_test(NAME ContrastEnhanceImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS ContrastEnhanceImageOutput)
#  
# add_test(NAME CountNonZeroVoxelsOutput COMMAND ${ExeDir}/pxcountnonzerovoxels )
# add_test(NAME CountNonZeroVoxelsTest COMMAND ${ExeDir}/pximagecompare DEPENDS CountNonZeroVoxelsOutput)
#  
# add_test(NAME CreateBoxOutput COMMAND ${ExeDir}/pxcreatebox )
# add_test(NAME CreateBoxTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateBoxOutput)
#  
# add_test(NAME CreateCylinderOutput COMMAND ${ExeDir}/pxcreatecylinder )
# add_test(NAME CreateCylinderTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateCylinderOutput)
#  
# add_test(NAME CreateEllipsoidOutput COMMAND ${ExeDir}/pxcreateellipsoid )
# add_test(NAME CreateEllipsoidTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateEllipsoidOutput)
#  
# add_test(NAME CreateGridImageOutput COMMAND ${ExeDir}/pxcreategridimage )
# add_test(NAME CreateGridImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateGridImageOutput)
#  
# add_test(NAME CreatePointsInImageOutput COMMAND ${ExeDir}/pxcreatepointsinimage )
# add_test(NAME CreatePointsInImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreatePointsInImageOutput)
# 
# add_test(NAME CreateRandomImageOutput COMMAND ${ExeDir}/pxcreaterandomimage )
# add_test(NAME CreateRandomImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateRandomImageOutput)
# 
# add_test(NAME CreateSimpleBoxOutput COMMAND ${ExeDir}/pxcreatesimplebox )
# add_test(NAME CreateSimpleBoxTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateSimpleBoxOutput)
# 
# add_test(NAME CreateSphereOutput COMMAND ${ExeDir}/pxcreatesphere )
# add_test(NAME CreateSphereTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateSphereOutput)
# 
# add_test(NAME CreateZeroImageOutput COMMAND ${ExeDir}/pxcreatezeroimage )
# add_test(NAME CreateZeroImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS CreateZeroImageOutput)
# 
# add_test(NAME CropImageOutput COMMAND ${ExeDir}/pxcropimage )
# add_test(NAME CropImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS CropImageOutput)
# 
# add_test(NAME DeformationFieldGeneratorOutput COMMAND ${ExeDir}/pxdeformationfieldgenerator )
# add_test(NAME DeformationFieldGeneratorTest COMMAND ${ExeDir}/pximagecompare DEPENDS DeformationFieldGeneratorOutput)
#  
# add_test(NAME DetectGoldMarkersOutput COMMAND ${ExeDir}/pxdetectgoldmarkers )
# add_test(NAME DetectGoldMarkersTest COMMAND ${ExeDir}/pximagecompare DEPENDS DetectGoldMarkersOutput)
#  
# add_test(NAME DistanceTransformOutput COMMAND ${ExeDir}/pxdistancetransform )
# add_test(NAME DistanceTransformTest COMMAND ${ExeDir}/pximagecompare DEPENDS DistanceTransformOutput)
#  
# add_test(NAME ExtractEveryOtherSliceOutput COMMAND ${ExeDir}/pxextracteveryotherslice )
# add_test(NAME ExtractEveryOtherSliceTest COMMAND ${ExeDir}/pximagecompare DEPENDS ExtractEveryOtherSliceOutput)
 
add_test(NAME ExtractIndexFromVectorImageOutput COMMAND ${ExeDir}/pxextractindexfromvectorimage -in ${DataDir}/BlackSquare.png -out ${OutDir}/ExtractIndex0.mhd -ind 0)
add_test(NAME ExtractIndexFromVectorImageTest COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ExtractIndex0.mhd -test ${OutDir}/ExtractIndex0.mhd DEPENDS ExtractIndexFromVectorImageOutput)
#  
# add_test(NAME ExtractSliceOutput COMMAND ${ExeDir}/pxextractslice )
# add_test(NAME ExtractSliceTest COMMAND ${ExeDir}/pximagecompare DEPENDS ExtractSliceOutput)
# 
# add_test(NAME FFTImageOutput COMMAND ${ExeDir}/pxfftimage )
# add_test(NAME FFTImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS FFTImageOutput)
# 
# add_test(NAME GaussianImageFilterOutput COMMAND ${ExeDir}/pxgaussianimagefilter )
# add_test(NAME GaussianImageFilterTest COMMAND ${ExeDir}/pximagecompare DEPENDS GaussianImageFilterOutput)
# 
# add_test(NAME GetDICOMInformationOutput COMMAND ${ExeDir}/pxgetdicominformation )
# add_test(NAME GetDICOMInformationTest COMMAND ${ExeDir}/pximagecompare DEPENDS GetDICOMInformationOutput)
# 
# add_test(NAME GetDICOMSeriesUIDsOutput COMMAND ${ExeDir}/pxgetDICOMseriesUIDs )
# add_test(NAME GetDICOMSeriesUIDsTest COMMAND ${ExeDir}/pximagecompare DEPENDS GetDICOMSeriesUIDsOutput)
# 
# add_test(NAME GetImageInformationOutput COMMAND ${ExeDir}/pxgetimageinformation )
# add_test(NAME GetImageInformationTest COMMAND ${ExeDir}/pximagecompare DEPENDS GetImageInformationOutput)
# 
# add_test(NAME GetPointsInImageOutput COMMAND ${ExeDir}/pxgetpointsinimage )
# add_test(NAME GetPointsInImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS GetPointsInImageOutput)
# 
# add_test(NAME GIPLConvertOutput COMMAND ${ExeDir}/pxgiplconvert )
# add_test(NAME GIPLConvertTest COMMAND ${ExeDir}/pximagecompare DEPENDS GIPLConvertOutput)
#  
# add_test(NAME HistogramEqualizeImageOutput COMMAND ${ExeDir}/pxhistogramequalizeimage )
# add_test(NAME HistogramEqualizeImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS HistogramEqualizeImageOutput)
# 
# add_test(NAME ImageCompareOutput COMMAND ${ExeDir}/pximagecompare )
# add_test(NAME ImageCompareTest COMMAND ${ExeDir}/pximagecompare DEPENDS ImageCompareOutput)
#  
add_test(NAME ImagesToVectorImageOutput COMMAND ${ExeDir}/pximagestovectorimage -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -out ${OutDir}/imagestovectorimage.mhd)
add_test(NAME ImagesToVectorImageTest COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ImagesToVectorImage.mhd -test ${OutDir}/imagestovectorimage.mhd DEPENDS ImagesToVectorImageOutput)
#  
# add_test(NAME IntensityReplaceOutput COMMAND ${ExeDir}/pxintensityreplace )
# add_test(NAME IntensityReplaceTest COMMAND ${ExeDir}/pximagecompare DEPENDS IntensityReplaceOutput)
#  
# add_test(NAME IntensityWindowingOutput COMMAND ${ExeDir}/pxintensitywindowing )
# add_test(NAME IntensityWindowingTest COMMAND ${ExeDir}/pximagecompare DEPENDS IntensityWindowingOutput)
#  
# add_test(NAME InvertIntensityImageFilterOutput COMMAND ${ExeDir}/pxinvertintensityimagefilter )
# add_test(NAME InvertIntensityImageFilterTest COMMAND ${ExeDir}/pximagecompare DEPENDS InvertIntensityImageFilterOutput)
#  
# add_test(NAME KappaStatisticOutput COMMAND ${ExeDir}/pxkappastatistic )
# add_test(NAME KappaStatisticTest COMMAND ${ExeDir}/pximagecompare DEPENDS KappaStatisticOutput)
#  
#########
add_test(NAME LogicalImageOperatorOutput_AND COMMAND ${ExeDir}/pxlogicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops AND -out ${OutDir}/logicalimageoperator_and.png)
add_test(NAME LogicalImageOperatorTest_AND COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/LogicalImageOperator_And.png -test ${OutDir}/logicalimageoperator_and.png DEPENDS LogicalImageOperatorOutput_AND)

add_test(NAME LogicalImageOperatorOutput_OR COMMAND ${ExeDir}/pxlogicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops OR -out ${OutDir}/logicalimageoperator_or.png)
add_test(NAME LogicalImageOperatorTest_OR COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/LogicalImageOperator_Or.png -test ${OutDir}/logicalimageoperator_or.png DEPENDS LogicalImageOperatorOutput_OR)

#########
# add_test(NAME MorphologyOutput COMMAND ${ExeDir}/pxmorphology )
# add_test(NAME MorphologyTest COMMAND ${ExeDir}/pximagecompare DEPENDS MorphologyOutput)
#  
# add_test(NAME NaryImageOperatorOutput COMMAND ${ExeDir}/pxnaryimageoperator )
# add_test(NAME NaryImageOperatorTest COMMAND ${ExeDir}/pximagecompare DEPENDS NaryImageOperatorOutput)
#  
# add_test(NAME PCAOutput COMMAND ${ExeDir}/pxpca )
# add_test(NAME PCATest COMMAND ${ExeDir}/pximagecompare DEPENDS PCAOutput)
#  
# add_test(NAME ReflectOutput COMMAND ${ExeDir}/pxreflect )
# add_test(NAME ReflectTest COMMAND ${ExeDir}/pximagecompare DEPENDS ReflectOutput)
#  
# add_test(NAME ReplaceVoxelOutput COMMAND ${ExeDir}/pxreplacevoxel )
# add_test(NAME ReplaceVoxelTest COMMAND ${ExeDir}/pximagecompare DEPENDS ReplaceVoxelOutput)
#  
# add_test(NAME RescaleIntensityImageFilterOutput COMMAND ${ExeDir}/pxrescaleintensityimagefilter )
# add_test(NAME RescaleIntensityImageFilterTest COMMAND ${ExeDir}/pximagecompare DEPENDS RescaleIntensityImageFilterOutput)
# 
# add_test(NAME ReshapeOutput COMMAND ${ExeDir}/pxreshape )
# add_test(NAME ReshapeTest COMMAND ${ExeDir}/pximagecompare DEPENDS ReshapeOutput)
#  
# add_test(NAME ResizeImageOutput COMMAND ${ExeDir}/pxresizeimage )
# add_test(NAME ResizeImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS ResizeImageOutput)
#  
# add_test(NAME SegmentationDistanceOutput COMMAND ${ExeDir}/pxsegmentationdistance )
# add_test(NAME SegmentationDistanceTest COMMAND ${ExeDir}/pximagecompare DEPENDS SegmentationDistanceOutput)
#  
# add_test(NAME StatisticsOnImageOutput COMMAND ${ExeDir}/pxstatisticsonimage )
# add_test(NAME StatisticsOnImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS StatisticsOnImageOutput)
#  
# add_test(NAME TextureOutput COMMAND ${ExeDir}/pxtexture )
# add_test(NAME TextureTest COMMAND ${ExeDir}/pximagecompare DEPENDS TextureOutput)
#  
# add_test(NAME TileImagesOutput COMMAND ${ExeDir}/pxtileimages )
# add_test(NAME TileImagesTest COMMAND ${ExeDir}/pximagecompare DEPENDS TileImagesOutput)
#  
# add_test(NAME TTestOutput COMMAND ${ExeDir}/pxttest )
# add_test(NAME TTestTest COMMAND ${ExeDir}/pximagecompare DEPENDS TTestOutput)
#  
# add_test(NAME ThresholdImageOutput COMMAND ${ExeDir}/pxthresholdimage )
# add_test(NAME ThresholdImageTest COMMAND ${ExeDir}/pximagecompare DEPENDS ThresholdImageOutput)
#  
# add_test(NAME UnaryImageOperatorOutput COMMAND ${ExeDir}/pxunaryimageoperator )
# add_test(NAME UnaryImageOperatorTest COMMAND ${ExeDir}/pximagecompare DEPENDS UnaryImageOperatorOutput)
#  
# add_test(NAME WeightedAdditionOutput COMMAND ${ExeDir}/pxweightedaddition )
# add_test(NAME WeightedAdditionTest COMMAND ${ExeDir}/pximagecompare DEPENDS WeightedAdditionOutput)
