# tests/CMakeLists.txt
#
# CMake build rules for the test suite
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

file( GLOB test_files *.cpp )

foreach( test IN LISTS test_files )
	string( FIND ${test} "/" pos REVERSE )
	string( SUBSTRING ${test} ${pos} -1 test_name )
	string( SUBSTRING ${test_name} 1 -1 test_name )
	string( FIND ${test_name} "." pos REVERSE )
	string( SUBSTRING ${test_name} 0 ${pos} test_name )

	set( source_files ${test} )
	set( resource_files "" )
	if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}In )
		set( resource_files ${resource_files} ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}In )
	endif()
	if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}Out )
		set( resource_files ${resource_files} ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}Out )
	endif()
	add_executable( ${test_name} ${source_files} ${resource_files} )
	set_target_properties( ${test_name} PROPERTIES 
						   COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS}
						   FOLDER tests )
	source_group( "Source Files" FILES ${source_files} )
	source_group( "Resource Files" FILES ${resource_files} )
	get_target_property( RUNTIME_DIRECTORY ${test_name} RUNTIME_OUTPUT_DIRECTORY )
	set( link_libraries NTL )
	target_link_libraries( ${test_name} ${link_libraries} )
	add_custom_command( TARGET ${test_name} PRE_BUILD 
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/copy_resources.bat ${test_name} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR} 
		COMMENT "Copying resource files to runtime directory ..." VERBATIM )
endforeach( test )
