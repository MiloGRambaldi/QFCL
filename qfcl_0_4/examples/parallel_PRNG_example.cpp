/* example/parallel_PRNG_example.cpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file example/parallel_PRNG_example.cpp
	\brief example of setting up a parallel Monte Carlo simulation

	\author James Hirschorn
	\date January 26, 2014
*/
#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/random/mersenne_twister.hpp>

#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>

/** configuration */

// could be any linear_generator
typedef qfcl::random::mt19937 MT19937_Engine;
typedef MT19937_Engine::UIntType UIntType;
// whether Engine supports peek
static const bool Engine_has_peek		= true;
// whether Engine is reversible
static const bool Engine_is_reversible	= true;

// could use any random distribution satisfying the C++11 standard (or possibly less)
typedef boost::normal_distribution<> NormalDistribution;

static const long default_num_simulations	= 10000;
static const long long default_num_consumed = 100000000;	// = 1E8
static const long default_num_streams		= 8;

// Time how long it would take to set up a parallel Monte Carlo simulation.
template<typename Engine>
std::pair<boost::timer::cpu_times, typename Engine::result_type*> setup_parallel_PRNG(Engine& eng, long N, long long p)
{
	boost::timer::cpu_timer timer;

	typename Engine::result_type* result = new Engine::result_type[N];

	result[0] = eng();
	// This is just a mock Monte Carlo simulation.
	// We need to advance for streams 2 through N.
	for (long i = 1; i < N; ++i)
	{
		eng.discard(p - 1);	// we have already consumed the first one
		result[i] = eng();
	}
	
	timer.stop();

	return std::make_pair(timer.elapsed(), result);
}

int main(int argc, char * argv[])
{
	using namespace std;

	long num_simulations, num_streams;
	long long num_consumed, step_size;
	UIntType seed;
	
	/* Handle command line options. */

	// Declare the supported options.
	po::options_description generic_options("Generic options");
	generic_options.add_options()
		("help,h", "this help message")
		("version,v", "version info");

	po::options_description optional_params("Optional parameters");
	optional_params.add_options()
		("simulations,n", po::value<long>(&num_simulations) -> default_value(default_num_simulations),
		 "number of simulations in the Monte Carlo")
		("max,m", po::value<long long>(&num_consumed) -> default_value(default_num_consumed),
		 "maximum # of random numbers consumed by one simulation")
		("streams,N", po::value<long>(&num_streams) -> default_value(default_num_streams),
		 "number of streams of pseudo-random numbers")
		("seed,s", po::value<UIntType>(&seed),
		 "pseudo-random number generator seed");

	po::options_description command_line_options;
	command_line_options.add(generic_options).add(optional_params);
		po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(command_line_options).run(), vm);
	po::notify(vm);    

	if (vm.count("version"))
	{
		cout << argv[0] << ", QFCL Version " << QFCL_VERSION << endl;
		cout << "Copyright 2012, James Hirschorn <James.Hirschorn@gmail.com>" << endl;
		return EXIT_SUCCESS;
	}
	if (vm.count("help")) 
	{
		cout << argv[0] << " set up a parallel Monte Carlo simulation." 
			 << endl << endl;
		cout << "Usage: " << argv[0] << " [options]" << endl << endl;
		cout << generic_options << endl;
		cout << optional_params << endl;
		return EXIT_SUCCESS;
	}

	/* perform set up */

	boost::timer::cpu_times time_taken;

	step_size = ((num_simulations + num_streams - 1) / num_streams) * num_consumed;

	MT19937_Engine qfcl_engine;

	MT19937_Engine::result_type* qfcl_result;

	tie(time_taken, qfcl_result) = setup_parallel_PRNG(qfcl_engine, num_streams, step_size);

	cout << boost::format("Time to set up parallel Monte Carlo simulation using QFCL PRNG: %|1.1f|s.\n") 
		% (time_taken.wall / 1000000000.0);

	boost::random::mt19937 boost_engine;
	boost::random::mt19937::result_type* boost_result;

	// set the seed if given
	if (vm.count("seed"))
	{
		boost_engine.seed(seed);
		qfcl_engine.seed(seed);
	}

	tie(time_taken, boost_result) = setup_parallel_PRNG(boost_engine, num_streams, step_size);

	cout << boost::format("Time to set up Monte Carlo simulation using boost PRNG: %|1.1f|s.\n\n") 
		% (time_taken.wall / 1000000000.0); 

	cout << "Verification: First random number for the i-th stream." << endl;
	for (int i = 0; i < num_streams; ++i)
		cout << boost::format("i = %|1|. QFCL: %|2|, boost: %|3|\n")
			% (i + 1) % qfcl_result[i] % boost_result[i];

}
