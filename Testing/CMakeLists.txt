Project( ITKToolsTesting )

#---------------------------------------------------------------------
# Every exectuable has at least one test. Each tests is composed of two add_test commands.
# The first is [name]Output. This runs the executable with the specified parameters to produce an output.
# The second is [name]Test. This runs the imagecompare program to compaer the baseline for the given test
# to the output of [name]Output.
# Additionally, a _[something] after the name means this is a specific type of test for this program.
# E.g. BinaryImageOperatorOutput_Add means that this test tests the ADD functionality of BinaryImageOperator.
#
# The files in DataDir should be descriptively named for their content. E.g. "BlackSquare.png".
# The files in OutDir and BaselineDir should be named for their test. E.g. "AverageVectorMagnitude.png"
#
# Application tests are divided by a ######### Application Name #########
# because many applications have multiple tests
#---------------------------------------------------------------------

# Setup directories
set(BaselineDir ${ITKTools_SOURCE_DIR}/../Testing/Baselines/)
set(DataDir ${ITKTools_SOURCE_DIR}/../Testing/Data/)
set(ExeDir ${EXECUTABLE_OUTPUT_PATH})
set(OutDir ${ITKTools_BINARY_DIR}/Testing/)

######### AverageVectorMagnitude #########
# This tests the magnitude image. Also should test the average vector magnitude (not an image)
# add_test(NAME AverageVectorMagnitudeOutput COMMAND averagevectormagnitude -in ${DataDir}/BlackSquare.png -out ${OutDir}/AverageVectorMagnitude.png)
# add_test(NAME AverageVectorMagnitudeTest COMMAND imagecompare -base ${BaselineDir}//AverageVectorMagnitude.png -test ${OutDir}/AverageVectorMagnitude.png DEPENDS AverageVectorMagnitudeOutput)

######### BinaryImageOperator #########
# Add images
add_test(NAME BinaryImageOperatorOutput_Add
         COMMAND ${ExeDir}/pxbinaryimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops ADDITION -out ${OutDir}/binaryimageoperator_add.png)
add_test(NAME BinaryImageOperatorTest_Add
         COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/BinaryImageOperator_Add.png -test ${OutDir}/binaryimageoperator_add.png
         DEPENDS BinaryImageOperatorOutput_Add)

######### BinaryThinning #########
# add_test(NAME BinaryThinningOutput
#          COMMAND ${ExeDir}/pxbinarythinning )
# add_test(NAME BinaryThinningTest
#          COMMAND ${ExeDir}/imagecompare
#          DEPENDS BinaryThinningOutput)

######### BrainDistance #########
# add_test(NAME BrainDistanceOutput
#          COMMAND ${ExeDir}/pxbraindistance )
# add_test(NAME BrainDistanceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test ${OutDir}/
#          DEPENDS BrainDistanceOutput)

######### CastConvert #########
add_test(NAME CastConvertOutput
         COMMAND ${ExeDir}/pxcastconvert -in ${DataDir}/WhiteSquare.png -out ${OutDir}/castconvert.mhd)
add_test(NAME CastConvertTest
         COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/CastConvert.mhd -test ${OutDir}/castconvert.mhd
         DEPENDS CastConvertOutput)

######### ClosestVersor3DTransform #########
# add_test(NAME ClosestVersor3DTransformOutput
#          COMMAND ${ExeDir}/pxclosestversor3Dtransform )
# add_test(NAME ClosestVersor3DTransformTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ClosestVersor3DTransformOutput)

######### CombineSegmentations #########
# add_test(NAME CombineSegmentationsOutput
#          COMMAND ${ExeDir}/pxcombinesegmentations )
# add_test(NAME CombineSegmentationsTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CombineSegmentationsOutput)

######### ComputeBoundingBox #########
# add_test(NAME ComputeBoundingBoxOutput
#          COMMAND ${ExeDir}/pxcomputeboundingbox )
# add_test(NAME ComputeBoundingBoxTest
#          COMMAND ${ExeDir}/pximagecompare
#          DEPENDS -base ${BaselineDir}/ -test ComputeBoundingBoxOutput)

######### ComputeDifferenceImageBIG #########
# add_test(NAME ComputeDifferenceImageBIGOutput
#          COMMAND ${ExeDir}/pxcomputedifferenceimageBIG )
# add_test(NAME ComputeDifferenceImageBIGTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ComputeDifferenceImageBIGOutput)

