/* test/linear_generator.cpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/linear_generator.cpp
	\brief unit tests for linear generators

	\author James Hirschorn
	\date April 26, 2012
*/

/// NOTE: To be removed!!
#pragma warning(disable:4307)

#include <algorithm>
using std::swap;

#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <boost/mpl/apply.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/vector_c.hpp>

#include <boost/random/mersenne_twister.hpp>

#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/twisted_generalized_feedback_shift_register.hpp>
using namespace qfcl::random;
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>

#include "test_generator.ipp"
using namespace boost::unit_test_framework;

#include "engine_common.ipp"

/*! \ingroup TestSuite
	@{
*/

// alias
namespace mpl = boost::mpl;

/*! \brief This is a list of all mersenne_twister generators, paired with their name,
    and again paired with their reverse versions.
*/
typedef mpl::list< mpl::pair<mt11213a,		reverse_mt11213a>,
				   mpl::pair<mt11213b,		reverse_mt11213b>,
				   mpl::pair<mt19937,		reverse_mt19937>,
				   mpl::pair<mt19937_64,	reverse_mt19937_64>,
				   mpl::pair<tt800,			reverse_tt800>,
				   mpl::pair<micro_mt,		reverse_micro_mt>
				 >
linear_generator_engine_pairs;
//! \cond
// a flat list of all engines, computed from \c linear_generator_engine_pairs
//typedef qfcl::tmp::list_of_flattened_pairs<linear_generator_engine_pairs>::type
//	reversible_linear_generator_engines;
//! \endcond
/*! \brief list of all engines
*/
typedef reversible_linear_generator_engines all_linear_generator_engines;

//! for formatting output
const size_t indent_width = 10;

// linear generator test suite
BOOST_AUTO_TEST_SUITE(linear_generator)
	
//! prints a message heading for the \c mersenne_twiser test suite
void print_linear_generator_header()
{
	using std::string;
	std::ostringstream oss;

#ifdef QFCL_VERBOSE_TEST
	/* format engines names for output */

	auto engine_names = qfcl::names::get_name_or_typenames<reversible_linear_generator_engines>();

	BOOST_FOREACH( std::string s, engine_names )
	{
		oss << std::setw(indent_width) << "" << s << std::endl;
	}
#endif	// QFCL_VERBOSE_TEST

	BOOST_TEST_MESSAGE( "\nTesting the linear generators:\n" << oss.str() );
}

