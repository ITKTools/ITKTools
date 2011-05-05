ITK Tools
==========

Practical command line tools based on the ITK, intended for image processing.
These tools are designed to take an input image from the command line, perform a single operation, and produce an output image. For example smoothing of an image can be done with the tool pxgaussianimagefilter.

Historical note
---------------

This toolkit originates from the toolkit praxix, created by Marius Staring and Stefan Klein. Development started halfway 2005 in a private cvs repository, and was meant to facilitate their PhD work. In March 2008 the repository was moved to svn, and hosted by Google Code at

  http://code.google.com/p/praxix/

The name praxix is related to a Dutch warehouse selling practical tools, and also to the image processing sofware ImageExplorer (iX). The tools are therefore still prefixed with 'px'.

The move to git(hub) in April 2011 under the name ITKTools was inspired by David Doria, who released the ITK\_OneShot repository and sent a mail to the ITK mailing list with the question if anyone had a same type of collection of tools.

Building
--------
- Create a 'bin' directory at the same level as the 'src' directory:
ITKTools]$ mkdir bin

- From the new 'build' directory, run cmake on the source directory:
ITKTools]$ cd bin
bin]$ cmake ../src

- Run 'make' from the 'build' directory
bin]$ make

Conventions
------------
Use --help with any of the programs to display their specific usage information.
Optional and required arguments are listed as [-key] and -key, respectively.

Contributing
------------
Please contact one of the authors or produce a pull request on github.

