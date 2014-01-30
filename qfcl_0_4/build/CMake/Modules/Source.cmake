# build/Modules/Source.cmake
#
# CMake build rules for QFCL library source code
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

# --------------------------------------------------------------
# SOURCE CODE
#
# Note: Would be nice to have a program that autogenerates 
# this CMakeLists.txt, rather than adding files/directories
# by hand.
# --------------------------------------------------------------

set( source_dir ./qfcl )

file( GLOB qfcl ${source_dir}/*.hpp )
file( GLOB random ${source_dir}/random/*.hpp )
file( GLOB engine ${source_dir}/random/engine/* )
file( GLOB distribution ${source_dir}/random/distribution/*.hpp )
file( GLOB utility ${source_dir}/utility/*.hpp )
#file( GLOB linear_algebra ${source_dir}/math/linear_algebra/*.hpp )
file( GLOB bits ${source_dir}/math/bits/*.hpp )
file( GLOB simple ${source_dir}/math/simple/*.hpp )
file( GLOB statistics ${source_dir}/statistics/*.hpp )
file( GLOB mc1 ${source_dir}/mc1/*.hpp ${source_dir}/mc1/*.cpp ${source_dir}/mc1/*.cc )
set_source_files_properties( ${mc1} PROPERTIES HEADER_FILE_ONLY TRUE )
file( GLOB skip_list ${source_dir}/data_structures/skip_list/*.hpp )

file( GLOB_RECURSE all_sources ${source_dir}/* )
set( target_name QFCL )
add_library( ${target_name} STATIC ${all_sources} )
set( output_directory ./lib )
set_target_properties( ${target_name} PROPERTIES 
					   ARCHIVE_OUTPUT_DIRECTORY ${output_directory}
					   LIBRARY_OUTPUT_DIRECTORY ${output_directory}
					   RUNTIME_OUTPUT_DIRECTORY ${output_directory} )
#add_executable( ${target_name} ./build/src/qfcl_ignore.cpp ${qfcl} ${random} ${distribution} ${engine} ${utility} ${bits} ${simple} )
#add_custom_target( ${target_name} SOURCES ${qfcl} ${random} ${distribution} ${engine} ${utility} )

set_target_properties( ${target_name} PROPERTIES FOLDER qfcl )
source_group( qfcl FILES ${qfcl} )
source_group( qfcl\\random FILES ${random} )
source_group( qfcl\\random\\distribution FILES ${distribution} )
source_group( qfcl\\random\\engine FILES ${engine} )
source_group( qfcl\\utility FILES ${utility} )
#source_group( qfcl\\math\\linear_algebra FILES ${linear_algebra} )
source_group( qfcl\\math\\bits FILES ${bits} )
source_group( qfcl\\math\\simple FILES ${simple} )
source_group( qfcl\\statistics FILES ${statistics} )
source_group( qfcl\\mc1 FILES ${mc1} )
source_group( qfcl\\data_structures\\skip_list FILES ${skip_list} )
