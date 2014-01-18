# test/CMakeLists.txt
#
# CMake build rules for the test suite
#
# Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

# ----------------------------------------------
# INCLUDES
# ----------------------------------------------

# ----------------------------------------------
# LIBRARIES
# ----------------------------------------------

# BoostUnitTestFramework
#
# The new unit test API is not backwards compatible,
# and the prebuilt libraries I've seen are still using the original API.
# However, with automatic test registration, the API is not used explicity. 
# Hence it is optional whether to rebuild boost's unit_test_framework.

set( QFCL_NEW_UNIT_TEST_FRAMEWORK_API FALSE CACHE BOOL "Whether to rebuild boost's unit_test_framework library using the new API" )

if( QFCL_NEW_UNIT_TEST_FRAMEWORK_API )
	add_library( BoostUnitTestFramework STATIC	${Boost_INCLUDE_DIR}/boost/test/impl/compiler_log_formatter.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/debug.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/exception_safety.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/execution_monitor.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/framework.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/interaction_based.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/logged_expectations.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/plain_report_formatter.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/progress_monitor.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/results_collector.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/results_reporter.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/test_tools.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/unit_test_log.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/unit_test_main.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/unit_test_monitor.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/unit_test_parameters.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/unit_test_suite.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/xml_log_formatter.ipp
												${Boost_INCLUDE_DIR}/boost/test/impl/xml_report_formatter.ipp )

	get_target_property( BoostUnitTest_Sources BoostUnitTestFramework SOURCES )
	source_group( "Source Files" FILES ${BoostUnitTest_Sources} )
	set_source_files_properties( ${BoostUnitTest_Sources} PROPERTIES LANGUAGE CXX )
	set_target_properties( BoostUnitTestFramework PROPERTIES 
						   LINKER_LANGUAGE CXX 
						   COMPILE_DEFINITIONS "BOOST_TEST_ALTERNATIVE_INIT_API" 
						   FOLDER test )
endif()

# ----------------------------------------------
# TEST SUITE
# ----------------------------------------------

set( QFCL_VERBOSE_TEST TRUE CACHE BOOL "Verbose output from test suite" )
if( QFCL_NEW_UNIT_TEST_FRAMEWORK_API )
	set( PREPROCESSOR_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}; BOOST_TEST_ALTERNATIVE_INIT_API" )
endif()
if( QFCL_VERBOSE_TEST )
	set( PREPROCESSOR_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}; QFCL_VERBOSE_TEST" )
endif()
set( QFCL_PROFILER_SAMPLESIZE 5000000 CACHE STRING "Default sample size for profiling" )
set( PREPROCESSOR_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}; QFCL_PROFILER_SAMPLESIZE=${QFCL_PROFILER_SAMPLESIZE}" )
separate_arguments( PREPROCESSOR_DEFINITIONS )
#message( "PREPROCESSOR_DEFINITIONS: " ${PREPROCESSOR_DEFINITIONS} )

set( Unit_Engine_Tests linear_generator mersenne_twister twisted_generalized_feedback_shift_register )
set( Unit_Tests uniform_continuous uniform_discrete ${Unit_Engine_Tests} )
foreach( test IN LISTS Unit_Tests )
	set( source_files ${test}.cpp test_generator.ipp )
	list( FIND Unit_Engine_Tests ${test} found )
	if( ${found} GREATER -1 )
		set( source_files ${source_files} engine_common.ipp )
	endif()
	add_executable( ${test} ${source_files} )
	source_group( "Source Files" FILES ${source_files} )
	set_target_properties( ${test} PROPERTIES 
						   COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}"
						   FOLDER test/QFCLUnitTestSuite )
	set( link_libraries ${test} QFCL NTL )
	if( QFCL_NEW_UNIT_TEST_FRAMEWORK_API )
		set( link_libraries "${link_libraries};BoostUnitTestFramework" )
	endif()
	target_link_libraries( ${link_libraries} )
	add_custom_command( TARGET ${test} POST_BUILD 
						COMMAND ${test} --log_level=message --build_info=yes --result_code=no --report_level=short 
						COMMENT "Auto run the test suite." VERBATIM )
	if( ${test} STREQUAL linear_generator )
		add_custom_command( TARGET ${test} PRE_BUILD 
							COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/delete.bat ${CMAKE_CURRENT_BINARY_DIR} *Matrix*
							COMMENT "Delete matrix files." VERBATIM )
	endif()
endforeach( test )

# performance tests

option( QFCL_RDTSCP "CPU has RDTSCP instruction?" OFF )
#option( QFCL_USE_ASMLIB "Use asmlib for profiling?" OFF )

