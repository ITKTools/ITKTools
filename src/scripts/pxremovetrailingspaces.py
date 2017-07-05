#!/usr/bin/env python

import fileinput
import sys
import glob
import os.path

#import os
#import shutil
#import re

#from optparse import OptionParser # Deprecated with python 2.7
#import argparse # Requires python 2.7 or greater

#-------------------------------------------------------------------------------
#  This script ...
#

allowable = [ '.txt', '.c', '.cpp', '.cxx', '.h', '.hpp', '.hxx', '.txx' ];

#-------------------------------------------------------------------------------
# the main function
def main() :

  # Get all arguments
  arguments = sys.argv[1:];

  # Construct a list of file names
  # all arguments may contain wild cards
  fileNameList = [];
  for argument in arguments :
    fileNameList.extend( glob.glob( argument ) );

  # Process the list
  for fileName in fileNameList :

    # Check if file exists
    # Probably not needed due to glob:
    if not os.path.exists( fileName ) :
      continue;

    # Check if file is not binary, is readable and writable

    # Check if file extension is allowable
    ( root, extension ) = os.path.splitext( fileName );
    if extension not in allowable :
      print "Skipping " + fileName;
      continue;
    else :
      print "Processing " + fileName;

    for lines in fileinput.FileInput( fileName, inplace = 1 ) :
      # Remove trailing whitespace
      lines = lines.rstrip();
      # Expand tabs
      lines = lines.expandtabs( 2 );
      print lines;

  return 0

#-------------------------------------------------------------------------------
if __name__ == '__main__':
  sys.exit(main())
