# This project is designed to be built outside the Insight source tree.
PROJECT( fftimage )

OPTION( USE_FFTIMAGE "Build pxfftimage only if ITK was built with FFTW (float AND double )" OFF )

IF( USE_FFTIMAGE )
  ADD_ITKTOOL( fftimage )
ENDIF( USE_FFTIMAGE )