######### ComputeMean #########
# add_test(NAME ComputeMeanOutput
#          COMMAND ${ExeDir}/pxcomputemean )
# add_test(NAME ComputeMeanTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ComputeMeanOutput)

######### ComputeOverlap #########
# add_test(NAME ComputeOverlapOutput
#          COMMAND ${ExeDir}/pxcomputeoverlap )
# add_test(NAME ComputeOverlapTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ComputeOverlapOutput)

######### ContrastEnhanceImage #########
# add_test(NAME ContrastEnhanceImageOutput
#          COMMAND ${ExeDir}/pxcontrastenhanceimage )
# add_test(NAME ContrastEnhanceImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ContrastEnhanceImageOutput)

######### CountNonZeroVoxels #########
# add_test(NAME CountNonZeroVoxelsOutput
#          COMMAND ${ExeDir}/pxcountnonzerovoxels )
# add_test(NAME CountNonZeroVoxelsTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CountNonZeroVoxelsOutput)

######### CreateBox #########
# add_test(NAME CreateBoxOutput
#          COMMAND ${ExeDir}/pxcreatebox )
# add_test(NAME CreateBoxTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateBoxOutput)

######### CreateCylinder #########
# add_test(NAME CreateCylinderOutput
#          COMMAND ${ExeDir}/pxcreatecylinder )
# add_test(NAME CreateCylinderTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateCylinderOutput)

######### CreateEllipsoid #########
# add_test(NAME CreateEllipsoidOutput
#          COMMAND ${ExeDir}/pxcreateellipsoid )
# add_test(NAME CreateEllipsoidTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateEllipsoidOutput)

######### CreateGridImage #########
# add_test(NAME CreateGridImageOutput
#          COMMAND ${ExeDir}/pxcreategridimage )
# add_test(NAME CreateGridImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateGridImageOutput)

######### CreatePointsInImage #########
# add_test(NAME CreatePointsInImageOutput
#         COMMAND ${ExeDir}/pxcreatepointsinimage )
# add_test(NAME CreatePointsInImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreatePointsInImageOutput)

######### CreateRandomImage #########
# add_test(NAME CreateRandomImageOutput
#          COMMAND ${ExeDir}/pxcreaterandomimage )
# add_test(NAME CreateRandomImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateRandomImageOutput)

######### CreateSimpleBox #########
# add_test(NAME CreateSimpleBoxOutput
#          COMMAND ${ExeDir}/pxcreatesimplebox )
# add_test(NAME CreateSimpleBoxTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateSimpleBoxOutput)

######### CreateSphere #########
# add_test(NAME CreateSphereOutput
#          COMMAND ${ExeDir}/pxcreatesphere )
# add_test(NAME CreateSphereTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateSphereOutput)

######### CreateZeroImage #########
# add_test(NAME CreateZeroImageOutput
#          COMMAND ${ExeDir}/pxcreatezeroimage )
# add_test(NAME CreateZeroImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CreateZeroImageOutput)

######### CropImage #########
# add_test(NAME CropImageOutput
#          COMMAND ${ExeDir}/pxcropimage )
# add_test(NAME CropImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS CropImageOutput)

######### DeformationFieldGenerator #########
# add_test(NAME DeformationFieldGeneratorOutput
#          COMMAND ${ExeDir}/pxdeformationfieldgenerator )
# add_test(NAME DeformationFieldGeneratorTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS DeformationFieldGeneratorOutput)

######### DetectGoldMarkers #########
# add_test(NAME DetectGoldMarkersOutput
#          COMMAND ${ExeDir}/pxdetectgoldmarkers )
# add_test(NAME DetectGoldMarkersTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS DetectGoldMarkersOutput)

######### DistanceTransform #########
# add_test(NAME DistanceTransformOutput
#          COMMAND ${ExeDir}/pxdistancetransform )
# add_test(NAME DistanceTransformTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS DistanceTransformOutput)

######### ExtractEveryOtherSlice #########
# add_test(NAME ExtractEveryOtherSliceOutput
#          COMMAND ${ExeDir}/pxextracteveryotherslice )
# add_test(NAME ExtractEveryOtherSliceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ExtractEveryOtherSliceOutput)

######### ExtractIndexFromVectorImage #########
add_test(NAME ExtractIndexFromVectorImageOutput
         COMMAND ${ExeDir}/pxextractindexfromvectorimage -in ${DataDir}/BlackSquare.png -out ${OutDir}/ExtractIndex0.mhd -ind 0)
