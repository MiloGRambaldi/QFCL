# FindBoost_Extensions.cmake
#
# CMake module for find_package
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

find_path( BOOST_EXTENSIONS_ROOT_DIR 
		   NAMES FindBoost_Extensions.cmake
		   PATHS ${BOOST_ROOT} ${BOOST_EXTENSIONS_ROOT} $ENV{BOOST_ROOT} 
		   PATH_SUFFIXES Modules )

set( ERROR_MESSAGE "Cannot find the Boost_Extensions installation. Please set BOOST_EXTENSIONS_ROOT to the root directory containing Boost_Extensions." )
mark_as_advanced( BOOST_EXTENSIONS_ROOT_DIR )
if( BOOST_EXTENSIONS_ROOT_DIR STREQUAL BOOST_EXTENSIONS_ROOT_DIR-NOTFOUND )
	message( SEND_ERROR ${ERROR_MESSAGE} )
endif()

string( FIND ${BOOST_EXTENSIONS_ROOT_DIR} "/Modules" pos REVERSE )
string( SUBSTRING ${BOOST_EXTENSIONS_ROOT_DIR} 0 ${pos} Boost_Extensions_INCLUDE_DIRS  )
