#!/usr/bin/env python

import fileinput
import sys
import subprocess
import glob
import os.path
from optparse import OptionParser

#import os
#import shutil
#import re

#from optparse import OptionParser # Deprecated with python 2.7
#import argparse # Requires python 2.7 or greater

#-------------------------------------------------------------------------------
#  This script ...
#

allowable = [ '.mhd', '.mha', '.gipl', '.tif', '.tiff', '.jpg' ];

#-------------------------------------------------------------------------------
# the main function
def main() :

  # usage, parse parameters
  usage = "usage: %prog [options] arg"
  parser = OptionParser( usage );

  # options
  parser.add_option( "-t", "--opct", dest="opct", help="output (component) type" );
  (options, args) = parser.parse_args();

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
      print( "Skipping " + fileName );
      continue;
    else :
      print( "Processing " + fileName + " ..." );

    # Compress image
    command = [ "pxcastconvert", "-in", fileName, "-out", fileName ];
    if options.opct : command.extend( [ "-opct", options.opct ] );
    #command.extend( [ "-z", ">>", "/dev/null" ] );
    command.extend( [ "-z" ] );
    print( ' '.join( command ) );
    subprocess.call( command );

  return 0

#-------------------------------------------------------------------------------
if __name__ == '__main__':
  sys.exit(main())
