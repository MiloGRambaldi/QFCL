# CMake include file for boost libraries
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

# not needed for most recent version of CMake
set( Boost_ADDITIONAL_VERSIONS "1.53.0" "1.52.0" "1.51.0" "1.50.0" "1.49.0" "1.48.0" "1.47.0" )

set( Boost_NO_BOOST_CMAKE ON )
set( Boost_USE_STATIC_LIBS ON CACHE BOOL "Whether to use static boost libraries" )
set( Boost_USE_STATIC_RUNTIME OFF )
set( Boost_NO_SYSTEM_PATHS ON CACHE BOOL "Turn OFF if your boost library is in the system path" )
#message( STATUS ${MSVC90} )
#message( STATUS ${MSVC10} )
#message( STATUS ${MSVC11} )
find_package( Boost 1.47.0 REQUIRED chrono program_options signals system timer unit_test_framework )
#message( ${Boost_LIB_PREFIX}boost_${COMPONENT}${_boost_COMPILER}${_boost_MULTITHREADED}${_boost_RELEASE_ABI_TAG}-${Boost_LIB_VERSION} )
if( Boost_FOUND )				
#    message( "Boost_LIBRARY_NAMES:" ${Boost_LIBRARY_NAMES} )
    include_directories( ${Boost_INCLUDE_DIRS} )
	link_directories( ${Boost_LIBRARY_DIRS} )
else()
	message( SEND_ERROR "Boost not found" )
endif()
mark_as_advanced( CLEAR BOOST_ROOT )

# clean up cache if Boost_DIR is not used
if( "${Boost_DIR}" STREQUAL "Boost_DIR-NOTFOUND" )
	unset( Boost_DIR CACHE )
endif()

if( CMAKE_HOST_UNIX )
	# also need librt for static builds
	set( Boost_LIBRARIES ${Boost_LIBRARIES} "-lrt" )
	# FindBoost does not get the order correct for static builds; hence, we use -Xlinker.
	set( xlinker "-Xlinker" )
	foreach( LIBRARY ${Boost_LIBRARIES} )
		set( _Boost_LIBRARIES ${_Boost_LIBRARIES} ${xlinker} ${LIBRARY} )
	endforeach( LIBRARY )
	set( Boost_LIBRARIES ${xlinker} "-start-group" ${_Boost_LIBRARIES} ${xlinker} "-end-group" )
	unset( _Boost_LIBRARIES )
endif( CMAKE_HOST_UNIX )

#message( ${Boost_LIBRARIES} )