add_test(NAME ExtractIndexFromVectorImageTest
         COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ExtractIndex0.mhd -test ${OutDir}/ExtractIndex0.mhd
         DEPENDS ExtractIndexFromVectorImageOutput)

######### ExtractSlice #########
# add_test(NAME ExtractSliceOutput
#          COMMAND ${ExeDir}/pxextractslice )
# add_test(NAME ExtractSliceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ExtractSliceOutput)

######### FFTImage #########
# add_test(NAME FFTImageOutput
#         COMMAND ${ExeDir}/pxfftimage )
# add_test(NAME FFTImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS FFTImageOutput)

######### GaussianImageFilter #########
# add_test(NAME GaussianImageFilterOutput
#          COMMAND ${ExeDir}/pxgaussianimagefilter )
# add_test(NAME GaussianImageFilterTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS GaussianImageFilterOutput)

######### GetDICOMInformation #########
# add_test(NAME GetDICOMInformationOutput
#          COMMAND ${ExeDir}/pxgetdicominformation )
# add_test(NAME GetDICOMInformationTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS GetDICOMInformationOutput)

######### GetDICOMSeriesUIDs #########
# add_test(NAME GetDICOMSeriesUIDsOutput
#          COMMAND ${ExeDir}/pxgetDICOMseriesUIDs )
# add_test(NAME GetDICOMSeriesUIDsTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS GetDICOMSeriesUIDsOutput)

######### GetImageInformation #########
# add_test(NAME GetImageInformationOutput
#          COMMAND ${ExeDir}/pxgetimageinformation )
# add_test(NAME GetImageInformationTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS GetImageInformationOutput)

######### GetPointsInImage #########
# add_test(NAME GetPointsInImageOutput
#          COMMAND ${ExeDir}/pxgetpointsinimage )
# add_test(NAME GetPointsInImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS GetPointsInImageOutput)

######### GIPLConvert #########
# add_test(NAME GIPLConvertOutput
#          COMMAND ${ExeDir}/pxgiplconvert )
# add_test(NAME GIPLConvertTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS GIPLConvertOutput)

######### HistogramEqualizeImage #########
# add_test(NAME HistogramEqualizeImageOutput
#          COMMAND ${ExeDir}/pxhistogramequalizeimage )
# add_test(NAME HistogramEqualizeImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS HistogramEqualizeImageOutput)

######### ImageCompare #########
# add_test(NAME ImageCompareOutput
#          COMMAND ${ExeDir}/pximagecompare )
# add_test(NAME ImageCompareTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ImageCompareOutput)

######### ImagesToVectorImage #########
add_test(NAME ImagesToVectorImageOutput
         COMMAND ${ExeDir}/pximagestovectorimage -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -out ${OutDir}/imagestovectorimage.mhd)
add_test(NAME ImagesToVectorImageTest
         COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ImagesToVectorImage.mhd -test ${OutDir}/imagestovectorimage.mhd
         DEPENDS ImagesToVectorImageOutput)

######### IntensityReplace #########
# add_test(NAME IntensityReplaceOutput
#          COMMAND ${ExeDir}/pxintensityreplace )
# add_test(NAME IntensityReplaceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS IntensityReplaceOutput)

######### IntensityWindowing #########
# add_test(NAME IntensityWindowingOutput
#          COMMAND ${ExeDir}/pxintensitywindowing )
# add_test(NAME IntensityWindowingTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS IntensityWindowingOutput)

######### InvertIntensityImageFilter #########
# add_test(NAME InvertIntensityImageFilterOutput
#         COMMAND ${ExeDir}/pxinvertintensityimagefilter )
# add_test(NAME InvertIntensityImageFilterTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS InvertIntensityImageFilterOutput)

######### KappaStatistic #########
# add_test(NAME KappaStatisticOutput
#          COMMAND ${ExeDir}/pxkappastatistic )
# add_test(NAME KappaStatisticTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS KappaStatisticOutput)

######### LogicalImageOperator #########
add_test(NAME LogicalImageOperatorOutput_AND
         COMMAND ${ExeDir}/pxlogicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops AND -out ${OutDir}/logicalimageoperator_and.png)
add_test(NAME LogicalImageOperatorTest_AND
         COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/LogicalImageOperator_And.png -test ${OutDir}/logicalimageoperator_and.png
         DEPENDS LogicalImageOperatorOutput_AND)

add_test(NAME LogicalImageOperatorOutput_OR
        COMMAND ${ExeDir}/pxlogicalimageoperator -in ${DataDir}/BlackSquare.png ${DataDir}/WhiteSquare.png -ops OR -out ${OutDir}/logicalimageoperator_or.png)
