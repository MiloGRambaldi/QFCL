boost_extensions

Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
-----------------------------------------------------------------------------

This is a packaging of three boost "extensions":

(1) John Salmon's prf_boost code, obtained from 
http://thesalmons.org/john/random123/prf_boost/prf_boost.zip.

(2) George van Venrooij's cartesian_product (2008), obtained from 
http://www.organicvectory.com.

(3) boost::tti from the boost/trunk (version 1.53).

The present version simply provides the source code, and nothing is built;
however, these are all header only libraries. There is no CMakeLists file 
yet. It remains to create a CMake target for the boost/libs subdirectories.

Modules/FindBoost_Extensions.cmake is the find_package module for using
boost_extensions with external CMake builds.
