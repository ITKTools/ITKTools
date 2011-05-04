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
# add_test(NAME BinaryThinningOutput COMMAND binarythinning )
# add_test(NAME BinaryThinningTest COMMAND imagecompare DEPENDS BinaryThinningOutput)
# 
# add_test(NAME BrainDistanceOutput COMMAND braindistance )
# add_test(NAME BrainDistanceTest COMMAND imagecompare DEPENDS BrainDistanceOutput)

add_test(NAME CastConvertOutput COMMAND ${ExeDir}/pxcastconvert -in ${DataDir}/WhiteSquare.png -out ${OutDir}/castconvert.mhd)
add_test(NAME CastConvertTest COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/CastConvert.mhd -test ${OutDir}/castconvert.mhd DEPENDS CastConvertOutput)
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
add_test(NAME ImagesToVectorImageOutput COMMAND ${ExeDir}/pximagestovectorimage -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -out ${OutDir}/imagestovectorimage.mhd)
add_test(NAME ImagesToVectorImageTest COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ImagesToVectorImage.mhd -test ${OutDir}/imagestovectorimage.mhd DEPENDS ImagesToVectorImageOutput)
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
add_test(NAME LogicalImageOperatorOutput_AND COMMAND ${ExeDir}/pxlogicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops AND -out ${OutDir}/logicalimageoperator_and.png)
add_test(NAME LogicalImageOperatorTest_AND COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/LogicalImageOperator_And.png -test ${OutDir}/logicalimageoperator_and.png DEPENDS LogicalImageOperatorOutput_AND)

add_test(NAME LogicalImageOperatorOutput_OR COMMAND ${ExeDir}/pxlogicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops OR -out ${OutDir}/logicalimageoperator_or.png)
add_test(NAME LogicalImageOperatorTest_OR COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/LogicalImageOperator_Or.png -test ${OutDir}/logicalimageoperator_or.png DEPENDS LogicalImageOperatorOutput_OR)

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