#if( QFCL_USE_ASMLIB )
#	set( ASMLIB_DIR CACHE PATH "asmlib root directory" )
#	set( PROFILER_SAMPLESIZE 1000000 CACHE STRING "profiling sample size. maximum approx.: 268434000" ) 
#	set( PREPROCESSOR_DEFINITIONS ${PREPROCESSOR_DEFINITIONS} 
#		 QFCL_USE_ASMLIB QFCL_PROFILER_LEVEL=1 QFCL_PROFILER_SAMPLESIZE=${PROFILER_SAMPLESIZE} ) 
#	include_directories( ${ASMLIB_DIR} )
#	link_directories( ${ASMLIB_DIR} )
#	add_library( ASMLIB STATIC IMPORTED )
#	set_target_properties( ASMLIB PROPERTIES
#						   IMPORTED_LOCATION alibcof32.lib )
#	set( asm_lib ASMLIB )
#endif( QFCL_USE_ASMLIB )
#separate_arguments( PREPROCESSOR_DEFINITIONS )
set( source_files engine_speed.cpp engine_common.ipp utility/cpu_timer.hpp )
add_executable( EngineSpeed ${source_files} )
source_group( "Source Files" FILES ${source_files} )
set_target_properties( EngineSpeed PROPERTIES
					   FOLDER test/QFCLPerformanceTests
					   COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}"
					   OUTPUT_NAME engine_speed )
target_link_libraries( EngineSpeed QFCL NTL ${Boost_LIBRARIES} ) #${asm_lib} )

#set( source_files engine_speed_hard_coded.cpp utility/cpu_timer.hpp )
#add_executable( EngineSpeed_HardCoded ${source_files} )
#source_group( "Source Files" FILES ${source_files} )
#set_target_properties( EngineSpeed_HardCoded PROPERTIES
#					   FOLDER test/QFCLPerformanceTests
#					   COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}"
#					   OUTPUT_NAME engine_speed_hard_coded )
#target_link_libraries( EngineSpeed_HardCoded QFCL NTL ) #${asm_lib} )

#set( source_files runtime_nary_for_each_performance.cpp utility/cpu_timer.hpp )
#add_executable( Runtime_nary_for_each_Performance ${source_files} )
#source_group( "Source Files" FILES ${source_files} )
#set_target_properties( Runtime_nary_for_each_Performance PROPERTIES
#					   FOLDER test/QFCLPerformanceTests
#					   COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}"
#					   OUTPUT_NAME runtime_nary_for_each_performance )
#target_link_libraries( Runtime_nary_for_each_Performance QFCL NTL ) #${asm_lib} )

set( source_files engine_profiler.cpp engine_common.ipp utility/cpu_timer.hpp )
add_executable( EngineProfiler ${source_files} )
source_group( "Source Files" FILES ${source_files} )
set_target_properties( EngineProfiler PROPERTIES
					   FOLDER test/QFCLPerformanceTests
					   COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINITIONS}"
					   OUTPUT_NAME engine_profiler )
target_link_libraries( EngineProfiler QFCL NTL ${Boost_LIBRARIES} ) #${asm_lib} )

add_executable( LinearGeneratorDiscardSpeed linear_generator_discard_speed.cpp )
set_target_properties( LinearGeneratorDiscardSpeed PROPERTIES
					   FOLDER test/QFCLPerformanceTests 
					   OUTPUT_NAME linear_generator_discard_speed )
target_link_libraries( LinearGeneratorDiscardSpeed QFCL NTL ${Boost_LIBRARIES} )

add_executable( MatrixPowerSpeed matrix_power_speed.cpp )
set_target_properties( MatrixPowerSpeed PROPERTIES
					   FOLDER test/QFCLPerformanceTests
					   OUTPUT_NAME matrix_power_speed )
target_link_libraries( MatrixPowerSpeed QFCL NTL ${Boost_LIBRARIES} )

# ----------------------------------------------
# EXECUTABLES
# ----------------------------------------------

#set( source_files TestMC.cpp Mesher.hpp FdmVisitor.hpp FdmVisitor.cpp 
#	 MCMediator.hpp MCPostProcess.hpp Sde.hpp SdeOneFactor.hpp
#	 SdeVisitor.hpp Range.cpp Range.hpp )
add_executable( TestMC TestMC.cpp engine_common.ipp )
set_target_properties( TestMC PROPERTIES
					   FOLDER test/MC1
					   OUTPUT_NAME test_MC )
target_link_libraries( TestMC ${Boost_LIBRARIES} )

# ----------------------------------------------
# CMake test
# ----------------------------------------------

enable_testing()

add_test( normal normal )
#add_test( NAME QFCLTestSuite
#		  COMMAND QFCLTestSuite --log_level=message --build_info=yes --result_code=no --report_level=short )
#ADD_TEST(uniform uniform)
#ADD_TEST(MT mersenne_twister)
