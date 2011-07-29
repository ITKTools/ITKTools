#ifndef TILEIMAGES2D3D_H
#define TILEIMAGES2D3D_H

#include "ITKToolsBase.h"

/** TileImages2D3D */

class TileImages2D3DBase : public itktools::ITKToolsBase
{ 
public:
  TileImages2D3DBase(){};
  ~TileImages2D3DBase(){};

  /** Input parameters */
  std::vector<std::string> m_InputFileNames;
  std::string m_OutputFileName;
  double m_Zspacing;
    
}; // end TileImages2D3DBase


template< class TComponentType >
class TileImages2D3D : public TileImages2D3DBase
{
public:
  typedef TileImages2D3D Self;

  TileImages2D3D(){};
  ~TileImages2D3D(){};

  static Self * New( itktools::ComponentType componentType )
  {
    if ( itktools::IsType<TComponentType>( componentType ) )
    {
      return new Self;
    }
    return 0;
  }

  void Run(void)
  {
    /** Define image type. */
    const unsigned int Dimension = 3;

    /** Some typedef's. */
    typedef itk::Image<TComponentType, Dimension>            ImageType;
    typedef typename ImageType::SpacingType             SpacingType;
    typedef itk::ImageSeriesReader<ImageType>           ImageSeriesReaderType;
    typedef itk::ImageFileWriter<ImageType>             ImageWriterType;

    /** Create reader. */
    typename ImageSeriesReaderType::Pointer reader = ImageSeriesReaderType::New();
    reader->SetFileNames( m_InputFileNames );

    /** Update the reader. */
    std::cout << "Input images are read..." << std::endl;
    reader->Update();
    std::cout << "Reading done." << std::endl;
    typename ImageType::Pointer tiledImage = reader->GetOutput();

    /** Get and set the spacing, if it was set by the user. */
    if ( m_Zspacing > 0.0 )
    {
      /** Make sure that changes are not undone */
      tiledImage->DisconnectPipeline();
      /** Set the zspacing */
      SpacingType spacing = tiledImage->GetSpacing();
      spacing[ 2 ] = m_Zspacing;
      tiledImage->SetSpacing( spacing );
    }

    /** Write to disk. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( m_OutputFileName.c_str() );
    writer->SetInput( tiledImage );
    std::cout << "Writing tiled image..." << std::endl;
    writer->Update();
    std::cout << "Ready." << std::endl;
  }

}; // end TileImages2D3D

#endif
