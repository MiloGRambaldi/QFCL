/* qfcl/random/engine/reverse_adapter.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_REVERSE_ADAPTER_HPP
#define QFCL_RANDOM_REVERSE_ADAPTER_HPP

/*! \file qfcl/random/engine/reverse_adapter.hpp
	\brief Original MT psuedo-random number generator

	explain ...

	\author James Hirschorn
	\date February 29, 2012
*/

#include <utility>

#include <boost/mpl/string.hpp>

namespace qfcl {

namespace random {
	
/* reverse engine name */

//! \cond
namespace {

typedef boost::mpl::string<'R', 'e', 'v', 'e', 'r', 's', 'e', '-'>::type _reverse_prefix;
}	// anonymous namespace
//! \endcond

/*! \ingroup random
	@{
*/

/*! \brief Adapts any model of the Reversible Pseudo-Random
	Number Generator concept and Named concept (e.g. 
	\c mersenne_twister), to an engine 
	that produces pseudo-random numbers in reverse.

	The is an application of the Adapter Pattern. The adapted 
	engine also models the Reversible Pseudo-Random 
	Number Generator concept as well as the Named concept.

	The constructors and \c seed member functions take the 
	exact same parameters as the original engine.
	
	In the case of the \c mersenne_twister, the only conceptual difference
	is that the state is defined as the \em next \c n numbers
	that are generated (rather than the previous \c n numbers).
*/
template<typename Engine>
class reverse_adapter : public Engine::EngineTraits
{
public:
	// use perfect forwarding for constructors
	reverse_adapter() : e() {}

	template<typename T1>
	explicit reverse_adapter(T1 && a1) 
		: e( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	reverse_adapter(T1 && a1, T2 && a2) 
		: e( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	typedef typename Engine::result_type result_type;
	typedef typename Engine::state state;

	result_type min() {return e.min();}
	result_type max() {return e.max();}

	result_type operator()() {return e.reverse();}
	result_type reverse() {return e();}

	// recall that peek(0) is the *next* generated random number
	result_type peek(unsigned long long v) const 
	{
		return v > 0 ? e.reverse_peek(v - 1) : e.peek(1);
	}
	// we want to avoid "overflow" of long long
	result_type peek(long long v) const 
	{
        return v > 0 ? e.reverse_peek(v - 1) : e.peek( static_cast<unsigned long long>(1 - v) );
	}
	// note that overflow occurs iff v == numeric_limits<unsigned long long>::max()
	result_type reverse_peek(unsigned long long v) const {return e.peek(v + 1);}
	
	void discard(unsigned long long v) {e.reverse_discard(v);}
	void reverse_discard(unsigned long long v) {e.discard(v);}

	// use perfect forwarding for seeding
	void seed() {e.seed();}

	template<typename T1>
	void seed(T1 && a1) {e.seed( std::forward<T1>(a1) );}

	template<typename T1, typename T2>
	void seed(T1 && a1, T2 && a2) {e.seed( std::forward<T1>(a1), std::forward<T2>(a2) );}

	static void correct(state & s) {Engine::correct(s);}

	friend bool operator==(const reverse_adapter & ra1, const reverse_adapter & ra2)
	{
		return ra1.e == ra2.e;
	}
	friend bool operator!=(const reverse_adapter & ra1, const reverse_adapter & ra2)
	{
		return ra1.e != ra2.e;
	}

	template<typename charT, typename Traits>
	friend std::basic_ostream<charT, Traits> & 
	operator<<(std::basic_ostream<charT, Traits> & os, 
			   const reverse_adapter & ra)
	{
		return os << ra.e;
	}
	template<typename charT, typename Traits>
	friend std::basic_istream<charT, Traits> & 
	operator>>(std::basic_istream<charT, Traits> & is,
			   reverse_adapter & ra)
	{
		return is >> ra.e;
	}

	typedef typename qfcl::tmp::concatenate<_reverse_prefix, typename Engine::name>::type name;
//	typedef typename Engine::parameter parameter;
private:
	Engine e;
};

//! @}

}	// namespace random

}	// namespace qfcl

#endif	// QFCL_RANDOM_REVERSE_ADAPTER_HPP
