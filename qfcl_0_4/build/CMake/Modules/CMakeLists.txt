# build/CMake/Modules/CMakeLists.txt
#
# CMake build rules for the test suite
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

file( GLOB CMake_module_sources *.cmake )

add_custom_target( CMake_modules SOURCES ${CMake_module_sources} )
set_target_properties( CMake_modules PROPERTIES
					   FOLDER CMakeModules )