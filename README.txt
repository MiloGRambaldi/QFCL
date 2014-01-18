QFCL/random

Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
Copyright (C) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
-----------------------------------------------------------------------------

This is the QFCL random library.

Core functionality requires the following external libraries:
(1) Boost
(2) NTL
It is recommended to build and use our CMake-d version of NTL, available at
http://qfcl.wilmott.com/svn/wilmott/sandbox/MiloRambaldi/WinNTL-5_5_2-CMake.

The complete build also requires 
(3) prf_boost
(4) a sufficiently current version of boost::tti (from boost/trunk)
(5) cartesian_product from http://www.organicvectory.com
For convenience, (3)-(5) are packaged together at
http://qfcl.wilmott.com/svn/wilmott/sandbox/MiloRambaldi/boost_extensions 
(these are headers only). If you have already supplemented your boost 
installation, you can imply set BOOST_EXTENSIONS_ROOT to the root of your 
boost installation in the CMake-GUI.

The following are optional, but are needed to build the documentation
(6) doxygen
(7) A LaTeX installation that is usable by doxygen. 