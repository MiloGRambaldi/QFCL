# doc/CMakeLists.txt
#
# CMake build rules for QFCL documentation
#
# Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
#
# Use, modification and distribution are subject to 
# the BOOST Software License, Version 1.0.
# (See accompanying file LICENSE.txt)
# --------------------------------------------------------------

set( DOXYFILE_PROJECT_BRIEF "Quantitative Finance Code Library Documentation" CACHE STRING "One line description of the project" )
set( DOXYFILE_LOGO ${CMAKE_HOME_DIRECTORY}/doc/qlogo.small.png CACHE FILEPATH "Project logo" )
set( DOXYFILE_SOURCE_DIR ${CMAKE_HOME_DIRECTORY} CACHE PATH "Top level directory scanned by Doxygen" )
set( DOXYFILE_LATEX OFF CACHE BOOL "Whether to build LaTeX documentation" )
set( DOXYFILE_PAPER_SIZE letter CACHE STRING "Which paper size to use for LaTeX documentation: a4, a4wide, letter, legal, executive" )

include( UseDoxygen )
