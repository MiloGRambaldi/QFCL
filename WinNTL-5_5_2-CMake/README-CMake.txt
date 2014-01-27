NTL CMake

Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
-----------------------------------------------------------------------------

This is the WinNTL distribution, version 5.2.2, with the addition of a CMake
build system. The WinNTL distribution has exactly the same library source
code as the unix NTL distribution, and the packaging seems more suitable for 
CMake. 

It is still work in progress, as the CMake build system is incomplete. The 
present version does include a target for the main NTL library. It also 
includes a target for the test suite. What remains to be added to the build 
system are performance tests, the ability to set compile time configurations
and support for GMP.

Modules/FindNTL.cmake is the find_package module for using NTL with external
CMake builds.

NOTES
-----

(1) This was tested with MSVC 10.