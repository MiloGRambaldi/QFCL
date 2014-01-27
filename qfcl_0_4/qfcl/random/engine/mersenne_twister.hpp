/* qfcl/random/engine/mersenne_twister.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_MERSENNE_TWISTER_HPP
#define QFCL_RANDOM_MERSENNE_TWISTER_HPP

/*! \file qfcl/random/engine/mersenne_twister.hpp
	\brief Original MT psuedo-random number generator

	Some parts are loosely based on the implementation from 
	\c mersenne_twister.hpp from \c boost 1.47.0.

	\author James Hirschorn
	\date February 18, 2012
*/

#include <algorithm>
#include <iostream>
#include <iterator>
//#include <stdexcept>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/integer/integer_mask.hpp>
#include <boost/mpl/string.hpp>
//! alias
namespace mpl = boost::mpl;

#include <qfcl/types.hpp>
#include <qfcl/utility/tmp.hpp>

#include "linear_generator.hpp"
#include "reverse_adapter.hpp"
#include "engine.hpp"

namespace qfcl {

/*! \defgroup random pseudo-random number generation
	@{
*/

//! declarations for random number generation
namespace random {

/*! \brief mersenne_twister_engine is a template implementation 
	of the Mersenne Twister PRNG algorithm.

	The algorithm is described in 
	"Mersenne Twister: A 623-dimensionally equidistributed uniform
	pseudo-random number generator", Makoto Matsumoto and Takuji Nishimura,
	ACM Transactions on Modeling and Computer Simulation: Special Issue on
	Uniform Random Number Generation, Vol. 8, No. 1, January 1998, pp. 3-30. 
  
	The seeding from an integer was changed in January 2002 to address a
	<a href="http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html">weakness</a>.

	Instantiations of the mersenne_twister_engine template class model
	a Psuedo Random Number Generator, as defined in the Boost documentation,
	version 1.48.0. They furthermore model a <em>Reversible Pseudo Random Number Generator</em>, 
	meaning that they have a \c reverse() method which generates psuedo-random numbers
	in the reverse order of \c operator()(). They moreover satisfy the \em Named Concept,
	meaning that they have a \c typedef member \c name, which is a type indicating the name 
	of the class of objects, that is compatible with \c mpl::string.

	\tparam UIntType The unsigned integer type to be used.
	\tparam w	The word-size (in bits) of the generated random numbers. Can be smaller than the number of bits in \p UIntType.
	\tparam n	The state-size (in words) of the RNG.
	\tparam m	Shift-size (in words) used in the linear recurrence. Must satisfy <tt>1 <= m <= n</tt>.
	\tparam r	Number of lower bits used in the \f$\mathbf{x}^l_{k+1}\f$ terms of the recurrence. Must satisfy <tt>r <= w - 1</tt>.
	\tparam a	The last row of the matrix A in the recurrence. It is assumed that A is of the special form in the paper. 
				Moreover, the high-bit of \p a must be 1. This is equivalent to the requirement that A is invertible, 
				which is necessary for a maximal period generator.
	\tparam u	1st integer (right)shift-size in the tempering.
	\tparam d	1st bitmask using in the tempering.
	\tparam s	2nd integer (left)shift-size in the tempering.
	\tparam b	2nd bitmask using in the tempering.
	\tparam t	3rd integer (left)shift-size in the tempering.
	\tparam c	3rd bitmask using in the tempering.
	\tparam l	4th integer (right)shift-size in the tempering.
	\tparam ds	Default seed.
	\tparam f	Multiplier used in seeding. See Knuth TAOCP vol. 2, 3rd ed., p. 106.
	\tparam dms Default seed used when a sequence of seeds is given.
	\tparam g1	1st multiplier for multiple seeding.
	\tparam g2	2nd multiplier for multiple seeding.
	\tparam Name engine name
*/

template<typename UIntType, std::size_t w, std::size_t n, std::size_t m, std::size_t r,
	     UIntType a, std::size_t u, UIntType d, std::size_t s,
		 UIntType b, std::size_t t, 
		 UIntType c, std::size_t l,
		 UIntType ds, UIntType f, 
		 UIntType dms, UIntType g1, UIntType g2,
		 typename Name>
struct mersenne_twister_traits : public engine_traits<linear_generator_engine_tag, UIntType>
{
	// make EngineTraits publicly available, e.g. to adapters
	typedef mersenne_twister_traits EngineTraits;
    //! type of pseudo-random number generated
    typedef UIntType result_type;

