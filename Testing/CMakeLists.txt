Project( ITKToolsTesting )

#---------------------------------------------------------------------
#
# To run those tests on Windows, change to the binary directory, and type:
#   ctest -C Release
# Or build the RUN_TESTS project in the IDE of ITKTools.sln.
#
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
set(BaselineDir ${ITKTOOLS_SOURCE_DIR}/../Testing/Baselines)
set(DataDir ${ITKTOOLS_SOURCE_DIR}/../Testing/Data)
set(ExeDir ${EXECUTABLE_OUTPUT_PATH})
set(OutDir ${ITKTOOLS_BINARY_DIR}/Testing)

# Define helpful testing macro
# This macro assumes there are two tests: one that creates an output,
# and one that compares the output with a baseline.
#  _name: test main name
#  subtest: name of subtest
#  ext: file extension of the output
#  cl1: command line of creation test
#  cl2: command line of comparison test (baseline name)
macro( itktools_add_test _name subtest ext cl1 cl2 )
  set( subtestname _${subtest} )
  string( COMPARE EQUAL ${subtestname} "_" eq )
  if( eq )
    set( subtestname "" )
  endif()
  set( outName ${OutDir}/${_name}${subtestname}.${ext} )
  add_test( NAME ${_name}${subtestname}_OUTPUT
    COMMAND ${ExeDir}/px${_name} ${cl1} -out ${outName} )
  add_test( NAME ${_name}${subtestname}_COMPARE
    COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/${cl2} -test ${outName} )
  set_tests_properties( ${_name}${subtestname}_COMPARE
    PROPERTIES DEPENDS ${_name}${subtestname}_OUTPUT )
endmacro()


###########################################################
# Start of tests


######### AverageVectorMagnitude #########
# This tests the magnitude image. Also should test the average vector magnitude (not an image)
# add_test(NAME AverageVectorMagnitudeOutput COMMAND averagevectormagnitude -in ${DataDir}/BlackSquare.png -out ${OutDir}/AverageVectorMagnitude.png)
# add_test(NAME AverageVectorMagnitudeTest COMMAND imagecompare -base ${BaselineDir}//AverageVectorMagnitude.png -test ${OutDir}/AverageVectorMagnitude.png PROPERTIES DEPENDS AverageVectorMagnitudeOutput)

######### BinaryImageOperator #########
itktools_add_test( binaryimageoperator add png
  "-in;${DataDir}/BlackSquare.png;${DataDir}/WhiteSquare.png;-ops;ADDITION"
  "BinaryImageOperator_Add.png" )

######### BinaryThinning #########
# add_test(NAME BinaryThinningOutput
#          COMMAND ${ExeDir}/pxbinarythinning )
# add_test(NAME BinaryThinningTest
#          COMMAND ${ExeDir}/imagecompare
#          PROPERTIES DEPENDS BinaryThinningOutput)

######### BrainDistance #########
# add_test(NAME BrainDistanceOutput
#          COMMAND ${ExeDir}/pxbraindistance )
# add_test(NAME BrainDistanceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test ${OutDir}/
#          PROPERTIES DEPENDS BrainDistanceOutput)

######### CastConvert #########
itktools_add_test( castconvert "SCALAR" mhd
  "-in;${DataDir}/WhiteSquare.png"
  "CastConvert.mhd" )
itktools_add_test( castconvert "DICOM" mha
  "-in;${DataDir}/dicom"
  "CastConvert_DICOM.mha" )

######### ClosestVersor3DTransform #########
# add_test(NAME ClosestVersor3DTransformOutput
#          COMMAND ${ExeDir}/pxclosestversor3Dtransform )
# add_test(NAME ClosestVersor3DTransformTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ClosestVersor3DTransformOutput)

######### CombineSegmentations #########
# add_test(NAME CombineSegmentationsOutput
#          COMMAND ${ExeDir}/pxcombinesegmentations )
# add_test(NAME CombineSegmentationsTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CombineSegmentationsOutput)

######### ComputeBoundingBox #########
# add_test(NAME ComputeBoundingBoxOutput
#          COMMAND ${ExeDir}/pxcomputeboundingbox )
# add_test(NAME ComputeBoundingBoxTest
#          COMMAND ${ExeDir}/pximagecompare
#          PROPERTIES DEPENDS -base ${BaselineDir}/ -test ComputeBoundingBoxOutput)

######### ComputeDifferenceImageBIG #########
# add_test(NAME ComputeDifferenceImageBIGOutput
#          COMMAND ${ExeDir}/pxcomputedifferenceimageBIG )
# add_test(NAME ComputeDifferenceImageBIGTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ComputeDifferenceImageBIGOutput)

