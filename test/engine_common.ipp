#include <string>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/vector.hpp>
namespace mpl = boost::mpl;

//#include <boost/test/included/unit_test.hpp>

#include <qfcl/random/engine/counting.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/named_adapter.hpp>
#include <qfcl/random/engine/twisted_generalized_feedback_shift_register.hpp>
#include <qfcl/utility/tmp.hpp>

// list of engines
typedef mpl::vector< mpl::pair<qfcl::random::mt11213a,		qfcl::random::reverse_mt11213a>,
					 mpl::pair<qfcl::random::mt11213b,		qfcl::random::reverse_mt11213b>,
					 mpl::pair<qfcl::random::mt19937,		qfcl::random::reverse_mt19937>,
				     mpl::pair<qfcl::random::mt19937_64,	qfcl::random::reverse_mt19937_64>,
					 mpl::pair<qfcl::random::tt800,			qfcl::random::reverse_tt800>,
					 mpl::pair<qfcl::random::micro_mt,		qfcl::random::reverse_micro_mt>
				   >
linear_generator_reversible_engine_pairs;
//! \cond
// a flat list of all engines, computed from \c linear_generator_engine_pairs
typedef qfcl::tmp::vector_of_flattened_pairs<linear_generator_reversible_engine_pairs>::type
	reversible_linear_generator_engines;
typedef mpl::push_back<reversible_linear_generator_engines, qfcl::random::boost_mt19937>::type linear_generator_engines; 
//! \endcond

//! List of all engines
typedef mpl::vector< qfcl::random::mt19937,
					 qfcl::random::boost_mt19937,
					 qfcl::random::reverse_mt19937,
					 qfcl::random::mt19937_64,	
					 qfcl::random::reverse_mt19937_64,
					 qfcl::random::counting_uint,
					 qfcl::random::Numberline,
					 qfcl::random::mt11213a,	
					 qfcl::random::reverse_mt11213a,
					 qfcl::random::mt11213b,	
					 qfcl::random::reverse_mt11213b,
					 qfcl::random::tt800,		
					 qfcl::random::reverse_tt800,
					 qfcl::random::micro_mt,	
					 qfcl::random::reverse_micro_mt
				   > all_engines;

/// NOTE: Put somewhere else?
//! print the \c Engine name concatenated with \c str
template<typename Engine>
void print_engine_name(Engine & e, const std::string & str, size_t indent_width = 0)
{
	BOOST_TEST_MESSAGE( std::setw(indent_width) << "" 
												<< qfcl::names::name_or_typename(e)
                                                << str );
}

