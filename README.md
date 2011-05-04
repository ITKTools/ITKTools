ITK Tools
==========
Practical command line tools based on the ITK. These tools are design to do take an input from the command line, perform a a single function, and produce an output.

Historical note
---------------

This toolkit originates from the toolkit praxix, created by Marius Staring and Stefan Klein, which was hosted on Google Code using svn:

  http://code.google.com/p/praxix/

The name praxix is related to a Dutch warehouse selling tools, and also to the image processing sofware ImageExplorer (iX).

The move to git under the name ITKTools was inspired by David Doria, who released the ITK_OneShot repository and sent a mail to the ITK mailing list.

The tools are therefore still prefixed with '``'px'.

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
Use --help with any of the programs to display their specific usage information. Optional arguments are listed as [-key].

Contributing
------------
Please contact one of the authors or produce a pull request on github.