######### ComputeMean #########
# add_test(NAME ComputeMeanOutput
#          COMMAND ${ExeDir}/pxcomputemean )
# add_test(NAME ComputeMeanTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ComputeMeanOutput)

######### ComputeOverlap #########
# add_test(NAME ComputeOverlapOutput
#          COMMAND ${ExeDir}/pxcomputeoverlap )
# add_test(NAME ComputeOverlapTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ComputeOverlapOutput)

######### ContrastEnhanceImage #########
# add_test(NAME ContrastEnhanceImageOutput
#          COMMAND ${ExeDir}/pxcontrastenhanceimage )
# add_test(NAME ContrastEnhanceImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ContrastEnhanceImageOutput)

######### CountNonZeroVoxels #########
# add_test(NAME CountNonZeroVoxelsOutput
#          COMMAND ${ExeDir}/pxcountnonzerovoxels )
# add_test(NAME CountNonZeroVoxelsTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CountNonZeroVoxelsOutput)

######### CreateBox #########
# add_test(NAME CreateBoxOutput
#          COMMAND ${ExeDir}/pxcreatebox )
# add_test(NAME CreateBoxTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateBoxOutput)

######### CreateCylinder #########
# add_test(NAME CreateCylinderOutput
#          COMMAND ${ExeDir}/pxcreatecylinder )
# add_test(NAME CreateCylinderTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateCylinderOutput)

######### CreateEllipsoid #########
# add_test(NAME CreateEllipsoidOutput
#          COMMAND ${ExeDir}/pxcreateellipsoid )
# add_test(NAME CreateEllipsoidTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateEllipsoidOutput)

######### CreateGridImage #########
# add_test(NAME CreateGridImageOutput
#          COMMAND ${ExeDir}/pxcreategridimage )
# add_test(NAME CreateGridImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateGridImageOutput)

######### CreatePointsInImage #########
# add_test(NAME CreatePointsInImageOutput
#         COMMAND ${ExeDir}/pxcreatepointsinimage )
# add_test(NAME CreatePointsInImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreatePointsInImageOutput)

######### CreateRandomImage #########
# add_test(NAME CreateRandomImageOutput
#          COMMAND ${ExeDir}/pxcreaterandomimage )
# add_test(NAME CreateRandomImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateRandomImageOutput)

######### CreateSimpleBox #########
# add_test(NAME CreateSimpleBoxOutput
#          COMMAND ${ExeDir}/pxcreatesimplebox )
# add_test(NAME CreateSimpleBoxTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateSimpleBoxOutput)

######### CreateSphere #########
# add_test(NAME CreateSphereOutput
#          COMMAND ${ExeDir}/pxcreatesphere )
# add_test(NAME CreateSphereTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateSphereOutput)

######### CreateZeroImage #########
# add_test(NAME CreateZeroImageOutput
#          COMMAND ${ExeDir}/pxcreatezeroimage )
# add_test(NAME CreateZeroImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CreateZeroImageOutput)

######### CropImage #########
# add_test(NAME CropImageOutput
#          COMMAND ${ExeDir}/pxcropimage )
# add_test(NAME CropImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS CropImageOutput)

######### DeformationFieldGenerator #########
# add_test(NAME DeformationFieldGeneratorOutput
#          COMMAND ${ExeDir}/pxdeformationfieldgenerator )
# add_test(NAME DeformationFieldGeneratorTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS DeformationFieldGeneratorOutput)

######### DetectGoldMarkers #########
# add_test(NAME DetectGoldMarkersOutput
#          COMMAND ${ExeDir}/pxdetectgoldmarkers )
# add_test(NAME DetectGoldMarkersTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS DetectGoldMarkersOutput)

######### DistanceTransform #########
# add_test(NAME DistanceTransformOutput
#          COMMAND ${ExeDir}/pxdistancetransform )
# add_test(NAME DistanceTransformTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS DistanceTransformOutput)

######### ExtractEveryOtherSlice #########
# add_test(NAME ExtractEveryOtherSliceOutput
#          COMMAND ${ExeDir}/pxextracteveryotherslice )
# add_test(NAME ExtractEveryOtherSliceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ExtractEveryOtherSliceOutput)

######### ExtractIndexFromVectorImage #########
itktools_add_test( extractindexfromvectorimage "" mhd
  "-in;${DataDir}/BlackSquare.png;-ind;0"
  "ExtractIndex0.mhd" )