	static const size_t		word_size = w;
	static const size_t		state_size = n;
	static const size_t		shift_size = m;
	static const size_t		mask_bits = r;
	static const UIntType	xor_mask = a;
	static const size_t		tempering_u = u;
	static const UIntType	tempering_d = d;
	static const size_t		tempering_s = s;
	static const UIntType	tempering_b = b;
	static const size_t		tempering_t = t; 
	static const UIntType	tempering_c = c;
	static const size_t		tempering_l = l;
	static const UIntType	default_seed = ds;
	static const UIntType	initialization_multiplier = f;
	static const UIntType	default_multiseed = dms;
	static const UIntType	multiseed_initialization_multiplier1 = g1;
	static const UIntType	multiseed_initialization_multiplier2 = g2;

	static const size_t		modulus = 2;

	//! engine name
	/*! A type indicating the name of the MT generator.
		Must be compatible with \c mpl::string.
	*/
	typedef Name name;

	/* derived values */

	//! upper mask, used in \f$\mathbf{x}\mapsto\mathbf{x}^u\f$
	static const UIntType umask = ( ~UIntType(0) ) << mask_bits;
	//! lower mask, used in \f$\mathbf{x}\mapsto\mathbf{x}^l\f$
	static const UIntType lmask = ~umask;
	
	/* compile time asserts */

	// verify UIntType is big enough for w
	static_assert( UIntType(1) << (w - 1), "word-size to large for unsigned integer type" );
	// verify that the shift size is reasonable
	static_assert(1 <= m && m < n, "incorrect shift-size");
	// verify that r is usable (why can't r == w ?)
	static_assert(r <= w - 1, "lower bit-mask size too large");
	// verify that A is invertible
	static_assert( a & ( UIntType(1) << (w - 1) ), "A is not invertible" );
};

/** mersenne twister policy traits */

//! the twist for a single value
template<typename EngineTraits, bool is_TGFSR>
struct mersenne_twister_policy_traits_twist;

template<typename EngineTraits>
struct mersenne_twister_policy_traits_twist<EngineTraits, false>
{
	typedef typename EngineTraits::UIntType UIntType;

	static UIntType twist(UIntType xk, UIntType xkp1, UIntType xkpm)
	{
		return xkpm ^ ( ( (xk & EngineTraits::umask) | (xkp1 & EngineTraits::lmask) ) >> 1 ) ^ ( (xkp1 & 1) * EngineTraits::xor_mask );
	}
};

template<typename EngineTraits>
struct mersenne_twister_policy_traits_twist<EngineTraits, true>
{
	typedef typename EngineTraits::UIntType UIntType;

	/// NOTE: Needs redesign.
	static UIntType twist(UIntType xk, UIntType xkp1, UIntType xkpm)
	{
		return xkpm ^ (xk >> 1) ^ ( (xk & 1) * EngineTraits::xor_mask ); 
	}
};

template<typename EngineTraits, bool use_tempering_u>
struct mersenne_twister_policy_traits_tempering_u;

template<typename EngineTraits>
struct mersenne_twister_policy_traits_tempering_u<EngineTraits, false>
{
	typedef typename EngineTraits::UIntType UIntType;

	static UIntType temper_u(UIntType y)
	{
		return UIntType(0);
	}
};

template<typename EngineTraits>
struct mersenne_twister_policy_traits_tempering_u<EngineTraits, true>
{
	typedef typename EngineTraits::UIntType UIntType;

	static UIntType temper_u(UIntType y)
	{
		return (y >> EngineTraits::tempering_u) & EngineTraits::tempering_d;
	}
};

template<typename EngineTraits, bool use_tempering_l>
struct mersenne_twister_policy_traits_tempering_l;

template<typename EngineTraits>
struct mersenne_twister_policy_traits_tempering_l<EngineTraits, false>
{
	typedef typename EngineTraits::UIntType UIntType;

