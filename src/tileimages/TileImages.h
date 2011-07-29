#ifndef TILEIMAGES_H
#define TILEIMAGES_H

#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

/** TileImages */

class TileImagesBase : public itktools::ITKToolsBase
{ 
public:
  TileImagesBase(){};
  ~TileImagesBase(){};

  /** Input parameters */
  std::vector<std::string> m_InputFileNames;
  std::string m_OutputFileName;
  std::vector<unsigned int> m_Layout;
  double m_Defaultvalue;
    
}; // end TileImagesBase


template< class TComponentType, unsigned int VDimension >
class TileImages : public TileImagesBase
{
public:
  typedef TileImages Self;

  TileImages(){};
  ~TileImages(){};

  static Self * New( itktools::ComponentType componentType, unsigned int dim )
  {
    if ( itktools::IsType<TComponentType>( componentType ) && VDimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
  /** Some typedef's. */
    typedef itk::Image<TComponentType, VDimension>            ImageType;
    typedef itk::ImageFileReader<ImageType>             ImageReaderType;
    typedef itk::TileImageFilter<ImageType, ImageType>  TilerType;
    typedef itk::ImageFileWriter<ImageType>             ImageWriterType;
    //typedef typename ImageType::SpacingType             SpacingType;

    /** Copy layout into a fixed array. */
    itk::FixedArray< unsigned int, VDimension > Layout;
    for ( unsigned int i = 0; i < VDimension; i++ )
    {
      Layout[ i ] = m_Layout[ i ];
    }

    /** Cast the defaultvalue. */
    TComponentType defaultValue = static_cast<TComponentType>( m_Defaultvalue );

    /** Create tiler. */
    typename TilerType::Pointer tiler = TilerType::New();
    tiler->SetLayout( Layout );
    tiler->SetDefaultPixelValue( defaultValue );

    /** Read input images and set it into the tiler. */
    for ( unsigned int i = 0; i < m_InputFileNames.size(); i++ )
    {
      typename ImageReaderType::Pointer reader = ImageReaderType::New();
      reader->SetFileName( m_InputFileNames[ i ].c_str() );
      reader->Update();
      tiler->SetInput( i, reader->GetOutput() );
    }

    /** Do the tiling. */
    tiler->Update();

    /** Write to disk. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( tiler->GetOutput() );
    writer->Update();
  }// end Run()

}; // end TileImages


#endif
template<class TComponentType, class VDimension >
class ReplaceVoxel;