######### ExtractSlice #########
# add_test(NAME ExtractSliceOutput
#          COMMAND ${ExeDir}/pxextractslice )
# add_test(NAME ExtractSliceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ExtractSliceOutput)

######### FFTImage #########
# add_test(NAME FFTImageOutput
#         COMMAND ${ExeDir}/pxfftimage )
# add_test(NAME FFTImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS FFTImageOutput)

######### GaussianImageFilter #########
# add_test(NAME GaussianImageFilterOutput
#          COMMAND ${ExeDir}/pxgaussianimagefilter )
# add_test(NAME GaussianImageFilterTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS GaussianImageFilterOutput)

######### GetDICOMInformation #########
# add_test(NAME GetDICOMInformationOutput
#          COMMAND ${ExeDir}/pxgetdicominformation )
# add_test(NAME GetDICOMInformationTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS GetDICOMInformationOutput)

######### GetDICOMSeriesUIDs #########
# add_test(NAME GetDICOMSeriesUIDsOutput
#          COMMAND ${ExeDir}/pxgetDICOMseriesUIDs )
# add_test(NAME GetDICOMSeriesUIDsTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS GetDICOMSeriesUIDsOutput)

######### GetImageInformation #########
# add_test(NAME GetImageInformationOutput
#          COMMAND ${ExeDir}/pxgetimageinformation )
# add_test(NAME GetImageInformationTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS GetImageInformationOutput)

######### GetPointsInImage #########
# add_test(NAME GetPointsInImageOutput
#          COMMAND ${ExeDir}/pxgetpointsinimage )
# add_test(NAME GetPointsInImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS GetPointsInImageOutput)

######### GIPLConvert #########
# add_test(NAME GIPLConvertOutput
#          COMMAND ${ExeDir}/pxgiplconvert )
# add_test(NAME GIPLConvertTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS GIPLConvertOutput)

######### HistogramEqualizeImage #########
# add_test(NAME HistogramEqualizeImageOutput
#          COMMAND ${ExeDir}/pxhistogramequalizeimage )
# add_test(NAME HistogramEqualizeImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS HistogramEqualizeImageOutput)

######### ImageCompare #########
# add_test(NAME ImageCompareOutput
#          COMMAND ${ExeDir}/pximagecompare )
# add_test(NAME ImageCompareTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ImageCompareOutput)

######### ImagesToVectorImage #########
itktools_add_test( imagestovectorimage "" mhd
  "-in;${DataDir}/BlackSquare.png;${DataDir}/WhiteSquare.png"
  "ImagesToVectorImage.mhd" )

######### IntensityReplace #########
# add_test(NAME IntensityReplaceOutput
#          COMMAND ${ExeDir}/pxintensityreplace )
# add_test(NAME IntensityReplaceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS IntensityReplaceOutput)

######### IntensityWindowing #########
# add_test(NAME IntensityWindowingOutput
#          COMMAND ${ExeDir}/pxintensitywindowing )
# add_test(NAME IntensityWindowingTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS IntensityWindowingOutput)

######### InvertIntensityImageFilter #########
# add_test(NAME InvertIntensityImageFilterOutput
#         COMMAND ${ExeDir}/pxinvertintensityimagefilter )
# add_test(NAME InvertIntensityImageFilterTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS InvertIntensityImageFilterOutput)

######### KappaStatistic #########
# add_test(NAME KappaStatisticOutput
#          COMMAND ${ExeDir}/pxkappastatistic )
# add_test(NAME KappaStatisticTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS KappaStatisticOutput)

######### LogicalImageOperator #########
itktools_add_test( logicalimageoperator "NOT" png
  "-in;${DataDir}/BlackSquare.png;-ops;NOT"
  "LogicalImageOperator_Not.png" )
itktools_add_test( logicalimageoperator "AND" png
  "-in;${DataDir}/BlackSquare.png;${DataDir}/WhiteSquare.png;-ops;AND"
  "LogicalImageOperator_And.png" )
itktools_add_test( logicalimageoperator "OR" png
  "-in;${DataDir}/BlackSquare.png;${DataDir}/WhiteSquare.png;-ops;OR"
  "LogicalImageOperator_Or.png" )

######### Morphology #########
# add_test(NAME MorphologyOutput
#          COMMAND ${ExeDir}/pxmorphology )
# add_test(NAME MorphologyTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS MorphologyOutput)

######### NaryImageOperator #########
# add_test(NAME NaryImageOperatorOutput
#         COMMAND ${ExeDir}/pxnaryimageoperator )
# add_test(NAME NaryImageOperatorTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS NaryImageOperatorOutput)

