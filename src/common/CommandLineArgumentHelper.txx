#ifndef COMMANDLINEARGUMENTHELPER_TXX
#define COMMANDLINEARGUMENTHELPER_TXX

/**
 * *************** ConvertImageInformationToITKTypes ***********************
 */

template<unsigned int Dimension>
void ConvertImageInformationToITKTypes(
  const std::vector<unsigned int> & size,
  const std::vector<double> & spacing,
  const std::vector<double> & origin,
  const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::SizeType      sizeITK,
  typename itk::ImageBase<Dimension>::SpacingType   spacingITK,
  typename itk::ImageBase<Dimension>::PointType     originITK,
  typename itk::ImageBase<Dimension>::DirectionType directionITK )
{
  typedef itk::ImageBase<Dimension> ImageBaseType;
  typedef typename ImageBaseType::SizeValueType      SizeValueType;

  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    sizeITK[ i ] = static_cast<SizeValueType>( size[ i ] );
    spacingITK[ i ] = spacing[ i ];
    originITK[ i ] = origin[ i ];
  }

  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    for ( unsigned int j = 0; j < Dimension; j++ )
    {
      directionITK[ i ][ j ] = direction[ j + i * Dimension ];
    }
  }

} // end ConvertImageInformationToITKTypes()


/** Convert a vector to an itk::Size. */
template< unsigned int Dimension>
void ConvertVectorToSize(const std::vector<unsigned int> & size,
  typename itk::ImageBase<Dimension>::SizeType      sizeITK)
{
  typedef itk::ImageBase<Dimension> ImageBaseType;
  typedef typename ImageBaseType::SizeValueType      SizeValueType;
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    sizeITK[ i ] = static_cast<SizeValueType>( size[ i ] );
  }
}

/** Convert vector to an ITK spacing type. */
template< unsigned int Dimension>
void ConvertVectorToSpacing(const std::vector<double> & spacing,
  typename itk::ImageBase<Dimension>::SpacingType   spacingITK)
{
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    spacingITK[ i ] = spacing[ i ];
  }
}

/** Convert vector to an ITK point type. */
template< unsigned int Dimension>
void ConvertVectorToOrigin(const std::vector<double> & origin,
  typename itk::ImageBase<Dimension>::PointType     originITK)
{
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    originITK[ i ] = origin[ i ];
  }
}

/** Convert vector to an ITK direction type. */
template< unsigned int Dimension>
void ConvertVectorToDirection(const std::vector<double> & direction,
  typename itk::ImageBase<Dimension>::DirectionType directionITK)
{
  for ( unsigned int i = 0; i < Dimension; i++ )
  {
    for ( unsigned int j = 0; j < Dimension; j++ )
    {
      directionITK[ i ][ j ] = direction[ j + i * Dimension ];
    }
  }
}

#endif