add_test(NAME LogicalImageOperatorTest_OR
         COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/LogicalImageOperator_Or.png -test ${OutDir}/logicalimageoperator_or.png
         DEPENDS LogicalImageOperatorOutput_OR)

######### Morphology #########
# add_test(NAME MorphologyOutput
#          COMMAND ${ExeDir}/pxmorphology )
# add_test(NAME MorphologyTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS MorphologyOutput)

######### NaryImageOperator #########
# add_test(NAME NaryImageOperatorOutput
#         COMMAND ${ExeDir}/pxnaryimageoperator )
# add_test(NAME NaryImageOperatorTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS NaryImageOperatorOutput)

######### PCA #########
# add_test(NAME PCAOutput
#          COMMAND ${ExeDir}/pxpca )
# add_test(NAME PCATest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS PCAOutput)

######### Reflect #########
# add_test(NAME ReflectOutput
#          COMMAND ${ExeDir}/pxreflect )
# add_test(NAME ReflectTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ReflectOutput)

######### ReplaceVoxel #########
# add_test(NAME ReplaceVoxelOutput
#          COMMAND ${ExeDir}/pxreplacevoxel )
# add_test(NAME ReplaceVoxelTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ReplaceVoxelOutput)

######### RescaleIntensityImageFilter #########
# add_test(NAME RescaleIntensityImageFilterOutput
#          COMMAND ${ExeDir}/pxrescaleintensityimagefilter )
# add_test(NAME RescaleIntensityImageFilterTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS RescaleIntensityImageFilterOutput)

######### Reshape #########
# add_test(NAME ReshapeOutput
#          COMMAND ${ExeDir}/pxreshape )
# add_test(NAME ReshapeTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ReshapeOutput)

######### ResizeImage #########
# add_test(NAME ResizeImageOutput
#          COMMAND ${ExeDir}/pxresizeimage )
# add_test(NAME ResizeImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ResizeImageOutput)

######### SegmentationDistance #########
# add_test(NAME SegmentationDistanceOutput
#          COMMAND ${ExeDir}/pxsegmentationdistance )
# add_test(NAME SegmentationDistanceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS SegmentationDistanceOutput)

######### StatisticsOnImage #########
# add_test(NAME StatisticsOnImageOutput
#          COMMAND ${ExeDir}/pxstatisticsonimage )
# add_test(NAME StatisticsOnImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS StatisticsOnImageOutput)

######### Texture #########
# add_test(NAME TextureOutput
#          COMMAND ${ExeDir}/pxtexture )
# add_test(NAME TextureTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test DEPENDS TextureOutput)

######### TileImages #########
# add_test(NAME TileImagesOutput
#          COMMAND ${ExeDir}/pxtileimages )
# add_test(NAME TileImagesTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS TileImagesOutput)

######### TTest #########
# add_test(NAME TTestOutput
#          COMMAND ${ExeDir}/pxttest )
# add_test(NAME TTestTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS TTestOutput)

######### ThresholdImage #########
# add_test(NAME ThresholdImageOutput
#          COMMAND ${ExeDir}/pxthresholdimage )
# add_test(NAME ThresholdImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS ThresholdImageOutput)

######### UnaryImageOperator #########
# add_test(NAME UnaryImageOperatorOutput
#          COMMAND ${ExeDir}/pxunaryimageoperator )
# add_test(NAME UnaryImageOperatorTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS UnaryImageOperatorOutput)

######### WeightedAddition #########
# add_test(NAME WeightedAdditionOutput
#          COMMAND ${ExeDir}/pxweightedaddition )
# add_test(NAME WeightedAdditionTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          DEPENDS WeightedAdditionOutput)

#These tests are not px applications, but internal tests
ADD_EXECUTABLE( ChannelByChannelVectorImageFilterTest
  ChannelByChannelVectorImageFilterTest.cxx )

TARGET_LINK_LIBRARIES( ChannelByChannelVectorImageFilterTest ITKTools-Common ${ITK_LIBRARIES} )

INSTALL( TARGETS ChannelByChannelVectorImageFilterTest
  RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )

add_test(NAME ChannelByChannelVectorImageFilterOutput COMMAND ChannelByChannelVectorImageFilterTest )
add_test(NAME ChannelByChannelVectorImageFilterValidate COMMAND ${ExeDir}/pximagecompare DEPENDS ChannelByChannelVectorImageFilterOutput)
