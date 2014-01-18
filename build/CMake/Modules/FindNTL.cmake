# FindNTL.cmake
#
# CMake module for find_package
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

if( CMAKE_HOST_WIN32 )
    set( NTL_libname NTL.lib )
    set( NTL_filename NTL.lib )
    set( NTL_subdirname "Release/" )
else( CMAKE_HOST_WIN32 )
    set( NTL_libname "-lNTL" )
    set( NTL_filename libNTL.a )
    set( NTL_subdirname "" )
endif( CMAKE_HOST_WIN32)
find_path( NTL_ROOT_DIR 
           NAMES lib/${NTL_subdirname}${NTL_filename}
           PATHS ${NTL_ROOT} $ENV{NTL_ROOT} )
#           PATH_SUFFIXES lib )

set( ERROR_MESSAGE "Cannot find the NTL installation. Please set NTL_ROOT to the root directory containing NTL." )
mark_as_advanced( NTL_ROOT_DIR )
if( NTL_ROOT_DIR STREQUAL NTL_ROOT_DIR-NOTFOUND )
	message( SEND_ERROR ${ERROR_MESSAGE} )
else()
	set( NTL_FOUND TRUE )
endif()

set( NTL_INCLUDE_DIRS ${NTL_ROOT_DIR}/include )
set( NTL_LIBRARY_DIRS ${NTL_ROOT_DIR}/lib )
