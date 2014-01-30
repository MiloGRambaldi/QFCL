/* example/linear_generator_example.cpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file example/linear_generator_example.cpp
	\brief example of setting up a parallel Monte Carlo simulation

	\author James Hirschorn
	\date January 23, 2014
*/
#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>

/** configuration */

// could be any linear_generator
typedef qfcl::random::mt19937 MT19937_Engine;
// whether Engine supports peek
static const bool Engine_has_peek		= true;
// whether Engine is reversible
static const bool Engine_is_reversible	= true;

// could use any random distribution satisfying the C++11 standard (or possibly less)
typedef boost::normal_distribution<> NormalDistribution;

static const size_t num_raw					= 10;
static const size_t num_variates			= 10;
// allow for 64-bit integer type
// 2^n + 1 is fastest
static const long long default_step_size	= INT64_C( (1ll << 30) + 1 );
static const size_t num_steps				= 10;

int main(int argc, char * argv[])
{
	using namespace std;

	long long step_size;

	// parse command line arguments
	if (argc == 1)	// use default jump size
	{
		step_size = default_step_size;
	}
	else if (argc == 2 && argv[1][0] != '?')
	{
		step_size = boost::lexical_cast<long long>(argv[1]);
	}
	else
	{
		cerr << "Usage: " << argv[0] 
			 << " [step_size (default is 2^30)]" << endl;
		exit(EXIT_FAILURE);
	}

	// time the entire run
	boost::timer::auto_cpu_timer timer;

	//--Example 1.-----------------------------------------------------------

	// create random engine with default seed
	MT19937_Engine eng;

	// output some raw random numbers
	cout << "Some raw random numbers from the " 
		 << mpl::c_str<MT19937_Engine::name>::value << " engine:" << endl;
	generate_n( ostream_iterator<MT19937_Engine::result_type>(cout, "\n"), 
				num_raw, eng );
	cout << endl;
	
	//--Example 2.-----------------------------------------------------------

	// create a distribution
	NormalDistribution dist;

	// output some random univariates
	cout << "Some standard normal random univariate numbers:" << endl;
	cout.precision( numeric_limits<NormalDistribution::result_type>::digits10 );
	generate_n( ostream_iterator<NormalDistribution::result_type>(cout, "\n"),
				num_variates, bind( dist, eng ) );
	cout << endl;

	//--Example 3.-----------------------------------------------------------

	/* save the state of the RNG */
	cout << "Saving the random engine state." << endl << endl;
	// capture the state in a stream
	stringstream ss;
	ss << eng;

	// show the next number generated
	cout << "Next raw random number: " << endl;
	cout << eng() << endl << endl;
	// we shall call the current state the "reference state"
	cout << "Currently at \"reference state\"." << endl << endl;

	//--Example 4.-----------------------------------------------------------

	// look ahead 4 x step_size numbers without affecting eng,
	// but don't do this if the engine does not support peek
	// NOTE: A traits class is planned 
	// so that this "if (...)" kludge won't be necessary.
	if (Engine_has_peek)
	{
		cout << "Peeking ahead:" << endl;
		cout << "Random number " 
			 << qfcl::io::custom_formatted(4 * step_size) 
			 << " steps from the reference state:" << endl;
		cout << eng.peek(4 * step_size) << endl << endl;
	}

	//--Example 5.-----------------------------------------------------------

	// whether a jump matrix had to be computed at any point in the program
	bool computed_jump_matrix = false;

	// check whether jump matrix exists
	cout << "Checking whether jump matrix for jump size " 
		 << qfcl::io::custom_formatted(step_size - 1)
		 << " exists ... ";
	bool exists = MT19937_Engine::JumpMatrix_file_exists(step_size - 1);
	cout << (exists ? " yes." 
		: "no.\nComputing jump matrix. This may take some time ...");
	cout << endl << endl;
	// note that if jump matrix does not exist, it will automatically 
	// be computed on the first jump of the given size
	computed_jump_matrix = computed_jump_matrix || !exists;

	// make num_jump jumps
	for (size_t i = 0; i < num_steps; ++i)
	{
		eng.discard(step_size - 1);
		cout << "Random number " 
			 << qfcl::io::custom_formatted( (i + 1) * step_size )
			 << " from reference state:" << endl;
		cout << eng() << endl << endl;
	}

	//--Example 6.-----------------------------------------------------------

	// recall a previous number, but only if Engine is reversible 
	// NOTE: A traits class is planned 
	// so that this "if (...)" kludge won't be necessary.
	if (Engine_is_reversible)
	{
		static const long long reverse_jump_size = 4 * step_size;
		cout << "Recalling random number " 
			 << qfcl::io::custom_formatted(num_steps * step_size 
										   - reverse_jump_size)
			 << " from reference state: " << endl;
		
		/* reverse_discard may be too slow. 
		   Use "#if 0" to use peek instead. */
#if 1
		// warn of delay if corresponding jump matrix does not exist
		// "true" indicates that the direction is reversed
		if ( !MT19937_Engine::JumpMatrix_file_exists(reverse_jump_size + 1, true) )
		{
			cout << "   ... Computing reverse jump matrix for jump size " 
				 << qfcl::io::custom_formatted(reverse_jump_size + 1) << ".\n" \
					"       This may take some time ..." << endl;
			computed_jump_matrix = true;
		}

		// reverse the engine reverse_jump_size + 1 states
		// (1 step back returns us to num_jumps * jump_size.)
		eng.reverse_discard(reverse_jump_size + 1);
		// output next number
		cout << eng() << endl << endl;
#else
		cout << eng.peek(-reverse_jump_size) << endl << endl;
#endif
	}

	//--Example 7.-----------------------------------------------------------

	// restore saved state
	cout << "Restoring the state of the random engine." << endl << endl;
	ss >> eng;
	cout << "Next random number after restore: " << endl << eng() << endl;
	cout << endl;

	//-----------------------------------------------------------------------

	if (computed_jump_matrix)
		cout << "Execution will be rapid on the next run, because the " \
				"jump matrices have been\nprecomputed." << endl << endl;
}