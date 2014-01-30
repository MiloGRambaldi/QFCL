# examples/CMakeLists.txt
#
# CMake build rules for the examples
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

# not currently using Eigen3
#find_package( Eigen3 )
#if( EIGEN3_FOUND )				
#	include_directories( ${EIGEN3_INCLUDE_DIR} )
#endif( EIGEN3_FOUND )

# ----------------------------------------------
# EXECUTABLES
# ----------------------------------------------

add_executable( bench_normal bench_normal.cpp )
set_target_properties( bench_normal PROPERTIES
					   FOLDER examples )

add_executable( ParallelMonteCarlo parallel_monte_carlo.cpp )
set_target_properties( ParallelMonteCarlo PROPERTIES
					   FOLDER examples )

add_executable( linear_generator_example linear_generator_example.cpp )
set_target_properties( linear_generator_example PROPERTIES
					   FOLDER examples )
target_link_libraries( linear_generator_example QFCL NTL )

add_executable( parallel_PRNG_example parallel_PRNG_example.cpp )
set_target_properties( parallel_PRNG_example PROPERTIES
					   FOLDER examples )
target_link_libraries( parallel_PRNG_example QFCL NTL )
