#ifndef __statisticsprinters_h
#define __statisticsprinters_h


#include <fstream>

/** this file defines two function that print statistics information */

/**
 * Print the results of an itk::StatisticsImageFilter
 */

template<class TStatisticsFilter>
void PrintStatistics( const TStatisticsFilter * statistics ) 
{
  /** Print to screen. */
	std::cout << "\tmin: \t" << statistics->GetMinimum() << std::endl;
  std::cout << "\tmax: \t" << statistics->GetMaximum() << std::endl;
	std::cout << "\tmean:\t" << statistics->GetMean() << std::endl;
	std::cout << "\tstd: \t" << statistics->GetSigma() << std::endl;
	std::cout << "\tvar: \t" << statistics->GetVariance() << std::endl;
  std::cout << "\tsum: \t" << statistics->GetSum() << std::endl;

} // end PrintStatistics


/**
 * Print histogram statistics
 */

template<class THistogram>
void PrintHistogramStatistics( const THistogram * histogram, 
                               const std::string & histogramOutputFileName ) 
{
  /** Print to screen. */
	//median, quartiles, histogram, percentiles.

  typedef typename THistogram::FrequencyType FrequencyType;
  typename THistogram::TotalFrequencyType nrOfPixels = histogram->GetTotalFrequency();
  double median = histogram->Quantile(0, 0.5);
  double firstquartile = histogram->Quantile(0,0.25);
  double thirdquartile = histogram->Quantile(0,0.75);

  std::cout << "\tnumber of pixels:\t" << nrOfPixels << std::endl;
  std::cout << "\tmedian:          \t" << median << std::endl;
  std::cout << "\t1st quartile:    \t" << firstquartile << std::endl;
  std::cout << "\t3rd quartile:    \t" << thirdquartile << std::endl;

  /** print histogram to output file */
  if (histogramOutputFileName != "")
  {
    std::cout << "\tHistogram is written to file: " <<
      histogramOutputFileName << " ..." << std::endl;
    std::ofstream histogramOutputFile;
    histogramOutputFile.open( histogramOutputFileName.c_str() );
		if ( !histogramOutputFile.is_open() )
		{
      itkGenericExceptionMacro(<< "ERROR: Output file for histogram cannot be opened!");
    }
    histogramOutputFile << std::fixed;
    histogramOutputFile << std::showpoint;
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
      FrequencyType freq = histogram->GetFrequency(i,0);
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
    std::cout << "\tDone writing histogram to file." << std::endl;
  } // end if

} // end PrintHistogramStatistics


#endif // #ifndef __statisticsprinters_h