	static UIntType temper_l(UIntType y)
	{
		return UIntType(0);
	}
};

template<typename EngineTraits>
struct mersenne_twister_policy_traits_tempering_l<EngineTraits, true>
{
	typedef typename EngineTraits::UIntType UIntType;

	static UIntType temper_l(UIntType y)
	{
		return y >> EngineTraits::tempering_l;
	}
};

template<typename EngineTraits, bool use_tempering_u, bool use_tempering_l>
struct mersenne_twister_policy_traits_temper
{
	typedef typename EngineTraits::UIntType UIntType;
	
	//! does the tempering, i.e. \f$\mathbf{x}\mapsto\mathbf{x}\,T\f$ for an invertible matrix \f$T\f$
	static UIntType temper(UIntType y)
	{
		y ^= mersenne_twister_policy_traits_tempering_u<EngineTraits, use_tempering_u>::temper_u(y);
		y ^= (y << EngineTraits::tempering_s) & EngineTraits::tempering_b;
		y ^= (y << EngineTraits::tempering_t) & EngineTraits::tempering_c;
		y ^= mersenne_twister_policy_traits_tempering_l<EngineTraits, use_tempering_l>::temper_l(y);

		return y;
	}
};

template<typename EngineTraits>
struct mersenne_twister_policy_traits
	: public mersenne_twister_policy_traits_twist< EngineTraits, 
		mpl::equal_to< mpl::long_<EngineTraits::mask_bits>, mpl::long_<0> >::value >,
	  public mersenne_twister_policy_traits_temper< EngineTraits,
		mpl::less< mpl::long_<EngineTraits::tempering_u>, mpl::long_<EngineTraits::word_size> >::value,
		mpl::less< mpl::long_<EngineTraits::tempering_l>, mpl::long_<EngineTraits::word_size> >::value >
{
};

template<typename EngineTraits>
class mersenne_twister_engine : public invertible_linear_generator< mersenne_twister_engine<EngineTraits>, EngineTraits >
{
    // imports from base
	typedef invertible_linear_generator< mersenne_twister_engine<EngineTraits>, EngineTraits > base_type;
public:
	typedef mersenne_twister_policy_traits<EngineTraits> PolicyTraits;
    QFCL_USING_TYPE(result_type, base_type);
	typedef typename base_type::UIntType UIntType;
    QFCL_USING_TYPE(state, base_type);
    using base_type::n;
    using base_type::w;
    using base_type::xor_mask;
    using base_type::lmask;
    using base_type::umask;
    using base_type::shift_size;
    using base_type::default_seed;
    using base_type::default_multiseed;
    using base_type::initialization_multiplier;
    using base_type::multiseed_initialization_multiplier1;
    using base_type::multiseed_initialization_multiplier2;

	/*! \name constructors
		@{
	*/
	//! default constructor taking optional seed
    mersenne_twister_engine(UIntType seed_ = default_seed);
	//! constructor taking an arbitrarily long sequence of seeds 
	template<typename It>
	mersenne_twister_engine(It begin, It end);

	//! constructor directly setting the state
	/*! \sa seed(const state &)
	*/
	mersenne_twister_engine(const state & s);
#ifdef	DOXYGEN
	//! compiler generated copy constructor
	/*! Performs a deep copy. Hence the copied generator will produce
		an identical stream of numbers as the original. 
	*/
	mersenne_twister_engine(const mersenne_twister_engine & mt);
#endif	// DOXYGEN
	//!	@}
	/*! \name mersenne_twister_engine interface
		@{
	*/
#ifdef	DOXYGEN
	//! compiler generated assignment operator (deep copy)
	mersenne_twister_engine & operator=(const mersenne_twister_engine &);
#endif	// DOXYGEN
	//! minimum pseudo random number generated
	static result_type min() {return 0;}
	//! maximum pseudo random number generated
	/*! \f$\max() = 2^w-1\f$.
	*/
	static result_type max() {return boost::low_bits_mask_t<w>::sig_bits;}

	//! @}

    // not supported by gcc until 4.9.0?
    //friend base_type;
    friend class invertible_linear_generator< mersenne_twister_engine<EngineTraits>, EngineTraits >;
    // yet this works?
    friend class base_type::base_type;
private:

	//! returns the transition applied to the state \p s
	static state Transition(const state & s);
	//! returns the transition associated with the reverse engine, to the state \p s
	static state ReverseTransition(const state & s);