//! for all linear generator engines, checks that <tt>reverse()</tt> is really the inverse of <tt>operator()</tt>
BOOST_AUTO_TEST_CASE_TEMPLATE(rewind, Engine, reversible_linear_generator_engines)
{
	// The following refers to the entire test suite
	// (it is not specific to this test case).
	if ( qfcl::tmp::is_first<reversible_linear_generator_engines, Engine>::value )
		 print_linear_generator_header();

	// only print the message the first time
	if ( qfcl::tmp::is_first<reversible_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing reversal of all linear generators ...");
	
	const size_t rewind_distance = 2 * Engine::state_size;
	
	// use the default seed
	Engine eng;

#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng, " ...");
#endif	// QFCL_VERBOSE_TEST
	
	// generate rewind_distance random numbers
	std::vector<Engine::result_type> fwdOutput(rewind_distance);
	std::generate( fwdOutput.begin(), fwdOutput.end(), std::ref(eng) );

	// then generate rewind_distance random numbers in the reverse direction
	// and compare to the first set
	std::vector<Engine::result_type> bwdOutput(rewind_distance);
	std::generate( bwdOutput.begin(), bwdOutput.end(), boost::bind( &Engine::reverse, boost::ref(eng) ) );

	BOOST_REQUIRE_EQUAL_COLLECTIONS( fwdOutput.begin(), fwdOutput.end(), bwdOutput.rbegin(), bwdOutput.rend() );
}

//! check that \c reverse_adapter works for each non-reverse engine, i.e. <tt>eng() == reverse_eng.reverse()</tt>.
BOOST_AUTO_TEST_CASE_TEMPLATE(reverse_adapter, pair, linear_generator_engine_pairs)
{
	if ( qfcl::tmp::is_first<linear_generator_engine_pairs, pair>::value )
		BOOST_TEST_MESSAGE("Verifying that the reverse_adapter applied to each reversible linear generator, is its inverse ...");

	typedef pair::first Engine;
	typedef pair::second reverseEngine;

	const Engine::result_type seed = 10829732u;
	const size_t testSize = 10000;

	// initialize Engine and reverseEngine with the same seed
	Engine eng(seed);
	reverseEngine reverse_eng(seed);
	
#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng, " ...");
#endif	// QFCL_VERBOSE_TEST

	for (size_t i = 0; i < testSize; ++i)
		BOOST_REQUIRE_EQUAL( eng(), reverse_eng.reverse() );
}

//! check that streaming works
BOOST_AUTO_TEST_CASE_TEMPLATE(streaming, Engine, all_linear_generator_engines)
{
	if ( qfcl::tmp::is_first<all_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing streaming of the generator state ...");
	
	// use the default seed
	Engine eng;

#ifdef	QFCL_VERBOSE_TEST
	/* commented out because it produces too much ouput
		// for mt19937, output the state to TEST_MESSAGE stream
		if ( boost::is_same<Engine, mt19937>::value )
			BOOST_TEST_MESSAGE("State of " << mpl::c_str<mt19937::name>::value << " with default seed: " << '\n'
										   << std::hex << mt << std::dec);
		else
	*/
		print_engine_name(eng, " ...");
#endif	// QFCL_VERBOSE_TEST

	// move ahead n / 2 numbers
	for (size_t i = 0; i < Engine::state_size / 2; ++i)
		eng();
	
	// capture the state in a stream
	std::stringstream ss;

	ss << eng;

	// create another with an arbitrary seed
	const Engine::result_type seed = 987654321u;
	Engine engOther(seed);

	// check that they are unequal
	BOOST_CHECK(eng != engOther);

	// set the stream from oss
	ss >> engOther;
	
	// verify that they are equal
	BOOST_CHECK(eng == engOther);
	BOOST_CHECK(engOther == eng);

	// check directly too, since the equality test case has not been done yet
	const size_t testSize = 10000;

	for (size_t i = 0; i < testSize; ++i)
	{
		Engine::result_type result = eng();
		Engine::result_type resultOther = engOther();
		BOOST_REQUIRE_MESSAGE( result == resultOther, "critical check eng() == engOther() [" 
			<< result << " != " << resultOther << "] failed at iteration " << i);
	}
}

//! test that changing \c UIntType results in equivalent generator
BOOST_AUTO_TEST_CASE(UIntType)
{
	BOOST_TEST_MESSAGE("Testing effect of changing UIntType of " << mpl::c_str<mt19937::name>::value << " to 64-bit ...");

	mt19937 mt;
	typedef mt19937_traits t;
	typedef qfcl::random::mersenne_twister_traits<
		uint64_t,t::word_size,t::state_size,t::shift_size,t::mask_bits,t::xor_mask,
		t::tempering_u,t::tempering_d,t::tempering_s,t::tempering_b,t::tempering_t,t::tempering_c,t::tempering_l,
		t::default_seed,t::initialization_multiplier,
		t::default_multiseed,t::multiseed_initialization_multiplier1,t::multiseed_initialization_multiplier2,
		t::name> 
	mtOther_traits;

	mersenne_twister_engine<mtOther_traits> mtOther;

	// note these two are not comparable under operator==, even though they are equivalent generators
	// will not compile: BOOST_CHECK(mt == mtOther);

	const size_t testSize = 10000;

	for (size_t i = 0; i < testSize; ++i)
		BOOST_REQUIRE_EQUAL( mt(), mtOther() );
}

//! Tests Assignable concept
BOOST_AUTO_TEST_CASE_TEMPLATE(assignment, Engine, all_linear_generator_engines)
{
	if( qfcl::tmp::is_first<all_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing assignment and copy constructors ...");

	const unsigned initial_seed = 13;

	const size_t testSize = 10000;

	std::vector<Engine::result_type> results1(testSize), results2(testSize), results3(testSize);

	Engine eng(initial_seed);
	// use copy constructor
	Engine engCopy = eng;
	// use assignment
	Engine engEquals;
	engEquals = eng;

	// check that they are all equal after initialization
	BOOST_CHECK(eng == engCopy);
	BOOST_CHECK(engCopy == engEquals);

	for (size_t i = 0; i < testSize; ++i)
	{
		results1[i] = eng();
		results2[i] = engCopy();
		results3[i] = engEquals();
	}
	
#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng, " ...");
#endif	// QFCL_VERBOSE_TEST

	// double check that they were initially all equal
	BOOST_CHECK_EQUAL_COLLECTIONS( results1.begin(), results1.end(), results2.begin(), results2.end() );
	BOOST_CHECK_EQUAL_COLLECTIONS( results2.begin(), results2.end(), results3.begin(), results3.end() );

	// now check that they are still equal after number generation
	BOOST_CHECK(eng == engCopy);
	BOOST_CHECK(engCopy == engEquals);
}

//! Tests discard
BOOST_AUTO_TEST_CASE_TEMPLATE(discard, Engine, all_linear_generator_engines)
{
	if( qfcl::tmp::is_first<all_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing discard() ...");

	const unsigned long long discard_size = 10 * Engine::state_size;

	// use default seed
	Engine eng1, eng2;
	
#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng1, " ... this may take some time.");
#endif	// QFCL_VERBOSE_TEST

	// discard one step at a time
	for (size_t i = 0; i < discard_size; ++i)
		eng1();
	// discard using the member functions
	eng2.discard(discard_size);

	BOOST_CHECK(eng1 == eng2);
}

/*! \brief Tests reverse_discard

	This is fast because the matrix files have already been generated in the last test.
*/
BOOST_AUTO_TEST_CASE_TEMPLATE(reverse_discard, Engine, reversible_linear_generator_engines)
{
	if( qfcl::tmp::is_first<reversible_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing reverse_discard() ...");

	const unsigned long long discard_size = 10 * Engine::state_size;

	// use default seed
	Engine eng1, eng2;
	
#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng1, " ...");
#endif	// QFCL_VERBOSE_TEST

	// discard one step at a time
	for (size_t i = 0; i < discard_size; ++i)
		eng1.reverse();
	// discard using the member functions
	eng2.reverse_discard(discard_size);

	BOOST_REQUIRE(eng1 == eng2);
}

//! Tests peek
BOOST_AUTO_TEST_CASE_TEMPLATE(peek, Engine, all_linear_generator_engines)
{
	if( qfcl::tmp::is_first<all_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing peek() ...");

	const unsigned long long discard_size = 10 * Engine::state_size;

	// use default seed
	Engine eng1, eng2;
	
#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng1, " ...");
#endif	// QFCL_VERBOSE_TEST
	
	// discard using the member functions
	eng1.discard(discard_size);

	// compare next number generated by eng1 with peek
	BOOST_REQUIRE_EQUAL( eng1(), eng2.peek(discard_size + 1) );
}

//! Tests reverse_peek and peek(long long)
BOOST_AUTO_TEST_CASE_TEMPLATE(reverse_peek, Engine, reversible_linear_generator_engines)
{
	if( qfcl::tmp::is_first<reversible_linear_generator_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing reverse_peek() and peek(long long) ...");

	// NOTE: We use *signed* long long here. 
	const long long discard_size = 10 * Engine::state_size;

	// use default seed
	Engine eng1, eng2;
	
#ifdef	QFCL_VERBOSE_TEST
	print_engine_name(eng1, " ...");
#endif	// QFCL_VERBOSE_TEST
	
	// discard using the member functions
	eng1.reverse_discard(discard_size);

	// compare next number generated by eng1 with peek,
	// using two methods
	typedef typename Engine::result_type result_t;
	result_t next_rn = eng1();
	BOOST_REQUIRE_EQUAL( next_rn, eng2.reverse_peek(discard_size - 1) );
	BOOST_REQUIRE_EQUAL( next_rn, eng2.peek(1 - discard_size) );

	// some special tests for peek(0)
	result_t prev_rn = eng1.peek(0ll), prev_rn2 = eng1.reverse_peek(0);
	eng1.reverse();
	result_t prev_rn3 = eng1();
	BOOST_REQUIRE_EQUAL( prev_rn, prev_rn2 );
	BOOST_REQUIRE_EQUAL( prev_rn2, prev_rn3 );
}

BOOST_AUTO_TEST_SUITE_END()

//!	@}

