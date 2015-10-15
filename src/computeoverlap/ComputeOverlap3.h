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
#ifndef __ComputeOverlap3_h_
#define __ComputeOverlap3_h_

#include "itkImageFileReader.h"
#include "itkDiceOverlapImageFilter.h"

#include <string>
#include <vector>


/** \class ITKToolsComputeOverlap3Base
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsComputeOverlap3Base : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsComputeOverlap3Base(){};
  /** Destructor. */
  ~ITKToolsComputeOverlap3Base(){};

  /** Input member parameters */
  std::vector<std::string> m_InputFileNames;
  std::vector<unsigned int> m_Labels;

}; // end ITKToolsComputeOverlap3Base


/** \class ITKToolsComputeOverlap3
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsComputeOverlap3 : public ITKToolsComputeOverlap3Base
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsComputeOverlap3 Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsComputeOverlap3(){};
  ~ITKToolsComputeOverlap3(){};

  /** Run function. */
  void Run( void )
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
    for ( std::size_t i = 0; i < this->m_Labels.size(); i++ )
    {
      requestedLabels.insert( this->m_Labels[ i ] );
    }

    /** Create and setup readers. */
    ImageReaderPointer reader1 = ImageReaderType::New();
    reader1->SetFileName( this->m_InputFileNames[ 0 ].c_str() );
    ImageReaderPointer reader2 = ImageReaderType::New();
    reader2->SetFileName( this->m_InputFileNames[ 1 ].c_str() );

    /** Create Dice overlap filter. */
    typename DiceComputeFilter::Pointer diceFilter = DiceComputeFilter::New();
    diceFilter->SetInput( 0, reader1->GetOutput() );
    diceFilter->SetInput( 1, reader2->GetOutput() );
    diceFilter->SetRequestedLabels( requestedLabels );
    diceFilter->SetCoordinateTolerance( 1e-3 );
    diceFilter->SetDirectionTolerance( 1e-3 );
    diceFilter->Update();

    /** Print the results. */
    diceFilter->PrintRequestedDiceOverlaps();

  } // end Run()

}; // end class ITKToolsComputeOverlap3

#endif // end #ifndef __ComputeOverlap3_h_
