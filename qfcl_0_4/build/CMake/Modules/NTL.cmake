# CMake include file for NTL library
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

find_package( NTL )
if( NTL_FOUND )		
	include_directories( ${NTL_INCLUDE_DIRS} )
	link_directories( ${NTL_LIBRARY_DIRS} )
	add_library( NTL STATIC IMPORTED )
	set_target_properties( NTL PROPERTIES 
                           IMPORTED_LOCATION ${NTL_libname} )
#    message( ${NTL_libname} )
else()
	message( SEND_ERROR "NTL not found" )
endif()
