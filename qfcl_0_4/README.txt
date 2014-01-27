QFCL/random

Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
-----------------------------------------------------------------------------

This is the QFCL random library.

About
=====

Currently, the main contribution of this library is a version of the Mersenne 
Twister (MT19937, MT19937-64 and some older variants) pseudo-random number 
generator, with a discard function that is amortized O(1) with respect to the
jump size (i.e. how many random numbers are discarded). This is an improve-
ment over the C++ standard as well as the boost::random library version of
the Mersenne Twister PRNG which have O(n) discard. 

The ability to discard, i.e. skip ahead, by large jumps is very important for
applications that require parallel pseudo-random number generation, such as
parallelized Monte Carlo simulations for example. For a given seed, suppose 
that a given PRNG generates the sequence x(0), x(1), x(2), ... . The most 
basic method of using this sequence to generate multiple, say N, streams of 
pseudo-random numbers simultaneously is to use the N sequences:
x(0), x(1), x(2), ...
x(p), x(p + 1), x(p + 2), ...
x(2p), x(2p + 1), x(2p + 2), ...
...
x((N-1)p), x((N-1)p + 1), x((N-1)p + 2), ...
The idea is to choose p large enough to ensure that the streams do not over-
lap, as this would artificially introduce inter-stream correlations. In the 
example of parallel Monte Carlo simulation, p must be larger than the total
number of pseudo-random numbers consumed by all of the simulations of any one
stream. 

Numerical Example
-----------------

Suppose that we want to perform a MC simulation on a computer that is capable
of processing 8 threads simultaneously (e.g. an 8-core processor). The MC re-
quires 10,000 simulations, and each simulation can consume up to 100,000,000
random numbers. Then we can divide the 10,000 simulations into 8 streams of 
1250 simulations, and use a jump size of p = 1250 * 100,000,000 = 1.25E11. In
order to setup the simulation, we need 8 PRNGs, the first generating the se-
quence x(0), x(1), ..., the second generating x(p), x(p + 1) and so on.

For a standard C++ PRNG such as boost::random::mt19937 (boost's Mersenne 
Twister), discarding p random numbers requires advancing the state of the
generator p times. This is almost as costly as generating p random numbers 
(it is actually C times as costly for a constant C < 1, since advancing to 
the next state is a bit less work than generating the next number, but C is 
not much smaller than 1). This means that to setup the MC simulation we need
to advance the state p times to get the second generator, then advance ano-
ther p times for the third generator and so on. In total the setup cost is 
similar to generating 7p random numbers, nearly one trillion! Moreover,
clearly the process must be done sequentially and cannot be parallelized.

On the other hand, QFCL's Mersenne Twister has amortized O(1) discard. The
"amortized" refers to the need to compute a "jump matrix" for each jump size
p that we wish to use. Once we have a jump matrix, it costs the same to dis-
card regardless of the value of p. This jump matrix is also saved to disk so
that it does not need to be recomputed in future runs. Note that the computa-
tion of the jump matrix is O(log p). Moreover, the setup of the MC simulation
can be parallelized for further savings. In our example code (see below) a
Monte Carlo simulation is set up with the above parameters. On the author's 
system it took 6381.4 seconds (1h46m21.4s) to set up the MC using boost's 
MT19937, versus 2.3 seconds using the QFCL MT19937. We note that the MT19937
is a precise specification for pseudo-random number generation, and thus the
actual simulation will produce identical results for both generators. We also
note that the jump matrix for p = 1.25E11 was included with the QFCL distri-
bution. Without the precomputed matrix, it took QFCL 4423.4 seconds to set up
the MC the first time (after which future runs will not need to recompute the 
matrix).

Example Code
============

See examples/README.txt for description of the example code. It demonstrates
the usage of discard and related functionality. The included 
examples/parallel_PRNG_example project implements the above numerical 
example.

QFCL also includes performance tests. Of particular interest is the 
EngineSpeed test which compares the speed of raw random number generation for
various PRNGs. See test/README.txt.

Correctness
===========

QFCL includes a thorough unit test suite, which fully covers the 
functionality of all the included PRNGs. The unit tests are automatically run
at build time.

Build
=====

QFCL uses the CMake build system (www.cmake.org). The main CMakeLists.txt
file is in this directory: qfcl_0_4/CMakeLists.txt.

The only required external library is boost. The build also depends on NTL
(Number Theory Library) and some boost extensions. Our CMake-d version of NTL
is in the directory WinNTL-5_5_2-CMake (this can be used on all platforms).
It is recommended to use this source for NTL, and it must be build before
attempting to build QFCL. QFCL also depends on some boost extensions such as
prf_boost (D. E. Shaw Research) and boost::tti (from boost/trunk). These are
included in the directory boost_extensions, and do not require building since
they are headers only. If you have already supplemented your boost 
installation, you can simply set BOOST_EXTENSIONS_ROOT to the root of your 
boost installation in the CMake-GUI.

In order to build the documentation, doxygen must be installed as well as a 
LaTeX installation that is usable by doxygen. 