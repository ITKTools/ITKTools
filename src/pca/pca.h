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
#ifndef __pca_h_
#define __pca_h_

#include "ITKToolsBase.h"

#include <itksys/SystemTools.hxx>
#include <sstream>
#include "itkPCAImageToImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


/** \class ITKToolsPCABase
 *
 * Untemplated pure virtual base class that holds
 * the Run() function and all required parameters.
 */

class ITKToolsPCABase : public itktools::ITKToolsBase
{
public:
  /** Constructor. */
  ITKToolsPCABase()
  {
    this->m_OutputDirectory = "";
    this->m_NumberOfPCs = 0;
  };
  /** Destructor. */
  ~ITKToolsPCABase(){};

  /** Input member parameters. */
  std::vector< std::string > m_InputFileNames;
  std::string m_OutputDirectory;
  unsigned int m_NumberOfPCs;

}; // end class ITKToolsPCABase


/** \class ITKToolsPCA
 *
 * Templated class that implements the Run() function
 * and the New() function for its creation.
 */

template< unsigned int VDimension, class TComponentType >
class ITKToolsPCA : public ITKToolsPCABase
{
public:
  /** Standard ITKTools stuff. */
  typedef ITKToolsPCA Self;
  itktoolsOneTypeNewMacro( Self );

  ITKToolsPCA(){};
  ~ITKToolsPCA(){};

  /** Run function. */
  void Run( void )
  {
    /** Typedefs. */
    typedef itk::Image< TComponentType, VDimension >      OutputImageType;
    typedef itk::Image< double, VDimension >              DoubleImageType;
    typedef itk::PCAImageToImageFilter<
      DoubleImageType, OutputImageType >                  PCAEstimatorType;
    typedef typename PCAEstimatorType::VectorOfDoubleType VectorOfDoubleType;
    typedef typename PCAEstimatorType::MatrixOfDoubleType MatrixOfDoubleType;
    typedef itk::ImageFileReader< DoubleImageType >       ReaderType;
    typedef typename ReaderType::Pointer                  ReaderPointer;
    typedef itk::ImageFileWriter< OutputImageType >       WriterType;
    typedef typename WriterType::Pointer                  WriterPointer;

    /** Get some sizes. */
    unsigned int noInputs = this->m_InputFileNames.size();

    /** Create the PCA estimator. */
    typename PCAEstimatorType::Pointer pcaEstimator = PCAEstimatorType::New();
    pcaEstimator->SetNumberOfFeatureImages( noInputs );
    pcaEstimator->SetNumberOfPrincipalComponentsRequired( this->m_NumberOfPCs );

    /** For all inputs... */
    std::vector<ReaderPointer> readers( noInputs );
    for( unsigned int i = 0; i < noInputs; ++i )
    {
      /** Read in the input images. */
      readers[ i ] = ReaderType::New();
      readers[ i ]->SetFileName( this->m_InputFileNames[ i ] );
      readers[ i ]->Update();

      /** Setup PCA estimator. */
      pcaEstimator->SetInput( i, readers[ i ]->GetOutput() );
    }

    /** Do the PCA analysis. */
    pcaEstimator->Update();

    /** Get eigenvalues and vectors, and print it to screen. */
    //pcaEstimator->Print( std::cout );
    VectorOfDoubleType vec = pcaEstimator->GetEigenValues();
    MatrixOfDoubleType mat = pcaEstimator->GetEigenVectors();

    std::cout << "Eigenvalues: " << std::endl;
    for( unsigned int i = 0; i < vec.size(); ++i )
    {
      std::cout << vec[ i ] << " ";
    }
    std::cout << std::endl;

    std::cout << "Eigenvectors: " << std::endl;
    for( unsigned int i = 0; i < vec.size(); ++i )
    {
      std::cout << mat.get_row( i ) << std::endl;
    }

    /** Setup and process the pipeline. */
    unsigned int noo = pcaEstimator->GetNumberOfOutputs();
    std::vector<WriterPointer> writers( noo );
    for( unsigned int i = 0; i < noo; ++i )
    {
      /** Create output filename. */
      std::ostringstream makeFileName( "" );
      makeFileName << this->m_OutputDirectory << "pc" << i << ".mhd";

      /** Write principal components. */
      writers[ i ] = WriterType::New();
      writers[ i ]->SetFileName( makeFileName.str().c_str() );
      writers[ i ]->SetInput( pcaEstimator->GetOutput( i ) );
      writers[ i ]->Update();
    }
  } // end Run()

}; // end class ITKToolsPCA


#endif // end #ifndef __pca_h_
