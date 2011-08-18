ITK Tools
==========

Practical command line tools based on the ITK, intended for image processing.
These tools are designed to take one or more input image(s) from the command line, perform a single operation, and produce an output image. For example smoothing of an image can be done with the tool pxgaussianimagefilter.

Historical note
---------------

This toolkit originates from the toolkit praxix, created by Marius Staring and Stefan Klein. Development started halfway 2005 in a private cvs repository, and was meant to facilitate their PhD work. In March 2008 the repository was moved to svn, and hosted by Google Code at

  http://code.google.com/p/praxix/

The name praxix is related to a Dutch warehouse selling practical tools, and also to the image processing sofware ImageExplorer (iX). The tools are therefore still prefixed with 'px'.

The move to git(hub) in April 2011 under the name ITKTools was inspired by David Doria, who released the ITK\_OneShot repository and sent a mail to the ITK mailing list with the question if anyone had a same type of collection of tools.

Building
--------

Building ITKTools from source requires to first build the ITK. Currently we relky on ITK4 the latest git version, available at git://itk.org/ITK.git.

Linux:

- Create a 'bin' directory at the same level as the 'src' directory:

ITKTools]$ mkdir bin

- From the new 'build' directory, run cmake on the source directory:

ITKTools]$ cd bin

bin]$ cmake ../src

- Run 'make' from the 'build' directory

Windows:

- Create a 'bin' directory at the same level as the 'src' directory

- Run cmake, set the source directory to ITKTools/src and the binary directory to ITKTools/bin, press configure, press generate.

- Open bin/ITKTools.sln, and start the build.

Conventions
-----------

Use --help with any of the programs to display their specific usage information.
Optional and required arguments are listed as [-key] and -key, respectively.

PixelType vs ComponentType
--------------------------

In ITK, PixelType is either SCALAR or VECTOR. ComponentType is the type of each component of the pixels, e.g. float, char, etc.

Contributing
------------

Please contact one of the authors or produce a pull request on github.

Coding style
------------

Good and consistent coding style makes software maintenance easier! Therefore, we have adopted the following style rules:

- Don't use tabs, but two (2) spaces.

- Be explicit, so e.g. use void to declare a function with no arguments, i.e. Function( void ).

- Use full names with CamelCase for variable and function names.

- Use helper functionality from src/common as much as possible.

- Functionality is implemented in a Run() function, which is a member of a class derived from itktools::ITKToolsBase.

- Put a help text function std::string GetHelpString( void ) on top of the main cxx file. Put the implementation is a helper header file, especially if it is large.


Testing
-------

First, set the CMake option ITKTOOLS_BUILD_TESTING=ON while building ITKTools. Then, on Linux, from your build directory, run 'ctest'. This will execute the entire suite of tests. On Windows, type "ctest -C Release", or "ctest -C Debug".

Nightly Dashboard
-----

The nightly CDash dashboard is located at

  http://my.cdash.org/index.php?project=ITKTools

To submit a test, copy and edit a dashboard script from Testing/Dashboard, and call the script using:

ctest -C Release -S path/to/dashboardscript.cmake,TestType -VV

where TestType is Nightly, Experimental, or Continuous.

Sometimes git clone does not work for you and you get something like:

$ git "clone" "-b" "master" "git://github.com/ITKTools/ITKTools.git" "D:/local directory/nightly/ITKTools"

co-out> Cloning into D:/local directory/nightly/ITKTools...

co-err> github.com[0: 207.97.227.239]: errno=No error

co-err> fatal: unable to connect a socket (No error)

This may be caused by your company firewall refusing git protocol traffic. In this case you can resort to the html protocol.
Either replace the dashboard_git_url variable in the dashboard script by:

  set( dashboard_git_url "http://github.com/ITKTools/ITKTools.git" )

or use git config:

  $ git config --global url.http://github.com/ITKTools/.insteadOf git://github.com/ITKTools/

