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
#ifndef __itkMeshFileReaderBase_h_
#define __itkMeshFileReaderBase_h_

#include "itkMeshSource.h"

namespace itk
{

  /** \brief Base exception class for IO conflicts. */
  class MeshFileReaderException : public ExceptionObject
  {
  public:
    /** Run-time information. */
    itkTypeMacro( MeshFileReaderException, ExceptionObject );

    /** Constructor. */
    MeshFileReaderException(const char *file, unsigned int line,
                            const char* message = "Error in IO",
                            const char* loc = "Unknown") :
      ExceptionObject(file, line, message, loc)
    {
    }

    /** Constructor. */
    MeshFileReaderException(const std::string &file, unsigned int line,
                            const char* message = "Error in IO",
                            const char* loc = "Unknown") :
      ExceptionObject(file, line, message, loc)
    {
    }
  };


  /** \class MeshFileReaderBase
   *
   * \brief Base class for mesh readers
   *
   * A base class for classes that read a file containing
   * a mesh or a pointset.
   */

  template <class TOutputMesh>
  class MeshFileReaderBase : public MeshSource<TOutputMesh>
  {
  public:
    /** Standard class typedefs. */
    typedef MeshFileReaderBase        Self;
    typedef MeshSource<TOutputMesh>   Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(MeshFileReaderBase, MeshSource);

    /** Some convenient typedefs. */
    typedef typename Superclass::DataObjectPointer DatabObjectPointer;
    typedef typename Superclass::OutputMeshType    OutputMeshType;
    typedef typename Superclass::OutputMeshPointer OutputMeshPointer;

    /** Set/Get the filename */
    itkGetStringMacro(FileName);
    itkSetStringMacro(FileName);

    /** Prepare the allocation of the output mesh during the first back
     * propagation of the pipeline. */
    virtual void GenerateOutputInformation( void );

    /** Give the reader a chance to indicate that it will produce more
     * output than it was requested to produce. MeshFileReader cannot
     * currently read a portion of a mesh, so the MeshFileReader must
     * enlarge the RequestedRegion to the size of the mesh on disk. */
    virtual void EnlargeOutputRequestedRegion(DataObject *output);

  protected:
    MeshFileReaderBase();
    virtual ~MeshFileReaderBase(){};

    /** Test whether the given filename exist and it is readable,
     this is intended to be called before attempting to use
     subclasses for actually reading the file. If the file
     doesn't exist or it is not readable, and exception with an
     approriate message will be thrown. */
    virtual void TestFileExistanceAndReadability();

    std::string m_FileName;

  private:
    MeshFileReaderBase(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented


  }; // end class

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMeshFileReaderBase.txx"
#endif

#endif // end #ifndef __itkMeshFileReaderBase_h_