######### PCA #########
# add_test(NAME PCAOutput
#          COMMAND ${ExeDir}/pxpca )
# add_test(NAME PCATest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS PCAOutput)

######### Reflect #########
# add_test(NAME ReflectOutput
#          COMMAND ${ExeDir}/pxreflect )
# add_test(NAME ReflectTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ReflectOutput)

######### ReplaceVoxel #########
# add_test(NAME ReplaceVoxelOutput
#          COMMAND ${ExeDir}/pxreplacevoxel )
# add_test(NAME ReplaceVoxelTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ReplaceVoxelOutput)

######### RescaleIntensityImageFilter #########
# add_test(NAME RescaleIntensityImageFilterOutput
#          COMMAND ${ExeDir}/pxrescaleintensityimagefilter )
# add_test(NAME RescaleIntensityImageFilterTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS RescaleIntensityImageFilterOutput)

######### Reshape #########
# add_test(NAME ReshapeOutput
#          COMMAND ${ExeDir}/pxreshape )
# add_test(NAME ReshapeTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ReshapeOutput)

######### ResizeImage #########
# add_test(NAME ResizeImageOutput
#          COMMAND ${ExeDir}/pxresizeimage )
# add_test(NAME ResizeImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ResizeImageOutput)

######### SegmentationDistance #########
# add_test(NAME SegmentationDistanceOutput
#          COMMAND ${ExeDir}/pxsegmentationdistance )
# add_test(NAME SegmentationDistanceTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS SegmentationDistanceOutput)

######### StatisticsOnImage #########
# add_test(NAME StatisticsOnImageOutput
#          COMMAND ${ExeDir}/pxstatisticsonimage )
# add_test(NAME StatisticsOnImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS StatisticsOnImageOutput)

######### Texture #########
# add_test(NAME TextureOutput
#          COMMAND ${ExeDir}/pxtexture )
# add_test(NAME TextureTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test PROPERTIES DEPENDS TextureOutput)

######### TileImages #########
# add_test(NAME TileImagesOutput
#          COMMAND ${ExeDir}/pxtileimages )
# add_test(NAME TileImagesTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS TileImagesOutput)

######### TTest #########
# add_test(NAME TTestOutput
#          COMMAND ${ExeDir}/pxttest )
# add_test(NAME TTestTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS TTestOutput)

######### ThresholdImage #########
# add_test(NAME ThresholdImageOutput
#          COMMAND ${ExeDir}/pxthresholdimage )
# add_test(NAME ThresholdImageTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS ThresholdImageOutput)

######### UnaryImageOperator #########
itktools_add_test( unaryimageoperator "RDIVIDE" png
  "-in;${DataDir}/WhiteStripe4.png;-ops;RDIVIDE;-arg;2;-z"
  "unaryimageoperator_RDIVIDE.png" )
itktools_add_test( unaryimageoperator "SIN" mhd
  "-in;${DataDir}/brain_pd.png;-ops;SIN;-opct;float"
  "unaryimageoperator_SIN.mha" )

# add_test(NAME UnaryImageOperatorOutput
#          COMMAND ${ExeDir}/pxunaryimageoperator )
# add_test(NAME UnaryImageOperatorTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS UnaryImageOperatorOutput)

######### WeightedAddition #########
# add_test(NAME WeightedAdditionOutput
#          COMMAND ${ExeDir}/pxweightedaddition )
# add_test(NAME WeightedAdditionTest
#          COMMAND ${ExeDir}/pximagecompare -base ${BaselineDir}/ -test
#          PROPERTIES DEPENDS WeightedAdditionOutput)

#These tests are not px applications, but internal tests
# ADD_EXECUTABLE( ChannelByChannelVectorImageFilterTest
#   ChannelByChannelVectorImageFilterTest.cxx )
#
# TARGET_LINK_LIBRARIES( ChannelByChannelVectorImageFilterTest ITKTools-Common ${ITK_LIBRARIES} )
#
# INSTALL( TARGETS ChannelByChannelVectorImageFilterTest
#   RUNTIME DESTINATION ${ITKTools_INSTALL_DIR} )
#
# add_test(NAME ChannelByChannelVectorImageFilterOutput COMMAND ChannelByChannelVectorImageFilterTest )
# add_test(NAME ChannelByChannelVectorImageFilterValidate COMMAND ${ExeDir}/pximagecompare PROPERTIES DEPENDS ChannelByChannelVectorImageFilterOutput)

