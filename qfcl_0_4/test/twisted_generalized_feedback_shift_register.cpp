#include <iomanip>
#include <string>

#include <boost/foreach.hpp>
#include <boost/mpl/list.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/twisted_generalized_feedback_shift_register.hpp>
using namespace qfcl::random;
#include <qfcl/utility/names.hpp>

#include "test_generator.ipp"
using namespace boost::unit_test_framework;

#include "engine_common.ipp" 

/*! \ingroup TestSuite
	@{
*/

// alias
namespace mpl = boost::mpl;

/*! \brief This is a list of all TGFSR generators, paired with their name,
    and again paired with their reverse versions.
*/
typedef mpl::list< mpl::pair<tt800,		reverse_tt800>,
				   mpl::pair<micro_mt,	reverse_micro_mt> >
TGFSR_engine_pairs;
//! \cond
// a flat list of all engines, computed from \c TGFSR_engine_pairs
typedef qfcl::tmp::list_of_flattened_pairs<TGFSR_engine_pairs>::type
reversible_TGFSR_engines;
//! \endcond
/*! \brief list of all engines
*/
typedef reversible_TGFSR_engines all_TGFSR_engines;

//! for formatting output
const size_t indent_width = 10;

// Mersenne Twister test suite
BOOST_AUTO_TEST_SUITE(TGFSR)
	
//! prints a message heading for the \c TGFSR test suite
void print_TGFSR_header()
{
	std::ostringstream oss;

#ifdef QFCL_VERBOSE_TEST
	/* format engines names for output */

	auto engine_names = qfcl::names::get_name_or_typenames<all_TGFSR_engines>();

	BOOST_FOREACH( std::string s, engine_names )
	{
		oss << std::setw(indent_width) << "" << s << std::endl;
	}
#endif	// QFCL_VERBOSE_TEST

	BOOST_TEST_MESSAGE( "\nTesting the Twisted Generalized Feedback Shift Register engines:\n" << oss.str() );
}

//! Testing EqualityComparable concept
BOOST_AUTO_TEST_CASE_TEMPLATE(equality, Engine, all_TGFSR_engines)
{
	// The following refers to the entire test suite
	// (it is not specific to this test case).
	if ( qfcl::tmp::is_first<all_TGFSR_engines, Engine>::value )
		 print_TGFSR_header();
	
	if( qfcl::tmp::is_first<all_TGFSR_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing operator== ...");

	QFCL_CREATE_VECTOR(key, unsigned long, 12345, 67890, 1111111111u, 0);
	Engine eng( key.begin(), key.end() );
	
#ifdef QFCL_VERBOSE_TEST
	print_engine_name(eng, " ...");
#endif	// QFCL_VERBOSE_TEST

	const size_t advance_distance = 13;

	// advance the generator
	for (size_t i = 0; i < advance_distance; ++i)
		eng();

	std::stringstream ss, ss_copy, alt_ss;
	ss << eng;
	ss_copy << eng;

	// alter one of the a single bit in the stream
	Engine::result_type first;
	ss >> first;
	first ^= 1;
	alt_ss << first << ' ';
	// copy the rest of ss to alt_ss
	alt_ss << ss.rdbuf();

	// the first test is very similar to the streaming test case
	Engine engOther;
	ss_copy >> engOther;
	BOOST_CHECK(eng == engOther);
	BOOST_CHECK(engOther == eng);

	// make sure change 1 bit has an effect
	alt_ss >> engOther;
	BOOST_CHECK(eng != engOther);
	BOOST_CHECK(engOther != eng);
}


//! Tests for agreement between constructors and seed member functions
BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_seed, Engine, all_TGFSR_engines)
{
	if( qfcl::tmp::is_first<all_TGFSR_engines, Engine>::value )
		BOOST_TEST_MESSAGE("Testing user-provided seeds/states via constructors and the seed member function ...");
	
#ifdef QFCL_VERBOSE_TEST
	print_engine_name( Engine(), " ..." );
#endif	// QFCL_VERBOSE_TEST

	/* default seed */

	Engine eng1;

	// use something other than default seed
	const Engine::result_type seed1 = 0;
	Engine engSeed(seed1);
	BOOST_CHECK(eng1 != engSeed);
	// set to default seed
	engSeed.seed();
	BOOST_CHECK(eng1 == engSeed);

	/* single seed */

	// must be different from seed1
	const Engine::result_type seed2 = 209419383;
	BOOST_REQUIRE_NE(seed1, seed2);
	Engine eng2(seed2);
	BOOST_CHECK(eng2 != engSeed);
	engSeed.seed(seed2);
	BOOST_CHECK(eng2 == engSeed);

	/* multiple seeds */

	// test with a large number of seeds: > n
	const size_t numSeeds = Engine::state_size * 2;
	std::vector<Engine::result_type> seed_array(numSeeds);
	for (size_t i = 0; i < numSeeds; ++i)
		seed_array[i] = i;
	Engine eng3( seed_array.begin(), seed_array.end() );
	// make sure they are not accidentally the same
	BOOST_REQUIRE(eng3 != engSeed);
	engSeed.seed( seed_array.begin(), seed_array.end() );
	BOOST_CHECK(eng3 == engSeed);

	/* state */

	Engine::result_type state_array[Engine::state_size];
	// make up some state
	for (size_t i = 0; i < Engine::state_size; ++i)
		state_array[i] = i;
	// this may be invalid as is
	// correct the extra bits in the state, in case
	Engine::state s(state_array);
	Engine::correct(s);
	Engine eng4(s);
	BOOST_REQUIRE(eng4 != engSeed);
	engSeed.seed(s);
	BOOST_CHECK(eng4 == engSeed);
}

BOOST_AUTO_TEST_SUITE_END()

//!	@}
