#ifndef __statisticsprinters_h
#define __statisticsprinters_h


#include <fstream>
#include <iomanip>

/** this file defines three function that print statistics information */

/**
 * Print the results of an itk::StatisticsImageFilter
 */

template<class TStatisticsFilter>
void PrintStatistics( const TStatisticsFilter * statistics )
{
  /** Print to screen. */
  std::cout << std::setprecision(10);
  std::cout << "\tmin             : " << statistics->GetMinimum() << std::endl;
  std::cout << "\tmax             : " << statistics->GetMaximum() << std::endl;
  std::cout << "\tarithmetic mean : " << statistics->GetMean() << std::endl;
  std::cout << "\tarithmetic stdev: " << statistics->GetSigma() << std::endl;
  std::cout << "\tarithmetic var  : " << statistics->GetVariance() << std::endl;
  std::cout << "\tsum             : " << statistics->GetSum() << std::endl;

} // end PrintStatistics()


/**
 * Print the results of an itk::StatisticsImageFilter
 * Assume that statistics were calculated on the log
 * of the actual image. exp gives the Geometric mean.
 */

template<class TStatisticsFilter>
void PrintGeometricStatistics( const TStatisticsFilter * statistics )
{
  /** Print to screen. */
  std::cout << std::setprecision(10);
  double geometricmean = vcl_exp( statistics->GetMean() );
  double geometricstdev = vcl_exp( statistics->GetSigma() );
  std::cout << "\tgeometric mean : " << geometricmean << std::endl;
  std::cout << "\tgeometric stdev: " << geometricstdev << std::endl;

} // end PrintGeometricStatistics()


/**
 * Print histogram statistics
 */

template<class THistogram>
void PrintHistogramStatistics( const THistogram * histogram,
  const std::string & histogramOutputFileName )
{
  /** Print to screen. */
  //median, quartiles, histogram, percentiles.

  typedef typename THistogram::AbsoluteFrequencyType AbsoluteFrequencyType;
  typename THistogram::TotalAbsoluteFrequencyType nrOfPixels = histogram->GetTotalFrequency();
  double median = histogram->Quantile(0, 0.5);
  double fifteenthpercentile = histogram->Quantile( 0, 0.15 );
  double firstquartile = histogram->Quantile( 0, 0.25 );
  double thirdquartile = histogram->Quantile( 0, 0.75 );
  double binsize = histogram->GetBinMax( 0, 0 ) - histogram->GetBinMin( 0, 0 );

  std::cout << std::setprecision( 10 );
  std::cout << "\tnumber of pixels:\t" << nrOfPixels << std::endl;
  std::cout << "\tbinsize:         \t" << binsize << std::endl;
  std::cout << "\tmedian:          \t" << median << std::endl;
  std::cout << "\t1st quartile:    \t" << firstquartile << std::endl;
  std::cout << "\t3rd quartile:    \t" << thirdquartile << std::endl;
  std::cout << "\t15th percentile: \t" << fifteenthpercentile << std::endl;

  /** Print histogram to output file */
  if ( histogramOutputFileName != "" )
  {
    std::cout << "Histogram is written to file: " <<
      histogramOutputFileName << " ..." << std::endl;
    std::ofstream histogramOutputFile;
    histogramOutputFile.open( histogramOutputFileName.c_str() );
    if ( !histogramOutputFile.is_open() )
    {
      itkGenericExceptionMacro(<< "ERROR: Output file for histogram cannot be opened!");
    }
    histogramOutputFile << std::fixed;
    histogramOutputFile << std::showpoint;
    histogramOutputFile << std::setprecision(16);
    histogramOutputFile
      << "nr"
      << "\t"
      << "min"
      << "\t"
      << "max"
      << "\t"
      << "freq"
      << "\t"
      << "prob"
      << std::endl;
    for (unsigned long i = 0; i < histogram->GetSize(0); ++i)
    {
      AbsoluteFrequencyType freq = histogram->GetFrequency(i,0);
      double prob = static_cast<double>(freq) / static_cast<double>(nrOfPixels);
      histogramOutputFile
        << i
        << "\t"
        << histogram->GetBinMin(0,i)
        << "\t"
        << histogram->GetBinMax(0,i)
        << "\t"
        << freq
        << "\t"
        << prob
        << std::endl;
    } // end for
    histogramOutputFile.close();
    std::cout << "Done writing histogram to file." << std::endl;
  } // end if

} // end PrintHistogramStatistics()


#endif // #ifndef __statisticsprinters_h

