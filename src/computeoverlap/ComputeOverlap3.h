#ifndef ComputeOverlap3_H
#define ComputeOverlap3_H

/** ComputeOverlap */

class ComputeOverlap3Base : public itktools::ITKToolsBase
{ 
public:
  ComputeOverlap3Base(){};
  ~ComputeOverlap3Base(){};

  /** Input parameters */
  std::vector<std::string> m_InputFileNames;
  std::vector<unsigned int> m_Labels;
    
}; // end ComputeOverlap2


template< class TComponentType, unsigned int VDimension >
class ComputeOverlap3 : public ComputeOverlap3Base
{
public:
  typedef ComputeOverlap3 Self;

  ComputeOverlap3(){};
  ~ComputeOverlap3(){};

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
    typedef itk::Image<TComponentType, VDimension>      ImageType;
    typedef itk::ImageFileReader<ImageType>             ImageReaderType;
    typedef typename ImageReaderType::Pointer           ImageReaderPointer;
    typedef itk::DiceOverlapImageFilter<ImageType>      DiceComputeFilter;
    //typedef typename DiceComputeFilter::OverlapMapType  OverlapMapType;
    typedef typename DiceComputeFilter::LabelsType      LabelsType;

    /** Translate vector of labels to set. */
    LabelsType requestedLabels;
    for ( std::size_t i = 0; i < m_Labels.size(); i++ )
    {
      requestedLabels.insert( m_Labels[ i ] );
    }

    /** Create and setup readers. */
    ImageReaderPointer reader1 = ImageReaderType::New();
    reader1->SetFileName( m_InputFileNames[ 0 ].c_str() );
    ImageReaderPointer reader2 = ImageReaderType::New();
    reader2->SetFileName( m_InputFileNames[ 1 ].c_str() );

    /** Create Dice overlap filter. */
    typename DiceComputeFilter::Pointer diceFilter = DiceComputeFilter::New();
    diceFilter->SetInput( 0, reader1->GetOutput() );
    diceFilter->SetInput( 1, reader2->GetOutput() );
    diceFilter->SetRequestedLabels( requestedLabels );
    diceFilter->Update();

    /** Print the results. */
    diceFilter->PrintRequestedDiceOverlaps();
  }

}; // end ComputeOverlap3

#endif