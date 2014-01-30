##########################################################################
# CMakeLists.txt                                                         #
#                                                                        #
# CMake Build Rules for QFCL                                             #
##########################################################################
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>         #
#                                                                        #
# Use, modification and distribution are subject to                      #
# the BOOST Software License, Version 1.0.                               #
# (See accompanying file LICENSE.txt)                                    #
##########################################################################

cmake_minimum_required( VERSION 2.8.12 )
set( BUILD_TYPE_INFO "Choose the type of build, options are: None(CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used), Release, Debug, RelWithDebInfo, MinSizeRel." )
set( CMAKE_BUILD_TYPE "Release" CACHE STRING ${BUILD_TYPE_INFO} )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/CMake/Modules" )
project( QFCL )

# ------------------------------------------------------------------------
# COMPILER PREPROCESSING
# ------------------------------------------------------------------------

# to prevent MS specific warning about unsafe STL functions
set( PREPROCESSOR_DEFINITIONS ${PREPROCESSOR_DEFINITIONS} _SCL_SECURE_NO_WARNINGS )

# ------------------------------------------------------------------------
# COMPILER FLAGS
# ------------------------------------------------------------------------

if( CMAKE_HOST_UNIX )
	# option( What "who?" ON )
	#  mark_as_advanced( CLEAR CMAKE_CXX_FLAGS )
    set( QFCL_CXX_FLAGS "-std=c++0x" CACHE STRING "C++ compiler flags" )
    set( CMAKE_CXX_FLAGS ${QFCL_CXX_FLAGS} )
	#   set( CMAKE_CXX_FLAGS "-std=c++0x" CACHE STRING "C++ compiler flags" )
endif( CMAKE_HOST_UNIX )

# ------------------------------------------------------------------------
# EXTERNAL LIBRARIES
# ------------------------------------------------------------------------

#-----------------------------------------------
# Boost
#-----------------------------------------------

# set BOOST_ROOT to your boost installation directory 
# if CMake has trouble finding boost (can be done from CMake GUI)
set( BOOST_ROOT_DOCSTRING "boost installation directory: set if CMake has trouble finding boost" )
set( BOOST_ROOT $ENV{Boost_DIR} CACHE PATH ${BOOST_ROOT_DOCSTRING} )
# whether to use Boost_Extensions (needed to build all examples)
option( BOOST_USE_EXTENSIONS "Use boost extensions (currently needed for some tests and examples)?" ON )

#-----------------------------------------------
# Eigen3
#-----------------------------------------------

# set EIGEN3_ROOT to your Eigen3 installation directory 
# if CMake has trouble finding Eigen3 (can be done from CMake GUI)
# set( EIGEN3_ROOT_DOCSTRING "Eigen3 installation directory: set if CMake has trouble finding Eigen3" )
# set( EIGEN3_ROOT $ENV{EIGEN3_DIR} CACHE PATH ${EIGEN3_ROOT_DOCSTRING} )

#-----------------------------------------------
# NTL
#-----------------------------------------------

# set NTL_ROOT to your NTL installation directory
# if CMake has trouble finding NTL (can be done from CMake GUI)
set( NTL_ROOT_DOCSTRING "NTL installation directory: set if CMake has trouble finding NTL" )
set( NTL_ROOT_DEFAULT "../WinNTL-5_5_2-CMake" )
if( DEFINED $ENV{NTL_DIR} )
    set( NTL_ROOT $ENV{NTL_DIR} CACHE PATH ${NTL_ROOT_DOCSTRING} )
else( DEFINED $ENV{NTL_DIR} )
    set( NTL_ROOT ${NTL_ROOT_DEFAULT} CACHE PATH ${NTL_ROOT_DOCSTRING} )
endif( DEFINED $ENV{NTL_DIR} )

# ------------------------------------------------------------------------
# INCLUDES
# ------------------------------------------------------------------------

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include( Boost )

# boost extensions
if( BOOST_USE_EXTENSIONS )
	set( BOOST_EXTENSIONS_ROOT_DOCSTRING "Boost_Extensions installation directory: set if CMake has trouble finding Boost_Extensions" )
	set( BOOST_EXTENSIONS_ROOT_DEFAULT "../boost_extensions" )
	if( DEFINED $ENV{Boost_Extensions_ROOT} )
		set( BOOST_EXTENSIONS_ROOT $ENV{Boost_Extensions_ROOT} CACHE PATH ${BOOST_EXTENSIONS_ROOT_DOCSTRING} )
	else( DEFINED $ENV{Boost_Extensions_ROOT} )
		set( BOOST_EXTENSIONS_ROOT ${BOOST_EXTENSIONS_ROOT_DEFAULT} CACHE PATH ${BOOST_EXTENSIONS_ROOT_DOCSTRING} )
	endif( DEFINED $ENV{Boost_Extensions_ROOT} )
	find_package( Boost_Extensions )
	include_directories( ${Boost_Extensions_INCLUDE_DIRS} )
	mark_as_advanced( CLEAR BOOST_EXTENSIONS_ROOT )
else( BOOST_USE_EXTENSIONS )
	mark_as_advanced( FORCE BOOST_EXTENSIONS_ROOT )
endif( BOOST_USE_EXTENSIONS )

include( NTL )
#message( "NTL_ROOT:" ${NTL_ROOT} )


# ----------------------------------------------
# add build rules for test suite
# ----------------------------------------------

add_subdirectory( test )

# ----------------------------------------------
# add build rules for examples
# ----------------------------------------------

add_subdirectory( examples )

# ----------------------------------------------
# add build rules for qfcl library source code
# ----------------------------------------------

include( Source )

# ----------------------------------------------
# add build rules for documentation
# ----------------------------------------------

add_subdirectory( doc )

# ----------------------------------------------
# add CMake Modules (for IDE, convenience only)
# ----------------------------------------------

add_subdirectory( build/CMake )

# ----------------------------------------------
# add READMES
# ----------------------------------------------

add_custom_target( README SOURCES "README.txt" "../README.md" )

# ----------------------------------------------
# add LICENSE
# ----------------------------------------------

add_custom_target( LICENSE SOURCES "LICENSE.txt" )

# ----------------------------------------------
# CMake testing
# ----------------------------------------------

enable_testing()
