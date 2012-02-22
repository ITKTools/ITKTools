/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef __ComputeOverlap2_h_
#define __ComputeOverlap2_h_

/** ComputeOverlap2 */

// Seems not to be used ???

class ComputeOverlap2Base : public itktools::ITKToolsBase
{
public:
  ComputeOverlap2Base(){};
  ~ComputeOverlap2Base(){};

  /** Input member parameters. */
  std::vector<std::string> this->m_InputFileNames;
  std::vector<unsigned int> m_Labels;

}; // end ComputeOverlap2


template< unsigned int VDimension, class TComponentType >
class ComputeOverlap2 : public ComputeOverlap2Base
{
public:
  typedef ComputeOverlap2 Self;

  ComputeOverlap2(){};
  ~ComputeOverlap2(){};

  static Self * New( itktools::EnumComponentType componentType, unsigned int dim )
  {
    if( itktools::IsType<ComponentType>( componentType ) && Dimension == dim )
    {
      return new Self;
    }
    return 0;
  }

  /** Run function. */
  void Run( void )
  {
    /** Some typedef's. */
    typedef TImage                                      ImageType;
    typedef typename ImageType::Pointer                 ImagePointer;
    typedef typename ImageType::PixelType               PixelType;
    typedef itk::ImageFileReader<ImageType>             ImageReaderType;
    typedef typename ImageReaderType::Pointer           ImageReaderPointer;
    typedef itk::ImageRegionConstIterator<ImageType>    IteratorType;
    typedef std::map<PixelType, std::size_t>            OverlapMapType;
    typedef std::set<PixelType>                         LabelsType;

    /** Translate vector of labels to set. */
    LabelsType labels;
    for ( std::size_t i = 0; i < labelsArg.size(); i++ )
    {
      labels.insert( labelsArg[ i ] );
    }

    /**
    * Setup pipeline
    */

    /** Create readers. */
    ImageReaderPointer reader1 = ImageReaderType::New();
    reader1->SetFileName( inputFileNames[ 0 ].c_str() );
    reader1->Update();
    ImageReaderPointer reader2 = ImageReaderType::New();
    reader2->SetFileName( inputFileNames[ 1 ].c_str() );
    reader2->Update();

    ImagePointer imA = reader1->GetOutput();
    ImagePointer imB = reader2->GetOutput();

    /** Create iterators. */
    IteratorType itA( imA, imA->GetLargestPossibleRegion() );
    IteratorType itB( imB, imB->GetLargestPossibleRegion() );
    itA.GoToBegin();
    itB.GoToBegin();

    /** Determine size of objects, and size in the overlap. */
    OverlapMapType sumA, sumB, sumC;
    while ( !itA.IsAtEnd() )
    {
      PixelType A = itA.Value();
      PixelType B = itB.Value();

      sumA[ A ]++;
      sumB[ B ]++;
      if( A == B  ) ++sumC[ A ];

      ++itA; ++itB;
    }

    /** Check if all requested labels exist. */
    for ( typename LabelsType::const_iterator itL = labels.begin(); itL != labels.end(); itL++ )
    {
      if( sumA.count( *itL ) == 0 && sumB.count( *itL ) == 0 )
      {
	itkGenericExceptionMacro( << "The selected label "
	  << static_cast<std::size_t>( *itL ) << " does not exist in both input images." );
      }
    }

    /** Calculate and print the overlap. */
    std::cout << "label => sum input1 \t, sum input2 \t, sum overlap \t, overlap" << std::endl;
    std::map<PixelType, double>     overlap;
    typename OverlapMapType::const_iterator  it;
    for ( it = sumA.begin() ; it != sumA.end(); it++ )
    {
      PixelType currentLabel = (*it).first;

      /** Skip the current label if not selected by user.
      * Print all labels if nothing is selected.
      */
      if( labels.size() != 0 && labels.count( currentLabel ) == 0 )
      {
	continue;
      }

      /** Compute overlap. */
      const std::size_t sumAB = sumA[ currentLabel ] + sumB[ currentLabel ];
      if( sumAB == 0 )
      {
	overlap[ currentLabel ] = 0.0;
      }
      else
      {
	overlap[ currentLabel ]
	  = static_cast<double>( 2 * sumC[ currentLabel ] )
	  / static_cast<double>( sumAB );
      }

      /** Print information. */
      std::cout << static_cast<std::size_t>( currentLabel ) << " => "
	<< sumA[ currentLabel ]
	<< "\t, " << sumB[ currentLabel ]
	<< "\t, " << sumC[ currentLabel ]
	<< "\t, " << overlap[ currentLabel ] << std::endl;
    }
  }

}; // end ComputeOverlap2

#endif // end #ifndef __ComputeOverlap2_h_
