#ifndef QFCL_RANDOM_TWISTED_GENERALIZED_FEEDBACK_SHIFT_REGISTER_HPP
#define QFCL_RANDOM_TWISTED_GENERALIZED_FEEDBACK_SHIFT_REGISTER_HPP

#include <boost/mpl/string.hpp>

#include "mersenne_twister.hpp"

namespace qfcl {

namespace random {

//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

// abuse of language until there is support multicharacter literals of arbitrary length
typedef mpl::string<'T', 'T', '8', '0', '0'>::type tt800_name;
typedef mpl::string<'M', 'i', 'c', 'r', 'o', '-', 'M', 'T'>::type micro_mt_name;
}	// namespace detail
//! \endcond

/*! \brief TT800 (32-bit)

	25-dimensionally equidistributed PRNG. The predecessor of the Mersenne Twister. 
	The state size is 800 bits, and it has a full period of 2^800 - 1.
*/
typedef mersenne_twister_traits<uint32_t, 32, 25, 7, 0,
								UINT32_C(0x8ebfd028), 
								32,	UINT32_C(0xffffffff), 
								7,	UINT32_C(0x2b5b2500), 
								15,	UINT32_C(0xdb8b0000), 
								16,
								5489u, UINT32_C(1812433253), 
								UINT32_C(19650218), UINT32_C(1664525), UINT32_C(1566083941),
								detail::tt800_name> 
tt800_traits;

typedef mersenne_twister_engine<tt800_traits> tt800;

/*! \brief reverse TT800
*/
typedef reverse_adapter<tt800> reverse_tt800;

/*! \brief Micro-MT (32-bit)

	\warning For test purposes only!
*/
typedef mersenne_twister_traits<uint32_t, 32, 2, 1, 0,
						UINT32_C(0x80000000),
						32,UINT32_C(0xffffffff),
						0, UINT32_C(0x0),
						0, UINT32_C(0x0),
						32,
						1u, UINT32_C(1),
						1u, 1u, 1u,
						detail::micro_mt_name>
micro_mt_traits;

typedef mersenne_twister_engine<micro_mt_traits> micro_mt;

/*! \brief reverse Micro-MT
*/
typedef reverse_adapter<micro_mt> reverse_micro_mt;

}	// namespace random

}	// namespace qfcl

#endif	// QFCL_RANDOM_TWISTED_GENERALIZED_FEEDBACK_SHIFT_REGISTER_HPP