	//! generate a random number
	//static result_type GetRandomNumber(UIntType (&x)[n], size_t & i);
	//! generate a random number in reverse
	//static result_type GetRandomNumberReverse(UIntType (&x)[n], size_t & i);

	//static result_type Peek(const UIntType (&x)[n], size_t i, long long v);

	static void SeedInitialization_imp(UIntType seed_, UIntType (&)[n], size_t &);
	//! re-seed the generator, \c seed() resets to the default seed
	static void SeedInitialization(UIntType seed_, UIntType (&)[n], size_t &);
	//! re-seed the generator with a sequence of seeds of arbitrary length
	template<typename It>
	static void SeedInitialization(It begin, It end, UIntType (&)[n], size_t &);
	
	//! advance to the next state
	static void Next(UIntType (&x)[n], size_t & i);
	//! return to the previous state
	static void Previous(UIntType (&x)[n], size_t & i);
	//! apply the output transformation
	static result_type Transform(const UIntType (&x)[n], size_t i);

	//! get the first word of the next state
	static UIntType GetNext(const UIntType (&x)[n], size_t i);
	//! the twist for a single value, assuming <tt>x[k]</tt> has not been set for all <tt>k >= i</tt>, and using the \param previous buffer instead
	static UIntType GetNextState(UIntType (&x)[n], size_t & i, const UIntType (&previous)[n]);
	static UIntType GetNextState(UIntType (&x)[n], size_t & i) {return GetNextState(x, i, x);}
	static UIntType GetPreviousState(UIntType (&x)[n], size_t & i, const UIntType (&previous)[n]);
	static UIntType GetPreviousState(UIntType (&x)[n], size_t & i) {return GetPreviousState(x, i, x);}
	//! the twist refers to the \f$F_2\f$-linear recurrence relation in (2.1), applied \c n times in a row
	static void Next_n(UIntType (&x)[n]);
	//! the inverse recurrence
	static void Previous_n(UIntType (&x)[n]);
//#define TWIST_MACRO
//#ifndef TWIST_MACRO
//	static UIntType twist(UIntType xk, UIntType xkp1, UIntType xkpm)
//	{
//		//size_t um = umask, lm = lmask, xm = xor_mask;
//		// NOTE: INEFFICIENT. FIX THIS
//		return r ? xkpm ^ ( ( (xk & umask) | (xkp1 & lmask) ) >> 1 ) ^ ( (xkp1 & 1) * xor_mask )
//			: xkpm ^ ( ( (xk & umask) | (xkp1 & lmask) ) >> 1 ) ^ ( (xk & 1) * xor_mask );
//	}
//#else
//#define twist(xk, xkp1, xkpm) ( xkpm ^ ( ( (xk & umask) | (xkp1 & lmask) ) >> 1 ) ^ ( (xkp1 & 1) * xor_mask ) )
//#endif	// TWIST_MACRO

	/*! \brief inverts the \f$\oplus\f$ in (2.1) and the multiplication by A
		\param xkpn \f$\mathbf{x}_{k+n}\f$
		\param xkpm \f$\mathbf{x}_{k+m}\f$
		\return \f$\mathbf{x}^u_k | \mathbf{x}^l_{k+1}\f$
	*/
	static UIntType reverse_twist_invert(UIntType xkpn, UIntType xkpm)
	{
		UIntType y = xkpn ^ xkpm;

		// multiply on the right by the inverse of A
		// this requires that the high-bit of a is 1
		if ( y & ( UIntType(1) << (w - 1) ) ) // parenthesis around w-1 prevents MS warning C4554
			// low-bit of x_{k+1} is 1
			y = ( (y ^ xor_mask) << 1 ) | 1;
		else 
			y = y << 1;

		return y;
	}

	// the following methods are only needed to implement
	// operator== and the stream operators efficiently
	//! \cond
	/*! \brief returns the preceding sequence of generated numbers
		\param end pointer to end, i.e. one past last element, 
			of an array used to store the results; 
			thus the memory range <tt>[end - p, end)</tt>
			must be allocated.
		\param p the number of preceding numbers to obtain.
	*/
	static void rewind_imp(const UIntType (&x)[n], UIntType * end, std::size_t p)
	{
		UIntType y1 = rewind_invert(x, end, -1);

		for (ssize_t k = -1; k >= -static_cast<ssize_t>(p); --k)
		{
			UIntType y0 = rewind_invert(x, end, k - 1);

			// x_k
			end[k] = (y1 & umask)  | (y0 & lmask);

			y1 = y0;
		}
	}
	
