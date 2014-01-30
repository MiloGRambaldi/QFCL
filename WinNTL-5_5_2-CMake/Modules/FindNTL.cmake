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

find_path( NTL_ROOT_DIR 
		   NAMES lib/Release/NTL.lib
		   PATHS ${NTL_ROOT} $ENV{NTL_ROOT} )

set( ERROR_MESSAGE "Cannot find the NTL installation. Please set NTL_ROOT to the root directory containing NTL." )
mark_as_advanced( NTL_ROOT_DIR )
if( NTL_ROOT_DIR STREQUAL NTL_ROOT_DIR-NOTFOUND )
	message( SEND_ERROR ${ERROR_MESSAGE} )
else()
	set( NTL_FOUND TRUE )
endif()

set( NTL_INCLUDE_DIRS ${NTL_ROOT_DIR}/include )
set( NTL_LIBRARY_DIRS ${NTL_ROOT_DIR}/lib )