	/*! \brief inverts the \f$\oplus\f$ in (2.1) and the multiplication by A
		\return \f$\mathbf{x}^u_k | \mathbf{x}^l_{k+1}\f$
	*/
	static UIntType rewind_invert(const UIntType (&x)[n], UIntType * end, ssize_t k)
	{
		static const size_t m = shift_size;
		UIntType y = rewind_get(x, end, k + m) ^ rewind_get(x, end, k + n);

		// multiply on the right by the inverse of A
		// this requires that the high-bit of a is 1
		if ( y & ( UIntType(1) << (w - 1) ) )
			// low-bit of x_{k+1} is 1
			y = ( (y ^ xor_mask) << 1 ) | 1;
		else 
			y = y << 1;

		return y;
	}

	/*! \brief returns \f$\mathbf{x}_j\f$, assuming it has already been computed
		Requires <tt>j < n</tt> and <tt>j >= start - end</tt>,
		where \c start points to the start of the passed array.
	*/
	static UIntType rewind_get(const UIntType (&x)[n], UIntType * end, ssize_t j)
	{
		if (j >= 0)
			return x[j];

		return end[j];
	}
	//! \endcond
};

//!	@}

/* public member definitions */

// ctor
/*! \sa seed(UIntType)
*/
template<typename EngineTraits>
mersenne_twister_engine<EngineTraits>::mersenne_twister_engine(UIntType seed_) : base_type(seed_)
{
	//seed(seed_);
}

// ctor
/*! \sa seed(It, It)
*/
template<typename EngineTraits>
template<typename It>
mersenne_twister_engine<EngineTraits>::mersenne_twister_engine(It begin, It end)
{
	seed(begin, end);
}

// ctor
template<typename EngineTraits>
mersenne_twister_engine<EngineTraits>::mersenne_twister_engine(const state & s)
{
	seed(s);
}

template<typename EngineTraits>
void
mersenne_twister_engine<EngineTraits>::SeedInitialization_imp(UIntType seed_, UIntType (&x)[n], size_t & i)
{
	const UIntType mask = max();

	x[i = 0] = seed_ & mask;
	
	do
	{
		++i;
		x[i] = ( initialization_multiplier * ( x[i - 1] ^ ( x[i - 1] >> (w - 2) ) )  + i ) & mask;
	}
	while (i < n - 1);
}

// SeedInitialization
/*! Only the last \c w bits of the seed are used.

	\note This function is always called whenever a \c mersenne_twister_engine is constructed or initialized.
*/
template<typename EngineTraits>
void
mersenne_twister_engine<EngineTraits>::SeedInitialization(UIntType seed_, UIntType (&x)[n], size_t & i)
{
	SeedInitialization_imp(seed_, x, i);

	// go forward one step to be consistent with the MT specification
	Next(x, i);
}

// SeedInitialization
/*! See http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
	and http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/VERSIONS/C-LANG/mt19937-64.c.

	If the length of the sequence is 0 or 1, then seed(It, It) agrees with 
	the other seed function, i.e. it is equivalent to seed() or seed(UIntType),
	respectively. 

	\note As long as the key length, i.e. the length of the sequence of seeds, is 
	less than the state size, that is <tt>key_length < n</tt>, each key (i.e. sequence) 
	produces a unique initial generator state.
	
	Only the last \c w bits of each seed are used.
*/
template<typename EngineTraits>
template<typename It>
void
mersenne_twister_engine<EngineTraits>::SeedInitialization(It begin, It end, UIntType (&x)[n], size_t & i)
{
	using namespace std;

	size_t key_length = distance(begin, end);

	// use default seed if sequence is empty
	if (key_length == 0)
	{
		SeedInitialization(default_seed, x, i);
		return;
	}

	// initializing with a sequence of length 1 should be the same as initializing with a single seed
	if (key_length == 1)
	{
		SeedInitialization(*begin, x, i);
		return;
	}

	// copy the sequence to a vector since It might not be a random access iterator
	vector<UIntType> seed_array(key_length);
	copy( begin, end, seed_array.begin() );

	// first initialize the generator with default_multiseed
	SeedInitialization_imp(default_multiseed, x, i);
	
	const UIntType mask = max();

	size_t l = 1;

	/* k is the counter is the following code */

	for (size_t j = 0, k = std::max(key_length, n); k != 0; --k)
	{
		// non-linear
		x[l] = ( x[l] ^ ( multiseed_initialization_multiplier1 * ( x[l - 1] ^ ( x[l - 1] >> (w - 2) ) ) ) )
				+ seed_array[j] + j;
		x[l] &= mask;

		if (++l == n)
		{
			x[0] = x[n - 1];
			l = 1;
		}

		if (++j == key_length)
			j = 0;
	}

	for (size_t k = n - 1; k != 0; --k)
	{
		// non-linear
		x[l] = ( x[l] ^ ( multiseed_initialization_multiplier2 * ( x[l - 1] ^ ( x[l - 1] >> (w - 2) ) ) ) ) - l;
		x[l] &= mask;

		if (++l == n)
		{
			x[0] = x[n - 1];
			l = 1;
		}
	}

	// guarantee non-zero initial state
	x[0] = UIntType(1) << (w - 1); 

	// go forward one step
	Next(x, i);
}

/* private member definitions */

// Next
template<typename EngineTraits>
inline void
mersenne_twister_engine<EngineTraits>::Next(UIntType (&x)[n], size_t & i)
{
	if (++i == n)
	{
		Next_n(x);
		i = 0;
	}
}

// Previous
template<typename EngineTraits>
inline void
mersenne_twister_engine<EngineTraits>::Previous(UIntType (&x)[n], size_t & i)
{
	if (i-- == 0)
	{
		Previous_n(x);
		i = n - 1;
	}
}

// Transform
template<typename EngineTraits>
inline typename mersenne_twister_engine<EngineTraits>::result_type
mersenne_twister_engine<EngineTraits>::Transform(const UIntType (&x)[n], size_t i)
{
	return PolicyTraits::temper(x[i]);
}

// GetNext
template<typename EngineTraits>
inline typename mersenne_twister_engine<EngineTraits>::UIntType
mersenne_twister_engine<EngineTraits>::GetNext(const UIntType (&x)[n], size_t i)
{
	if (i < n - 1)
		return x[i + 1];

	return PolicyTraits::twist(x[n - 1], x[0], x[shift_size - 1]);
}

// GetNextState
/*! \p i = n - 1 means that no values in \p x have been compted yet.
*/
template<typename EngineTraits>
inline typename mersenne_twister_engine<EngineTraits>::UIntType
mersenne_twister_engine<EngineTraits>::GetNextState(UIntType (&x)[n], size_t & i, const UIntType (&previous)[n])
{
	// perform 1-step twist

	static const size_t m = shift_size;

	if (++i == n)
		i = 0;

	if (i < n - m)
	{
		return x[i] = PolicyTraits::twist(previous[i], previous[i + 1], previous[i + m]);
	}
	else if (i < n - 1)
	{
		return x[i] = PolicyTraits::twist(previous[i], previous[i + 1], x[i - (n - m)]);
	}
	else // i == n - 1
	{
		return x[n - 1] = PolicyTraits::twist(previous[n - 1], x[0], x[m - 1]);
	}
}

// GetPreviousState
/*! \note Assumes that \p x contains at least one value, namely <tt>x[n - 1]</tt>.
*/
template<typename EngineTraits>
inline typename mersenne_twister_engine<EngineTraits>::UIntType
mersenne_twister_engine<EngineTraits>::GetPreviousState(UIntType (&x)[n], size_t & i, const UIntType (&previous)[n])
{
	static const size_t m = shift_size;

	size_t j;
	result_type result = x[j = i];

	UIntType xkpn = previous[j];
	
	UIntType xkpnm1;
	if (j == 0)
	{
		i = n - 1;
		xkpnm1 = x[n - 1];
	}
	else
	{
		xkpnm1 = previous[--i];
	}

	UIntType xkpm = j >= n - m ? previous[j - (n - m)] : x[j + m];
	UIntType xkpmm1 = j > n - m ? previous[j - (n - m + 1)] : x[j + m - 1];

	UIntType y1 = reverse_twist_invert(xkpn, xkpm);
	UIntType y0 = reverse_twist_invert(xkpnm1, xkpmm1);

	x[j] = (y1 & umask) | (y0 & lmask);

	return result;
}

// Next_n
template<typename EngineTraits>
inline void
mersenne_twister_engine<EngineTraits>::Next_n(UIntType (&x)[n])
{
	/*  perform n-step twist

		the loop is split up for performance reasons: to avoid costly modulo operations
	*/

	static const size_t m = shift_size;

	for (size_t k = 0; k < n - m; ++k)
		x[k] = PolicyTraits::twist(x[k], x[k + 1], x[k + m]);

	for (size_t k = n - m; k < n - 1; ++k)
		x[k] = PolicyTraits::twist(x[k], x[k + 1], x[k - (n - m)]);

	x[n - 1] = PolicyTraits::twist(x[n - 1], x[0], x[m - 1]);
}

// reverse twist
template<typename EngineTraits>
inline void
mersenne_twister_engine<EngineTraits>::Previous_n(UIntType (&x)[n])
{
	using std::size_t;

	ssize_t sm = shift_size, sn = n;

	/*  perform n-step twist

		the loop is split up for performance reasons: to avoid costly modulo operations
	*/

	UIntType y1 = reverse_twist_invert(x[sn - 1], x[sm - 1]);
	for (ssize_t k = -1; k > -sm; --k)
	{
		UIntType y0 = reverse_twist_invert(x[sn + k - 1], x[sm + k - 1]);
	
		// x_k
		x[sn + k] = (y1 & umask)  | (y0 & lmask);

		y1 = y0;
	}

	for (ssize_t k = -sm; k > -sn; --k)
	{
		/// NOTE: INEFFICIENT. FIX THIS
		/// NOTE: m == 1 is only valid for r == 0
		UIntType y0 = k < -1 ? reverse_twist_invert(x[sn + k - 1], x[sn + sm + k - 1])
			: reverse_twist_invert(x[sn + k - 1], y1);

		x[sn + k] = (y1 & umask) | (y0 & lmask);

		y1 = y0;
	}

	UIntType y0 = reverse_twist_invert(x[sn - 1], x[sm - 1]);
	x[0] = (y1 & umask) | (y0 & lmask);
}

/* private static member functions */

// Transition
template<typename EngineTraits>
inline 
typename mersenne_twister_engine<EngineTraits>::state 
mersenne_twister_engine<EngineTraits>::Transition(const state & s)
{
	UIntType y[n];

	const UIntType * x = s.rep();

	std::copy(&x[1], &x[n], &y[0]);

	// use equation (2.1) with k = 0 to get x_n, which is now the next number generated,
	// and becomes the last number generated after transition to the next state
	static const size_t m = shift_size;
	y[n - 1] = PolicyTraits::twist(x[0], x[1], x[m]);

    // Why is explicit cast needed? A bug in g++?
    return static_cast<typename mersenne_twister_engine<EngineTraits>::state>(y);
}

// ReverseTransition
template<typename EngineTraits>
inline 
typename mersenne_twister_engine<EngineTraits>::state 
mersenne_twister_engine<EngineTraits>::ReverseTransition(const state & s)
{
	UIntType y[n];

	const UIntType * x = s.rep();

	std::copy(&x[0], &x[n - 1], &y[1]);

	// invert equation (2.1) with k = -1 and k = -2 to get x_(-1), 
	// which is now the next number to be generated in reverse
	static const size_t m = shift_size;
	UIntType y1 = reverse_twist_invert(x[n - 1], x[m - 1]);
	UIntType y0 = reverse_twist_invert(x[n - 2], x[m - 2]);
	y[0] = (y1 & umask)  | (y0 & lmask);

    return static_cast<typename mersenne_twister_engine<EngineTraits>::state>(y);
}

/* engine names */

//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

// abuse of language until there is support multicharacter literals of arbitrary length
typedef mpl::string<'M', 'T'>::type _mt_prefix;
typedef mpl::string<'1', '1', '2', '1', '3'>::type _11213_string;
typedef mpl::string<'1', '9', '9', '3', '7'>::type _19937_string;
typedef mpl::string<'-', '6', '4'>::type _64_suffix;

typedef qfcl::tmp::concatenate< _mt_prefix, _11213_string, mpl::string<'A'>::type >::type mt11213a_name;
typedef qfcl::tmp::concatenate< _mt_prefix, _11213_string, mpl::string<'B'>::type >::type mt11213b_name;
typedef qfcl::tmp::concatenate< _mt_prefix, _19937_string >::type mt19937_name;
typedef qfcl::tmp::concatenate< mt19937_name, _64_suffix >::type mt19937_64_name;
}	// namespace detail
//! \endcond

/*! \brief MT11213A (32-bit)

	350-dimensionally equidistributed PRNG from the original paper.
*/
typedef mersenne_twister_traits<uint32_t, 32, 351, 175, 19,
								UINT32_C(0xe4bd75f5), 11, UINT32_C(0xffffffff), 7,
								UINT32_C(0x655e5280), 15,
								UINT32_C(0xffd58000), 17,
								5489u, UINT32_C(1812433253), 
								UINT32_C(19650218), UINT32_C(1664525), UINT32_C(1566083941),
								detail::mt11213a_name> 
mt11213a_traits;

typedef mersenne_twister_engine<mt11213a_traits> mt11213a;

/*! \brief reverse MT11213A
*/
typedef reverse_adapter<mt11213a> reverse_mt11213a;

/*! \brief MT11213B (32-bit)

	350-dimensionally equidistributed PRNG from the original paper.
*/
typedef mersenne_twister_traits<uint32_t, 32, 351, 175, 19,
						 UINT32_C(0xccab8ee7), 11, UINT32_C(0xffffffff), 7,
						 UINT32_C(0x31b6ab00), 15,
						 UINT32_C(0xffe50000), 17,
						 5489u, UINT32_C(1812433253), 
						 UINT32_C(19650218), UINT32_C(1664525), UINT32_C(1566083941),
						 detail::mt11213b_name> 
mt11213b_traits;

typedef mersenne_twister_engine<mt11213b_traits> mt11213b;

/*! \brief reverse MT11213B
*/
typedef reverse_adapter<mt11213b> reverse_mt11213b;

/*! \brief MT19937 (32-bit)

	623-dimensionally equidistributed PRNG from the original paper.
*/
typedef mersenne_twister_traits<uint32_t, 32, 624, 397, 31,
						 UINT32_C(0x9908b0df), 11, UINT32_C(0xffffffff), 7,
						 UINT32_C(0x9d2c5680), 15,
						 UINT32_C(0xefc60000), 18,
						 5489u, UINT32_C(1812433253), 
						 UINT32_C(19650218), UINT32_C(1664525), UINT32_C(1566083941),
						 detail::mt19937_name> 
mt19937_traits;

typedef mersenne_twister_engine<mt19937_traits> mt19937;

/*! \brief reverse MT19937
*/
typedef reverse_adapter<mt19937> reverse_mt19937;

/*! \brief MT19937-64: 64-bit version of the MT19937

	Variant by Matsumoto and Nishimura, dated Feb. 23, 2005. 
	See <a href="http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt64.html">MT for 64-bit machines</a>.
*/
typedef mersenne_twister_traits<uint64_t, 64, 312, 156, 31,
						 UINT64_C(0xb5026f5aa96619e9), 29, UINT64_C(0x5555555555555555), 17,
						 UINT64_C(0x71d67fffeda60000), 37,
						 UINT64_C(0xfff7eee000000000), 43,
						 5489u, UINT64_C(6364136223846793005),
						 UINT32_C(19650218), 
						 UINT64_C(3935559000370003845), UINT64_C(2862933555777941757),
						 detail::mt19937_64_name> 
mt19937_64_traits;

typedef mersenne_twister_engine<mt19937_64_traits> mt19937_64;

/*! \brief reverse 64-bit MT19937
*/
typedef reverse_adapter<mt19937_64> reverse_mt19937_64;

}	// namespace random
}	// namespace qfcl

#endif	// QFCL_RANDOM_MERSENNE_TWISTER_HPP
